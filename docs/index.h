/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Here to organize documentation order

/**
 * \defgroup hardware ハードウェアAPI
 * このライブラリ群はハードウェアレジスタを直接読み書きすることなく、
 * RP2040ハードウェアにアクセスするための効率的で薄いC API/抽象化を提供します.
 * @{
 * \defgroup hardware_adc hardware_adc
 * \defgroup hardware_base hardware_base
 * \defgroup hardware_claim hardware_claim
 * \defgroup hardware_clocks hardware_clocks
 * \defgroup hardware_divider hardware_divider
 * \defgroup hardware_dma hardware_dma
 * \defgroup hardware_exception hardware_exception
 * \defgroup hardware_flash hardware_flash
 * \defgroup hardware_gpio hardware_gpio
 * \defgroup hardware_i2c hardware_i2c
 * \defgroup hardware_interp hardware_interp
 * \defgroup hardware_irq hardware_irq
 * \defgroup hardware_pio hardware_pio
 * \defgroup hardware_pll hardware_pll
 * \defgroup hardware_pwm hardware_pwm
 * \defgroup hardware_resets hardware_resets
 * \defgroup hardware_rtc hardware_rtc
 * \defgroup hardware_spi hardware_spi
 * \defgroup hardware_sync hardware_sync
 * \defgroup hardware_timer hardware_timer
 * \defgroup hardware_uart hardware_uart
 * \defgroup hardware_vreg hardware_vreg
 * \defgroup hardware_watchdog hardware_watchdog
 * \defgroup hardware_xosc hardware_xosc
 * @}
 *
 * \defgroup high_level 高水準API
 * このライブラリ群は、ハードウェアに関連しない、より高水準の機能や基本的な
 * ハードウェアインタフェースを超える、よりリッチな機能を提供します.
 * @{
 * \defgroup pico_async_context pico_async_context
 * \defgroup pico_flash pico_flash
 * \defgroup pico_i2c_slave pico_i2c_slave
 * \defgroup pico_multicore pico_multicore
 * \defgroup pico_rand pico_rand
 * \defgroup pico_stdlib pico_stdlib
 * \defgroup pico_sync pico_sync
 * \defgroup pico_time pico_time
 * \defgroup pico_unique_id pico_unique_id
 * \defgroup pico_util pico_util
 * @}
 *
 * \defgroup third_party サードパーティライブラリ
 * 高水準機能を実装するためのサードパーティライブラリです.
 * @{
 * \defgroup tinyusb_device tinyusb_device
 * \defgroup tinyusb_host tinyusb_host
 * @}
 *
 * \defgroup networking ネットワークライブラリ
 * ネットワックを実装するための関数です.
 * @{
 * \defgroup pico_btstack pico_btstack
 * \defgroup pico_lwip pico_lwip
 * \defgroup pico_cyw43_driver pico_cyw43_driver
 * \defgroup pico_cyw43_arch pico_cyw43_arch
 * @}
 *
 * \defgroup runtime ランタイムインフラストラクチャ
 * 特定の言語レベルとCライブラリ関数の効率的な実装を提供するために使用される
 * ライブラリとSDKのコンパイルとリンクのステップを抽象化するCMake INTERFACE
 * ライブラリです.
 * @{
 * \defgroup boot_stage2 boot_stage2
 * \defgroup pico_base pico_base
 * \defgroup pico_binary_info pico_binary_info
 * \defgroup pico_bit_ops pico_bit_ops
 * \defgroup pico_bootrom pico_bootrom
 * \defgroup pico_bootsel_via_double_reset pico_bootsel_via_double_reset
 * \defgroup pico_cxx_options pico_cxx_options
 * \defgroup pico_divider pico_divider
 * \defgroup pico_double pico_double
 * \defgroup pico_float pico_float
 * \defgroup pico_int64_ops pico_int64_ops
 * \defgroup pico_malloc pico_malloc
 * \defgroup pico_mem_ops pico_mem_ops
 * \defgroup pico_platform pico_platform
 * \defgroup pico_printf pico_printf
 * \defgroup pico_runtime pico_runtime
 * \defgroup pico_stdio pico_stdio
 * \defgroup pico_standard_link pico_standard_link
 * @}
 *
 * \defgroup misc 外部APIヘッダー
 * SDK以外のコードで共有されるインタフェース用のヘッダーです.
 * @{
 * \defgroup boot_picoboot boot_picoboot
 * \defgroup boot_uf2 boot_uf2
 * @}
*/
