/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_STDIO_H
#define _PICO_STDIO_H

/** \file stdio.h
*  \defgroup pico_stdio pico_stdio
* カスタマイズされたstdioサポートはUART、USB、セミホストなどからの入出力を可能にする.
*
* \note 入力出力デバイスを追加するためのAPIはまだ安定したものとはみなされていません。
*/

#include "pico.h"

// PICO_CONFIG: PICO_STDOUT_MUTEX, Enable/disable mutex around stdout, type=bool, default=1, group=pico_stdio
#ifndef PICO_STDOUT_MUTEX
#define PICO_STDOUT_MUTEX 1
#endif

// PICO_CONFIG: PICO_STDIO_ENABLE_CRLF_SUPPORT, Enable/disable CR/LF output conversion support, type=bool, default=1, group=pico_stdio
#ifndef PICO_STDIO_ENABLE_CRLF_SUPPORT
#define PICO_STDIO_ENABLE_CRLF_SUPPORT 1
#endif

// PICO_CONFIG: PICO_STDIO_DEFAULT_CRLF, Default for CR/LF conversion enabled on all stdio outputs, type=bool, default=1, depends=PICO_STDIO_ENABLE_CRLF_SUPPORT, group=pico_stdio
#ifndef PICO_STDIO_DEFAULT_CRLF
#define PICO_STDIO_DEFAULT_CRLF 1
#endif

// PICO_CONFIG: PICO_STDIO_STACK_BUFFER_SIZE, Define printf buffer size (on stack)... this is just a working buffer not a max output size, min=0, max=512, default=128, group=pico_stdio
#ifndef PICO_STDIO_STACK_BUFFER_SIZE
#define PICO_STDIO_STACK_BUFFER_SIZE 128
#endif

// PICO_CONFIG: PICO_STDIO_DEADLOCK_TIMEOUT_MS, Time after which to assume stdio_usb is deadlocked by use in IRQ and give up, type=int, default=1000, group=pico_stdio
#ifndef PICO_STDIO_DEADLOCK_TIMEOUT_MS
#define PICO_STDIO_DEADLOCK_TIMEOUT_MS 1000
#endif

#ifdef __cplusplus
extern "C" {
#endif


typedef struct stdio_driver stdio_driver_t;

/*! \brief 現在バイナリにリンクされているすべての標準stdio型を初期化する.
 * \ingroup pico_stdio
 *
 * クロックを設定したら、バイナリ内の各ライブラリの存在に基づいてUART、USB、
 * semihostingのstdioサポートを有効にするためにこの関数を呼び出します。
 *
 * stdio_usb が構成されている場合 \ref stdio_usb_init で指定した変数経由の
 * 接続を待機してオプションでこの関数をブロックするようにすることができます
 * （すなわち \ref PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS ）。
 *
 * \return outputを少なくとも¹つ初期化に成功したら true, そうでなければ false.
 * \see stdio_uart, stdio_usb, stdio_semihosting
 */
bool stdio_init_all(void);

/*! \brief バッファされた出力をフラッシュする.
 * \ingroup pico_stdio
 */
void stdio_flush(void);

/*! \brief タイムアウト時間内にstdinに入力があったら一文字返す.
 * \ingroup pico_stdio
 *
 * \param timeout_us マイクロ秒単位のタイムアウト, 文字入力がなかったら待たないにする場合は 0.
 * \return 0-255で表される文字、タイムアウトとなった場合は PICO_ERROR_TIMEOUT
 */
int getchar_timeout_us(uint32_t timeout_us);

/*! \brief 入出力用に使用するアクティブなドライバリストに/からドライバを追加/削除する
 * \ingroup pico_stdio
 *
 * \note この関数は常に初期化されたドライバから呼び出す必要があり、リエントラントではありません。
 * \param driver ドリア場
 * \param enabled 追加する場合は true, 削除する場合は false
 */
void stdio_set_driver_enabled(stdio_driver_t *driver, bool enabled);

/*! \brief 一つのドライバへの出力制限を制御する
 * \ingroup pico_stdio
 *
 * \note この関数は常に初期化されたドライバから呼び出す必要があります。
 *
 * \param driver 非NULLの場合、そのドライバだけが入出力に使用されます（有効なドライバリストに
 * あると仮定します）。
 *               NULLの場合、すべての有効なドライバが使用されます。
 */
void stdio_filter_driver(stdio_driver_t *driver);

/*! \brief 転送時に改行コードから復帰コードへの変換を制御する.
 * \ingroup pico_stdio
 *
 * \note この関数は常に初期化されたドライバから呼び出す必要があります。
 *
 * \param driver ドライバ
 * \param translate trueの場合, 転送時に改行コードを復帰コードに変換する
 */
void stdio_set_translate_crlf(stdio_driver_t *driver, bool translate);

/*! \brief 有効なすべてのCR/LF変換をスキップするputchar
 * \ingroup pico_stdio
 */
int putchar_raw(int c);

/*! \brief 有効なすべてのCR/LF変換をスキップするputs
 * \ingroup pico_stdio
 */
int puts_raw(const char *s);

/*! \brief 文字が入力されたら通知する関数を登録する
 * \ingroup pico_stdio
 *
 * \param fn 文字が入力された場合に呼び出されるコールバック関数. 既存のコールバックを
 * キャンセルする場合は NULL を渡す
 * \param param コールバックに渡すためのポインタ
 */
void stdio_set_chars_available_callback(void (*fn)(void*), void *param);

#ifdef __cplusplus
}
#endif

#endif
