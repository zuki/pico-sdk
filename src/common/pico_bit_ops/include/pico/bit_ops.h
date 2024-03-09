/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_BIT_OPS_H
#define _PICO_BIT_OPS_H

#include "pico.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \file bit_ops.h
*  \defgroup pico_bit_ops pico_bit_ops
*
* \brief 最適化されたビット操作関数.
*
* さらに、__builtin_popcountと__builtin_clz, __bulitin_ctzの
* コンパイラのビルトイン実装を置き換えます。
*/

/*! \brief 32ビットワードを反転
 *  \ingroup pico_bit_ops
 *
 * \param bits 32ビット入力
 * \return 32ビット入力を反転したワード
 */
uint32_t __rev(uint32_t bits);

/*! \brief 64ビットワードを反転
 *  \ingroup pico_bit_ops
 *
 * \param bits 64ビット入力
 * \return 64ビット入力を反転したワード
 */
uint64_t __revll(uint64_t bits);

#ifdef __cplusplus
}
#endif

#endif
