/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_STDIO_UART_H
#define _PICO_STDIO_UART_H

#include "pico/stdio.h"
#include "hardware/uart.h"

/** \brief UARTを使用したstdin_stdoutをサポートする
 *  \defgroup pico_stdio_uart pico_stdio_uart
 *  \ingroup pico_stdio
 *
 *  このライブラリをリンクするか、CMakeで
 * `pico_enable_stdio_uart(TARGET ENABLED)` を呼び出すと
 * （同じことです）、標準入出力に使用されるドライバにUARTが
 * 追加されます。
 */

// PICO_CONFIG: PICO_STDIO_UART_DEFAULT_CRLF, Default state of CR/LF translation for UART output, type=bool, default=PICO_STDIO_DEFAULT_CRLF, group=pico_stdio_uart
#ifndef PICO_STDIO_UART_DEFAULT_CRLF
#define PICO_STDIO_UART_DEFAULT_CRLF PICO_STDIO_DEFAULT_CRLF
#endif

// PICO_CONFIG: PICO_STDIO_UART_SUPPORT_CHARS_AVAILABLE_CALLBACK, Enable UART STDIO support for stdio_set_chars_available_callback. Can be disabled to make use of the uart elsewhere, type=bool, default=1, group=pico_stdio_uart
#ifndef PICO_STDIO_UART_SUPPORT_CHARS_AVAILABLE_CALLBACK
#define PICO_STDIO_UART_SUPPORT_CHARS_AVAILABLE_CALLBACK 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern stdio_driver_t stdio_uart;

/*! \brief UART経由のstdin/stdoutを明示的に初期化し、現在のstdin/stdoutドライバセットに追加する
 *  \ingroup pico_stdio_uart
 *
 * この関数は（定義されていれば）UART出力用にPICO_DEFAULT_UART_TX_PIN を、
 * （定義されていれば）UART入力用に PICO_DEFAULT_UART_RX_PIN を
 * 設定し、ボーレートを PICO_DEFAULT_UART_BAUD_RATE に構成します。
 *
 * \note この関数はビルド時に `pico_stdio_uart` が含まれている場合
 * \ref stdio_init_all() により自動的に呼び出されます。
 */
void stdio_uart_init(void);

/*! \brief UART経由のstdoutだけ（stdinはしない）を明示的に初期化し、現在のstdoutドライバセットに追加する
 *  \ingroup pico_stdio_uart
 *
 * この関数は（定義されていれば）UART出力用に PICO_DEFAULT_UART_TX_PIN を
 * 設定し、ボーレートを PICO_DEFAULT_UART_BAUD_RATE に構成します。
 */
void stdout_uart_init(void);

/*! \brief UART経由のstdinだけ（stdoutはしない）を明示的に初期化し、現在のstdinドライバセットに追加する
 *  \ingroup pico_stdio_uart
 *
 * この関数は（定義されていれば）UART入力用に PICO_DEFAULT_UART_RX_PIN を
 * 設定し、ボーレートを PICO_DEFAULT_UART_BAUD_RATE に構成します。
 */
void stdin_uart_init(void);

/*! \brief どくじの設定でUART経由のstdin/stdoutを初期化し、現在のstdin/stdoutドライバセットに追加する
 *  \ingroup pico_stdio_uart
 *
 * \param uart 使用するuartインスタンス, \ref uart0 または \ref uart1
 * \param baud_rate Hz単位のボーレート
 * \param tx_pin stdout用に使用するUARTピン（stdoutを使用しない場合は -1） * \param rx_pin stdin用に使用するUARTピン（stdinを使用しない場合は -1）
 */
void stdio_uart_init_full(uart_inst_t *uart, uint baud_rate, int tx_pin, int rx_pin);

#ifdef __cplusplus
}
#endif

#endif
