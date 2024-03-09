///////////////////////////////////////////////////////////////////////////////
// \author (c) Marco Paland (info@paland.com)
//             2014-2019, PALANDesign Hannover, Germany
//
// \license The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// \brief Tiny printf, sprintf and snprintf implementation, optimized for speed on
//        embedded systems with a very limited resources.
//        Use this instead of bloated standard/newlib printf.
//        These routines are thread safe and reentrant.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _PICO_PRINTF_H
#define _PICO_PRINTF_H

/** \file printf.h
 *  \defgroup pico_printf pico_printf
 *  \brief printfの代替関数.
 *
 * Marco Paland氏 (info@paland.com) によるprintfのコンパクトな代替関数.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "pico.h"
#include <stdio.h>
#include <stdarg.h>

// PICO_CONFIG: PICO_PRINTF_ALWAYS_INCLUDED, Whether to always include printf code even if only called weakly (by panic), type=bool, default=1 in debug build 0 otherwise, group=pico_printf
#ifndef PICO_PRINTF_ALWAYS_INCLUDED
#ifndef NDEBUG
#define PICO_PRINTF_ALWAYS_INCLUDED 1
#else
#define PICO_PRINTF_ALWAYS_INCLUDED 0
#endif
#endif

#if LIB_PICO_PRINTF_PICO
// weak raw printf may be a puts if printf has not been called,
// so that we can support gc of printf when it isn't called
//
// it is called raw to distinguish it from the regular printf which
// is in stdio.c and does mutex protection
#if !PICO_PRINTF_ALWAYS_INCLUDED
bool __printflike(1, 0) weak_raw_printf(const char *fmt, ...);
bool weak_raw_vprintf(const char *fmt, va_list args);
#else
#define weak_raw_printf(...) ({printf(__VA_ARGS__); true;})
#define weak_raw_vprintf(fmt,va) ({vprintf(fmt,va); true;})
#endif

/**!
 * \ingroup pico_printf
 * \brief 出力関数付きprintf.
 *
 * _putchar() による固定出力を持つ printf() の動的な代替関数として使用することができます。
 * \param out 引数として1文字と引数へのポインタを取る出力関数
 * \param arg 出力関数に渡すユーザデータ用の引数ポインタ
 * \param format 出力形式を指定する文字列
 * \return 出力関数に送信された文字数. 終端のnull文字は含まない
 */
int vfctprintf(void (*out)(char character, void *arg), void *arg, const char *format, va_list va);

#else

#define weak_raw_printf(...) ({printf(__VA_ARGS__); true;})
#define weak_raw_vprintf(fmt,va) ({vprintf(fmt,va); true;})

#endif

#ifdef __cplusplus
}
#endif

#endif  // _PRINTF_H_
