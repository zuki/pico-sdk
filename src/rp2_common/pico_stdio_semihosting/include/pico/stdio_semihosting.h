/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_STDIO_SEMIHOSTING_H
#define _PICO_STDIO_SEMIHOSTING_H

#include "pico/stdio.h"

/** \brief RAMセミホスティングを使用したstdoutの実験的サポート
 *  \defgroup pico_stdio_semihosting pico_stdio_semihosting
 *  \ingroup pico_stdio
 *
 *  このライブラリをリンクするか、CMakeで
 * `pico_enable_stdio_semihosting(TARGET ENABLED)` を呼び出すと
 * （同じことです）、標準出力に使用されるドライバに
 * セミホスティングが追加されます。
 */

// PICO_CONFIG: PICO_STDIO_SEMIHOSTING_DEFAULT_CRLF, Default state of CR/LF translation for semihosting output, type=bool, default=PICO_STDIO_DEFAULT_CRLF, group=pico_stdio_semihosting
#ifndef PICO_STDIO_SEMIHOSTING_DEFAULT_CRLF
#define PICO_STDIO_SEMIHOSTING_DEFAULT_CRLF PICO_STDIO_DEFAULT_CRLF
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern stdio_driver_t stdio_semihosting;

/*! \brief セミホスティング経由のstdoutを明示的に初期化し、現在のstdoutターゲットセットに追加する
 *  \ingroup pico_stdio_semihosting
 *
 * \note この関数はビルド時に `pico_stdio_semihosting` が含まれている場合
 * \ref stdio_init_all() により自動的に呼び出されます。
 */
void stdio_semihosting_init(void);

#ifdef __cplusplus
}
#endif

#endif
