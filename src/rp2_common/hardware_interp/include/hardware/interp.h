/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _HARDWARE_INTERP_H
#define _HARDWARE_INTERP_H

#include "pico.h"
#include "hardware/structs/interp.h"
#include "hardware/regs/sio.h"

// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_INTERP, Enable/disable assertions in the interpolation module, type=bool, default=0, group=hardware_interp
#ifndef PARAM_ASSERTIONS_ENABLED_INTERP
#define PARAM_ASSERTIONS_ENABLED_INTERP 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** \file hardware/interp.h
 *  \defgroup hardware_interp hardware_interp
 *
 * \brief ハードウェアインターポレータAPI.
 *
 * 各コアには2つのインターポレータ（INTERP0とINTERP1）が搭載されており、
 * 事前に設定された特定の単純な処理をシングルプロセッササイクルにまとめる
 * ことでタスクを高速化することができます。これはあらかじめ設定された
 * 演算が何度も繰り返されるようなケースを想定したものであり、その結果、
 * コードのタイムクリティカルな部分で使用されるCPUサイクルとCPUレジスタ
 * 数を少なくすることができます。
 *
 * インターポレータはSDKではオーディオ処理を高速化するために多用されて
 * いますが、その柔軟な構成により、量子化、ディザリング、アドレス表引き、
 * アフィンテクスチャマッピング、解凍、線形フィードバックなど、他の
 * 多くのタスクを最適化することができます。
 *
 * ハードウェアインターポレータとその動作の詳細についてはRP2040の
 * データシートを参照してください。
 */

#define interp0 interp0_hw
#define interp1 interp1_hw

/** \brief インターポレータの構成.
 *  \defgroup interp_config interp_config
 *  \ingroup hardware_interp
 *
 * 各インターポレータは構成する必要があります。これらの関数は構成
 * 構造体をセットアップするための便利なヘルパーを提供します。
 *
 */

typedef struct {
    uint32_t ctrl;
} interp_config;

static inline uint interp_index(interp_hw_t *interp) {
    valid_params_if(INTERP, interp == interp0 || interp == interp1);
    return interp == interp1 ? 1 : 0;
}

/*! \brief 指定したインターポレータレーンを要求する.
 *  \ingroup hardware_interp
 *
 * この関数を使用して指定したインターポレータレーンへの排他的アクセスを要求します。
 *
 * 指定したレースがすでに要求されていた場合、この関数はパニックを引き起こします。
 *
 * \param interp レーンを要求するインターポレータ. interp0かinterp1のいずれか
 * \param lane レーン番号. 0か1のいずれか
 */
void interp_claim_lane(interp_hw_t *interp, uint lane);
// 実際には一貫性を保つために上の関数は次のマクロで呼び出すべきである
#define interp_lane_claim interp_claim_lane

/*! \brief マスクで指定したインターポレータレーンを要求する.
 *  \ingroup hardware_interp
 *
 * \param interp レーンを要求するインターポレータ. interp0かinterp1のいずれか
 * \param lane_mask 要求するレーンのビットパターン（ビット0と1だけが有効）
 */
void interp_claim_lane_mask(interp_hw_t *interp, uint lane_mask);

/*! \brief 先に要求したインターポレータレーンを解放する.
 *  \ingroup hardware_interp
 *
 * \param interp レーンを解放するインターポレータ. interp0かinterp1のいずれか
 * \param lane レーン番号. 0か1のいずれか
 */
void interp_unclaim_lane(interp_hw_t *interp, uint lane);
// 実際には一貫性を保つために上の関数は次のマクロで呼び出すべきである
#define interp_lane_unclaim interp_unclaim_lane

/*! \brief インターポレータレーンが要求済みであるかチェックする.
 *  \ingroup hardware_interp
 *
 * \param interp チェックするレーンを持つインターポレータ
 * \param lane Tレーン番号. 0か1のいずれか
 * \return 要求済みの場合は true、そうでなければ false
 * \see interp_claim_lane
 * \see interp_claim_lane_mask
 */
