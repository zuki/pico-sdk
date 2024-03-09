/*
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/** \file pico/async_context.h
 *  \defgroup pico_async_context pico_async_context
 *
 * \ref async_context は論理的にはシングルスレッドのコンテキストを提供し、
 * workの実行や非同期イベントへの応答を行います。したって、 async_context の
 * インスタンスは、リエントラントでないサードパーティライブラリを扱うのに適しています。
 *
 * async_contextの"context"とは、async_context内でワーカーやタイムアウトを呼び出す際に
 * さまざまな事前条件が以下の事実を持っていることを意味します。
 *
 * <ol>
 * <li>論理的な実行スレッドはただ一つだけ存在する。すなわち、コンテキストは
 * ワーカー関数を同時には呼び出しません。</li>
 * <li>コンテキストは常に同じプロセッサコアからワーカーを呼び出す。なぜなら、
 * async_contextのほとんどの用途においてコア固有であるIRQとの相互作用に依存しているからです。</li>
 * </ol>
 *
 * async_contextは非同期workのために次の2つのメカニズムを提供します。
 *
 * * <em>when_pending</em> ワーカー。保留中のworkがあるたびに処理されます。
 * \ref async_context_add_when_pending_worker, \ref async_context_remove_when_pending_worker,
 * \ref async_context_set_work_pending を参照してください。最後の関数は、サービスするworkが
 * 通常の async_context でワーカーにより実行される必要があることを通知するために、割り込み
 * ハンドラから使用することができます。
 * * <em>at_time</em> ワーカー。特定の時刻のあとに実行されます。
 *
 * 注: 保留中のworkがある"when pending"ワーカーは"at time"ワーカーより先に実行されます。
 *
 * async_contextはロック機構を提供します。 \ref async_context_acquire_lock_blocking,
 * \ref async_context_release_lock, \ref async_context_lock_check を参照してください。
 * これらは外部コードの実行がワーカーコードとは同時に行われないことを保証するために
 * 外部コードにより使用することができます。ロックされたコードは呼び出したコアで実行されますが、
 * async_context のコアから同期的に関数を実行するために \ref async_ref async_context_execute_sync が
 * 提供されています。
 *
 * SDKにはデフォルトで以下のasync_contextが存在します。
 *
 * async_context_poll - このコンテキストはスレッドセーフではありません。利用者は定期的に
 * \ref async_context_poll() を呼び出す必要があります。なお、 \ref async_context_wait_for_work_until()
 * を使用することにより、他にすることがなければworkが必要になるまで呼び出しの間スリープさせる
 * ことができます。
 *
 * async_context_threadsafe_background - バックグラウンドで処理させるために優先度の低いIRQを使って
 * コールバックを処理します。通常、コードはこのIRQコンテキストから呼び出されますが、同じコア上で
 * 別の（非IRQ）コンテキストのasyncコンテキストを使用する他のコードの後に呼び出されることがあります。
 * \ref async_context_poll() を呼び出す必要はありません。この関数はno-opです。このコンテキストは
 * async_contextロックを実装しているため、各API固有の注意に従ってどちらのコアからも安全に呼び出す
 * ことができますす。
 *
 * async_context_freertos - 作業は独立した”async_context"タスクで実行されます。ただし、ここでも
 * async_contextが属する同じコアでasync_contextを直接使用しているを～区の後にコードが呼び出される
 * こともあります。 \ref async_context_poll()を呼び出す必要はありません。この関数はno-opです。
 * このコンテキストはasync_contextロックを実装しているため、各API固有の注意に従って任意のタスクから、
 * また、どちらのコアからも安全に呼び出すことができますす。
 *
 * 各async_contextは対応するヘッダー（たとえば、async_context_poll.h、
 * async_context_threadsafe_background.h、asyncn_context_freertos.h など）で提供されている
 * 専用のインスタンス化メソッドを提供しています。async_contextは共通の async_context_deint()
 * 関数により開放されます。
 *
 * 1つのアプリケーションで複数のasync_contextインスタンスを使用することができ、それらは独立して
 * 動作します。
 */

#ifndef _PICO_ASYNC_CONTEXT_H
#define _PICO_ASYNC_CONTEXT_H

#include "pico.h"
#include "pico/time.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    ASYNC_CONTEXT_POLL = 1,
    ASYNC_CONTEXT_THREADSAFE_BACKGROUND = 2,
    ASYNC_CONTEXT_FREERTOS = 3,
};

typedef struct async_context async_context_t;

/*! \brief async_contextで使用する"timeout"インスタンス
 *  \ingroup pico_async_context
 *
 *  "timeout"は特定の時刻に処理される何らかの将来のアクションを表します。
 *  その関数は指定された時刻にロック下でasync_contextから呼び出されます。
 *
 * \see async_context_add_worker_at
 * \see async_context_add_worker_in_ms
 */
