/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_BOOTROM_H
#define _PICO_BOOTROM_H

#include "pico.h"

/** \file bootrom.h
 * \defgroup pico_bootrom pico_bootrom
 * \brief RP2040のbootromの関数とデータにアクセスする.
 *
 * 個のヘッダはアセンブリコードでもインクルードできます。
 */

// ROM FUNCTIONS

#define ROM_FUNC_POPCOUNT32             ROM_TABLE_CODE('P', '3')
#define ROM_FUNC_REVERSE32              ROM_TABLE_CODE('R', '3')
#define ROM_FUNC_CLZ32                  ROM_TABLE_CODE('L', '3')
#define ROM_FUNC_CTZ32                  ROM_TABLE_CODE('T', '3')
#define ROM_FUNC_MEMSET                 ROM_TABLE_CODE('M', 'S')
#define ROM_FUNC_MEMSET4                ROM_TABLE_CODE('S', '4')
#define ROM_FUNC_MEMCPY                 ROM_TABLE_CODE('M', 'C')
#define ROM_FUNC_MEMCPY44               ROM_TABLE_CODE('C', '4')
#define ROM_FUNC_RESET_USB_BOOT         ROM_TABLE_CODE('U', 'B')
#define ROM_FUNC_CONNECT_INTERNAL_FLASH ROM_TABLE_CODE('I', 'F')
#define ROM_FUNC_FLASH_EXIT_XIP         ROM_TABLE_CODE('E', 'X')
#define ROM_FUNC_FLASH_RANGE_ERASE      ROM_TABLE_CODE('R', 'E')
#define ROM_FUNC_FLASH_RANGE_PROGRAM    ROM_TABLE_CODE('R', 'P')
#define ROM_FUNC_FLASH_FLUSH_CACHE      ROM_TABLE_CODE('F', 'C')
#define ROM_FUNC_FLASH_ENTER_CMD_XIP    ROM_TABLE_CODE('C', 'X')

/*! \brief 2つのASCII文字に基づいてbootromのlookupコードを返す.
 * \ingroup pico_bootrom
 *
 * これらのコードはbootromのデータと関数のアドレスの検索に使用されます。
 *
 * \param c1 第1文字
 * \param c2 第2文字
 * \return rom_func_lookup()とrom_data_lookup()で使用される 'code'
 */
#define ROM_TABLE_CODE(c1, c2) ((c1) | ((c2) << 8))

#ifndef __ASSEMBLER__

// ROM FUNCTION SIGNATURES

typedef uint32_t (*rom_popcount32_fn)(uint32_t);
typedef uint32_t (*rom_reverse32_fn)(uint32_t);
typedef uint32_t (*rom_clz32_fn)(uint32_t);
typedef uint32_t (*rom_ctz32_fn)(uint32_t);
typedef uint8_t *(*rom_memset_fn)(uint8_t *, uint8_t, uint32_t);
typedef uint32_t *(*rom_memset4_fn)(uint32_t *, uint8_t, uint32_t);
typedef uint32_t *(*rom_memcpy_fn)(uint8_t *, const uint8_t *, uint32_t);
typedef uint32_t *(*rom_memcpy44_fn)(uint32_t *, const uint32_t *, uint32_t);
typedef void __attribute__((noreturn)) (*rom_reset_usb_boot_fn)(uint32_t, uint32_t);
typedef rom_reset_usb_boot_fn reset_usb_boot_fn; // kept for backwards compatibility
typedef void (*rom_connect_internal_flash_fn)(void);
typedef void (*rom_flash_exit_xip_fn)(void);
typedef void (*rom_flash_range_erase_fn)(uint32_t, size_t, uint32_t, uint8_t);
typedef void (*rom_flash_range_program_fn)(uint32_t, const uint8_t*, size_t);
typedef void (*rom_flash_flush_cache_fn)(void);
typedef void (*rom_flash_enter_cmd_xip_fn)(void);

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief 2つのASCII文字に基づいてbootromのlookupコードを返す.
 * \ingroup pico_bootrom
 *
 * これらのコードはbootromのデータと関数のアドレスの検索に使用されます。
 *
 * \param c1 第1文字
 * \param c2 第2文字
 * \return rom_func_lookup()とrom_data_lookup()で使用される 'code'
 */
