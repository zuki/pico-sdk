/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _HARDWARE_I2C_H
#define _HARDWARE_I2C_H

#include "pico.h"
#include "pico/time.h"
#include "hardware/structs/i2c.h"
#include "hardware/regs/dreq.h"

// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_I2C, Enable/disable assertions in the I2C module, type=bool, default=0, group=hardware_i2c
#ifndef PARAM_ASSERTIONS_ENABLED_I2C
#define PARAM_ASSERTIONS_ENABLED_I2C 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** \file hardware/i2c.h
 *  \defgroup hardware_i2c hardware_i2c
 *
 * \brief I2C制御API
 *
 * I2Cバスは、シリアルデータラインSDAとシリアルクロックSCLからなる
 * 2線式のシリアルインターフェースです。これらの線はバスに接続された
 * デバイス間で情報を伝達します。各デバイスは一意の7ビットアドレスで
 * 認識され、デバイスの機能に応じて「トランスミッタ」または「レシーバ」と
 * して動作します。デバイスはまた、データ転送を実行する際にマスターまたは
 * スレーブと見なすこともできます。マスターは、バス上でデータ転送を開始し、
 * その転送を許可するクロック信号を生成するデバイスです。データ転送の最初の
 * バイトは常に7ビットのアドレスとLSB位置にリード/ライトビットを含みます。
 * このAPIはリード/ライトビットのトグル処理を行います。この後はアドレス指定
 * されたすべてのデバイスはスレーブとみなされます。
 *
 * このAPIではコントローラは \ref i2c_set_slave_mode 関数を使ってマスター
 * またはスレーブとして設定することができます。
 *
 * 各コントローラの外部ピンはデータシートのGPIO muxing tableで定義されている
 * GPIOピンに接続されます。muxingオプションによりIOには柔軟性が与えられて
 * いますが、各コントローラの外部ピンは1つのGPIOだけに接続する必要があります。
 *
 * コントローラは高速モードと超高速モードをサポートしていないことに注意して
 * ください。最速動作は最大1000Kb/秒のfastモードプラスです。
 *
 * I2Cコントローラとその使用方法の詳細についてはデータシートを参照して
 * ください。
 *
 * \subsection i2c_example サンプルコード
 * \addtogroup hardware_i2c
 * \include bus_scan.c
 */

typedef struct i2c_inst i2c_inst_t;

// PICO_CONFIG: PICO_DEFAULT_I2C, Define the default I2C for a board, min=0, max=1, group=hardware_i2c
// PICO_CONFIG: PICO_DEFAULT_I2C_SDA_PIN, Define the default I2C SDA pin, min=0, max=29, group=hardware_i2c
// PICO_CONFIG: PICO_DEFAULT_I2C_SCL_PIN, Define the default I2C SCL pin, min=0, max=29, group=hardware_i2c

/** The I2C identifiers for use in I2C functions.
 *
 * e.g. i2c_init(i2c0, 48000)
 *
 *  \ingroup hardware_i2c
 * @{
 */
extern i2c_inst_t i2c0_inst;
extern i2c_inst_t i2c1_inst;

#define i2c0 (&i2c0_inst) ///< Identifier for I2C HW Block 0
#define i2c1 (&i2c1_inst) ///< Identifier for I2C HW Block 1

#if !defined(PICO_DEFAULT_I2C_INSTANCE) && defined(PICO_DEFAULT_I2C)
#define PICO_DEFAULT_I2C_INSTANCE (__CONCAT(i2c,PICO_DEFAULT_I2C))
#endif

#ifdef PICO_DEFAULT_I2C_INSTANCE
#define i2c_default PICO_DEFAULT_I2C_INSTANCE
#endif

/** @} */

// ----------------------------------------------------------------------------
// Setup

/*! \brief   I2Cハードウェアブロックを初期化する
 *  \ingroup hardware_i2c
 *
 * I2Cハードウェアを既知の状態に置き、有効にします。他の関数の前に
 * 呼び出す必要があります。デフォルトでは、I2Cはマスターとして動作
 * するように構成されます。
 *
 * I2Cバス周波数には要求された値にできるだけ近い値が設定され、実際に
 * 設定されたレートが返されます。
 *
 * \param i2c \ref i2c0 か \ref i2c1 のいずれか
 * \param baudrate Hz単位のボーレート（たとえば、100kHz は 100000）
 * \return 実際に設定されたボーレート
 */
uint i2c_init(i2c_inst_t *i2c, uint baudrate);

