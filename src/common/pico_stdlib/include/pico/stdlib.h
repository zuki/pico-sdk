/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_STDLIB_H
#define _PICO_STDLIB_H

#include "pico.h"
#include "pico/stdio.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \file stdlib.h
 *  \defgroup pico_stdlib pico_stdlib
 *
 * ほとんどの実行ファイルで使用されるRaspberry Pi Pico SDKライブラリのコアサブセットと
 * いくつかの追加ユーティリティ関数をまとめたものです。pico_stdlibを追加することにより
 * 標準出力に出力したりLEDを点滅させたりする基本的なプログラムを実行するのに必要なものが
 * すべて揃います。
 *
 * このライブラリは以下をまとめています:
 *   - @ref hardware_uart
 *   - @ref hardware_gpio
 *   - @ref pico_binary_info
 *   - @ref pico_runtime
 *   - @ref pico_platform
 *   - @ref pico_printf
 *   - @ref pico_stdio
 *   - @ref pico_standard_link
 *   - @ref pico_util
 *
 * これらの関数で使用される基本的なデフォルト値は、デフォルトで適切な値に
 * 設定されていますが　config.h などを介してボード定義ヘッダーでカスタマイズ
 * することができます。
 */

// Note PICO_STDIO_UART, PICO_STDIO_USB, PICO_STDIO_SEMIHOSTING are set by the
// respective INTERFACE libraries, so these defines are set if the library
// is included for the target executable

#if LIB_PICO_STDIO_UART
#include "pico/stdio_uart.h"
#endif

#if LIB_PICO_STDIO_USB
#include "pico/stdio_usb.h"
#endif

#if LIB_PICO_STDIO_SEMIHOSTING
#include "pico/stdio_semihosting.h"
#endif

// PICO_CONFIG: PICO_DEFAULT_LED_PIN, Optionally define a pin that drives a regular LED on the board, group=pico_stdlib

// PICO_CONFIG: PICO_DEFAULT_LED_PIN_INVERTED, 1 if LED is inverted or 0 if not, type=int, default=0, group=pico_stdlib
#ifndef PICO_DEFAULT_LED_PIN_INVERTED
#define PICO_DEFAULT_LED_PIN_INVERTED 0
#endif

// PICO_CONFIG: PICO_DEFAULT_WS2812_PIN, Optionally define a pin that controls data to a WS2812 compatible LED on the board, group=pico_stdlib
// PICO_CONFIG: PICO_DEFAULT_WS2812_POWER_PIN, Optionally define a pin that controls power to a WS2812 compatible LED on the board, group=pico_stdlib

/*! \brief デフォルトのUARTを設定し、デフォルトのGPIOに割り当てる
 *  \ingroup pico_stdlib
 *
 * デフォルトでは、UART 0を使用し, TXは GPIO 0 ピン,
 * RXは GPIO 1 ピン, ボーレートは 115200 です。
 *
 * この関数を呼び出すと @ref pico_stdio_uart  ライブラリがリンクされている場合
 * UART経由のstdin/stdoutの初期化も行います。
 *
 * デフォルトは次の構成適宜を使って変更することができます。
 *  PICO_DEFAULT_UART_INSTANCE,
 *  PICO_DEFAULT_UART_BAUD_RATE
 *  PICO_DEFAULT_UART_TX_PIN
 *  PICO_DEFAULT_UART_RX_PIN
 */
void setup_default_uart(void);

/*! \brief システムクロックを 48MHz に初期化する
 *  \ingroup pico_stdlib
 *
 *  システムクロックを 48MHz に設定し、ペリフェラルクロックもそれに合わせます。
 */
void set_sys_clock_48mhz(void);

/*! \brief システムクロックを初期化する
 *  \ingroup pico_stdlib
 *
 * \param vco_freq SYS PLLで使用する電圧制御発振器周波数
 * \param post_div1 SYS PLLの第一ポストディバイダ
 * \param post_div2 SYS PLLの第二のポストディバイダ
 *
 * PLL駆動の詳細についてはデータシートのPLLドキュメントを参照してください。
 */
void set_sys_clock_pll(uint32_t vco_freq, uint post_div1, uint post_div2);

/*! \brief 指定したシステムクロック周波数が有効か/達成可能かをチェックする
 *  \ingroup pico_stdlib
 *
 * \param freq_khz 指定周波数
 * \param vco_freq_out 成功時にSYS PLLで使用される電圧制御発振器周波数
 * \param post_div1_out 成功時のSYS PLLの第一ポストディバイダ
 * \param post_div2_out 成功時のSYS PLLの第二ポストディバイダ
 * @return 周波数が可能で出力パラメータが書き込まれた場合は true
 */
bool check_sys_clock_khz(uint32_t freq_khz, uint *vco_freq_out, uint *post_div1_out, uint *post_div2_out);

/*! \brief khz単位のシステムクロック周波数の設定を試みる
 *  \ingroup pico_stdlib
 *
 * 必ずしもすべてのクロック周波数が可能でなないことに注意してください。
 * set_sys_clock_pll で使用するパラメータ は src/rp2_common/hardware_clocks/scripts/vcocalc.py を
 * 使用することが望ましいです。
 *
 * \param freq_khz 指定周波数
 * \param required true の場合、この関数は周波数が達成不可能な場合に assert する
 * \return クロックが構成された場合は true
 */
static inline bool set_sys_clock_khz(uint32_t freq_khz, bool required) {
    uint vco, postdiv1, postdiv2;
    if (check_sys_clock_khz(freq_khz, &vco, &postdiv1, &postdiv2)) {
        set_sys_clock_pll(vco, postdiv1, postdiv2);
        return true;
    } else if (required) {
        panic("System clock of %u kHz cannot be exactly achieved", freq_khz);
    }
    return false;
}

#ifdef __cplusplus
}
#endif
#endif
