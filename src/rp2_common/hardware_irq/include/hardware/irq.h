/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _HARDWARE_IRQ_H
#define _HARDWARE_IRQ_H

// These two config items are also used by assembler, so keeping separate
// PICO_CONFIG: PICO_MAX_SHARED_IRQ_HANDLERS, Maximum number of shared IRQ handlers, default=4, advanced=true, group=hardware_irq
#ifndef PICO_MAX_SHARED_IRQ_HANDLERS
#define PICO_MAX_SHARED_IRQ_HANDLERS 4
#endif

// PICO_CONFIG: PICO_DISABLE_SHARED_IRQ_HANDLERS, Disable shared IRQ handlers, type=bool, default=0, group=hardware_irq
#ifndef PICO_DISABLE_SHARED_IRQ_HANDLERS
#define PICO_DISABLE_SHARED_IRQ_HANDLERS 0
#endif

// PICO_CONFIG: PICO_VTABLE_PER_CORE, user is using separate vector tables per core, type=bool, default=0, group=hardware_irq
#ifndef PICO_VTABLE_PER_CORE
#define PICO_VTABLE_PER_CORE 0
#endif

#ifndef __ASSEMBLER__

#include "pico.h"
#include "hardware/address_mapped.h"
#include "hardware/regs/intctrl.h"
#include "hardware/regs/m0plus.h"

/** \file irq.h
 *  \defgroup hardware_irq hardware_irq
 *
 * \brief ハードウェア割り込み処理
 *
 * RP2040は標準的なARM NVIC (Nested Vectored Interrupt Controller) を
 * 使用しています。
 *
 * 割り込みは0から31までの番号で識別されます。
 *
 * RP2040では下位26までのIRQ信号だけがNVICに接続されており、IRQ 26から31は
 * ゼロに束縛されています（発火することはありません）。
 *
 * NVICはコアごとに1つあり、各コアのNVICには同じハードウェア割り込み線が
 * ルーティングされていますが、IO割り込みは例外であり、コアごと、バンクごとに
 * 1つIO割り込みがあります。これらは完全に独立しているため、たとえば、
 * プロセッサ 0はバンク 0のGPIO 0からの割り込みを受け、プロセッサ 1は同じ
 * バンクのGPIO 1からの割り込みを受け付けることができます。
 *
 * \note すべてのIRQ APIは実行中のコア（すなわち、関数を呼び出しているコア）
 * だけに影響します。
 *
 * 両方のコアで同じ（共有の）IRQ番号を有効にしてはいけません。なぜなら、
 * 競合条件やコアの1つでのスターベーションにつながるからです。さらに、ある
 * コアで割り込みを無効にしても他のコアでの割り込みは無効にならないことを
 * 忘れないでください。
 *
 * IRQにハンドラを設定する方法は3つあります。
 * - 実行時に irq_add_shared_handler() を呼び出して、現在のコアの多重化
 *   割り込み（GPIOバンクなど）のハンドラを追加する。各ハンドラは関連する
 *   ハードウェア割り込みソースをチェックし、クリアする必要があります。
 * - 実行時に irq_set_exclusive_handler() を呼び出して、現在のコアに割り込み
 *   ハンドラを1つインストールする。
 * - アプリケーションで割り込みハンドラを明示的に定義する（たとえば、
 *   void `isr_dma_0` を定義するとその関数はコア 0のDMA_IRQ_0のハンドラとなり、
 *   実行時に上記のAPIを使用して変更することはできません）。この関数を
 *   使用すると実行時にリンクの競合が発生する可能性があり、実行時の性能には
 *   何のメリットもありません（つまり、一般には使用すべきではありません）。
 *
 * \note IRQが有効化され、ハンドラがインストールされていない状態で発火すると、
 * ブレークポイントがヒットし、IRQ番号がレジスタr0に設定されます。
 *
 * \section interrupt_nums 割り込み番号
 *
 * 割り込み番号は以下の通りに振られています。これらの番号を直接使わないように
 * するためにその名前を持つ一連の定義 (intctrl.h) が用意されています。
 *
 * IRQ | 割り込みソース
 * ----|-----------------
 *  0 | TIMER_IRQ_0
 *  1 | TIMER_IRQ_1
 *  2 | TIMER_IRQ_2
 *  3 | TIMER_IRQ_3
 *  4 | PWM_IRQ_WRAP
 *  5 | USBCTRL_IRQ
 *  6 | XIP_IRQ
 *  7 | PIO0_IRQ_0
 *  8 | PIO0_IRQ_1
 *  9 | PIO1_IRQ_0
 * 10 | PIO1_IRQ_1
 * 11 | DMA_IRQ_0
 * 12 | DMA_IRQ_1
 * 13 | IO_IRQ_BANK0
 * 14 | IO_IRQ_QSPI
 * 15 | SIO_IRQ_PROC0
 * 16 | SIO_IRQ_PROC1
 * 17 | CLOCKS_IRQ
 * 18 | SPI0_IRQ
 * 19 | SPI1_IRQ
 * 20 | UART0_IRQ
 * 21 | UART1_IRQ
 * 22 | ADC0_IRQ_FIFO
 * 23 | I2C0_IRQ
 * 24 | I2C1_IRQ
 * 25 | RTC_IRQ
 *
 */

