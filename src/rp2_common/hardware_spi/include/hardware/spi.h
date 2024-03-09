/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _HARDWARE_SPI_H
#define _HARDWARE_SPI_H

#include "pico.h"
#include "hardware/structs/spi.h"
#include "hardware/regs/dreq.h"

// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_SPI, Enable/disable assertions in the SPI module, type=bool, default=0, group=hardware_spi
#ifndef PARAM_ASSERTIONS_ENABLED_SPI
#define PARAM_ASSERTIONS_ENABLED_SPI 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** \file hardware/spi.h
 *  \defgroup hardware_spi hardware_spi
 *
 * \brief ハードウェアSPI API
 *
 * RP2040はSPI (Serial Peripheral Interface) インタフェースコントローラを2つ搭載しています。
 *
 * PrimeCell SSPはMotorola SPI, National Semiconductor Microwire, Texas Instrumentsの
 * 同期式シリアルインタフェースを持つ周辺機器と同期式シリアル通信を行うマスターまたは
 * スレーブのインタフェースです。
 *
 * コントローラは \ref spi_set_slave 関数を使ってマスターとスレーブを定義することができます。
 *
 * 各コントローラは複数のGPIOピンに接続できます。詳しくはデータシートのGPIO機能選択の節を
 * 参照してください。
 */

// PICO_CONFIG: PICO_DEFAULT_SPI, Define the default SPI for a board, min=0, max=1, group=hardware_spi
// PICO_CONFIG: PICO_DEFAULT_SPI_SCK_PIN, Define the default SPI SCK pin, min=0, max=29, group=hardware_spi
// PICO_CONFIG: PICO_DEFAULT_SPI_TX_PIN, Define the default SPI TX pin, min=0, max=29, group=hardware_spi
// PICO_CONFIG: PICO_DEFAULT_SPI_RX_PIN, Define the default SPI RX pin, min=0, max=29, group=hardware_spi
// PICO_CONFIG: PICO_DEFAULT_SPI_CSN_PIN, Define the default SPI CSN pin, min=0, max=29, group=hardware_spi

/**
 * Opaque type representing an SPI instance.
 */
typedef struct spi_inst spi_inst_t;

/** \brief 第1(SPI 0)ハードウェアSPIインスタンスの識別子（SPI関数で使用）.
 *
 * 例: spi_init(spi0, 48000)
 *
 *  \ingroup hardware_spi
 */
#define spi0 ((spi_inst_t *)spi0_hw)

/** \brief 第2(SPI 1)ハードウェアSPIインスタンスの識別子（SPI関数で使用）.
 *
 * 例: spi_init(spi1, 48000)
 *
 *  \ingroup hardware_spi
 */
#define spi1 ((spi_inst_t *)spi1_hw)

#if !defined(PICO_DEFAULT_SPI_INSTANCE) && defined(PICO_DEFAULT_SPI)
#define PICO_DEFAULT_SPI_INSTANCE (__CONCAT(spi,PICO_DEFAULT_SPI))
#endif

#ifdef PICO_DEFAULT_SPI_INSTANCE
#define spi_default PICO_DEFAULT_SPI_INSTANCE
#endif

/** \brief SPI CPHA (クロックの位相) 値の列挙型.
 *  \ingroup hardware_spi
 */
typedef enum {
    SPI_CPHA_0 = 0, /*!< 0/1でデータ取り込み */
    SPI_CPHA_1 = 1  /*!< 1/0でデータ取り込み */
} spi_cpha_t;

/** \brief SPI CPOL (クロックの極性) 値の列挙型.
 *  \ingroup hardware_spi
 */
typedef enum {
    SPI_CPOL_0 = 0, /*!< 正論理 */
    SPI_CPOL_1 = 1  /*!< 負論理 */
} spi_cpol_t;

/** \brief SPI ビット順値の列挙型.
 *  \ingroup hardware_spi
 */
typedef enum {
    SPI_LSB_FIRST = 0,  /*!< LSBから */
    SPI_MSB_FIRST = 1   /*!< MSBから */
} spi_order_t;

// ----------------------------------------------------------------------------
// Setup

/*! \brief SPIインスタンスを初期化する.
 *  \ingroup hardware_spi
 *
 * SPIを既知の状態にして有効にする。他の関数の前に呼び出す必要があります。
 *
 * \note 要求したボーレートが正確に達成される保証はありません。最も
 * 近い値が選ばれて返されます。
 *
 * \param spi SPIインスタンス識別子, \ref spi0 か \ref spi1 のいずれか
 * \param baudrate 要求するボーレート（Hz単位）
 * \return 実際に設定されたボーレート
 */
