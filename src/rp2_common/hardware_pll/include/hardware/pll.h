/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _HARDWARE_PLL_H
#define _HARDWARE_PLL_H

#include "pico.h"
#include "hardware/structs/pll.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \file hardware/pll.h
 *  \defgroup hardware_pll hardware_pll
 *
 * \brief PLL (Phase Locked Loop) 制御API
 *
 * RP2040には次の2つのPLLがあります。
 *   - pll_sys - 最大 133MHz のシステムクロックの生成に使用します
 *   - pll_usb - 48MHzのUSB基準クロックの生成に使用します
 *
 * PLLの計算方法についての詳細はRP2040のデータシートを参照してください。
 */

typedef pll_hw_t *PLL;

#define pll_sys pll_sys_hw
#define pll_usb pll_usb_hw

#ifndef PICO_PLL_VCO_MIN_FREQ_KHZ
#ifndef PICO_PLL_VCO_MIN_FREQ_MHZ
#define PICO_PLL_VCO_MIN_FREQ_KHZ (750 * KHZ)
#else
#define PICO_PLL_VCO_MIN_FREQ_KHZ (PICO_PLL_VCO_MIN_FREQ_MHZ * KHZ)
#endif
#endif

#ifndef PICO_PLL_VCO_MAX_FREQ_KHZ
#ifndef PICO_PLL_VCO_MAX_FREQ_MHZ
#define PICO_PLL_VCO_MAX_FREQ_KHZ (1600 * KHZ)
#else
#define PICO_PLL_VCO_MAX_FREQ_KHZ (PICO_PLL_VCO_MAX_FREQ_MHZ * KHZ)
#endif
#endif

/*! \brief 指定したPLLを初期化する.
 *  \ingroup hardware_pll
 * \param pll pll_sys か pll_usb
 * \param ref_div 入力クロック除算器.
 * \param vco_freq  希望するVCO (voltage controlled oscillator) 出力
 * \param post_div1 分周比 1 - 範囲は 1-7. post_div1 >= post_div2 でなければならない
 * \param post_div2 分周比 2 - 範囲は 1-7
 */
void pll_init(PLL pll, uint ref_div, uint vco_freq, uint post_div1, uint post_div2);

/*! \brief 指定したPLLを解放/未初期化する.
 *  \ingroup hardware_pll
 *
 * 指定したPLLの電源をオフにする。この関数は電源を切る前に現在PLLが使用中で
 * あるかチェックしないので、使用には注意が必要です。
 *
 * \param pll pll_sys か pll_usb
 */
void pll_deinit(PLL pll);


#ifdef __cplusplus
}
#endif

#endif
