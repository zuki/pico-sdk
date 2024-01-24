/*
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_BTSTACK_RUN_LOOP_ASYNC_CONTEXT_H
#define _PICO_BTSTACK_RUN_LOOP_ASYNC_CONTEXT_H

#include "btstack_run_loop.h"
#include "pico/async_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief async_context APIと統合するシングルトンBTstack実行ループインスタンスを初期化して返す
 * \ingroup pico_btstack
 *
 * \param context 非同期workの処理のための抽象化を提供するasync_contextインスタンス.
 * \return BTstack実行ループインスタンス
 */
const btstack_run_loop_t *btstack_run_loop_async_context_get_instance(async_context_t *context);

#ifdef __cplusplus
}
#endif
#endif
