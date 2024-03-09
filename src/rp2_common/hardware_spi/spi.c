/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "hardware/resets.h"
#include "hardware/clocks.h"
#include "hardware/spi.h"

static inline void spi_reset(spi_inst_t *spi) {
    invalid_params_if(SPI, spi != spi0 && spi != spi1);
    reset_block(spi == spi0 ? RESETS_RESET_SPI0_BITS : RESETS_RESET_SPI1_BITS);
}

static inline void spi_unreset(spi_inst_t *spi) {
    invalid_params_if(SPI, spi != spi0 && spi != spi1);
    unreset_block_wait(spi == spi0 ? RESETS_RESET_SPI0_BITS : RESETS_RESET_SPI1_BITS);
}

uint spi_init(spi_inst_t *spi, uint baudrate) {
    spi_reset(spi);
    spi_unreset(spi);

    uint baud = spi_set_baudrate(spi, baudrate);
    spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    // Always enable DREQ signals -- harmless if DMA is not listening
    hw_set_bits(&spi_get_hw(spi)->dmacr, SPI_SSPDMACR_TXDMAE_BITS | SPI_SSPDMACR_RXDMAE_BITS);

    // Finally enable the SPI
    hw_set_bits(&spi_get_hw(spi)->cr1, SPI_SSPCR1_SSE_BITS);

    return baud;
}

void spi_deinit(spi_inst_t *spi) {
    hw_clear_bits(&spi_get_hw(spi)->cr1, SPI_SSPCR1_SSE_BITS);
    hw_clear_bits(&spi_get_hw(spi)->dmacr, SPI_SSPDMACR_TXDMAE_BITS | SPI_SSPDMACR_RXDMAE_BITS);
    spi_reset(spi);
}

uint spi_set_baudrate(spi_inst_t *spi, uint baudrate) {
    uint freq_in = clock_get_hz(clk_peri);
    uint prescale, postdiv;
    invalid_params_if(SPI, baudrate > freq_in);

    // Disable the SPI
    uint32_t enable_mask = spi_get_hw(spi)->cr1 & SPI_SSPCR1_SSE_BITS;
    hw_clear_bits(&spi_get_hw(spi)->cr1, SPI_SSPCR1_SSE_BITS);

    // Find smallest prescale value which puts output frequency in range of
    // post-divide. Prescale is an even number from 2 to 254 inclusive.
    for (prescale = 2; prescale <= 254; prescale += 2) {
        if (freq_in < (prescale + 2) * 256 * (uint64_t) baudrate)
            break;
    }
    invalid_params_if(SPI, prescale > 254); // Frequency too low

    // Find largest post-divide which makes output <= baudrate. Post-divide is
    // an integer in the range 1 to 256 inclusive.
    for (postdiv = 256; postdiv > 1; --postdiv) {
        if (freq_in / (prescale * (postdiv - 1)) > baudrate)
            break;
    }

    spi_get_hw(spi)->cpsr = prescale;
    hw_write_masked(&spi_get_hw(spi)->cr0, (postdiv - 1) << SPI_SSPCR0_SCR_LSB, SPI_SSPCR0_SCR_BITS);

    // Re-enable the SPI
    hw_set_bits(&spi_get_hw(spi)->cr1, enable_mask);

    // Return the frequency we were able to achieve
    return freq_in / (prescale * postdiv);
}

uint spi_get_baudrate(const spi_inst_t *spi) {
    uint prescale = spi_get_const_hw(spi)->cpsr;
    uint postdiv = ((spi_get_const_hw(spi)->cr0  & SPI_SSPCR0_SCR_BITS) >> SPI_SSPCR0_SCR_LSB) + 1;
    return clock_get_hz(clk_peri) / (prescale * postdiv);
}

// srcからSPIにlenバイト書き出す。同時にSPIからdstへlenバイト読み込む。
// この関数は既知の時間（送信ビット数 * ビットあたりの時間）で終了することが
// 保証されていることに注意。
int __not_in_flash_func(spi_write_read_blocking)(spi_inst_t *spi, const uint8_t *src, uint8_t *dst, size_t len) {
    invalid_params_if(SPI, 0 > (int)len);

    // RXのFIFOに収まる以上の転送を行ってはならない。このコードが何回も
    // 中断された場合、FIFOはオーバーフローするからである。
    const size_t fifo_depth = 8;
    size_t rx_remaining = len, tx_remaining = len;

    while (rx_remaining || tx_remaining) {
        if (tx_remaining && spi_is_writable(spi) && rx_remaining < tx_remaining + fifo_depth) {
            spi_get_hw(spi)->dr = (uint32_t) *src++;
            --tx_remaining;
        }
        if (rx_remaining && spi_is_readable(spi)) {
            *dst++ = (uint8_t) spi_get_hw(spi)->dr;
            --rx_remaining;
        }
    }

    return (int)len;
}

