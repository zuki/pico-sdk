/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_DIVIDER_H
#define _PICO_DIVIDER_H

#include "pico.h"
#include "hardware/divider.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup pico_divider pico_divider
 * \brief RP2040ハードウェア除算器により高速化された32ビットおよび64ビットの最適化された除算関数.
 * Cの '/' 演算子と '%' 演算子の組み合わせ関数も提供されています。
 */

/** \file pico/divider.h
 * \ingroup pico_divider
 *
 * \brief ハードウェア除算器により高速化された32ビットおよび64ビットの商/剰余の組み合わせ関数を含む高水準API.
 * これらの関数はすべて、ゼロ除算時には適用可能な最大の符号付き値を
 * 渡して __aeabi_idiv0 または __aebi_ldiv0 を呼び出します。
 *
 * 名前にunsafeと付いている関数は除算器の状態を保存/復元しません。そのため、
 * 割り込みからの呼び出しは安全ではありません。安全でない関数の方が若干高速です。
 */

/**
 * \brief 2つの符号付き32ビット値の整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \return 商
 */
int32_t div_s32s32(int32_t a, int32_t b);

/**
 * \brief 2つの符号付き32ビット値の剰余付き整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \param [out] rem 被除数/除数の剰余
 * \return 被除数/除数の商
 */
static inline int32_t divmod_s32s32_rem(int32_t a, int32_t b, int32_t *rem) {
    divmod_result_t r = hw_divider_divmod_s32(a, b);
    *rem = to_remainder_s32(r);
    return to_商_s32(r);
}

/**
 * \brief 2つの符号付き32ビット値の整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \return 低位ワード/r0に商, 高位ワード/r1に剰余
 */
divmod_result_t divmod_s32s32(int32_t a, int32_t b);

/**
 * \brief 2つの符号なし32ビット値の整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \return 商
 */
uint32_t div_u32u32(uint32_t a, uint32_t b);

/**
 * \brief 2つの符号なし32ビット値の剰余付き整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \param [out] rem The 被除数/除数の剰余
 * \return 被除数/除数の商
 */
static inline uint32_t divmod_u32u32_rem(uint32_t a, uint32_t b, uint32_t *rem) {
    divmod_result_t r = hw_divider_divmod_u32(a, b);
    *rem = to_remainder_u32(r);
    return to_商_u32(r);
}

/**
 * \brief 2つの符号なし32ビット値の整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \return 低位ワード/r0に商, 高位ワード/r1に剰余
 */
divmod_result_t divmod_u32u32(uint32_t a, uint32_t b);

/**
 * \brief 2つの符号付き64ビット値の整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \return 商
 */
int64_t div_s64s64(int64_t a, int64_t b);

/**
 * \brief 2つの符号付き64ビット値の剰余付き整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \param [out] rem 被除数/除数の剰余
 * \return 被除数/除数の商
 */
int64_t divmod_s64s64_rem(int64_t a, int64_t b, int64_t *rem);

/**
 * \brief 2つの符号付き64ビット値の整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \return レジスタ(r0,r1)に商, レジスタ(r2, r3)に剰余
 */
int64_t divmod_s64s64(int64_t a, int64_t b);

/**
 * \brief 2つの符号なし64ビット値の整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \return 商
 */
uint64_t div_u64u64(uint64_t a, uint64_t b);

/**
 * \brief 2つの符号なし64ビット値の剰余付き整数除算I
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \param [out] rem 被除数/除数の剰余
 * \return 被除数/除数の商
 */
uint64_t divmod_u64u64_rem(uint64_t a, uint64_t b, uint64_t *rem);


/**
 * \brief 2つの符号なし64ビット値の整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \return レジスタ(r0,r1)に商, レジスタ(r2, r3)に剰余
 */
uint64_t divmod_u64u64(uint64_t a, uint64_t b);

// -----------------------------------------------------------------------
// these "unsafe" functions are slightly faster, but do not save the divider state,
// so are not generally safe to be called from interrupts
// -----------------------------------------------------------------------

/**
 * \brief 安全でない2つの符号付き32ビット値の整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \return 商
 *
 * 割り込みハンドラでは使用しないこと。
 */
int32_t div_s32s32_unsafe(int32_t a, int32_t b);

/**
 * \brief 安全でない2つの符号付き32ビット値の剰余付き整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \param [out] rem 被除数/除数の剰余
 * \return 被除数/除数の商
 *
 * 割り込みハンドラでは使用しないこと。
 */
int32_t divmod_s32s32_rem_unsafe(int32_t a, int32_t b, int32_t *rem);

/**
 * \brief 安全でない2つの符号付き32ビット値の整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \return 低位ワード/r0に商, 高位ワード/r1に剰余
 *
 * 割り込みハンドラでは使用しないこと。
 */
int64_t divmod_s32s32_unsafe(int32_t a, int32_t b);

/**
 * \brief 安全でない2つの符号なし32ビット値の整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \return 商
 *
 * 割り込みハンドラでは使用しないこと。
 */
uint32_t div_u32u32_unsafe(uint32_t a, uint32_t b);

/**
 * \brief 安全でない2つの符号なし32ビット値の剰余付き整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \param [out] rem 被除数/除数の剰余
 * \return 被除数/除数の商
 *
 * 割り込みハンドラでは使用しないこと。
 */
uint32_t divmod_u32u32_rem_unsafe(uint32_t a, uint32_t b, uint32_t *rem);

/**
 * \brief 安全でない2つの符号なし32ビット値の整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \return 低位ワード/r0に商, 高位ワード/r1に剰余
 *
 * 割り込みハンドラでは使用しないこと。
 */
uint64_t divmod_u32u32_unsafe(uint32_t a, uint32_t b);

/**
 * \brief 安全でない2つの符号付き64ビット値の整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \return 商
 *
 * 割り込みハンドラでは使用しないこと。
 */
int64_t div_s64s64_unsafe(int64_t a, int64_t b);

/**
 * \brief 安全でない2つの符号付き64ビット値の剰余付き整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \param [out] rem 被除数/除数の剰余
 * \return 被除数/除数の商
 *
 * 割り込みハンドラでは使用しないこと。
 */
int64_t divmod_s64s64_rem_unsafe(int64_t a, int64_t b, int64_t *rem);

/**
 * \brief 安全でない2つの符号付き64ビット値の整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \return レジスタ(r0,r1)に商, レジスタ(r2, r3)に剰余
 *
 * 割り込みハンドラでは使用しないこと。
 */
int64_t divmod_s64s64_unsafe(int64_t a, int64_t b);

/**
 * \brief 安全でない2つの符号なし64ビット値の整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \return 商
 *
 * 割り込みハンドラでは使用しないこと。
 */
uint64_t div_u64u64_unsafe(uint64_t a, uint64_t b);

/**
 * \brief 安全でない2つの符号なし64ビット値の剰余付き整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \param [out] rem 被除数/除数の剰余
 * \return 被除数/除数の商
 *
 * 割り込みハンドラでは使用しないこと。
 */
uint64_t divmod_u64u64_rem_unsafe(uint64_t a, uint64_t b, uint64_t *rem);

/**
 * \brief 安全でない2つの符号なし64ビット値の整数除算
 * \ingroup pico_divider
 *
 * \param a 被除数
 * \param b 除数
 * \return レジスタ(r0,r1)に商, レジスタ(r2, r3)に剰余
 *
 * 割り込みハンドラでは使用しないこと。
 */
uint64_t divmod_u64u64_unsafe(uint64_t a, uint64_t b);

#ifdef __cplusplus
}
#endif
#endif