/*! \brief   I2Cハードウェアブロックを無効にする
 *  \ingroup hardware_i2c
 *
 * \param i2c \ref i2c0 か \ref i2c1 のいずれか
 *
 * I2Cが使用されなくなった場合は再度無効にします。再度使用する場合は
 * 再初期化する必要があります。
 */
void i2c_deinit(i2c_inst_t *i2c);

/*! \brief  I2Cのボーレートを設定する
 *  \ingroup hardware_i2c
 *
 * I2Cバス周波数に要求された値にできるだけ近い値を設定し、実際に
 * 設定されたレートを返します。クロックの制限により、ボーレートは
 * 要求通りの値にならない場合もあります。
 *
 * \param i2c \ref i2c0 か \ref i2c1 のいずれか
 * \param baudrate Hz単位のボーレート（たとえば、100kHz は 100000）
 * \return 実際に設定されたボーレート
 */
uint i2c_set_baudrate(i2c_inst_t *i2c, uint baudrate);

/*! \brief  I2Cポートをスレーブモードにセットする
 *  \ingroup hardware_i2c
 *
 * \param i2c \ref i2c0 か \ref i2c1 のいずれか
 * \param slave スレーブモードとして使う場合は true, マスターモードとして使う場合は false
 * \param addr If \p slave が true の場合はスレーブアドレスをセットする
 */
void i2c_set_slave_mode(i2c_inst_t *i2c, bool slave, uint8_t addr);

// ----------------------------------------------------------------------------
// Generic input/output

struct i2c_inst {
    i2c_hw_t *hw;
    bool restart_on_next;
};

/*! \brief I2Cインスタンスをハードウェアインスタンス番号に変換する
 *  \ingroup hardware_i2c
 *
 * \param i2c I2Cインスタンス
 * \return I2Cの番号, 0 か 1.
 */
static inline uint i2c_hw_index(i2c_inst_t *i2c) {
    invalid_params_if(I2C, i2c != i2c0 && i2c != i2c1);
    return i2c == i2c1 ? 1 : 0;
}

static inline i2c_hw_t *i2c_get_hw(i2c_inst_t *i2c) {
    i2c_hw_index(i2c); // check it is a hw i2c
    return i2c->hw;
}

static inline i2c_inst_t *i2c_get_instance(uint instance) {
    static_assert(NUM_I2CS == 2, "");
    invalid_params_if(I2C, instance >= NUM_I2CS);
    return instance ? i2c1 : i2c0;
}

/*! \brief 指定された絶対時間に達するまでブロックして、指定したアドレスへの指定したバイト数の書き込みを試みる.
 *  \ingroup hardware_i2c
 *
 * \param i2c \ref i2c0 か \ref i2c1 のいずれか
 * \param addr 書き込み先のデバイスの7ビットアドレス
 * \param src 送信するデータへのポインタ
 * \param len 送信するデータのバイト帳
 * \param nostop  true の場合、マスターは転送終了時にバスの制御を保持し
 * （Stopを発行しない）、StartではなくRestartで次の転送を開始する。
 * \param until トランザクション全体が完了するまでブロックが待機する絶対時間。
 * この値をデータ長で割った値が各バイトの転送に適用されるため、最初のバイト
 * またはそれ以降のバイトがそのサブタイムアウト内に転送できなかった場合、
 * この関数はエラーで返されることに注意されたい。
 *
 * \return 書き込まれたバイト数. または、アドレスが認識されなかった、または、
 * デバイスが存在しない場合は PICO_ERROR_GENERIC、タイムアウトが発生した場合は
 * PICO_ERROR_TIMEOUT
 */
int i2c_write_blocking_until(i2c_inst_t *i2c, uint8_t addr, const uint8_t *src, size_t len, bool nostop, absolute_time_t until);

/*! \brief  指定された絶対時間に達するまでブロックして、指定したアドレスから指定したバイト数の読み込みを試みる.
 *  \ingroup hardware_i2c
 *
 * \param i2c \ref i2c0 か \ref i2c1 のいずれか
 * \param addr 読み込むデバイスの7ビットアドレス
 * \param dst データを受信するバッファへのポインタ
 * \param len 受信するデータのバイト長
 * \param nostop  true の場合、マスターは転送終了時にバスの制御を保持し
 * （Stopを発行しない）、StartではなくRestartで次の転送を開始する。
 * \param until Tトランザクション全体が完了するまでブロックが待機する絶対時間。
 * \return 読み込んだバイト数. または、アドレスが認識されなかった、または、
 * デバイスが存在しない場合は PICO_ERROR_GENERIC、タイムアウトが発生した場合は
 * PICO_ERROR_TIMEOUT.
 */