bool interp_lane_is_claimed(interp_hw_t *interp, uint lane);

/*! \brief 先に要求したインターポレータレーンを解放する \see interp_claim_lane_mask
 *  \ingroup hardware_interp
 *
 * \param interp レーンを解放するインターポレータ. interp0かinterp1のいずれか
 * \param lane_mask 解放するレーンのビットパターン（ビット0と1だけが有効）
 */
void interp_unclaim_lane_mask(interp_hw_t *interp, uint lane_mask);

/*! \brief インターポレータのシフト値をセットする.
 *  \ingroup interp_config
 *
 * イテレーションごとにアキュムレータをシフトするビット数をセットします。
 *
 * \param c インターポレータconfig構造体へのポインタ
 * \param shift ビット数
 */
static inline void interp_config_set_shift(interp_config *c, uint shift) {
    valid_params_if(INTERP, shift < 32);
    c->ctrl = (c->ctrl & ~SIO_INTERP0_CTRL_LANE0_SHIFT_BITS) |
              ((shift << SIO_INTERP0_CTRL_LANE0_SHIFT_LSB) & SIO_INTERP0_CTRL_LANE0_SHIFT_BITS);
}

/*! \brief インターポレータのマスク範囲をセットする.
 *  \ingroup interp_config
 *
 * インターポレータにパススルーするビット範囲（lsbからmsb)を
 * セットします。
 *
 * \param c インターポレータconfig構造体へのポインタ
 * \param mask_lsb パスできるLSB (Least significant bit)
 * \param mask_msb パスできるMSB (most significant bit)
 */
static inline void interp_config_set_mask(interp_config *c, uint mask_lsb, uint mask_msb) {
    valid_params_if(INTERP, mask_msb < 32);
    valid_params_if(INTERP, mask_lsb <= mask_msb);
    c->ctrl = (c->ctrl & ~(SIO_INTERP0_CTRL_LANE0_MASK_LSB_BITS | SIO_INTERP0_CTRL_LANE0_MASK_MSB_BITS)) |
              ((mask_lsb << SIO_INTERP0_CTRL_LANE0_MASK_LSB_LSB) & SIO_INTERP0_CTRL_LANE0_MASK_LSB_BITS) |
              ((mask_msb << SIO_INTERP0_CTRL_LANE0_MASK_MSB_LSB) & SIO_INTERP0_CTRL_LANE0_MASK_MSB_BITS);
}

/*! \brief クロス入力を有効にする.
 *  \ingroup interp_config
 *
 *  対向レーンのアキュムレータ値をこのレーンのシフト+マスクハードウェアの
 *  入力値とします。これはinterp_config_set_add_rawオプションが設定されて
 *  いても有効です。クロス入力muxはシフト+マスクをバイパスする前に
 *  適用されるからです。
 *
 * \param c インターポレータconfig構造体へのポインタ
 * \param cross_input trueの場合、クロス入力を有効にする.
 */
static inline void interp_config_set_cross_input(interp_config *c, bool cross_input) {
    c->ctrl = (c->ctrl & ~SIO_INTERP0_CTRL_LANE0_CROSS_INPUT_BITS) |
              (cross_input ? SIO_INTERP0_CTRL_LANE0_CROSS_INPUT_BITS : 0);
}

/*! \brief クロス結果を有効にする.
 *  \ingroup interp_config
 *
 *  POP操作で対向レーンの結果をこのレーンのアキュムレータに入力するようにします。
 *
 * \param c 補間構成構造体へのポインタ
 * \param cross_result trueの場合、クロス結果を有効にする
 */
static inline void interp_config_set_cross_result(interp_config *c, bool cross_result) {
    c->ctrl = (c->ctrl & ~SIO_INTERP0_CTRL_LANE0_CROSS_RESULT_BITS) |
              (cross_result ? SIO_INTERP0_CTRL_LANE0_CROSS_RESULT_BITS : 0);
}

