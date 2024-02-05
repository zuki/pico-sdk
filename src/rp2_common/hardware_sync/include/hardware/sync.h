/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _HARDWARE_SYNC_H
#define _HARDWARE_SYNC_H

#include "pico.h"
#include "hardware/address_mapped.h"
#include "hardware/regs/sio.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \file hardware/sync.h
 *  \defgroup hardware_sync hardware_sync
 *
 * \brief 低水準のハードウェア・スピンロック、バリア、プロセッサイベントaPI
 *
 * スピンロック
 * ----------
 *
 * RP2040 は32個のハードウェア・スピンロックを提供しており、共有の
 * ソフトウェア/ハードウェア資源への相互排他的なアクセスの管理に
 * 使用することができます。
 *
 * 通常、各スピンロック自体は共有リソースです。すなわち、同一の
 * ハードウェア・スピンロックを複数の上位プリミティブが使用することが
 * できます（スピンロックを長期間保持したり、同一コアで他のスピンロックを
 * 同時に保持しない限りです。この場合はデッドロックが発生する可能性があります）。
 * 排他的に所有するハードウェアスピンロックは柔軟性はありませんが、他の
 * ソフトウェアに関係なく個別に使用することができます。ハードウェア・
 * スピンロックは再入可能な形で取得することはできません（すなわち、
 * ハードウェア・スピンロックはスレッドコードやIRQて使用するのは安全では
 * ありません）。ただし、デフォルトのスピンロック関連関数はロックが保持
 * されている間は常に割り込みを禁止します（ \ref spin_lock_blocking など）。
 * IRQハンドラやユーザーコードによる使用が一般的で望ましいからであり。
 * スピンロックは短時間しか保持されないと予想されるからです。
 *
 * SDKは以下のデフォルトのスピンロック割り当てを使用します。そして、スピン
 * ロックが排他的/特別な用途用に予約されているかｒ、あるいは、より一般的な
 * 共有使用に適しているかにより分類されています。
 *
 * 番号 (ID) | 説明
 * :---------: | -----------
 * 0-13        | 現在、SDKと他のライブラリによる排他的使用のために予約されています。これらのスピンロックを使用するとSDKや他のライブラリの機能を破壊するおそれがあります。個別に使用される予約スピンロックには各々独自のPICO_SPINLOCK_IDがあるのでそれらを検索することができます。
 * 14,15       | (\ref PICO_SPINLOCK_ID_OS1 と \ref PICO_SPINLOCK_ID_OS2). 現在、SDKと共存するオペレーティングシステム（または他のシステムレベルのソフトウェア）による排他的使用のために予約されています
 * 16-23       | (\ref PICO_SPINLOCK_ID_STRIPED_FIRST - \ref PICO_SPINLOCK_ID_STRIPED_LAST). この範囲のスピンロックは \ref next_striped_spin_lock_num() によりラウンドロビン方式で割り当てられます。これらのスピンロックは共有されますが、この範囲からの番号の割り当ては _割り当てられた_ スピンロックを使用する2つの高水準ロックプリミティブが実際に同じスピンロックを使用する確率を低下させます。
 * 24-31       | (\ref PICO_SPINLOCK_ID_CLAIM_FREE_FIRST - \ref PICO_SPINLOCK_ID_CLAIM_FREE_LAST). 排他的な使用のために予約されており、実行時に \ref spin_lock_claim_unused() により先着順に割り当てられます。
 */

// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_SYNC, Enable/disable assertions in the HW sync module, type=bool, default=0, group=hardware_sync
#ifndef PARAM_ASSERTIONS_ENABLED_SYNC
#define PARAM_ASSERTIONS_ENABLED_SYNC 0
#endif

/** \brief スピンロック識別子
 * \ingroup hardware_sync
 */
typedef volatile uint32_t spin_lock_t;

// PICO_CONFIG: PICO_SPINLOCK_ID_IRQ, Spinlock ID for IRQ protection, min=0, max=31, default=9, group=hardware_sync
#ifndef PICO_SPINLOCK_ID_IRQ
#define PICO_SPINLOCK_ID_IRQ 9
#endif

// PICO_CONFIG: PICO_SPINLOCK_ID_TIMER, Spinlock ID for Timer protection, min=0, max=31, default=10, group=hardware_sync
#ifndef PICO_SPINLOCK_ID_TIMER
#define PICO_SPINLOCK_ID_TIMER 10
#endif

// PICO_CONFIG: PICO_SPINLOCK_ID_HARDWARE_CLAIM, Spinlock ID for Hardware claim protection, min=0, max=31, default=11, group=hardware_sync
#ifndef PICO_SPINLOCK_ID_HARDWARE_CLAIM
#define PICO_SPINLOCK_ID_HARDWARE_CLAIM 11
#endif