// PICO_CONFIG: PICO_DEFAULT_IRQ_PRIORITY, Define the default IRQ priority, default=0x80, group=hardware_irq
#ifndef PICO_DEFAULT_IRQ_PRIORITY
#define PICO_DEFAULT_IRQ_PRIORITY 0x80
#endif

#define PICO_LOWEST_IRQ_PRIORITY 0xff
#define PICO_HIGHEST_IRQ_PRIORITY 0x00

// PICO_CONFIG: PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY, Set default shared IRQ order priority, default=0x80, group=hardware_irq
#ifndef PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY
#define PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY 0x80
#endif

#define PICO_SHARED_IRQ_HANDLER_HIGHEST_ORDER_PRIORITY 0xff
#define PICO_SHARED_IRQ_HANDLER_LOWEST_ORDER_PRIORITY 0x00

// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_IRQ, Enable/disable assertions in the IRQ module, type=bool, default=0, group=hardware_irq
#ifndef PARAM_ASSERTIONS_ENABLED_IRQ
#define PARAM_ASSERTIONS_ENABLED_IRQ 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief 割り込みハンドラ関数型
 *  \ingroup hardware_irq
 *
 * 割り込みハンドラはすべてこの型でなければなりません。また、標準的な
 * ARM EABIレジスタ保存規則に従う必要があります。
 */
typedef void (*irq_handler_t)(void);

static inline void check_irq_param(__unused uint num) {
    invalid_params_if(IRQ, num >= NUM_IRQS);
}

/*! \brief 指定した割り込みの優先度をセットする
 *  \ingroup hardware_irq
 *
 * \param num 割り込み番号 \ref interrupt_nums
 * \param hardware_priority セットする優先度.
 * 数値が小さいほど優先度が高いことを示します。ハードウェアの優先度は0
 * （最高優先度）から255（最低優先度）の範囲ですが、ARM Cortex-M0+では
 * 上位2ビットだけが有効です。デフォルトよりも高い優先度や低い優先度を簡単に
 * 指定できるように起動時にSDKランタイムによってすべてのIRQの優先度が
 * PICO_DEFAULT_IRQ_PRIORITYに初期化されます。PICO_DEFAULT_IRQ_PRIORITYの
 * デフォルトは0x80です。
 */
void irq_set_priority(uint num, uint8_t hardware_priority);

/*! \brief 指定の割り込みの優先度を取得する
 *  \ingroup hardware_irq
 *
 * 数値が小さいほど優先度が高いことを示します。ハードウェアの優先度は0
 * （最高優先度）から255（最低優先度）の範囲ですが、ARM Cortex-M0+では
 * 上位2ビットだけが有効です。デフォルトよりも高い優先度や低い優先度を簡単に
 * 指定できるように起動時にSDKランタイムによってすべてのIRQの優先度が
 * PICO_DEFAULT_IRQ_PRIORITYに初期化されます。PICO_DEFAULT_IRQ_PRIORITYの
 * デフォルトは0x80です。
 *
 * \param num 割り込み場号 \ref interrupt_nums
 * \return IRQの優先度
 */
uint irq_get_priority(uint num);

/*! \brief 実行中のコアにおける指定の割り込みを有効/無効にする
 *  \ingroup hardware_irq
 *
 * \param num 割り込み場号 \ref interrupt_nums
 * \param enabled 割り込みを有効にする場合は true, 無効にする場合は false
 */
void irq_set_enabled(uint num, bool enabled);

/*! \brief 指定の割り込みが実行中のコアにおいて有効であるかチェックする
 *  \ingroup hardware_irq
 *
 * \param num 割り込み場号 \ref interrupt_nums
 * \return 割り込みが有効である場合は true
 */
bool irq_is_enabled(uint num);

/*! \brief 実行中のコアにおける複数の割り込みを有効/無効にする
 *  \ingroup hardware_irq
 *
 * \param mask 32ビットのマスク. 各ビットが対象の割り込みの有効/無効を設定する \ref interrupt_nums
 * \param enabled 割り込みを有効にする場合は true, 無効にする場合は false
 */
void irq_set_mask_enabled(uint32_t mask, bool enabled);

