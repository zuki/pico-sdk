/*
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_ASYNC_CONTEXT_THREADSAFE_BACKGROUND_H
#define _PICO_ASYNC_CONTEXT_THREADSAFE_BACKGROUND_H

/** \file pico/async_context.h
 *  \defgroup async_context_threadsafe_background async_context_threadsafe_background
 *  \ingroup pico_async_context
 *  \brief スレッドセーフなバックグラウンド処理による非同期コンテキストの実装.
 *
 * async_context_threadsafe_background は低優先度IRQにおける
 * 非同期作業を処理する \ref async_context の実装を提供します。
 * ユーザはworkをポーリングする必要がありません。
 *
 * \note このasync_contextを使用するワーカはIRQからの呼び出しが
 * 安全である必要ああります。
 */

#include "pico/async_context.h"
#include "pico/sem.h"
#include "pico/mutex.h"
#include "hardware/irq.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ASYNC_CONTEXT_THREADSAFE_BACKGROUND_MULTI_CORE
#define ASYNC_CONTEXT_THREADSAFE_BACKGROUND_MULTI_CORE LIB_PICO_MULTICORE
#endif

typedef struct async_context_threadsafe_background async_context_threadsafe_background_t;

/**
 * \brief async_context_threadsafe_background インスタンスの構成おbジェクト.
 */
typedef struct async_context_threadsafe_background_config {
/**
 * 低優先度IRQの優先度
 */
    uint8_t low_priority_irq_handler_priority;
    /**
     * 使用する特定のアラームプール（デフォルトを使用する場合はNULL）
     *
     * \note このアラームプールはasync_contextと同じコアにある
     * 必要があります。
     *
     * 使用するデフォルトアラームプールは"default alarm pool"が
     * 利用でき、同じコアにある場合は "default alarm pool"です
     * ( \ref alarm_pool_get_default() )。そうでなければ初期化時に
     * 作成されたプライベートalarm_poolインスタンスです。
     */
    alarm_pool_t *custom_alarm_pool;
} async_context_threadsafe_background_config_t;

struct async_context_threadsafe_background {
    async_context_t core;
    alarm_pool_t *alarm_pool; // this must be on the same core as core_num
    absolute_time_t last_set_alarm_time;
    recursive_mutex_t lock_mutex;
    semaphore_t work_needed_sem;
    volatile alarm_id_t alarm_id;
#if ASYNC_CONTEXT_THREADSAFE_BACKGROUND_MULTI_CORE
    volatile alarm_id_t force_alarm_id;
    bool alarm_pool_owned;
#endif
    uint8_t low_priority_irq_num;
    volatile bool alarm_pending;
};

/*!
 * \brief async_context_threadsafe_background インスタンスを指定した構成で初期化する.
 * \ingroup async_context_threadsafe_background
 *
 * この関数が成功した（trueが返った）ら async_context が利用可能と
 * なります。また、async_context_deinit() を呼び出すことで解放する
 * ことができます。
 *
 * \param self 初期化する async_context_threadsafe_background
 * 構造体へのポインタ
 * \param config async_contextの特性を指定した構成オブジェクト
 * \return 初期化が成功したら true, そうでなければ false
 */
bool async_context_threadsafe_background_init(async_context_threadsafe_background_t *self, async_context_threadsafe_background_config_t *config);

/*!
 * \brief \ref async_context_threadsafe_background_init_with_defaults() で
 * 使用するデフォルト構成オブジェクトのコピーを返す.
 * \ingroup async_context_threadsafe_background
 *
 * 呼び出し元は必要な設定だけを変更して \ref async_context_threadsafe_background_init() を
 * 呼び出すことができます。
 * \return デフォルト構成オブジェクト
 */
async_context_threadsafe_background_config_t async_context_threadsafe_background_default_config(void);

/*!
 * \brief async_context_threadsafe_background インスタンスをデフォルト値で初期化する.instance with default values
 * \ingroup async_context_threadsafe_background
 *
 * この関数が成功した（trueが返った）ら async_context が利用可能と
 * なります。また、async_context_deinit() を呼び出すことで解放する
 * ことができます。
 *
 * \param self 初期化する async_context_threadsafe_background
 * 構造体へのポインタ
 * \return 初期化が成功したら true, そうでなければ false
 */
static inline bool async_context_threadsafe_background_init_with_defaults(async_context_threadsafe_background_t *self) {
    async_context_threadsafe_background_config_t config = async_context_threadsafe_background_default_config();
    return async_context_threadsafe_background_init(self, &config);
}

#ifdef __cplusplus
}
#endif

#endif
