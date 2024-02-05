/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_RUNTIME_H
#define _PICO_RUNTIME_H

#ifdef __cplusplus
extern "C" {
#endif

/** \file runtime.h
*  \defgroup pico_runtime pico_runtime
* @ref pico_bit_ops, @ref pico_divider, @ref pico_double, @ref pico_int64_ops, @ref pico_float, @ref pico_malloc, @ref pico_mem_ops, @ref pico_standard_link を含む実行時サポートを集めたもの.
*/


void runtime_install_stack_guard(void *stack_bottom);

#ifdef __cplusplus
}
#endif

#endif