/*! \brief 実行中のコアにおける割り込みに対して排他的な割り込みハンドラをセットする
 *  \ingroup hardware_irq
 *
 * この関数は単一のIRQソースの割り込みハンドラを設定する場合、または、あなたの
 * コード、ユースケース、パ フォーマンス要件がその割り込みに対して他の
 * ハンドラが存在しないことを指示する場合に使用してください。
 *
 * この関数は指定したIRQ番号に対して何らかの割り込みハンドラが既に
 * インストールされている場合にアサートします。
 *
 * \param num 割り込み場号 \ref interrupt_nums
 * \param handler セットするハンドラ. \ref irq_handler_t を参照
 * \see irq_add_shared_handler()
 */
void irq_set_exclusive_handler(uint num, irq_handler_t handler);

/*! \brief  実行中のコアにおける指定の割り込みに対する排他的な割り込みハンドラを取得する.
 *  \ingroup hardware_irq
 *
 * この関数は存在すれば irq_set_exclusive_handler によってこのコアに対して
 * セットされた排他的IRQハンドラを返します。
 *
 * \param num 割り込み場号 \ref interrupt_nums
 * \see irq_set_exclusive_handler()
 * \return 指定のIRQに排他的ハンドラがセットされている場合はそのハンドラ,
 * ハンドラがセットされていない、または、共有/共有可能ハンドラが
 * インストールされている場合は NULL
 */
irq_handler_t irq_get_exclusive_handler(uint num);

/*! \brief 実行中のコアにおける割り込みに対して共有の割り込みハンドラをセットする
 *  \ingroup hardware_irq
 *
 * 複数の異なるハードウェアソース（GPIO、DMA、PIO IRQなど）で共有されるIRQ
 * 番号にハンドラを追加するにはこの関数を使用してください。この関数で追加
 * されたハンドラはorder_priority の高いものから低いものへと順番にすべて
 * 呼び出されます。割り込みのハンドラが1つしかない、または1つしかあっては
 * いけないことが分かっている場合はこの関数の代わりに
 * irq_set_exclusive_handler() を使用する必要があります。
 *
 * この関数はこのコアにおける指定のIRQ番号に排他的な割り込みハンドラが設定
 * されている場合、または、（両コアにおけるすべてのIRQを合計した）共有
 * ハンドラの数が最大値（PICO_MAX_SHARED_IRQ_HANDLERS で設定可能）を超える
 * 場合にアサートします。
 *
 * \param num 割り込み場号 \ref interrupt_nums
 * \param handler セットするハンドラ. \ref irq_handler_t を参照
 * \param order_priority order_priorityは、コアのいける同じIRQ番号の
 * ハンドラが呼び出される順番を制御します。割り込みの共有IRQハンドラは
 * IRQが発火するとすべて呼び出されますが、呼び出し順序は order_priority に
 * 基づいています（高い優先度が最初に呼び出されます。同じ優先度の
 * 呼び出し順位は未定義です）。あまり気にしないのであれば優先度の
 * 真ん中であるPICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITYを使用
 * するのが良い経験則です。
 *
 * \note order_priority は高い優先度に \em 大きな 値を使用しますが、
 * これは 高い優先度も小さな値を使用する irq_set_priority()に渡される
 * CPU割り込みの優先度とは \em 真逆 です。
 *
 * \see irq_set_exclusive_handler()
 */
void irq_add_shared_handler(uint num, irq_handler_t handler, uint8_t order_priority);

/*! \brief 実行中のコアにおける指定のIRQ番号の指定の割り込みハンドラを削除する
 *  \ingroup hardware_irq
 *
 * この関数はirq_set_exclusive_handler(), または irq_add_shared_handler() で
 * セットされたIRQハンドラの削除に使うことができます。指定のIRQ番号に現在
 * ハンドラがインストールされていない場合はアサートします。
 *
 * \note この関数はユーザ（IRQコードでない）、または、ハンドラ自身の内部
 * （すなわち、IRQハンドラがIRQの処理の一部として自分を削除する）から
 * *しか* 呼び出すことができません。他のIRQからの呼び出しはアサートを
 * 引き起こします。
 *
 * \param num 割り込み場号 \ref interrupt_nums
 * \param handler 削除するハンドラ.
 * \see irq_set_exclusive_handler()
 * \see irq_add_shared_handler()
 */
void irq_remove_handler(uint num, irq_handler_t handler);

/*! \brief 指定したIRQ番号に現在共有ハンドラがセットされているかチェックする
 *  \ingroup hardware_irq
 *
 * \param num 割り込み場号 \ref interrupt_nums
 * \return 指定したIRQが共有ハンドラを持っている場合は true
 */
bool irq_has_shared_handler(uint num);

