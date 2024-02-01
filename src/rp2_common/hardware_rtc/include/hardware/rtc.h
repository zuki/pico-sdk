/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _HARDWARE_RTC_H
#define _HARDWARE_RTC_H

#include "pico.h"
#include "hardware/structs/rtc.h"

/** \file hardware/rtc.h
 *  \defgroup hardware_rtc hardware_rtc
 *
 * \brief ハードウェアリアルタイムクックAPI
 *
 * RTCは人間が読めるフォーマットで時間を記録し、時間がプリセット値と
 * 等しくなった際にイベントを発生させます。ほとんどのコンピューターで
 * 使用されているエポックタイムではなく、デジタル時計を思い浮かべて
 * ください。年（12ビット）、月（4ビット）、日（5ビット）、曜日（3ビット）、
 * 時（5ビット）、分（6ビット）、秒（6ビット）の7つのフィールドがあり、
 * データはバイナリ形式で格納されます。
 *
 * \sa datetime_t
 *
 * \subsection rtc_example サンプルコード
 * \addtogroup hardware_rtc
 *
 * \include hello_rtc.c
 */

#ifdef __cplusplus
extern "C" {
#endif

/*! RTCアラームのコールバック関数型
 *  \ingroup hardware_rtc
 *
 * \sa rtc_set_alarm()
 */
typedef void (*rtc_callback_t)(void);

/*! \brief RTCシステムを初期化する
 *  \ingroup hardware_rtc
 */
void rtc_init(void);

/*! \brief 指定した時間をRTCにセットする
 *  \ingroup hardware_rtc
 *
 * \note RTCの日付と時刻を設定した後、その値を（rtc_get_datetime()などで）
 * 読み取っても、潜在的に非常に遅いRTCクロックドメインの最大3サイクルが
 * 経過するまでは新しい設定が反映されない可能性があることに注意してください。
 * これはデフォルトのRTCクロックの構成では64マイクロ秒の周期に相当します。
 *
 * \param t セットする時間を含む \ref datetime_t 構造体へのポインタ
 * \return セットされた場合は true, 指定した日時が不正な場合は false
 */
bool rtc_set_datetime(datetime_t *t);

/*! \brief RTCから現在の時間を取得する
 *  \ingroup hardware_rtc
 *
 * \param t 現在のRTC時間を受け取る \ref datetime_t 構造体へのポインタ
 * \return 日時が有効な場合は true, RTCが動いていない場合は false
 */
bool rtc_get_datetime(datetime_t *t);

/*! \brief RTCが動いているかチェックする
 *  \ingroup hardware_rtc
 *
 */
bool rtc_running(void);

/*! \brief RTCがユーザ提供のコールバックを呼び出す未来の時間を設定する
 *  \ingroup hardware_rtc
 *
 *  \param t アラームを発火する将来の時間を含む \ref datetime_t 構造体へのポインタ. -1にセットされた値にはマッチしません
 *  \param user_callback アラームが発火した際に呼び出す \ref rtc_callback_t へのポインタ
 */
void rtc_set_alarm(datetime_t *t, rtc_callback_t user_callback);

/*! \brief RTCアラームを（アクティブであれば）有効にする
 *  \ingroup hardware_rtc
 */
void rtc_enable_alarm(void);

/*! \brief RTCアラームを（アクティブであれば）無効にする
 *  \ingroup hardware_rtc
 */
void rtc_disable_alarm(void);

#ifdef __cplusplus
}
#endif

#endif
