/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_BINARY_INFO_H
#define _PICO_BINARY_INFO_H

/** \file binary_info.h
 *  \defgroup pico_binary_info pico_binary_info
 *
 * \brief バイナリ情報はFLASHのバイナリに機械可読情報を埋め込むためのものである.
 *
 * 以下が使用例です。
 *
 * - プログラム識別子 / 情報
 * - ピンレイアウト
 * - 含まれる機能
 * - ブロックデバイス/ストレージとして使用されるフラッシュ領域の識別
 */

#include "pico/binary_info/defs.h"
#include "pico/binary_info/structure.h"
#if !PICO_ON_DEVICE && !defined(PICO_NO_BINARY_INFO)
#define PICO_NO_BINARY_INFO 1
#endif
#include "pico/binary_info/code.h"
#endif