uint spi_init(spi_inst_t *spi, uint baudrate);

/*! \brief SPIインスタンスを解放する.
 *  \ingroup hardware_spi
 *
 * SPIを向こうの状態にする。再度デバイスの機能を有効にするにはinitを呼び出す
 * 必要があります。
 *
 * \param spi SPIインスタンス識別子, \ref spi0 か \ref spi1 のいずれか
 */
void spi_deinit(spi_inst_t *spi);

/*! \brief SPIのボーレートをセットする.
 *  \ingroup hardware_spi
 *
 * SPI周波数に指定のボーレートにできるだけ近い値をセットし、
 * 実際にセットされたボーレートを返します。
 *
 * \param spi SPIインスタンス識別子, \ref spi0 か \ref spi1 のいずれか
 * \param baudrate 要求するボーレート（Hz単位）, システムクロックの設定にもよるが、少なくとも2Mbps、あるいはそれ以上のビットレートが可能でなければならない。
 * \return 実際に設定されたボーレート
 */
uint spi_set_baudrate(spi_inst_t *spi, uint baudrate);

/*! \brief SPIのボーレートを取得する.
 *  \ingroup hardware_spi
 *
 * \see spi_set_baudrate でセットしたSPIのボーレートを取得します。
 *
 * \param spi SPIインスタンス識別子, \ref spi0 か \ref spi1 のいずれか
 * \return 実際に設定されたボーレート
 */
uint spi_get_baudrate(const spi_inst_t *spi);

/*! \brief SPIインスタンスをハードウェアインスタンス番号に変換する.
 *  \ingroup hardware_spi
 *
 * \param spi SPIインスタンス
 * \return SPIの番号, 0 か 1.
 */
static inline uint spi_get_index(const spi_inst_t *spi) {
    invalid_params_if(SPI, spi != spi0 && spi != spi1);
    return spi == spi1 ? 1 : 0;
}

static inline spi_hw_t *spi_get_hw(spi_inst_t *spi) {
    spi_get_index(spi); // check it is a hw spi
    return (spi_hw_t *)spi;
}

static inline const spi_hw_t *spi_get_const_hw(const spi_inst_t *spi) {
    spi_get_index(spi);  // check it is a hw spi
    return (const spi_hw_t *)spi;
}

/*! \brief SPIを構成する.
 *  \ingroup hardware_spi
 *
 * SPIが回線上のデータを如何にシリアル化/デシリアライズするかを構成します。
 *
 * \param spi SPIインスタンス識別子, \ref spi0 か \ref spi1 のいずれか
 * \param data_bits 転送あたりのデータビット数. 正しい値は 4..16.
 * \param cpol SSPCLKOUTの極性, Motorola SPIフレームフォーマットにのみ適用可能.
 * \param cpha SSPCLKOUTの位相, Motorola SPIフレームフォーマットにのみ適用可能
 * \param order SPI_MSB_FIRSTでなければならない, PL022では他の値はサポートされていません
 */
static inline void spi_set_format(spi_inst_t *spi, uint data_bits, spi_cpol_t cpol, spi_cpha_t cpha, __unused spi_order_t order) {
    invalid_params_if(SPI, data_bits < 4 || data_bits > 16);
    // LSB-first not supported on PL022:
    invalid_params_if(SPI, order != SPI_MSB_FIRST);
    invalid_params_if(SPI, cpol != SPI_CPOL_0 && cpol != SPI_CPOL_1);
    invalid_params_if(SPI, cpha != SPI_CPHA_0 && cpha != SPI_CPHA_1);

    // SPIを無効に
    uint32_t enable_mask = spi_get_hw(spi)->cr1 & SPI_SSPCR1_SSE_BITS;  // 現在のcr1を保存
    hw_clear_bits(&spi_get_hw(spi)->cr1, SPI_SSPCR1_SSE_BITS);

    hw_write_masked(&spi_get_hw(spi)->cr0,
                    ((uint)(data_bits - 1)) << SPI_SSPCR0_DSS_LSB |
                    ((uint)cpol) << SPI_SSPCR0_SPO_LSB |
                    ((uint)cpha) << SPI_SSPCR0_SPH_LSB,
        SPI_SSPCR0_DSS_BITS |
        SPI_SSPCR0_SPO_BITS |
        SPI_SSPCR0_SPH_BITS);

    // SPIを最有効化
    hw_set_bits(&spi_get_hw(spi)->cr1, enable_mask);
}