typedef struct async_work_on_timeout {
    /*!
     * プライベートなリンク知るとポインタ
     */
    struct async_work_on_timeout *next;
    /*!
     * タイムアウトに達した際に呼び出される関数; NULLであってはいけない
     *
     * 注: この関数が呼び出された際, このtimeoutはasync_contextから削除されています。
     * したがって、再度タイムアウトが必要な場合はこのコールバックの中で再度timeoutを
     * 追加する必要があります。
     * @param context
     * @param timeout
     */
    void (*do_work)(async_context_t *context, struct async_work_on_timeout *timeout);
    /*!
     * 次のタイムアウト時刻; これの変更は上の関数内、またはasync_context関数経由でのみ
     * 行わなければいけません。
     */
    absolute_time_t next_time;
    /*!
     * timeoutインスタンスに関連するユーザデータ
     */
    void *user_data;
} async_at_time_worker_t;

/*! \brief async_contextが使用する"worker"インスタンス
 *  \ingroup pico_async_context
 *
 *  "worker"は何らかの外部状況（通常はIRQ）に反応するために処理しなければならない
 *  何らかの外部実体を表します。その関数は指定された時刻にロック下でasync_contextか
 *  ら呼び出されます。
 *
 * \see async_context_add_worker_at
 * \see async_context_add_worker_in_ms
 */
typedef struct async_when_pending_worker {
    /*!
     * プライベートなリンク知るとポインタ
     */
    struct async_when_pending_worker *next;
    /*!
     * ワーカーが"work pending"とマーク付されている際にasync_contextにより呼び出されます。
     *
     * @param context the async_context
     * @param worker the function to be called when work is pending
     */
    void (*do_work)(async_context_t *context, struct async_when_pending_worker *worker);
    /**
     * ワーカがdo_workを呼び出す必要がある場合はTrue
     */
    bool work_pending;
    /*!
     * このワーカーインスタンスに関連するユーザデータ
     */
    void *user_data;
} async_when_pending_worker_t;

#define ASYNC_CONTEXT_FLAG_CALLBACK_FROM_NON_IRQ 0x1
#define ASYNC_CONTEXT_FLAG_CALLBACK_FROM_IRQ 0x2
#define ASYNC_CONTEXT_FLAG_POLLED 0x4

/*!
 * \brief Implementation of an async_contextタイプの実装, そのタイプに共通の関数を提供する
 * \ingroup pico_async_context
 */
typedef struct async_context_type {
    uint16_t type;
    // see wrapper functions for documentation
    void (*acquire_lock_blocking)(async_context_t *self);
    void (*release_lock)(async_context_t *self);
    void (*lock_check)(async_context_t *self);
    uint32_t (*execute_sync)(async_context_t *context, uint32_t (*func)(void *param), void *param);
    bool (*add_at_time_worker)(async_context_t *self, async_at_time_worker_t *worker);
    bool (*remove_at_time_worker)(async_context_t *self, async_at_time_worker_t *worker);
    bool (*add_when_pending_worker)(async_context_t *self, async_when_pending_worker_t *worker);
    bool (*remove_when_pending_worker)(async_context_t *self, async_when_pending_worker_t *worker);
    void (*set_work_pending)(async_context_t *self, async_when_pending_worker_t *worker);
    void (*poll)(async_context_t *self); // may be NULL
    void (*wait_until)(async_context_t *self, absolute_time_t until);
    void (*wait_for_work_until)(async_context_t *self, absolute_time_t until);
    void (*deinit)(async_context_t *self);
} async_context_type_t;

/*!
 * \brief すべてのasync_contextのベースとなる構造体型. 使用法の詳細については
 *  \ref pico_async_context を参照。
 * \ingroup pico_async_context
 *
 * 追加の状態を持つ個々のasync_context_typesは最初にこの構造体を含む必要があります。
 */
struct async_context {
    const async_context_type_t *type;
    async_when_pending_worker_t *when_pending_list;
    async_at_time_worker_t *at_time_list;
    absolute_time_t next_time;
    uint16_t flags;
    uint8_t  core_num;
};

/*!
 * \brief async_contextロックを取得する
 * \ingroup pico_async_context
 *
 * async_contextロックの所有者はそのasync_contextのロジックの所有者であり、
 * このasync_contextに関連する他のworkは同時には実行されません。
 *
 * この関数はロック所有者によってネストされた形で呼び出すことができます。
 *
 * \note async_contextロックは同じcallerによりネスト可能なので、内部カウントが維持されます。
 *
 * \note （async_context_pollではなく）ロックを提供するasync_contextでは、この関数は
 * スレッドセーフであり、async_contextより、または、他の非IRQコンテキストから呼び出された
 * 任意のワーカーないから呼び出すことができます。
 *
 * \param context the async_context
 *
 * \see async_context_release_lock
 */
