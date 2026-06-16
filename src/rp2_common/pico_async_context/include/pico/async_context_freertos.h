/*
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_ASYNC_CONTEXT_FREERTOS_H
#define _PICO_ASYNC_CONTEXT_FREERTOS_H

/** \file pico/async_context.h
 *  \defgroup async_context_freertos async_context_freertos
 *  \ingroup pico_async_context
 *  \brief FreeRTOS向けの非同期コンテキストの実装.
 *
 * async_context_freertos は各FreeRTOSタスクにおける非同期作業を
 * 処理する \ref async_context の実装を提供します。
 */
#include "pico/async_context.h"

// FreeRTOS includes
#include "FreeRTOS.h"
#include "semphr.h"
#include "timers.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ASYNC_CONTEXT_DEFAULT_FREERTOS_TASK_PRIORITY
#define ASYNC_CONTEXT_DEFAULT_FREERTOS_TASK_PRIORITY ( tskIDLE_PRIORITY + 4)
#endif

#ifndef ASYNC_CONTEXT_DEFAULT_FREERTOS_TASK_STACK_SIZE
#define ASYNC_CONTEXT_DEFAULT_FREERTOS_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#endif

typedef struct async_context_freertos async_context_freertos_t;

/**
 * \brief async_context_freertos インスタンスの構成オブジェクト.
 */
typedef struct async_context_freertos_config {
    /**
     * async_context タスクのタスク優先度
     */
    UBaseType_t task_priority;
    /**
     * async_context タスクのスタックサイズ
     */
    configSTACK_DEPTH_TYPE task_stack_size;
    /**
     * タスクに割り当てられたコアのID ( \ref portGET_CORE_ID() 参照) .
     * SMPモードでのみ有効です。
     */
#if configUSE_CORE_AFFINITY && configNUM_CORES > 1
    UBaseType_t task_core_id;
#endif
} async_context_freertos_config_t;

struct async_context_freertos {
    async_context_t core;
    SemaphoreHandle_t lock_mutex;
    SemaphoreHandle_t work_needed_sem;
    TimerHandle_t timer_handle;
    TaskHandle_t task_handle;
    uint8_t nesting;
    volatile bool task_should_exit;
};

/*!
 * \brief async_context_freertos インスタンスを指定した構成で初期化する.
 * \ingroup async_context_freertos
 *
 * この関数が成功すると async_context が利用可能となります。また、
 * async_context_deinit() を呼び出すことで解放することができます。
 *
 * \param self 初期化する async_context_freertos 構造体へのポインタ
 * \param config async_contextの特性を指定した構成オブジェクト
 * \return 初期化が成功したら true, そうでなければ false
 */
bool async_context_freertos_init(async_context_freertos_t *self, async_context_freertos_config_t *config);

/*!
 * \brief \ref async_context_freertos_init_with_defaults() で使用する
 * デフォルト構成オブジェクトのコピーを返す.
 * \ingroup async_context_freertos
 *
 * 呼び出し元は必要な設定だけを変更して \ref async_context_freertos_init() を
 * 呼び出すことができます。
 * \return デフォルト構成オブジェクト
 */
 static inline async_context_freertos_config_t async_context_freertos_default_config(void) {
    async_context_freertos_config_t config = {
            .task_priority = ASYNC_CONTEXT_DEFAULT_FREERTOS_TASK_PRIORITY,
            .task_stack_size = ASYNC_CONTEXT_DEFAULT_FREERTOS_TASK_STACK_SIZE,
#if configUSE_CORE_AFFINITY && configNUM_CORES > 1
            .task_core_id = (UBaseType_t)-1, // none
#endif
    };
    return config;

}

/*!
 * \brief async_context_freertos インスタンスをデフォルト値で初期化する.
 * \ingroup async_context_freertos
 *
 * この関数が成功すると async_context が利用可能となります。また、
 * async_context_deinit() を呼び出すことで解放することができます。
 *
 * \param self 初期化する async_context_freertos 構造体へのポインタ
 * \return 初期化が成功したら true, そうでなければ false
 */
 static inline bool async_context_freertos_init_with_defaults(async_context_freertos_t *self) {
    async_context_freertos_config_t config = async_context_freertos_default_config();
    return async_context_freertos_init(self, &config);
}

#ifdef __cplusplus
}
#endif

#endif
