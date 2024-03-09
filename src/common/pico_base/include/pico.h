/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_H
#define _PICO_H

/** \file pico.h
 *  \defgroup pico_base pico_base
 *  \brief Raspberry Pi Pico SDKの型とマクロ.
 *
 * Raspberry Pi Pico SDKのコアな型とマクロです。このヘッダはすべての
 * ソースコードにインクルードされることを意図しています。個のヘッダは
 * 構成用のヘッダをインクルードし、正しい順番でオーバーライドするからです。
 *
 * このヘッダはアセンブリコードに含めることができます。
 */

// We may be included by assembly which cant include <cdefs.h>
#define	__PICO_STRING(x)	#x
#define	__PICO_XSTRING(x)	__PICO_STRING(x)
#define __PICO_CONCAT1(x, y) x ## y

#include "pico/types.h"
#include "pico/version.h"

// PICO_CONFIG: PICO_CONFIG_HEADER, unquoted path to header include in place of the default pico/config.h which may be desirable for build systems which can't easily generate the config_autogen header, group=pico_base
#ifdef PICO_CONFIG_HEADER
#include __PICO_XSTRING(PICO_CONFIG_HEADER)
#else
#include "pico/config.h"
#endif
#include "pico/platform.h"
#include "pico/error.h"

#endif