// PICO_CONFIG: PICO_SPINLOCK_ID_RAND, Spinlock ID for Random Number Generator, min=0, max=31, default=12, group=hardware_sync
#ifndef PICO_SPINLOCK_ID_RAND
#define PICO_SPINLOCK_ID_RAND 12
#endif

// PICO_CONFIG: PICO_SPINLOCK_ID_OS1, First Spinlock ID reserved for use by low level OS style software, min=0, max=31, default=14, group=hardware_sync
#ifndef PICO_SPINLOCK_ID_OS1
#define PICO_SPINLOCK_ID_OS1 14
#endif

// PICO_CONFIG: PICO_SPINLOCK_ID_OS2, Second Spinlock ID reserved for use by low level OS style software, min=0, max=31, default=15, group=hardware_sync
#ifndef PICO_SPINLOCK_ID_OS2
#define PICO_SPINLOCK_ID_OS2 15
#endif

// PICO_CONFIG: PICO_SPINLOCK_ID_STRIPED_FIRST, Lowest Spinlock ID in the 'striped' range, min=0, max=31, default=16, group=hardware_sync
#ifndef PICO_SPINLOCK_ID_STRIPED_FIRST
#define PICO_SPINLOCK_ID_STRIPED_FIRST 16
#endif

// PICO_CONFIG: PICO_SPINLOCK_ID_STRIPED_LAST, Highest Spinlock ID in the 'striped' range, min=0, max=31, default=23, group=hardware_sync
#ifndef PICO_SPINLOCK_ID_STRIPED_LAST
#define PICO_SPINLOCK_ID_STRIPED_LAST 23
#endif

// PICO_CONFIG: PICO_SPINLOCK_ID_CLAIM_FREE_FIRST, Lowest Spinlock ID in the 'claim free' range, min=0, max=31, default=24, group=hardware_sync
#ifndef PICO_SPINLOCK_ID_CLAIM_FREE_FIRST
#define PICO_SPINLOCK_ID_CLAIM_FREE_FIRST 24
#endif

#ifdef PICO_SPINLOCK_ID_CLAIM_FREE_END
#warning PICO_SPINLOCK_ID_CLAIM_FREE_END has been renamed to PICO_SPINLOCK_ID_CLAIM_FREE_LAST
#endif

// PICO_CONFIG: PICO_SPINLOCK_ID_CLAIM_FREE_LAST, Highest Spinlock ID in the 'claim free' range, min=0, max=31, default=31, group=hardware_sync
#ifndef PICO_SPINLOCK_ID_CLAIM_FREE_LAST
#define PICO_SPINLOCK_ID_CLAIM_FREE_LAST 31
#endif

/*! \brief コードパスにSEV命令を挿入する.
 *  \ingroup hardware_sync

 * SEV (send event) 命令はイベントを両コアに送信します。
 */
#if !__has_builtin(__sev)
__force_inline static void __sev(void) {
    pico_default_asm_volatile ("sev");
}
#endif

/*! \brief コードパスにWFE命令を挿入する.
 *  \ingroup hardware_sync
 *
 * WFE (wait for event) 命令は、どちらかのコアでSEV命令により通知された
 * イベントを含む、多くのイベントの内の1つが発生するまで待機します。
 */
#if !__has_builtin(__wfe)
__force_inline static void __wfe(void) {
    pico_default_asm_volatile ("wfe");
}
#endif

/*! \brief コードパスにWFI命令を挿入する.
  *  \ingroup hardware_sync
*
 * WFI (wait for interrupt) 命令はコアを起床っさせる割り込みが発生するまで待機します。
 */
#if !__has_builtin(__wfi)
__force_inline static void __wfi(void) {
    pico_default_asm_volatile("wfi");
}
#endif

/*! \brief コードパスにDMB命令を挿入する.
 *  \ingroup hardware_sync
 *
 * DMB (data memory barrier) はメモリバリアとしてとして機能し、この命令
 * 以前のすべてのメモリアクセスは命令後の明示的なアクセスの前に観測されます。
 */
__force_inline static void __dmb(void) {
    pico_default_asm_volatile("dmb" : : : "memory");
}

/*! \brief コードパスにDSB命令を挿入する.
 *  \ingroup hardware_sync
 *
 * DSB (data synchronization barrier) は特殊なメモリバリア（DMB）として
 * 機能します。DSB動作はこの命令より前のすべての明示的なメモリアクセスが
 * 完了したときに完了します。
 */
__force_inline static void __dsb(void) {
    pico_default_asm_volatile("dsb" : : : "memory");
}

