/*
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_LWIP_NOSYS_H
#define _PICO_LWIP_NOSYS_H

#include "pico.h"
#include "pico/async_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \file pico/lwip_nosys.h
* \defgroup pico_lwip_nosys pico_lwip_nosys
* \ingroup pico_lwip
* \brief \c NO_SYS=1 モードのSDKにlwIPを統合するためのGlueライブラリです.
* \ref async_context インスタンスを介してlwIPをフックするため必要な関数は
* シンプルな \c init と \c deinit だけです。
*/

/*! \brief 指定されたasync_contextを使って（NO_SYS=1 モードの）lwIPサポートを初期化します
*  \ingroup pico_lwip_nosys
*
*  初期化が成功した場合、 \ref lwip_nosys_deinit() を呼び出してlwIPのサポートを終了することが
* できます。
*
* \param context 非同期workを処理するための抽象化を提供するasync_contextインスタンス.
* \return  初期化が成功した場合は true
*/
bool lwip_nosys_init(async_context_t *context);

/*! \brief （NO_SYS=1 モードの）lwIPサポートを終了する
 *  \ingroup pico_lwip_nosys
 *
 * lwIPが初期化できるのは一度だけであり、lwIPはシャットダウンメカニズムを提供しないので、
 * lwIPは依然としてリソースを消費する可能性があることに注意してください。
 *
 * ただし、後に再び \ref lwip_nosys_init を呼び出すことは安全です。
 *
 * \param context \ref lwip_nosys_init 経由でlwip_nosysサポートが追加されたasync_context
*/
void lwip_nosys_deinit(async_context_t *context);

#ifdef __cplusplus
}
#endif
#endif