static inline void async_context_acquire_lock_blocking(async_context_t *context) {
    context->type->acquire_lock_blocking(context);
}

/*!
 * \brief async_contextロックを解放する
 * \ingroup pico_async_context
 *
 * \note async_contextロックはネストして呼び出すことができるため、内部カウントが維持されます。
 * 最も外側のロックの場合、最も外側のロックが解放されると、ロックが保持されている間にスキップ
 * された可能性のあるworkがチェックされ、async_contextが属するものと同じコアからの呼び出しで
 * ある場合、そのようなworkはこの呼び出し中に実行される場合があります。
 *
 * \note for async_contexts that provide locking (not async_context_poll), this method is threadsafe. and may be called from within any
 * worker method called by the async_context or from any other non-IRQ context.
 *
 * \param context the async_context
 *
 * \see async_context_acquire_lock_blocking
 */
static inline void async_context_release_lock(async_context_t *context) {
    context->type->release_lock(context);
}

/*!
 * \brief Assert if the caller does not own the lock for the async_context
 * \ingroup pico_async_context
 * \note this method is thread-safe
 *
 * \param context the async_context
 */
static inline void async_context_lock_check(async_context_t *context) {
    context->type->lock_check(context);
}

/*!
 * \brief Execute work synchronously on the core the async_context belongs to.
 * \ingroup pico_async_context
 *
 * This method is intended for code external to the async_context (e.g. another thread/task) to
 * execute a function with the same guarantees (single core, logical thread of execution) that
 * async_context workers are called with.
 *
 * \note you should NOT call this method while holding the async_context's lock
 *
 * \param context the async_context
 * \param func the function to call
 * \param param the paramter to pass to the function
 * \return the return value from func
 */
static inline uint32_t async_context_execute_sync(async_context_t *context, uint32_t (*func)(void *param), void *param) {
    return context->type->execute_sync(context, func, param);
}

/*!
 * \brief Add an "at time" worker to a context
 * \ingroup pico_async_context
 *
 * An "at time" worker will run at or after a specific point in time, and is automatically when (just before) it runs.
 *
 * The time to fire is specified in the next_time field of the worker.
 *
 * \note for async_contexts that provide locking (not async_context_poll), this method is threadsafe. and may be called from within any
 * worker method called by the async_context or from any other non-IRQ context.
 *
 * \param context the async_context
 * \param worker the "at time" worker to add
 * \return true if the worker was added, false if the worker was already present.
 */
static inline bool async_context_add_at_time_worker(async_context_t *context, async_at_time_worker_t *worker) {
    return context->type->add_at_time_worker(context, worker);
}

/*!
 * \brief Add an "at time" worker to a context
 * \ingroup pico_async_context
 *
 * An "at time" worker will run at or after a specific point in time, and is automatically when (just before) it runs.
 *
 * The time to fire is specified by the at parameter.
 *
 * \note for async_contexts that provide locking (not async_context_poll), this method is threadsafe. and may be called from within any
 * worker method called by the async_context or from any other non-IRQ context.
 *
 * \param context the async_context
 * \param worker the "at time" worker to add
 * \param at the time to fire at
 * \return true if the worker was added, false if the worker was already present.
 */
static inline bool async_context_add_at_time_worker_at(async_context_t *context, async_at_time_worker_t *worker, absolute_time_t at) {
    worker->next_time = at;
    return context->type->add_at_time_worker(context, worker);
}

/*!
 * \brief Add an "at time" worker to a context
 * \ingroup pico_async_context
 *
 * An "at time" worker will run at or after a specific point in time, and is automatically when (just before) it runs.
 *
 * The time to fire is specified by a delay via the ms parameter
 *
 * \note for async_contexts that provide locking (not async_context_poll), this method is threadsafe. and may be called from within any
 * worker method called by the async_context or from any other non-IRQ context.
 *
 * \param context the async_context
 * \param worker the "at time" worker to add
 * \param ms the number of milliseconds from now to fire after
 * \return true if the worker was added, false if the worker was already present.
 */
static inline bool async_context_add_at_time_worker_in_ms(async_context_t *context, async_at_time_worker_t *worker, uint32_t ms) {
    worker->next_time = make_timeout_time_ms(ms);
    return context->type->add_at_time_worker(context, worker);
}

/*!
 * \brief Remove an "at time" worker from a context
 * \ingroup pico_async_context
 *
 * \note for async_contexts that provide locking (not async_context_poll), this method is threadsafe. and may be called from within any
 * worker method called by the async_context or from any other non-IRQ context.
 *
 * \param context the async_context
 * \param worker the "at time" worker to remove
 * \return true if the worker was removed, false if the instance not present.
 */
