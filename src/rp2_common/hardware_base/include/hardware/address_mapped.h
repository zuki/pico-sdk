/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _HARDWARE_ADDRESS_MAPPED_H
#define _HARDWARE_ADDRESS_MAPPED_H

#include "pico.h"
#include "hardware/regs/addressmap.h"

/** \file address_mapped.h
 *  \defgroup hardware_base hardware_base
 *  \brief メモリマップドハードウェアレジスタの低水準の型と（アトミック）アクセッサ
 *
 *  `hardware_base`はメモリマップされたハードウェアレジスタの低水準の型とアクセス関数を定義します。
 * これはデフォルトで他のすべてのハードウェアライブラリにインクルードされます。
 *
 *  以下のレジスタのアクセス型定義は、ハードウェアレジスタのアクセス型 (read/write) と
 * バスサイズ (8/16/32) をコード化したものです。
 * レジスタタイプ名はA、B、Cの3つのパートから1ずつ選んで連結して作られています。

 *   A    | B | C | 意味
 *  ------|---|---|--------
 *  io_   |   |   | メモリマップドIOレジスタ
 *  &nbsp;|ro_|   | read-only アクセス
 *  &nbsp;|rw_|   | read-write アクセス
 *  &nbsp;|wo_|   | write-only アクセス（C APIからは実際には矯正できない）
 *  &nbsp;|   |  8| 8-ビット幅アクセス
 *  &nbsp;|   | 16| 16-ビット幅アクセス
 *  &nbsp;|   | 32| 32-ビット幅アクセス
 *
 * これらの型を扱う場合は常にポインタを使うことになります。たとえば、
 * `io_rw_32 *some_reg` は読み書き可能な32ビットのレジスタへのポインタで
 * あり、`*some_reg = value` で書き込み、`value = *some_reg` で読み込みを行うことができます。
 *
 * RP2040ハードウェアではハードウェアレジスタ内のビットサブセットのアトミックな設定、クリア、
 * 反転を提供するためのエイリアスもあり、2つのコアによる同時アクセスは常にまず1つ目のアトミック
 * 操作が実行され、次に2つ目の操作が実行されるという一貫性があります。
 *
 * 32ビットレジスタへのポインタを介したアトミックなアクセスを提供する hw_set_bits(),
 * hw_clear_bits(), hw_xor_bits() を参照してください。
 *
 * さらに、ハードウェア部品を表す構造体へのポインタ（たとえば、DMAコントローラの `dma_hw_t *dma_hw`）が
 * 与えられた場合、その構造体内の任意のメンバ（レジスタ）への書き込みが hw_set_alias()、 hw_clear_alias()、
 * hw_xor_alias() によるアトミック操作と等価になるような構造体全体へのエイリアスを得ることができます。
 *
 * たとえば、`hw_set_alias(dma_hw)->inte1 = 0x80;` は DMAコントローラのINTE1レジスタのビット 7をセットし、
 * 他のビットは変更しません。
 */

