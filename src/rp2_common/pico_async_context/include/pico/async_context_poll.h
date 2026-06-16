/*
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_ASYNC_CONTEXT_POLL_H
#define _PICO_ASYNC_CONTEXT_POLL_H

/** \file pico/async_context.h
 *  \defgroup async_context_poll async_context_poll
 *  \ingroup pico_async_context
 *  \brief ポーリングによる非同期コンテキストの実装.
 *
 * async_context_poll は1つのコアでの単純なポーリングの使用を
 * 意図した \ref async_context の実装を提供します。スレッド
 * セーフではありません。
 *
 * 保留状態になっているかもしれない非同期作業を処理するために
 * 定期的に \ref async_context_poll() を呼び出す必要があります。
 * するべき作業を現れるまでポーリングループをブロックして、
 * タイとスピニングを避けるために
 * \ref async_context_wait_for_work_until() を使うことができます。
 */
#include "pico/async_context.h"
#include "pico/sem.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct async_context_poll {
    async_context_t core;
    semaphore_t sem;
} async_context_poll_t;

/*!
 * \brief async_context_poll インスタンスをデフォルト値で初期化する.
 * \ingroup async_context_poll
 *
 * この関数が成功した（trueが返った）ら async_context が利用可能と
 * なります。また、async_context_deinit() を呼び出すことで解放する
 * ことができます。
 *
 * \param self 初期化する async_context_freertos 構造体へのポインタ
 * \return 初期化が成功したら true, そうでなければ false
 */
bool async_context_poll_init_with_defaults(async_context_poll_t *self);

#ifdef __cplusplus
}
#endif

#endif