/*! \brief SPIをマスター/スレーブにセットする.
 *  \ingroup hardware_spi
 *
 * SPIをマスターモードまたはスレーブモードに構成します。
 * デフォルトでは spi_init() はマスターモードに設定します。
 *
 * \param spi SPIインスタンス識別子, \ref spi0 か \ref spi1 のいずれか
 * \param slave SPIデバイスをスレーブデバイスにする場合は true,
 *              マスターにする場合は false.
 */
static inline void spi_set_slave(spi_inst_t *spi, bool slave) {
    // Disable the SPI
    uint32_t enable_mask = spi_get_hw(spi)->cr1 & SPI_SSPCR1_SSE_BITS;
    hw_clear_bits(&spi_get_hw(spi)->cr1, SPI_SSPCR1_SSE_BITS);

    if (slave)
        hw_set_bits(&spi_get_hw(spi)->cr1, SPI_SSPCR1_MS_BITS);
    else
        hw_clear_bits(&spi_get_hw(spi)->cr1, SPI_SSPCR1_MS_BITS);

    // Re-enable the SPI
    hw_set_bits(&spi_get_hw(spi)->cr1, enable_mask);
}

// ----------------------------------------------------------------------------
// 汎用の入出力関数

/*! \brief SPIデバイスで書き込みができるかチェックする.
 *  \ingroup hardware_spi
 *
 * \param spi SPIインスタンス識別子, \ref spi0 か \ref spi1 のいずれか
 * \return 書き込み用のスペースがない場合は false, 書き込みが可能な場合は true
 */
static inline bool spi_is_writable(const spi_inst_t *spi) {
    return (spi_get_const_hw(spi)->sr & SPI_SSPSR_TNF_BITS);
}

/*! \brief SPIデバイスで読み込みができるかチェックする.
 *  \ingroup hardware_spi
 *
 * \param spi SPIインスタンス識別子, \ref spi0 か \ref spi1 のいずれか
 * \return 読み込みが可能な（すなわち、データが存在する）場合は true
 */
static inline bool spi_is_readable(const spi_inst_t *spi) {
    return (spi_get_const_hw(spi)->sr & SPI_SSPSR_RNE_BITS);
}

/*! \brief SPIがビジーであるかチェックする.
 *  \ingroup hardware_spi
 *
 * \param spi SPIインスタンス識別子, \ref spi0 か \ref spi1 のいずれか
 * \return SPIがビジーの場合は true
 */
static inline bool spi_is_busy(const spi_inst_t *spi) {
    return (spi_get_const_hw(spi)->sr & SPI_SSPSR_BSY_BITS);
}

/*! \brief SPIデバイスに書き込み/から読み込みを行う.
 *  \ingroup hardware_spi
 *
 * SPIに \p src から \p len バイト書き込みます。同時に、
 * SPIから \p len バイト \p dst に読み込みます。
 * 全てのデータが転送されるまでブロックします。SPIハードウェアは
 * 常に基地のデータ速度で転送をおこいますのでタイムアウトはしません。
 *
 * \param spi SPIインスタンス識別子, \ref spi0 か \ref spi1 のいずれか
 * \param src 書き込むデータが入っているバッファ
 * \param dst データを読み込むバッファ
 * \param len 両バッファの長さ
 * \return 読み書きされたバイト数
*/
int spi_write_read_blocking(spi_inst_t *spi, const uint8_t *src, uint8_t *dst, size_t len);

/*! \brief SPIデバイスにブロックして書き込む.
 *  \ingroup hardware_spi
 *
 * SPIに \p src から \p len バイト書き込みます。受信したデータはすべて破棄します。
 * 全てのデータが転送されるまでブロックします。SPIハードウェアは
 * 常に基地のデータ速度で転送をおこいますのでタイムアウトはしません。
 *
 * \param spi SPIインスタンス識別子, \ref spi0 か \ref spi1 のいずれか
 * \param src 書き込むデータが入っているバッファ
 * \param len \p src の長さ
 * \return 書きこまれたバイト数
 */
int spi_write_blocking(spi_inst_t *spi, const uint8_t *src, size_t len);

/*! \brief SPIデバイスから読み込む.
 *  \ingroup hardware_spi
 *
 * SPIから \p len バイト \p dst に読み込みます。
 * 全てのデータが転送されるまでブロックします。SPIハードウェアは
 * 常に基地のデータ速度で転送をおこいますのでタイムアウトはしません。
 * RXからデータが読み込まれるたびに \p repeated_tx_data がTXに出力されます。
 * 通常これは 0 ですが、他の値を要求するデバイスもあります。
 * たとえば、SDカードは 0xff を要求します。
 *
 * \param spi SPIインスタンス識別子, \ref spi0 か \ref spi1 のいずれか
 * \param repeated_tx_data 書き込むデータが入っているバッファ
 * \param dst データを読み込むバッファ
 * \param len \p dst バッファ長
 * \return 読み込んだバイト数
 */