static inline bool async_context_remove_at_time_worker(async_context_t *context, async_at_time_worker_t *worker) {
    return context->type->remove_at_time_worker(context, worker);
}

/*!
 * \brief コンテキストに"when pending"ワーカーを追加する
 * \ingroup pico_async_context
 *
 * "when pending"ワーカーは保留中に実行され（ \ref async_context_set_work_pending で設定できます）、
 * 実行されても自動的には削除されません。
 *
 * 発火時間はmsパラメータ経由でdelayにより指定されます。
 *
 * \note （async_context_pollではない) ロックを提供するasync_context の場合、
 * この関数はスレッドセーフです。async_contextから呼び出された任意のワーカー関数や
 * その他の非IRQコンテキストから呼び出すことができます。
 *
 * \param context async_context
 * \param worker 追加する "when pending" ワーカー
 * \return ワーカーが追加されたら true, ワーカーがすでに存在したら false.
 */
static inline bool async_context_add_when_pending_worker(async_context_t *context, async_when_pending_worker_t *worker) {
    return context->type->add_when_pending_worker(context, worker);
}

/*!
 * \brief Remove a "when pending" worker from a context
 * \ingroup pico_async_context
 *
 * \note for async_contexts that provide locking (not async_context_poll), this method is threadsafe. and may be called from within any
 * worker method called by the async_context or from any other non-IRQ context.
 *
 * \param context the async_context
 * \param worker the "when pending" worker to remove
 * \return true if the worker was removed, false if the instance not present.
 */
static inline bool async_context_remove_when_pending_worker(async_context_t *context, async_when_pending_worker_t *worker) {
    return context->type->remove_when_pending_worker(context, worker);
}

/*!
 * \brief Mark a "when pending" worker as having work pending
 * \ingroup pico_async_context
 *
 * The worker will be run from the async_context at a later time.
 *
 * \note this method may be called from any context including IRQs
 *
 * \param context the async_context
 * \param worker the "when pending" worker to mark as pending.
 */
static inline void async_context_set_work_pending(async_context_t *context, async_when_pending_worker_t *worker) {
    context->type->set_work_pending(context, worker);
}

/*!
 * \brief Perform any pending work for polling style async_context
 * \ingroup pico_async_context
 *
 * For a polled async_context (e.g. \ref async_context_poll) the user is responsible for calling this method
 * periodically to perform any required work.
 *
 * This method may immediately perform outstanding work on other context types, but is not required to.
 *
 * \param context the async_context
 */
static inline void async_context_poll(async_context_t *context) {
    if (context->type->poll) context->type->poll(context);
}

/*!
 * \brief sleep until the specified time in an async_context callback safe way
 * \ingroup pico_async_context
 *
 * \note for async_contexts that provide locking (not async_context_poll), this method is threadsafe. and may be called from within any
 * worker method called by the async_context or from any other non-IRQ context.
 *
 * \param context the async_context
 * \param until the time to sleep until
 */
static inline void async_context_wait_until(async_context_t *context, absolute_time_t until) {
    context->type->wait_until(context, until);
}

/*!
 * \brief Block until work needs to be done or the specified time has been reached
 * \ingroup pico_async_context
 *
 * \note this method should not be called from a worker callback
 *
 * \param context the async_context
 * \param until the time to return at if no work is required
 */
static inline void async_context_wait_for_work_until(async_context_t *context, absolute_time_t until) {
    context->type->wait_for_work_until(context, until);
}

/*!
 * \brief Block until work needs to be done or the specified number of milliseconds have passed
 * \ingroup pico_async_context
 *
 * \note this method should not be called from a worker callback
 *
 * \param context the async_context
 * \param ms the number of milliseconds to return after if no work is required
 */
static inline void async_context_wait_for_work_ms(async_context_t *context, uint32_t ms) {
    async_context_wait_for_work_until(context, make_timeout_time_ms(ms));
}

/*!
 * \brief Return the processor core this async_context belongs to
 * \ingroup pico_async_context
 *
 * \param context the async_context
 * \return the physical core number
 */
static inline uint async_context_core_num(const async_context_t *context) {
    return context->core_num;
}

/*!
 * \brief End async_context processing, and free any resources
 * \ingroup pico_async_context
 *
 * Note the user should clean up any resources associated with workers
 * in the async_context themselves.
 *
 * Asynchronous (non-polled) async_contexts guarantee that no
 * callback is being called once this method returns.
 *
 * \param context the async_context
 */
static inline void async_context_deinit(async_context_t *context) {
    context->type->deinit(context);
}

#ifdef __cplusplus
}
#endif

#endif