/*! \brief コードパスにISB命令を挿入する.
 *  \ingroup hardware_sync
 *
 * ISBは命令同期バリアとして機能します。ISBはプロセッサのパイプラインを
 * フラッシュするため、ISBに続くすべての命令はISB命令が完了した後に
 * キャッシュまたはメモリから再びフェッチされます。
 */
__force_inline static void __isb(void) {
    pico_default_asm_volatile("isb" ::: "memory");
}

/*! \brief メモリフェンスを取得する
 *  \ingroup hardware_sync
 */
__force_inline static void __mem_fence_acquire(void) {
    // the original code below makes it hard for us to be included from C++ via a header
    // which itself is in an extern "C", so just use __dmb instead, which is what
    // is required on Cortex M0+
    __dmb();
//#ifndef __cplusplus
//    atomic_thread_fence(memory_order_acquire);
//#else
//    std::atomic_thread_fence(std::memory_order_acquire);
//#endif
}

/*! \brief メモリフェンスを解除する
 *  \ingroup hardware_sync
 *
 */
__force_inline static void __mem_fence_release(void) {
    // the original code below makes it hard for us to be included from C++ via a header
    // which itself is in an extern "C", so just use __dmb instead, which is what
    // is required on Cortex M0+
    __dmb();
//#ifndef __cplusplus
//    atomic_thread_fence(memory_order_release);
//#else
//    std::atomic_thread_fence(std::memory_order_release);
//#endif
}

/*! \brief Save and disable interrupts
 *  \ingroup hardware_sync
 *
 * \return The prior interrupt enable status for restoration later via restore_interrupts()
 */
__force_inline static uint32_t save_and_disable_interrupts(void) {
    uint32_t status;
    pico_default_asm_volatile(
            "mrs %0, PRIMASK\n"
            "cpsid i"
            : "=r" (status) ::);
    return status;
}

/*! \brief Restore interrupts to a specified state
 *  \ingroup hardware_sync
 *
 * \param status Previous interrupt status from save_and_disable_interrupts()
  */
__force_inline static void restore_interrupts(uint32_t status) {
    pico_default_asm_volatile("msr PRIMASK,%0"::"r" (status) : );
}

/*! \brief Get HW Spinlock instance from number
 *  \ingroup hardware_sync
 *
 * \param lock_num Spinlock ID
 * \return The spinlock instance
 */
__force_inline static spin_lock_t *spin_lock_instance(uint lock_num) {
    invalid_params_if(SYNC, lock_num >= NUM_SPIN_LOCKS);
    return (spin_lock_t *) (SIO_BASE + SIO_SPINLOCK0_OFFSET + lock_num * 4);
}

/*! \brief Get HW Spinlock number from instance
 *  \ingroup hardware_sync
 *
 * \param lock The Spinlock instance
 * \return The Spinlock ID
 */
__force_inline static uint spin_lock_get_num(spin_lock_t *lock) {
    invalid_params_if(SYNC, (uint) lock < SIO_BASE + SIO_SPINLOCK0_OFFSET ||
                            (uint) lock >= NUM_SPIN_LOCKS * sizeof(spin_lock_t) + SIO_BASE + SIO_SPINLOCK0_OFFSET ||
                            ((uint) lock - SIO_BASE + SIO_SPINLOCK0_OFFSET) % sizeof(spin_lock_t) != 0);
    return (uint) (lock - (spin_lock_t *) (SIO_BASE + SIO_SPINLOCK0_OFFSET));
}

/*! \brief Acquire a spin lock without disabling interrupts (hence unsafe)
 *  \ingroup hardware_sync
 *
 * \param lock Spinlock instance
 */
__force_inline static void spin_lock_unsafe_blocking(spin_lock_t *lock) {
    // Note we don't do a wfe or anything, because by convention these spin_locks are VERY SHORT LIVED and NEVER BLOCK and run
    // with INTERRUPTS disabled (to ensure that)... therefore nothing on our core could be blocking us, so we just need to wait on another core
    // anyway which should be finished soon
    while (__builtin_expect(!*lock, 0));
    __mem_fence_acquire();
}

/*! \brief Release a spin lock without re-enabling interrupts
 *  \ingroup hardware_sync
 *
 * \param lock Spinlock instance
 */
__force_inline static void spin_unlock_unsafe(spin_lock_t *lock) {
    __mem_fence_release();
    *lock = 0;
}

/*! \brief Acquire a spin lock safely
 *  \ingroup hardware_sync
 *
 * This function will disable interrupts prior to acquiring the spinlock
 *
 * \param lock Spinlock instance
 * \return interrupt status to be used when unlocking, to restore to original state
 */
