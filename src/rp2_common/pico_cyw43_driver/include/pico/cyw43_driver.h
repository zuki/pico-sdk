/*
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_CYW43_DRIVER_H
#define _PICO_CYW43_DRIVER_H

/** \file pico/cyw43_driver.h
 *  \defgroup pico_cyw43_driver pico_cyw43_driver
 *
 * 低水準cyw43_driverのラッパーです。バックグラウンドworkを処理する
 * ために \ref pico_async_context と統合されています。
 */

#include "pico.h"
#include "pico/async_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief 低水準cyw43_driveを初期化し、指定されたasync_contextと統合する
 *  \ingroup pico_cyw43_driver
 *
 * 初期化が成功した場合、 \ref lwip_nosys_deinit() を呼び出してlwIPのサポートを
 * 終了させることができます。
 *
 * \param context 非同期workの週釣果を提供するasync_contextインスタンス.
 * \return 初期化が成功した場合はtrue
*/
bool cyw43_driver_init(async_context_t *context);

/*! \brief 低水準cyw43_driverを解放し、async_contextとの統合を解除する
 *  \ingroup pico_cyw43_driver
 *
 * \param context \ref cyw43_driver_init 経由でcyw43_driverが追加されたasync_context
*/
void cyw43_driver_deinit(async_context_t *context);

#ifdef __cplusplus
}
#endif
#endif
