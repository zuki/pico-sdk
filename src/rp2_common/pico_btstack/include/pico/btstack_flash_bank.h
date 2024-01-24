/*
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_BTSTACK_FLASH_BANK_H
#define _PICO_BTSTACK_FLASH_BANK_H

#include "pico.h"
#include "hal_flash_bank.h"

#ifdef __cplusplus
extern "C" {
#endif

// PICO_CONFIG: PICO_FLASH_BANK_TOTAL_SIZE, Total size of the Bluetooth flash storage. Must be an even multiple of FLASH_SECTOR_SIZE, type=int, default=FLASH_SECTOR_SIZE * 2, group=pico_btstack
#ifndef PICO_FLASH_BANK_TOTAL_SIZE
#define PICO_FLASH_BANK_TOTAL_SIZE (FLASH_SECTOR_SIZE * 2u)
#endif

// PICO_CONFIG: PICO_FLASH_BANK_STORAGE_OFFSET, Offset in flash of the Bluetooth flash storage, type=int, default=PICO_FLASH_SIZE_BYTES - PICO_FLASH_BANK_TOTAL_SIZE, group=pico_btstack
#ifndef PICO_FLASH_BANK_STORAGE_OFFSET
#define PICO_FLASH_BANK_STORAGE_OFFSET (PICO_FLASH_SIZE_BYTES - PICO_FLASH_BANK_TOTAL_SIZE)
#endif

/**
 * \brief 不揮発性ストレージに使用されるシングルトンのBTstack HALフラッシュインスタンスを返す
 * \ingroup pico_btstack
 *
 * \note デフォルトではフラッシュ末尾の2セクタが使用されます
 * （ \c PICO_FLASH_BANK_STORAGE_OFFSET と  \c PICO_FLASH_BANK_TOTAL_SIZE を
 * 参照）。
 */
const hal_flash_bank_t *pico_flash_bank_instance(void);

#ifdef __cplusplus
}
#endif
#endif