int spi_read_blocking(spi_inst_t *spi, uint8_t repeated_tx_data, uint8_t *dst, size_t len);

// ----------------------------------------------------------------------------
// SPI固有の操作とエイリアス

// FIXME need some instance-private data for select() and deselect() if we are going that route

/*! \brief ハーフワード（16ビット）をSPIデバイスに書き込み/から読み込みを行う.
 *  \ingroup hardware_spi
 *
 * SPIに \p src から \p len ハーフワードを書き込みます。同時に、
 * SPIから \p len ハーフワードを \p dst に読み込みます。
 * 全てのデータが転送されるまでブロックします。SPIハードウェアは
 * 常に基地のデータ速度で転送をおこいますのでタイムアウトはしません。
 *
 * \note SPIは事前に \ref spi_set_format を使って16データビットで初期化する
 * 必要があります。そうしないと、この関数は8データビットの読み書きしか行いません。
 *
 * \param spi SPIインスタンス識別子, \ref spi0 か \ref spi1 のいずれか
 * \param src 書き込むデータが入っているバッファ
 * \param dst データを読み込むバッファ
 * \param len 両バッファのハーフワード単位の長さ
 * \return 読み書きされたハーフワード数
*/
int spi_write16_read16_blocking(spi_inst_t *spi, const uint16_t *src, uint16_t *dst, size_t len);

/*! \brief SPIデバイスに書き込む.
 *  \ingroup hardware_spi
 *
 * SPIに \p src から \p len ハーフワードを書き込みます。受信したデータはすべて破棄します。
 * Write \p len halfwords from \p src to SPI. Discard any data received back.
 * 全てのデータが転送されるまでブロックします。SPIハードウェアは
 * 常に基地のデータ速度で転送をおこいますのでタイムアウトはしません。
 *
 * \note SPIは事前に \ref spi_set_format を使って16データビットで初期化する
 * 必要があります。そうしないと、この関数は8データビットの書き込みしか行いません。
 *
 * \param spi SPIインスタンス識別子, \ref spi0 か \ref spi1 のいずれか
 * \param src 書き込むデータが入っているバッファ
 * \param len バッファのハーフワード単位の長さ
 * \return 書き込まれたハーフワード数
*/
int spi_write16_blocking(spi_inst_t *spi, const uint16_t *src, size_t len);

/*! \brief SPIデバイスから読み込む.
 *  \ingroup hardware_spi
 *
 * SPIから \p len ハーフワードを \p dst に読み込みます。
 * 全てのデータが転送されるまでブロックします。SPIハードウェアは
 * 常に基地のデータ速度で転送をおこいますのでタイムアウトはしません。
 * RXからデータが読み込まれるたびに \p repeated_tx_data がTXに出力されます。
 * 通常これは 0 ですが、他の値を要求するデバイスもあります。
 * たとえば、SDカードは 0xff を要求します。
 *
 * \note SPIは事前に \ref spi_set_format を使って16データビットで初期化する
 * 必要があります。そうしないと、この関数は8データビットの読み込みしか行いません。
 *
 * \param spi SPIインスタンス識別子, \ref spi0 か \ref spi1 のいずれか
 * \param repeated_tx_data 書き込むデータが入っているバッファ
 * \param dst データを読み込むバッファ
 * \param len \p dst バッファのハーフワード単位の長さ
 * \return 読み込んだハーフワード数
 */
int spi_read16_blocking(spi_inst_t *spi, uint16_t repeated_tx_data, uint16_t *dst, size_t len);

/*! \brief 指定したSPIインスタンスとの間の転送のペーシングに使用するDREQを返す.
 *  \ingroup hardware_spi
 *
 * \param spi SPIインスタンス識別子, \ref spi0 か \ref spi1 のいずれか
 * \param is_tx SPIインスタンスへデータを送信する場合は true,
 * SPIインスタンスからデータを受信する場合は false
 */
static inline uint spi_get_dreq(spi_inst_t *spi, bool is_tx) {
    static_assert(DREQ_SPI0_RX == DREQ_SPI0_TX + 1, "");
    static_assert(DREQ_SPI1_RX == DREQ_SPI1_TX + 1, "");
    static_assert(DREQ_SPI1_TX == DREQ_SPI0_TX + 2, "");
    return DREQ_SPI0_TX + spi_get_index(spi) * 2 + !is_tx;
}

#ifdef __cplusplus
}
#endif

#endif
