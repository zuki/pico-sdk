/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _HARDWARE_FLASH_H
#define _HARDWARE_FLASH_H

#include "pico.h"

/** \file flash.h
 *  \defgroup hardware_flash hardware_flash
 *
 * \brief 低レベルflashプログラミング/消去API
 *
 * これらの関数は、2つのコアを使用していて、もう一方のコアが
 * この操作と同時にFlashから実行されている場合、*安全ではない*
 * ことに注意してください。このような場合はFlashプログラミング
 * 中にXIPアクセスが行われないように独自の同期処理を実行する
 * 必要があります。一つの選択肢は \ref multicore_lockout 関数を
 * 使うことです。
 *
 * 同様に、割り込みハンドラや割り込みベクタテーブルがFlashにある
 * 場合も*安全ではありません。* そのためこの場合は呼び出す前に
 * 割り込みを無効にする必要があります。
 *
 * PICO_NO_FLASH=1が定義されていない場合（すなわち、プログラムが
 * Flashから実行されるようにビルドされている場合）、これらの関数は
 * SRAMに第2ステージブートローダの静的コピーを作成し、これを使用して
 * Flashのプログラミングまたは消去後に再びexecute-in-placeモードに
 * 入ります。そのためフラッシュ常駐コードから安全に呼び出すことが
 * できます。
 *
 * \subsection flash_example Example
 * \include flash_program.c
 */

// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_FLASH, Enable/disable assertions in the flash module, type=bool, default=0, group=hardware_flash
#ifndef PARAM_ASSERTIONS_ENABLED_FLASH
#define PARAM_ASSERTIONS_ENABLED_FLASH 0
#endif

#define FLASH_PAGE_SIZE (1u << 8)
#define FLASH_SECTOR_SIZE (1u << 12)
#define FLASH_BLOCK_SIZE (1u << 16)

#define FLASH_UNIQUE_ID_SIZE_BYTES 8

// PICO_CONFIG: PICO_FLASH_SIZE_BYTES, size of primary flash in bytes, type=int, group=hardware_flash

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief  flashの領域を消去する
 *  \ingroup hardware_flash
 *
 * \param flash_offs 消去を開始するFlashのバイト単位のオフセット。
 * 4096バイトのFlashセクタにアライメントされなければならない。
 * \param count 消去するバイト数。4096バイト（1セクタ）の倍数で
 * なければならない。
 */
void flash_range_erase(uint32_t flash_offs, size_t count);

/*! \brief  flashをプログラムする
 *  \ingroup hardware_flash
 *
 * \param flash_offs プログラムするFlashの最初のバイトアドレス
 * \param data Flashにプログラムするデータへのポインタ
 * \param count プログラムするバイト数。256バイト（1ページ）の
 * 倍数でなければならない。
 */

void flash_range_program(uint32_t flash_offs, const uint8_t *data, size_t count);

/*! \brief Get flashのユニークな64ビット識別子を取得する
 *  \ingroup hardware_flash
 *
 * 標準的な4Bh RUID命令を使用してQSPIインタフェースに接続されている
 * Flashデバイスから64ビットのユニークな識別子を取得します。MCUと
 * このFlashは1:1の関連性があるため、これはボードの一意識別子と
 * しても機能します。
 *
 *  \param id_out IDを書き込むための8バイトのバッファへのポインタ
 */
void flash_get_unique_id(uint8_t *id_out);

/*! \brief 双方向のFlashコマンドを実行する
 *  \ingroup hardware_flash
 *
 * QSPIインタフェースに接続されているたFlashデバイス上でシリアル
 * コマンドを実行する低レベル関数です。バイト列はtxbufとrxbufから
 * 同時に送受信されます。したがって、両バッファは同じ長さ、（トランザク
 * ション全体の長さである）カウントでなければなりません。これは
 * FlashチップからデバイスIDやSFDPパラメタなどのメタデータを読み出すのに
 * 便利です。
 *
 * XIPキャッシュはFlashの状態が変更された場合に備えて各コマンド事に
 * フラッシュされます。他のhardware_flash関数と同様にコマンドの実行中は
 * execute-in-place転送のためにFLashにアクセスできないため、Flash常駐の
 * 割り込みハンドラに入ったり、もう1つのコアでFlashコードを同時に実行
 * したりすることは致命的です。このような落とし穴を避けるために、この関数は
 * mainアプリケーションが実行を開始する前の起動時にFlashのメタデータを抽出
 * するためだけに使用することを推奨します。この例については
 * pico_get_unique_id() の実装を参照してください。
 *
 *  \param txbuf Flashに送信するための8バイトのバッファへのポインタ
 *  \param rxbuf Flashからデータを受信するための8バイトのバッファへのポインタ.
 *  txbufとrxbufは同じバッファでもよい
 *  \param count txbufとrxbufのバイト数
 */
void flash_do_cmd(const uint8_t *txbuf, uint8_t *rxbuf, size_t count);


#ifdef __cplusplus
}
#endif

#endif