/*! \brief 符号拡張をセットする.
 *  \ingroup interp_config
 *
 *  符号拡張モードを有効にします。このモードではシフト+マスクされた
 *  アキュムレータ値はBASE1に加算される前に32ビットに符号拡張されます。
 *  また、LANE1のPEEK/POPの結果もプロセッサが読み取る際に32ビット拡張
 *  された値が読み取られます。
 *
 * \param c インターポレータconfig構造体へのポインタ
 * \param  _signed trueの場合、符号拡張を有効にする
 */
static inline void interp_config_set_signed(interp_config *c, bool _signed) {
    c->ctrl = (c->ctrl & ~SIO_INTERP0_CTRL_LANE0_SIGNED_BITS) |
              (_signed ? SIO_INTERP0_CTRL_LANE0_SIGNED_BITS : 0);
}

/*! \brief RAW_ADDオプションをセットする.
 *  \ingroup interp_config
 *
 *  有効な場合、LANE0の結果にはシフト+マスク操作がバイパスされます。フルの結果
 *  には影響しません。
 *
 * \param c インターポレータconfig構造体へのポインタ
 * \param add_raw trueの場合、RAW_ADDオプションを有効にする
 */
static inline void interp_config_set_add_raw(interp_config *c, bool add_raw) {
    c->ctrl = (c->ctrl & ~SIO_INTERP0_CTRL_LANE0_ADD_RAW_BITS) |
              (add_raw ? SIO_INTERP0_CTRL_LANE0_ADD_RAW_BITS : 0);
}

/*! \brief ブレンドモードをセットする.
 *  \ingroup interp_config
 *
 *  有効な場合、LANE1の結果はBASE0とBASE1の線形補間となります。α値はレーン1の
 *  シフト+マスク値の8 LSBにより制御されます（0 - 255/256 の小数値）
 *
 *  レーン0の結果にはBASE0の値は加算されません（レーン1のシフト+マスク値の
 *  8 LSBだけがセットされます）
 *
 *  フル結果にはレーン1のシフト+マスク値は加算されません（BASE2 + レーン0の
 *  シフト+マスク値）
 *
 *  補間が符号付きか符号なしかはレーン1のSIGNEDフラグが制御します。
 *
 * \param c インターポレータconfig構造体へのポインタ
 * \param blend ブレンドモードを有効にするには true をセットする
*/
static inline void interp_config_set_blend(interp_config *c, bool blend) {
    c->ctrl = (c->ctrl & ~SIO_INTERP0_CTRL_LANE0_BLEND_BITS) |
              (blend ? SIO_INTERP0_CTRL_LANE0_BLEND_BITS : 0);
}

/*! \brief Set interpolator clamp mode (Interpolator 1 only)
 *  \ingroup interp_config
 *
 * Only present on INTERP1 on each core. If CLAMP mode is enabled:
 * - LANE0 result is a shifted and masked ACCUM0, clamped by a lower bound of BASE0 and an upper bound of BASE1.
 * - Signedness of these comparisons is determined by LANE0_CTRL_SIGNED
 *
 * \param c インターポレータconfig構造体へのポインタ
 * \param clamp Set true to enable clamp mode
 */
static inline void interp_config_set_clamp(interp_config *c, bool clamp) {
    c->ctrl = (c->ctrl & ~SIO_INTERP1_CTRL_LANE0_CLAMP_BITS) |
              (clamp ? SIO_INTERP1_CTRL_LANE0_CLAMP_BITS : 0);
}

/*! \brief Set interpolator Force bits
 *  \ingroup interp_config
 *
 * ORed into bits 29:28 of the lane result presented to the processor on the bus.
 *
 * No effect on the internal 32-bit datapath. Handy for using a lane to generate sequence
 * of pointers into flash or SRAM
 *
 * \param c インターポレータconfig構造体へのポインタ
 * \param bits Sets the force bits to that specified. Range 0-3 (two bits)
 */