#ifdef __cplusplus
extern "C" {
#endif

#define check_hw_layout(type, member, offset) static_assert(offsetof(type, member) == (offset), "hw offset mismatch")
#define check_hw_size(type, size) static_assert(sizeof(type) == (size), "hw size mismatch")

// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_ADDRESS_ALIAS, Enable/disable assertions in memory address aliasing macros, type=bool, default=0, group=hardware_base
#ifndef PARAM_ASSERTIONS_ENABLED_ADDRESS_ALIAS
#define PARAM_ASSERTIONS_ENABLED_ADDRESS_ALIAS 0
#endif

typedef volatile uint32_t io_rw_32;
typedef const volatile uint32_t io_ro_32;
typedef volatile uint32_t io_wo_32;
typedef volatile uint16_t io_rw_16;
typedef const volatile uint16_t io_ro_16;
typedef volatile uint16_t io_wo_16;
typedef volatile uint8_t io_rw_8;
typedef const volatile uint8_t io_ro_8;
typedef volatile uint8_t io_wo_8;

typedef volatile uint8_t *const ioptr;
typedef ioptr const const_ioptr;

// IDEが hardware/structs/xxx.h にある自動生成されたハードウェア構造体ヘッダーから
// hardware/regs/xxx.h にある生のレジスタ定義へのリンクをたどるのを助けるための
// 機能しない（空の）ヘルパーマクロです。TIMER_TIMEHW_OFFSET (a timer register offset) の
// ようなプリプロセッサ定義はCのコメント内に置かれた場合、一般的に（IDEにおいて）クリック
// 可能にならないので、かわりに _REG_(TIMER_TIMEHW_OFFSET)がコメントの外に含まれています。
#define _REG_(x)

// hw_aliasマクロがオプションで入力の妥当性をチェックするために使用するヘルパーメソッド
#define hw_alias_check_addr(addr) ((uintptr_t)(addr))
// 以下の実装は使用できません。hw_aliasを使った既存の静的宣言を壊してしまい、
// 下位互換性がなくなるからです。
//static __force_inline uint32_t hw_alias_check_addr(volatile void *addr) {
//    uint32_t rc = (uintptr_t)addr;
//    invalid_params_if(ADDRESS_ALIAS, rc < 0x40000000); // catch likely non HW pointer types
//    return rc;
//}

// xip_aliasマクロがオプションで入力の妥当性をチェックするために使用するヘルパーメソッド
__force_inline static uint32_t xip_alias_check_addr(const void *addr) {
    uint32_t rc = (uintptr_t)addr;
    valid_params_if(ADDRESS_ALIAS, rc >= XIP_MAIN_BASE && rc < XIP_NOALLOC_BASE);
    return rc;
}

// 型なし変換エイリアスポインタ生成マクロ
#define hw_set_alias_untyped(addr) ((void *)(REG_ALIAS_SET_BITS | hw_alias_check_addr(addr)))
#define hw_clear_alias_untyped(addr) ((void *)(REG_ALIAS_CLR_BITS | hw_alias_check_addr(addr)))
#define hw_xor_alias_untyped(addr) ((void *)(REG_ALIAS_XOR_BITS | hw_alias_check_addr(addr)))
#define xip_noalloc_alias_untyped(addr) ((void *)(XIP_NOALLOC_BASE | xip_alias_check_addr(addr)))
#define xip_nocache_alias_untyped(addr) ((void *)(XIP_NOCACHE_BASE | xip_alias_check_addr(addr)))
#define xip_nocache_noalloc_alias_untyped(addr) ((void *)(XIP_NOCACHE_NOALLOC_BASE | xip_alias_check_addr(addr)))

// 型付き変換エイリアスポインタ生成マクロ
#define hw_set_alias(p) ((typeof(p))hw_set_alias_untyped(p))
#define hw_clear_alias(p) ((typeof(p))hw_clear_alias_untyped(p))
#define hw_xor_alias(p) ((typeof(p))hw_xor_alias_untyped(p))
#define xip_noalloc_alias(p) ((typeof(p))xip_noalloc_alias_untyped(p))
#define xip_nocache_alias(p) ((typeof(p))xip_nocache_alias_untyped(p))
#define xip_nocache_noalloc_alias(p) ((typeof(p))xip_nocache_noalloc_alias_untyped(p))

/*! \brief HWレジスタの指定のビットにアトミックに1をセットする
 *  \ingroup hardware_base
 *
 * \param addr 書き込み可能レジスタのアドレス
 * \param mask セットするビットを指定するビットマスク
 */
__force_inline static void hw_set_bits(io_rw_32 *addr, uint32_t mask) {
    *(io_rw_32 *) hw_set_alias_untyped((volatile void *) addr) = mask;
}

/*! \brief HWレジスタの指定のビットをアトミックに0にクリアする
 *  \ingroup hardware_base
 *
 * \param addr 書き込み可能レジスタのアドレス
 * \param mask クリアするビットを指定するビットマスク
 */
__force_inline static void hw_clear_bits(io_rw_32 *addr, uint32_t mask) {
    *(io_rw_32 *) hw_clear_alias_untyped((volatile void *) addr) = mask;
}

/*! \brief HWレジスタの指定のビットをアトミックに反転する
 *  \ingroup hardware_base
 *
 * \param addr 書き込み可能レジスタのアドレス
 * \param mask 反転するビットを指定するビットマスク
 */
__force_inline static void hw_xor_bits(io_rw_32 *addr, uint32_t mask) {
    *(io_rw_32 *) hw_xor_alias_untyped((volatile void *) addr) = mask;
}

/*! \brief HWレジスタのビットのサブセットに新しい値をセットする
 *  \ingroup hardware_base
 *
 * \p values で指定された値を目的のビットにセットします（ただし、 \p write_mask の
 * 対応するビットが設定されている場合に限ります）。
 * 注: この方法では、レジスタの *異なる* ビットを同時に安全に変更できますが、
 * 同じビットへの複数の同時アクセスは安全ではありません。
 *
 * \param addr 書き込み可能レジスタのアドレス
 * \param values ビット値
 * \param write_mask 変更対象のビットマスク
 */
__force_inline static void hw_write_masked(io_rw_32 *addr, uint32_t values, uint32_t write_mask) {
    hw_xor_bits(addr, (*addr ^ values) & write_mask);
}

#ifdef __cplusplus
}
#endif

#endif