// srcからSPIに直接lenバイト書き出す。SPIから受信したデータは破棄する
int __not_in_flash_func(spi_write_blocking)(spi_inst_t *spi, const uint8_t *src, size_t len) {
    invalid_params_if(SPI, 0 > (int)len);
    // TX FIFOに書き込むがRXは無視する。その後クリーンアップする。
    // RXがフルの場合、PL022はRXプッシュを禁止し、プッシュオンフルの
    // スティッキーフラグを設定するがシフトは続行する。SSPIMSC_RORIMが
    // 設定されていなければ安全である
    for (size_t i = 0; i < len; ++i) {
        while (!spi_is_writable(spi))
            tight_loop_contents();
        spi_get_hw(spi)->dr = (uint32_t)src[i];
    }
    // RX FIFOをドレインし、シフトが終了するのを待ち（これはおそらく
    // TX FIFOがドレインした*後*）、RX FIFOを再度ドレインする。
    while (spi_is_readable(spi))
        (void)spi_get_hw(spi)->dr;
    while (spi_get_hw(spi)->sr & SPI_SSPSR_BSY_BITS)
        tight_loop_contents();
    while (spi_is_readable(spi))
        (void)spi_get_hw(spi)->dr;

    // オーバーランフラグはセットしたままにしない
    spi_get_hw(spi)->icr = SPI_SSPICR_RORIC_BITS;

    return (int)len;
}

// SPIからdstへ直接lenバイト読み込む。repeated_tx_dataはSIからデータが読み込める
// 限りSOに繰り返し出力される。一般に、これには0を指定するが特定の値を要求する
// デバイスもある。たとえば、SDカードは0xffを要求する
int __not_in_flash_func(spi_read_blocking)(spi_inst_t *spi, uint8_t repeated_tx_data, uint8_t *dst, size_t len) {
    invalid_params_if(SPI, 0 > (int)len);
    const size_t fifo_depth = 8;
    size_t rx_remaining = len, tx_remaining = len;

    while (rx_remaining || tx_remaining) {
        if (tx_remaining && spi_is_writable(spi) && rx_remaining < tx_remaining + fifo_depth) {
            spi_get_hw(spi)->dr = (uint32_t) repeated_tx_data;
            --tx_remaining;
        }
        if (rx_remaining && spi_is_readable(spi)) {
            *dst++ = (uint8_t) spi_get_hw(spi)->dr;
            --rx_remaining;
        }
    }

    return (int)len;
}

// srcからSPIにlen個のハーフバイト書き出す。同時にSPIからdstへlen個のハーフバイトを
// 読み込む
int __not_in_flash_func(spi_write16_read16_blocking)(spi_inst_t *spi, const uint16_t *src, uint16_t *dst, size_t len) {
    invalid_params_if(SPI, 0 > (int)len);
    // Never have more transfers in flight than will fit into the RX FIFO,
    // else FIFO will overflow if this code is heavily interrupted.
    const size_t fifo_depth = 8;
    size_t rx_remaining = len, tx_remaining = len;

    while (rx_remaining || tx_remaining) {
        if (tx_remaining && spi_is_writable(spi) && rx_remaining < tx_remaining + fifo_depth) {
            spi_get_hw(spi)->dr = (uint32_t) *src++;
            --tx_remaining;
        }
        if (rx_remaining && spi_is_readable(spi)) {
            *dst++ = (uint16_t) spi_get_hw(spi)->dr;
            --rx_remaining;
        }
    }

    return (int)len;
}

// Write len bytes directly from src to the SPI, and discard any data received back
int __not_in_flash_func(spi_write16_blocking)(spi_inst_t *spi, const uint16_t *src, size_t len) {
    invalid_params_if(SPI, 0 > (int)len);
    // Deliberately overflow FIFO, then clean up afterward, to minimise amount
    // of APB polling required per halfword
    for (size_t i = 0; i < len; ++i) {
        while (!spi_is_writable(spi))
            tight_loop_contents();
        spi_get_hw(spi)->dr = (uint32_t)src[i];
    }

    while (spi_is_readable(spi))
        (void)spi_get_hw(spi)->dr;
    while (spi_get_hw(spi)->sr & SPI_SSPSR_BSY_BITS)
        tight_loop_contents();
    while (spi_is_readable(spi))
        (void)spi_get_hw(spi)->dr;

    // Don't leave overrun flag set
    spi_get_hw(spi)->icr = SPI_SSPICR_RORIC_BITS;

    return (int)len;
}

// Read len halfwords directly from the SPI to dst.
// repeated_tx_data is output repeatedly on SO as data is read in from SI.
int __not_in_flash_func(spi_read16_blocking)(spi_inst_t *spi, uint16_t repeated_tx_data, uint16_t *dst, size_t len) {
    invalid_params_if(SPI, 0 > (int)len);
    const size_t fifo_depth = 8;
    size_t rx_remaining = len, tx_remaining = len;

    while (rx_remaining || tx_remaining) {
        if (tx_remaining && spi_is_writable(spi) && rx_remaining < tx_remaining + fifo_depth) {
            spi_get_hw(spi)->dr = (uint32_t) repeated_tx_data;
            --tx_remaining;
        }
        if (rx_remaining && spi_is_readable(spi)) {
            *dst++ = (uint16_t) spi_get_hw(spi)->dr;
            --rx_remaining;
        }
    }

    return (int)len;
}