static inline void interp_config_set_force_bits(interp_config *c, uint bits) {
    invalid_params_if(INTERP, bits > 3);
    // note cannot use hw_set_bits on SIO
    c->ctrl = (c->ctrl & ~SIO_INTERP0_CTRL_LANE0_FORCE_MSB_BITS) |
              (bits << SIO_INTERP0_CTRL_LANE0_FORCE_MSB_LSB);
}

/*! \brief デフォルトの構成を取得する.
 *  \ingroup interp_config
 *
 * \return インターポレータのデフォルトの構成
 */
static inline interp_config interp_default_config(void) {
    interp_config c = {0};
    // 単にすべてをパスするーする
    interp_config_set_mask(&c, 0, 31);
    return c;
}

/*! \brief 構成データをレーンに送信する.
 *  \ingroup interp_config
 *
 * 無効な構成データが指定された場合（レーン固有の項目が間違ったレーンに
 * 設定された場合）、セットアップによってはこの関数はパニックに陥る
 * 可能性があります。
 *
 * \param interp インターポレータインスタンス. interp0かinterp1のいずれか
 * \param lane セットするレーン
 * \param config インターポレータ構成構造体へのポインタ
 */

static inline void interp_set_config(interp_hw_t *interp, uint lane, interp_config *config) {
    invalid_params_if(INTERP, lane > 1);
    invalid_params_if(INTERP, config->ctrl & SIO_INTERP1_CTRL_LANE0_CLAMP_BITS &&
                              (!interp_index(interp) || lane)); // only interp1 lane 0 has clamp bit
    invalid_params_if(INTERP, config->ctrl & SIO_INTERP0_CTRL_LANE0_BLEND_BITS &&
                              (interp_index(interp) || lane)); // only interp0 lane 0 has blend bit
    interp->ctrl[lane] = config->ctrl;
}

/*! \brief Directly set the force bits on a specified lane
 *  \ingroup hardware_interp
 *
 * These bits are ORed into bits 29:28 of the lane result presented to the processor on the bus.
 * There is no effect on the internal 32-bit datapath.
 *
 * Useful for using a lane to generate sequence of pointers into flash or SRAM, saving a subsequent
 * OR or add operation.
 *
 * \param interp Interpolator instance, interp0 or interp1.
 * \param lane The lane to set
 * \param bits The bits to set (bits 0 and 1, value range 0-3)
 */
static inline void interp_set_force_bits(interp_hw_t *interp, uint lane, uint bits) {
    // note cannot use hw_set_bits on SIO
    interp->ctrl[lane] = interp->ctrl[lane] | (bits << SIO_INTERP0_CTRL_LANE0_FORCE_MSB_LSB);
}

typedef struct {
    uint32_t accum[2];
    uint32_t base[3];
    uint32_t ctrl[2];
} interp_hw_save_t;

/*! \brief Save the specified interpolator state
 *  \ingroup hardware_interp
 *
 * Can be used to save state if you need an interpolator for another purpose, state
 * can then be recovered afterwards and continue from that point
 *
 * \param interp Interpolator instance, interp0 or interp1.
 * \param saver Pointer to the save structure to fill in
 */
void interp_save(interp_hw_t *interp, interp_hw_save_t *saver);

/*! \brief Restore an interpolator state
 *  \ingroup hardware_interp
 *
 * \param interp Interpolator instance, interp0 or interp1.
 * \param saver Pointer to save structure to reapply to the specified interpolator
 */
void interp_restore(interp_hw_t *interp, interp_hw_save_t *saver);

/*! \brief Sets the interpolator base register by lane
 *  \ingroup hardware_interp
 *
 * \param interp Interpolator instance, interp0 or interp1.
 * \param lane The lane number, 0 or 1 or 2
 * \param val The value to apply to the register
 */
static inline void interp_set_base(interp_hw_t *interp, uint lane, uint32_t val) {
    interp->base[lane] = val;
}

