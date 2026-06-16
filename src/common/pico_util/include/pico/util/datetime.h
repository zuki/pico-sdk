/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_UTIL_DATETIME_H
#define _PICO_UTIL_DATETIME_H

#include "pico.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \file datetime.h
 * \defgroup util_datetime datetime
 * \brief Date/Timeのフォーマット
 * \ingroup pico_util
 */

/*! \brief  datetime_t構造体を文字列に変換する.
 *  \ingroup util_datetime
 *
 * \param buf 生成される文字列を収める文字バッファ
 * \param buf_size 渡されたパスのサイズ
 * \param t 変換するdatetime
 */
void datetime_to_str(char *buf, uint buf_size, const datetime_t *t);

#ifdef __cplusplus
}
#endif
#endif