/*! \brief 指定したIRQの現在のIRQハンドラを現在実行中のコアにインストールされているハードウェアベクタテーブル（VTOR)から取得する
 *  \ingroup hardware_irq
 *
 * \param num 割り込み場号 \ref interrupt_nums
 * \return 指定したIRQ番号用にVTABLEに格納されているアドレス
 */
irq_handler_t irq_get_vtable_handler(uint num);

/*! \brief 実行中のコアにおける指定の割り込みをクリアする
 *  \ingroup hardware_irq
 *
 * この関数はハードウェアに接続されていない"ソフトウェア"IRQ（すなわち、
 * IRQ 26～31）に対してのみ有効です。なぜなら、NVICは常にハードウェアIRQに
 * 対してハードウェアのIRQ状態の現在の状態を反映しており、ハードウェアの
 * IRQ状態のクリアはハードウェアのレジスタを介して実行されるからです。
 *
 * \param int_num 割り込み場号 \ref interrupt_nums
 */
static inline void irq_clear(uint int_num) {
    *((volatile uint32_t *) (PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET)) = (1u << ((uint32_t) (int_num & 0x1F)));
}

/*! \brief 実行中のコアにおける割り込みを強制的に保留する
 *  \ingroup hardware_irq
 *
 * この関数は一般にハードウェアに接続されているIRQには使用する
 * べきではありません。
 *
 * \param num 割り込み場号 \ref interrupt_nums
 */
void irq_set_pending(uint num);


/*! \brief 現在のコアにおけるIRQ優先度の初期化を行う
 *
 * \note これは内部関数であり、一般にユーザはこの関数を呼び出すべきでは
 * ありません。
 */
void irq_init_priorities(void);

/*! \brief 呼び出したコアにおけるユーザIRQの所有権を主張する
 *  \ingroup hardware_irq
 *
 * ユーザIRQの番号は26-31であり、ハードウェアには接続されていませんが、
 * \ref irq_set_pending によりトリガーすることができます。
 *
 * \note ユーザIRQはコアローカルの機能です。コア間の通信には使用できません。
 * したがって、ユーザIRQを扱うすべての関数は呼び出したコアにしか影響しません。
 *
 * この関数はユーザIRQの所有権を明示的に主張するので、他のコードはそれが
 * 使用されていることを知ることができます。
 *
 * \param irq_num 主張するユーザIRQ
 */
void user_irq_claim(uint irq_num);

/*! \brief ユーザIrqを呼び出しコアでもはや使用していないものとしてマークするMark a user IRQ as no longer used on the calling core *  \ingroup hardware_irq
 *
 * ユーザIRQの番号は26-31であり、ハードウェアには接続されていませんが、
 * \ref irq_set_pending によりトリガーすることができます。
 *
 * \note ユーザIRQはコアローカルの機能です。コア間の通信には使用できません。
 * したがって、ユーザIRQを扱うすべての関数は呼び出したコアにしか影響しません。
 *
 * この関数はユーザIRQの所有権を明示的に解放するので、他のコードはその番号が
 * 開いていることを知ることができます。
 *
 * \note 通常、この関数を呼び出す前に、irqを無効にし、ハンドラを削除して
 * おきます。
 *
 * \param irq_num 解除するirq番号
 */
void user_irq_unclaim(uint irq_num);

/*! \brief 呼び出したコアの空いているユーザIRQの所有権を主張する
 *  \ingroup hardware_irq
 *
 * ユーザIRQの番号は26-31であり、ハードウェアには接続されていませんが、
 * \ref irq_set_pending によりトリガーすることができます。
 *
 * \note ユーザIRQはコアローカルの機能です。コア間の通信には使用できません。
 * したがって、ユーザIRQを扱うすべての関数は呼び出したコアにしか影響しません。
 *
 * この関数は未使用のユーザIRQの所有権を明示的に主張するので、他のコードは
 * それが使用されていることを知ることができます。
 *
 * \param required 利用可能なirqがない場合に関数をパニックさせる場合は true
 * \return ユーザIRQ番号, または、要求が失敗し、空きがない場合は -1
 */
int user_irq_claim_unused(bool required);

/*! \brief ユーザIRQが呼び出したコアで使用されているかチェックする
 *  \ingroup hardware_irq
 *
 * ユーザIRQの番号は26-31であり、ハードウェアには接続されていませんが、
 * \ref irq_set_pending によりトリガーすることができます。
 *
 * \note ユーザIRQはコアローカルの機能です。コア間の通信には使用できません。
 * したがって、ユーザIRQを扱うすべての関数は呼び出したコアにしか影響しません。
 *
 * \param irq_num irq番号
 * \return irq_numが使用中の場合は true, そうでなければ false
 * \sa user_irq_claim
 * \sa user_irq_unclaim
 * \sa user_irq_claim_unused
 */
bool user_irq_is_claimed(uint irq_num);

#ifdef __cplusplus
}
#endif

#endif
#endif