/*! \brief Gets the content of interpolator base register by lane
 *  \ingroup hardware_interp
 *
 * \param interp Interpolator instance, interp0 or interp1.
 * \param lane The lane number, 0 or 1 or 2
 * \return  The current content of the lane base register
 */
static inline uint32_t interp_get_base(interp_hw_t *interp, uint lane) {
    return interp->base[lane];
}

/*! \brief Sets the interpolator base registers simultaneously
 *  \ingroup hardware_interp
 *
 *  The lower 16 bits go to BASE0, upper bits to BASE1 simultaneously.
 *  Each half is sign-extended to 32 bits if that lane’s SIGNED flag is set.
 *
 * \param interp Interpolator instance, interp0 or interp1.
 * \param val The value to apply to the register
 */
static inline void interp_set_base_both(interp_hw_t *interp, uint32_t val) {
    interp->base01 = val;
}


/*! \brief Sets the interpolator accumulator register by lane
 *  \ingroup hardware_interp
 *
 * \param interp Interpolator instance, interp0 or interp1.
 * \param lane The lane number, 0 or 1
 * \param val The value to apply to the register
 */
static inline void interp_set_accumulator(interp_hw_t *interp, uint lane, uint32_t val) {
    interp->accum[lane] = val;
}

/*! \brief Gets the content of the interpolator accumulator register by lane
 *  \ingroup hardware_interp
 *
 * \param interp Interpolator instance, interp0 or interp1.
 * \param lane The lane number, 0 or 1
 * \return The current content of the register
 */
static inline uint32_t interp_get_accumulator(interp_hw_t *interp, uint lane) {
    return interp->accum[lane];
}

/*! \brief Read lane result, and write lane results to both accumulators to update the interpolator
 *  \ingroup hardware_interp
 *
 * \param interp Interpolator instance, interp0 or interp1.
 * \param lane The lane number, 0 or 1
 * \return The content of the lane result register
 */
static inline uint32_t interp_pop_lane_result(interp_hw_t *interp, uint lane) {
    return interp->pop[lane];
}

/*! \brief Read lane result
 *  \ingroup hardware_interp
 *
 * \param interp Interpolator instance, interp0 or interp1.
 * \param lane The lane number, 0 or 1
 * \return The content of the lane result register
 */
static inline uint32_t interp_peek_lane_result(interp_hw_t *interp, uint lane) {
    return interp->peek[lane];
}

/*! \brief Read lane result, and write lane results to both accumulators to update the interpolator
 *  \ingroup hardware_interp
 *
 * \param interp Interpolator instance, interp0 or interp1.
 * \return The content of the FULL register
 */
static inline uint32_t interp_pop_full_result(interp_hw_t *interp) {
    return interp->pop[2];
}

/*! \brief Read lane result
 *  \ingroup hardware_interp
 *
 * \param interp Interpolator instance, interp0 or interp1.
 * \return The content of the FULL register
 */
static inline uint32_t interp_peek_full_result(interp_hw_t *interp) {
    return interp->peek[2];
}

/*! \brief Add to accumulator
 *  \ingroup hardware_interp
 *
 * Atomically add the specified value to the accumulator on the specified lane
 *
 * \param interp Interpolator instance, interp0 or interp1.
 * \param lane The lane number, 0 or 1
 * \param val Value to add
 */
static inline void interp_add_accumulater(interp_hw_t *interp, uint lane, uint32_t val) {
    interp->add_raw[lane] = val;
}

/*! \brief Get raw lane value
 *  \ingroup hardware_interp
 *
 * Returns the raw shift and mask value from the specified lane, BASE0 is NOT added
 *
 * \param interp Interpolator instance, interp0 or interp1.
 * \param lane The lane number, 0 or 1
 * \return The raw shift/mask value
 */
static inline uint32_t interp_get_raw(interp_hw_t *interp, uint lane) {
    return interp->add_raw[lane];
}

#ifdef __cplusplus
}
#endif

#endif