static inline uint32_t rom_table_code(uint8_t c1, uint8_t c2) {
    return ROM_TABLE_CODE((uint32_t) c1, (uint32_t) c2);
}

/*!
 * \brief コードによりbootromの関数を探す
 * \ingroup pico_bootrom
 * \param code コード
 * \return 関数へのポインタ、または、コードがいずれのbootrom関数にも
 *         マッチしない場合はNULL
 */
void *rom_func_lookup(uint32_t code);

/*!
 * \brief コードによりbootromのアドレスを探す
 * \ingroup pico_bootrom
 * \param code コード
 * \return データへのポインタ、または、コードがいずれのbootrom関数にも
 *         マッチしない場合はNULLa
 */
void *rom_data_lookup(uint32_t code);

/*!
 * \brief 複数のbootrom関数のアドレスを検索するヘルパー関数
 * \ingroup pico_bootrom
 *
 * この関数はテーブル内の'codes'を検索して、そのコードがbootromにある場合は
 * 各テーブルエントリを検索関数へのポインタに変換します。
 *
 * \param table IN/OUT配列、要素は入力時はコード、成功した際の出力は関数ポインタ
 * \param count テーブルの要素数
 * \return すべてのコードが見つかり関数ポインタに変換したらtrue、創でなければ false
 */
bool rom_funcs_lookup(uint32_t *table, unsigned int count);

// Bootrom function: rom_table_lookup
// Returns the 32 bit pointer into the ROM if found or NULL otherwise.
typedef void *(*rom_table_lookup_fn)(uint16_t *table, uint32_t code);

#if PICO_C_COMPILER_IS_GNU && (__GNUC__ >= 12)
// Convert a 16 bit pointer stored at the given rom address into a 32 bit pointer
static inline void *rom_hword_as_ptr(uint16_t rom_address) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
    return (void *)(uintptr_t)*(uint16_t *)(uintptr_t)rom_address;
#pragma GCC diagnostic pop
}
#else
// Convert a 16 bit pointer stored at the given rom address into a 32 bit pointer
#define rom_hword_as_ptr(rom_address) (void *)(uintptr_t)(*(uint16_t *)(uintptr_t)(rom_address))
#endif

/*!
 * \brief コードによりbootromのアドレスを探す.
 * この関数はFLASH/RAMのセンシティブなコード使用法のため呼び出し元にインライン化を
 * 強制します。
 * \ingroup pico_bootrom
 * \param code the code
 * \return 関数へのポインタ、または、コードがいずれのbootrom関数にも
 *         マッチしない場合はNULL
 */
static __force_inline void *rom_func_lookup_inline(uint32_t code) {
    rom_table_lookup_fn rom_table_lookup = (rom_table_lookup_fn) rom_hword_as_ptr(0x18);
    uint16_t *func_table = (uint16_t *) rom_hword_as_ptr(0x14);
    return rom_table_lookup(func_table, code);
}

/*!
 * \brief デバイスをBOOTSELモードで再起動する
 * \ingroup pico_bootrom
 *
 * この関数はデバイスをBOOTSELモード（"usb boot"）で再起動します。
 *
 * USB大容量記憶装置用に接続されているLEDのGPIO経由の「アクティビティ・ライト」を有効にし、
 * USBインタフェースの公開を制限するための機能が提供されています。
 *
 * \param usb_activity_gpio_pin_mask コールドブートではピンを使用しない場合は0。
 *    そうでなければ、出力に設定してホストからマスストレージのアクセスがあるたびに
 *    オンにするピンを示すビットをセットする
 * \param disable_interface_mask 公開インタフェースを制御する値
 *  - 0 両インタフェースを有効にする（コールドブート時）
 *  - 1 USBマスストレージインタフェースを無効にする
 *  - 2 USB PICOBOOTインタフェースを無効にする
 */
static inline void __attribute__((noreturn)) reset_usb_boot(uint32_t usb_activity_gpio_pin_mask,
                                                            uint32_t disable_interface_mask) {
    rom_reset_usb_boot_fn func = (rom_reset_usb_boot_fn) rom_func_lookup(ROM_FUNC_RESET_USB_BOOT);
    func(usb_activity_gpio_pin_mask, disable_interface_mask);
}

#ifdef __cplusplus
}
#endif

#endif // !__ASSEMBLER__
#endif
