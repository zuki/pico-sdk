/*
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_LWIP_FREERTOS_H
#define _PICO_LWIP_FREERTOS_H

#include "pico.h"
#include "pico/async_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \file pico/lwip_freertos.h
* \defgroup pico_lwip_freertos pico_lwip_freertos
* \ingroup pico_lwip
* \brief \c NO_SYS=0 モードのlwIPとSDKを統合するためのGlueライブラリ.
* \ref async_context インスタンスを介してlwIP（完全なブロッキングAPIのサポートを含む）
* をフックするため必要な関数はシンプルな \c init と \c deinit だけです。
*/

/*! \brief 指定されたasync_contextを使ってFreeRTOS用の（NO_SYS=0 モードの）lwIP サポートを初期化する
 *  \ingroup pico_lwip_freertos
 *
 * 初期化が成功した場合、 \ref lwip_freertos_deinit() を呼び出してlwIPのサポートを終了することができます。
 *
 * \param context 非同期workを処理するための抽象化を提供するasync_contextインスタンス.
 * 一般にこれはその必要はありませんが \ref  async_context_freertos のインスタンスである
 * ことに注意してください。
 *
 * \return 初期化が成功した場合は true
*/
bool lwip_freertos_init(async_context_t *context);

/*! \brief FreeRTOS用の（NO_SYS=0 モードの）lwIP サポートを解放する
 *  \ingroup pico_lwip_freertos
 *
 * lwIPが初期化できるのは一度だけであり、lwIPはシャットダウンメカニズムを提供しないので
 * lwIPは依然としてリソースを消費する可能性があることに注意してください。
 *
 * ただし、後に再び  \ref lwip_freertos_init を呼び出すことは安全です。
 *
 * \param context \ref lwip_freertos_init 経由でlwip_freertosサポートが追加されたasync_context
*/
void lwip_freertos_deinit(async_context_t *context);

#ifdef __cplusplus
}
#endif
#endif
