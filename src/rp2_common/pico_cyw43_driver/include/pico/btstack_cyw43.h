/*
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_BTSTACK_CYW43_H
#define _PICO_BTSTACK_CYW43_H

#include "pico/async_context.h"
#ifdef __cplusplus
extern "C" {
#endif

/** \file pico/btstack_cyw43.h
 * \defgroup pico_btstack_cyw43 pico_btstack_cyw43
 * \ingroup pico_cyw43_driver
 *
 * \brief 低水準Bluetooth HCIのサポート.
 *
 * このライブラリはCYW43用のBTStackを初期化・開放するためのユーティリティ関数を
 * 提供します。
*/

/**
 * \brief BTstack/CYW43統合の初期化を実行する
 * \ingroup pico_btstack_cyw43
 *
 * \param context 非同期workの抽象化を提供するasync_contextインスタンス.
 * \return true on success or false an error
 */
bool btstack_cyw43_init(async_context_t *context);

/**
 * \brief BTstack/CYW43統合を解放する
 * \ingroup pico_btstack_cyw43
 *
 * \param context \ref btstack_cyw43_init 経由でbtstack_cyw43サポートが追加されたasync_context
 */
void btstack_cyw43_deinit(async_context_t *context);

#ifdef __cplusplus
}
#endif
#endif