__force_inline static uint32_t spin_lock_blocking(spin_lock_t *lock) {
    uint32_t save = save_and_disable_interrupts();
    spin_lock_unsafe_blocking(lock);
    return save;
}

/*! \brief Check to see if a spinlock is currently acquired elsewhere.
 *  \ingroup hardware_sync
 *
 * \param lock Spinlock instance
 */
inline static bool is_spin_locked(spin_lock_t *lock) {
    check_hw_size(spin_lock_t, 4);
    uint lock_num = spin_lock_get_num(lock);
    return 0 != (*(io_ro_32 *) (SIO_BASE + SIO_SPINLOCK_ST_OFFSET) & (1u << lock_num));
}

/*! \brief Release a spin lock safely
 *  \ingroup hardware_sync
 *
 * This function will re-enable interrupts according to the parameters.
 *
 * \param lock Spinlock instance
 * \param saved_irq Return value from the \ref spin_lock_blocking() function.
 *
 * \sa spin_lock_blocking()
 */
__force_inline static void spin_unlock(spin_lock_t *lock, uint32_t saved_irq) {
    spin_unlock_unsafe(lock);
    restore_interrupts(saved_irq);
}

/*! \brief Initialise a spin lock
 *  \ingroup hardware_sync
 *
 * The spin lock is initially unlocked
 *
 * \param lock_num The spin lock number
 * \return The spin lock instance
 */
spin_lock_t *spin_lock_init(uint lock_num);

/*! \brief Release all spin locks
 *  \ingroup hardware_sync
 */
void spin_locks_reset(void);

/*! \brief Return a spin lock number from the _striped_ range
 *  \ingroup hardware_sync
 *
 * Returns a spin lock number in the range PICO_SPINLOCK_ID_STRIPED_FIRST to PICO_SPINLOCK_ID_STRIPED_LAST
 * in a round robin fashion. This does not grant the caller exclusive access to the spin lock, so the caller
 * must:
 *
 * -# Abide (with other callers) by the contract of only holding this spin lock briefly (and with IRQs disabled - the default via \ref spin_lock_blocking()),
 * and not whilst holding other spin locks.
 * -# Be OK with any contention caused by the - brief due to the above requirement - contention with other possible users of the spin lock.
 *
 * \return lock_num a spin lock number the caller may use (non exclusively)
 * \see PICO_SPINLOCK_ID_STRIPED_FIRST
 * \see PICO_SPINLOCK_ID_STRIPED_LAST
 */
uint next_striped_spin_lock_num(void);

/*! \brief Mark a spin lock as used
 *  \ingroup hardware_sync
 *
 * Method for cooperative claiming of hardware. Will cause a panic if the spin lock
 * is already claimed. Use of this method by libraries detects accidental
 * configurations that would fail in unpredictable ways.
 *
 * \param lock_num the spin lock number
 */
void spin_lock_claim(uint lock_num);

/*! \brief Mark multiple spin locks as used
 *  \ingroup hardware_sync
 *
 * Method for cooperative claiming of hardware. Will cause a panic if any of the spin locks
 * are already claimed. Use of this method by libraries detects accidental
 * configurations that would fail in unpredictable ways.
 *
 * \param lock_num_mask Bitfield of all required spin locks to claim (bit 0 == spin lock 0, bit 1 == spin lock 1 etc)
 */
void spin_lock_claim_mask(uint32_t lock_num_mask);

/*! \brief Mark a spin lock as no longer used
 *  \ingroup hardware_sync
 *
 * Method for cooperative claiming of hardware.
 *
 * \param lock_num the spin lock number to release
 */
void spin_lock_unclaim(uint lock_num);

/*! \brief Claim a free spin lock
 *  \ingroup hardware_sync
 *
 * \param required if true the function will panic if none are available
 * \return the spin lock number or -1 if required was false, and none were free
 */
int spin_lock_claim_unused(bool required);

/*! \brief Determine if a spin lock is claimed
 *  \ingroup hardware_sync
 *
 * \param lock_num the spin lock number
 * \return true if claimed, false otherwise
 * \see spin_lock_claim
 * \see spin_lock_claim_mask
 */
bool spin_lock_is_claimed(uint lock_num);

// no longer use __mem_fence_acquire here, as it is overkill on cortex M0+
#define remove_volatile_cast(t, x) ({__compiler_memory_barrier(); Clang_Pragma("clang diagnostic push"); Clang_Pragma("clang diagnostic ignored \"-Wcast-qual\""); (t)(x); Clang_Pragma("clang diagnostic pop"); })

#ifdef __cplusplus
}
#endif

#endif