int i2c_read_blocking_until(i2c_inst_t *i2c, uint8_t addr, uint8_t *dst, size_t len, bool nostop, absolute_time_t until);

/*! \brief タイムアウトを指定して指定したアドレスへの指定したバイト数の書き込みを試みる.
 *  \ingroup hardware_i2c
 *
 * \param i2c \ref i2c0 か \ref i2c1 のいずれか
 * \param addr 書き込み先のデバイスの7ビットアドレス
 * \param src 送信するデータへのポインタ
 * \param len 送信するデータのバイト帳
 * \param nostop  true の場合、マスターは転送終了時にバスの制御を保持し
 * （Stopを発行しない）、StartではなくRestartで次の転送を開始する。
 * \param timeout_us トランザクション全体が完了するまでブロックが待機する絶対時間。
 * この値をデータ長で割った値が各バイトの転送に適用されるため、最初のバイト
 * またはそれ以降のバイトがそのサブタイムアウト内に転送できなかった場合、
 * この関数はエラーで返されることに注意されたい。
 *
 * \return 書き込まれたバイト数. または、アドレスが認識されなかった、または、
 * デバイスが存在しない場合は PICO_ERROR_GENERIC、タイムアウトが発生した場合は
 * PICO_ERROR_TIMEOUT
 */
static inline int i2c_write_timeout_us(i2c_inst_t *i2c, uint8_t addr, const uint8_t *src, size_t len, bool nostop, uint timeout_us) {
    absolute_time_t t = make_timeout_time_us(timeout_us);
    return i2c_write_blocking_until(i2c, addr, src, len, nostop, t);
}

int i2c_write_timeout_per_char_us(i2c_inst_t *i2c, uint8_t addr, const uint8_t *src, size_t len, bool nostop, uint timeout_per_char_us);

/*! \brief  タイムアウトを指定して指定したアドレスから指定したバイト数の読み込みを試みる.
 *  \ingroup hardware_i2c
 *
 * \param i2c \ref i2c0 か \ref i2c1 のいずれか
 * \param addr 読み込むデバイスの7ビットアドレス
 * \param dst データを受信するバッファへのポインタ
 * \param len 受信するデータのバイト長
 * \param nostop  true の場合、マスターは転送終了時にバスの制御を保持し
 * （Stopを発行しない）、StartではなくRestartで次の転送を開始する。
 * \param timeout_us トランザクション全体が完了するまで関数が待機する時間
 * \return 読み込んだバイト数. または、アドレスが認識されなかった、または、
 * デバイスが存在しない場合は PICO_ERROR_GENERIC、タイムアウトが発生した場合は
 * PICO_ERROR_TIMEOUT.
 */
static inline int i2c_read_timeout_us(i2c_inst_t *i2c, uint8_t addr, uint8_t *dst, size_t len, bool nostop, uint timeout_us) {
    absolute_time_t t = make_timeout_time_us(timeout_us);
    return i2c_read_blocking_until(i2c, addr, dst, len, nostop, t);
}

int i2c_read_timeout_per_char_us(i2c_inst_t *i2c, uint8_t addr, uint8_t *dst, size_t len, bool nostop, uint timeout_per_char_us);

/*! \brief ブロックして指定したアドレスへの指定したバイト数の書き込みを試みる.
 *  \ingroup hardware_i2c
 *
 * \param i2c \ref i2c0 か \ref i2c1 のいずれか
 * \param addr 書き込み先のデバイスの7ビットアドレス
 * \param src 送信するデータへのポインタ
 * \param len 送信するデータのバイト帳
 * \param nostop  true の場合、マスターは転送終了時にバスの制御を保持し
 * （Stopを発行しない）、StartではなくRestartで次の転送を開始する。
 * \return 書き込まれたバイト数. または、アドレスが認識されなかった、または、
 * デバイスが存在しない場合は PICO_ERROR_GENERIC.
 */
int i2c_write_blocking(i2c_inst_t *i2c, uint8_t addr, const uint8_t *src, size_t len, bool nostop);

/*! \brief  ブロックして指定したアドレスから指定したバイト数の読み込みを試みる.
 *  \ingroup hardware_i2c
 *
 * \param i2c \ref i2c0 か \ref i2c1 のいずれか
 * \param addr 読み込むデバイスの7ビットアドレス
 * \param dst データを受信するバッファへのポインタ
 * \param len 受信するデータのバイト長
 * \param nostop  true の場合、マスターは転送終了時にバスの制御を保持し
 * （Stopを発行しない）、StartではなくRestartで次の転送を開始する。
 * \return 読み込んだバイト数. または、アドレスが認識されなかった、または、
 * デバイスが存在しない場合は PICO_ERROR_GENERIC.
 */
