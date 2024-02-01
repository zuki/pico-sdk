/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _HARDWARE_RESETS_H
#define _HARDWARE_RESETS_H

#include "pico.h"
#include "hardware/structs/resets.h"

/** \file hardware/resets.h
 *  \defgroup hardware_resets hardware_resets
 *
 * \brief ハードウェアリセットAPI
 *
 * リセットコントローラはRP2040のプロセッサをブートするのに重要では
 * ないすべてのペリフェラルのリセットをソフトウェアで制御することを
 * 可能にします。
 *
 * \subsubsection reset_bitmask
 * \addtogroup hardware_resets
 *
 * 複数のブロックが以下のビットマスクを用いて参照されます。
 *
 * リセットするブロック | ビット
 * ---------------|----
 * USB | 24
 * UART 1 | 23
 * UART 0 | 22
 * Timer | 21
 * TB Manager | 20
 * SysInfo | 19
 * System Config | 18
 * SPI 1 | 17
 * SPI 0 | 16
 * RTC | 15
 * PWM | 14
 * PLL USB | 13
 * PLL System | 12
 * PIO 1 | 11
 * PIO 0 | 10
 * Pads - QSPI | 9
 * Pads - bank 0 | 8
 * JTAG | 7
 * IO Bank 1 | 6
 * IO Bank 0 | 5
 * I2C 1 | 4
 * I2C 0 | 3
 * DMA | 2
 * Bus Control | 1
 * ADC 0 | 0
 *
 * \subsection reset_example サンプルコード
 * \addtogroup hardware_resets
 * \include hello_reset.c
 */

#ifdef __cplusplus
extern "C" {
#endif

/// \tag::reset_funcs[]

/*! \brief 指定したブロックをリセットする
 *  \ingroup hardware_resets
 *
 * \param bits リセットするブロックを示すビットパターン. \ref reset_bitmask を参照
 */
static inline void reset_block(uint32_t bits) {
    hw_set_bits(&resets_hw->reset, bits);
}

/*! \brief 指定したHWブロックをリセットから復帰させる
 *  \ingroup hardware_resets
 *
 * \param bits アンリセットするブロックを示すビットパターン. \ref reset_bitmask を参照
 */
static inline void unreset_block(uint32_t bits) {
    hw_clear_bits(&resets_hw->reset, bits);
}

/*! \brief 指定したHWブロックをリセットから復帰させ完了を待つ
 *  \ingroup hardware_resets
 *
 * \param bits アンリセットするブロックを示すビットパターン. \ref reset_bitmask を参照
 */
static inline void unreset_block_wait(uint32_t bits) {
    hw_clear_bits(&resets_hw->reset, bits);
    while (~resets_hw->reset_done & bits)
        tight_loop_contents();
}
/// \end::reset_funcs[]

#ifdef __cplusplus
}
#endif

#endif