int i2c_read_blocking(i2c_inst_t *i2c, uint8_t addr, uint8_t *dst, size_t len, bool nostop);


/*! \brief ノンブロッキングの書き込みスペースがあるかチェックする
 *  \ingroup hardware_i2c
 *
 * \param i2c \ref i2c0 か \ref i2c1 のいずれか
 * \return これ以上データを書き込むスペースがI2Cにない場合は 0. 0以外の
 * 場合は、少なくともこのバイト数だけはブロックなしで書き込むことができる.
 */
static inline size_t i2c_get_write_available(i2c_inst_t *i2c) {
    const size_t IC_TX_BUFFER_DEPTH = 16;
    return IC_TX_BUFFER_DEPTH - i2c_get_hw(i2c)->txflr;
}

/*! \brief 受信できるバイト数をチェックする
 *  \ingroup hardware_i2c
 *
 * \param i2c \ref i2c0 か \ref i2c1 のいずれか
 * \return データがない場合は 0, 0 以外の場合は少なくともこのバイト数
 * だけはブロックなしで読み込むことができる.
 */
static inline size_t i2c_get_read_available(i2c_inst_t *i2c) {
    return i2c_get_hw(i2c)->rxflr;
}

/*! \brief TX FIFOに直接書き込む
 *  \ingroup hardware_i2c
 *
 * \param i2c \ref i2c0 か \ref i2c1 のいずれか
 * \param src 送信するデータ
 * \param len 送信されたバイト数
 *
 * I2C FIFOに直接書き込みます。主にスレーブモードの操作に便利です。
 */
static inline void i2c_write_raw_blocking(i2c_inst_t *i2c, const uint8_t *src, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        // TODO NACK or STOP on end?
        while (!i2c_get_write_available(i2c))
            tight_loop_contents();
        i2c_get_hw(i2c)->data_cmd = *src++;
    }
}

/*! \brief RX FIFOから直接読み込む
 *  \ingroup hardware_i2c
 *
 * \param i2c \ref i2c0 か \ref i2c1 のいずれか
 * \param dst データを受け取るバッファ
 * \param len 読み込むバイト数
 *
 * I2C RX FIFOから直接読み込みます。主にスレーブモードの操作に便利です。
 */
static inline void i2c_read_raw_blocking(i2c_inst_t *i2c, uint8_t *dst, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        while (!i2c_get_read_available(i2c))
            tight_loop_contents();
        *dst++ = (uint8_t)i2c_get_hw(i2c)->data_cmd;
    }
}

/**
 * \brief I2C Rx FIFOから1バイト取り出す
 * \ingroup hardware_i2c
 *
 * この関数はノンブロッキングです。RX FIFOが空でないことを仮定しています。
 *
 * \param i2c I2Cインスタンス.
 * \return uint8_t バイト値.
 */
static inline uint8_t i2c_read_byte_raw(i2c_inst_t *i2c) {
    i2c_hw_t *hw = i2c_get_hw(i2c);
    assert(hw->status & I2C_IC_STATUS_RFNE_BITS); // Rx FIFO must not be empty
    return (uint8_t)hw->data_cmd;
}

/**
 * \brief I2C Tx FIFOに1バイト置く
 * \ingroup hardware_i2c
 *
 * この関数はノンブロッキングです。TX FIFOがフルでないことを仮定しています。
 *
 * \param i2c I2Cインスタンス.
 * \param value バイト値.
 */
static inline void i2c_write_byte_raw(i2c_inst_t *i2c, uint8_t value) {
    i2c_hw_t *hw = i2c_get_hw(i2c);
    assert(hw->status & I2C_IC_STATUS_TFNF_BITS); // Tx FIFO must not be full
    hw->data_cmd = value;
}


/*! \brief 指定のI2Cインスタンスとの間の転送のペース設定に使用するDREQを返す
 *  \ingroup hardware_i2c
 *
 * \param i2c \ref i2c0 か \ref i2c1 のいずれか
 * \param is_tx I2Cインスタンスにデータを送信する場合は true, 受信する場合は false
 */
static inline uint i2c_get_dreq(i2c_inst_t *i2c, bool is_tx) {
    static_assert(DREQ_I2C0_RX == DREQ_I2C0_TX + 1, "");
    static_assert(DREQ_I2C1_RX == DREQ_I2C1_TX + 1, "");
    static_assert(DREQ_I2C1_TX == DREQ_I2C0_TX + 2, "");
    return DREQ_I2C0_TX + i2c_hw_index(i2c) * 2 + !is_tx;
}

#ifdef __cplusplus
}
#endif

#endif
