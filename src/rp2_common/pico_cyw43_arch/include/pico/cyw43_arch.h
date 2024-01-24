/*
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_CYW43_ARCH_H
#define _PICO_CYW43_ARCH_H

#include "pico.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "cyw43.h"
#include "cyw43_country.h"
#include "pico/async_context.h"

#ifdef PICO_CYW43_ARCH_HEADER
#include __XSTRING(PICO_CYW43_ARCH_HEADER)
#else
#if PICO_CYW43_ARCH_POLL
#include "pico/cyw43_arch/arch_poll.h"
#elif PICO_CYW43_ARCH_THREADSAFE_BACKGROUND
#include "pico/cyw43_arch/arch_threadsafe_background.h"
#elif PICO_CYW43_ARCH_FREERTOS
#include "pico/cyw43_arch/arch_freertos.h"
#else
#error must specify support pico_cyw43_arch architecture type or set PICO_CYW43_ARCH_HEADER
#endif
#endif

/**
 * \defgroup cyw43_driver cyw43_driver
 * \ingroup pico_cyw43_arch
 * \brief Pico W wirelessで使用されれるドライバ
*/

/**
 * \defgroup cyw43_ll cyw43_ll
 * \ingroup cyw43_driver
 * \brief 低水準CYW43ドライバインタフェース
*/

/** \file pico/cyw43_arch.h
 *  \defgroup pico_cyw43_arch pico_cyw43_arch
 *
 * CYW43ドライバ（Pico Wのワイヤレス用）とlwIP（TCP/IPスタック用）をSDKに統合する
 * ためのアーキテクチャです。Pico WのオンボードLEDにアクセスするためにも必要です。
 *
 * 低水準 \c cyw43_driver とlwIPスタックは定期的なサービスを必要とします。また、
 * 複数のコア/スレッドから呼び出すことが可能か否かについては制限があります。
 *
 * \c pico_cyw43_arch はこれらの複雑性をいくつかの振る舞いグループに抽象化しています。
 *
 * * \em 'poll' - マルチコア/IRQセーフではありません。メインループから定期的に \ref cyw43_arch_poll を呼び出す必要があります。
 * * \em 'thread_safe_background' - マルチコア/スレッド/タスクセーフです。ドライバとTCP/IPスタックのメンテナンスはバックグラウンドで自動的に処理されます。
 * * \em 'freertos' - マルチコア/スレッド/タスクセーフです。lwIPとドライバ作業の処理に個別のFreeRTOSタスクを使用します。
 *
 * 現時点ではサポートされているTCP/IPスタックはlwIPだけですが、\c pico_cyw43_arch  の使用は
 * 将来的には特定のTCP/IPスタックに（また、おそらくはBluetoothスタックにも）依存しないように
 * することをめざしています。そのため、lwIP の統合は、\c cyw43_driverで使用される
 * #define \ref CYW43_LWIP に基づいてベースライブラリ（ \ref pico_ciw43_arch）で処理されます。
 *
 * \note Raspberry Pi Pico SDKバージョン 1.5.0以降、 \c pico_cyw43_arch ライブリは個別の動作抽象を
 * 直接実装しなくなり、より汎用的な \ref pico_async_context ライブラリで処理するようになりました。
 * しかし、ユーザが直面するpico_cyw43_archの振る舞いはこの実証の詳細の結果変わりません。
 * pico_cyw43_archは今や、適切なasync_contextを作成し、必要に応じてlwIPまたはcyw43_driverサポートを
 * 追加する単純な呼び出しを行う、単なる薄いラッパーになったからです。このコンテキストの作成や
 * lwIP、cyw43_driver、さらに将来追加されるであろう任意のプロトコル/ドライバサポートを
 * async_context に自由に追加することができますが、現時点ではpico_cyw43_archはいぜんとして
 * 接続管理やロック、GPIOインタラクションのためのcyw43_ 固有（すなわち、Pico W用)の APIを提供しています。
 *
 * \note 将来のリリースでは少なくとも接続管理APIはより汎用的なライブラリに移行される可能性が
 * あります。ロック関数は、今のところ \ref pico_async_context の相当関数によりバックアップされて
 * おり、 それらの関数は互換的に使用することができます（詳細については、 \ref cyw43_arch_lwip_begin,
 *  \ref cyw43_arch_lwip_end,  \ref cyw43_arch_lwip_check を参照してください）。
 *
 * \note 独自のasync_contextの作成、 \c cyw43_driver や \c lwIP サポートの追加の例については、
 * ソースファイル \c cyw43_arch_poll.cや \c cyw43_arch_threadsafe_background.c 、
 * \c cyw43_arch_freertos.cを参照してください。
 *
 * \c pico_cyw43_arch ライブラリを直接使用して \ref CYW43_LWIP （やその他の定義）を自分で指定する
 * こともできますが、定義やその他の依存関係を集約してビルドするために使用することができるライブラリも
 * あります。
 *
 * * \b pico_cyw43_arch_lwip_poll - RAW lwIP API（`NO_SYS=1`モード）をバックグラウンド処理やマルチコア/スレッドセーフなしで使用する。
 *    ユーザはメインループから定期的に \ref cyw43_arch_pollを呼び出す必要があります。
 *
 *    このラッパーライブラリは
 *    - \c CYW43_LWIP=1 を設定して \c pico_cyw43_arch と \c cyw43_driver でlwIP サポートを有効にします。
 *    - \c PICO_CYW43_ARCH_POLL=1 を設定してポーリング処理を選択します。
 *    - lwIPを取り込むために依存関係に \c pico_lwip を追加します。
 *
 * * \b pico_cyw43_arch_lwip_threadsafe_background - RAW lwIP APIを (`NO_SYS=1` モードで)マルチコア/スレッドセーフで使用し、バックグラウンドで自動的に  \c cyw43_driver と lwIP を処理します。
 *
 *    \c cyw43_driver の高レベルAPI (cyw43.h) の呼び出しはいずれかのコアから、またはlwIPコールバック
 * から可能ですが、lwIPコールバック以外からの（（スレッドセーフではない）lwIPへの呼び出しは必ず
 * \ref cyw43_arch_lwip_begin と \ref cyw43_arch_lwip_end で囲んでください。lwIPコールバック内から
 * 行われる呼び出しも囲むことも問題ありません。
 *
 *    \note lwIPコールバックは（アラームコールバックと同様に）（優先順位の低い）IRQコンテキストで発生することがあるので他のコードとやりとりする際には注意が必要です。
 *
 *    このラッパーライブラリは
 *    - \c CYW43_LWIP=1 を設定して \c pico_cyw43_arch と \c cyw43_driver でlwIPサポートを有効にします。
 *    - \c PICO_CYW43_ARCH_THREADSAFE_BACKGROUND=1 を設定してスレッドセーフな非ポーリング処理を選択します。
 *    - lwIPを取り込むために依存関係に \c pico_lwip を追加します。
 *
 *    このライブラリはFreeRTOSのRP2040ポートでもlwIPを`NO_SYS=1`モードで使用することが
 * \em できます (どのタスクからでも \c cyw43_driver APIを呼び出すことができます。また、
 * lwIPコールバックから、あるいは、呼び出しを \ref cyw43_arch_lwip_begin と \ref cyw43_arch_lwip_end
 * で囲むことにより任意のタスクから、lwIPを呼び出すことができます）。ここでもlwIPコールバックで行う
 * ことについては注意が必要です。IRQコンテキスト内からはほとんどのFreeRTOS APIを呼び出すことが
 * できないからです。よほどの理由がない限り、 \c pico_cyw43_arch_lwip_sys_freertos で提供される
 * 完全なFreeRTOS統合 (`NO_SYS=0`) を使うべきです。
 *
 * * \b pico_cyw43_arch_lwip_sys_freertos - OS(`NO_SYS=0`)モードでのブロッキングソケット、マルチ
 * コア/タスク/スレッドセーフ、 \c cyw43_driver とlwIPスタックの自動サービスなど、lwIP APIをすべて
 * 使用します。
 *
 *    このラッパーライブラリは
 *    - \c CYW43_LWIP=1 を設定して \c pico_cyw43_arch と \c cyw43_driver でlwIPサポートを有効にします。
 *    - \c PICO_CYW43_ARCH_FREERTOS=1 を設定して NO_SYS=0 lwip/FreeRTOS 統合を選択します。
 *    - \c LWIP_PROVIDE_ERRNO=1 を設定してOSなしのコンパイルに必要なエラー番号を提供します。
 *    - lwIPを取り込むために依存関係に \c pico_lwip を追加します。
 *    - （lwIPのcontribディレクトリにある）lwip-contrib からlwIP/FreeRTOSのコードを追加します。
 *
 *    \c cyw43_driver の高レベルAPI (cyw43.h) の呼び出しは任意のタスクから、また、lwIPコールバックから
 *    行えますが、IRQからは行えません。（スレッドセーフでない）lwIP RAW APIの呼び出しは必ず
 *    \ref cyw43_arch_lwip_begin と \ref cyw43_arch_lwip_end で囲んでください。lwIPコールバック内から
 *    の呼び出しをブラケットで囲むことは問題ありませんが、する必要はありません。
 *
 *    \note このラッパーライブラリは各自でFreeRTOSの機能をアプリケーションにリンクする必要があります。
 *
 * * \b pico_cyw43_arch_none - TCP/IPスタックは必要ないがオンボードLEDを使用する場合に指定します。
 *
 *    このラッパーライブラリは
 *    - Sets \c CYW43_LWIP=0 を設定して \c pico_cyw43_arch と \c cyw43_driver でlwIPサポートを無効にします。
 */

// PICO_CONFIG: PARAM_ASSERTIONS_ENABLED_CYW43_ARCH, Enable/disable assertions in the pico_cyw43_arch module, type=bool, default=0, group=pico_cyw43_arch
#ifndef PARAM_ASSERTIONS_ENABLED_CYW43_ARCH
#define PARAM_ASSERTIONS_ENABLED_CYW43_ARCH 0
#endif

// PICO_CONFIG: PICO_CYW43_ARCH_DEBUG_ENABLED, Enable/disable some debugging output in the pico_cyw43_arch module, type=bool, default=1 in debug builds, group=pico_cyw43_arch
#ifndef PICO_CYW43_ARCH_DEBUG_ENABLED
#ifndef NDEBUG
#define PICO_CYW43_ARCH_DEBUG_ENABLED 1
#else
#define PICO_CYW43_ARCH_DEBUG_ENABLED 0
#endif
#endif

// PICO_CONFIG: PICO_CYW43_ARCH_DEFAULT_COUNTRY_CODE, Default country code for the cyw43 wireless driver, default=CYW43_COUNTRY_WORLDWIDE, group=pico_cyw43_arch
#ifndef PICO_CYW43_ARCH_DEFAULT_COUNTRY_CODE
#define PICO_CYW43_ARCH_DEFAULT_COUNTRY_CODE CYW43_COUNTRY_WORLDWIDE
#endif

/*!
 * \brief CYW43アーキテクチャを初期化する
 * \ingroup pico_cyw43_arch
 *
 * この関数は`cyw43_driver`コードを初期化し、（ビルド時に有効になっている場合は）
 * lwIPスタックを初期化します。この関数は他の \c pico_cyw43_arch,  \c cyw43_driver,
 * lwIP関数を使用する前に呼び出す必要があります。
 *
 * \note この関数はワイヤレスを \c PICO_CYW43_ARCH_DEFAULT_COUNTRY_CODE で
 * ワイヤレスを初期化します。デフォルトは \c CYW43_COUNTRY_WORLDWIDE です。
 * この設定では最適なパフォーマンスが得られない可能性があります;
 * PICO_CYW43_ARCH_DEFAULT_COUNTRY_CODEに別の値に設定するか、
 * \ref cyw43_arch_init_with_country を呼び出すことを検討してください。
 *
 * デフォルトでは、この関数は \ref cyw43_arch_init_default_async_context を呼び出して
 * cyw43_archコードのasync_contextを初期化しますが、この関数を呼び出す前に
 * \ref cyw43_arch_set_async_context() を呼び出すことにより独自のasync_contextの使用を
 * 指定することができます。
 *
 * \return 初期化が成功した場合は 0. そうでない場合はエラーコード \see pico_error_codes
 */
int cyw43_arch_init(void);

/*!
 * \brief 指定の国で使用するようにCYW43アーキテクチャを初期化する
 * \ingroup pico_cyw43_arch
 *
 * この関数は`cyw43_driver`コードを初期化し、（ビルド時に有効になっている場合は）
 * lwIPスタックを初期化します。この関数は他の \c pico_cyw43_arch,  \c cyw43_driver,
 * lwIP関数を使用する前に呼び出す必要があります。
 *
 * * デフォルトでは、この関数は \ref cyw43_arch_init_default_async_context を呼び出して
 * cyw43_archコードのasync_contextを初期化しますが、この関数を呼び出す前に
 * \ref cyw43_arch_set_async_context() を呼び出すことにより独自のasync_contextの使用を
 * 指定することができます。
 *
 * \param country 使用する国コード ( \ref CYW43_COUNTRY_ を参照)
 * \return 初期化が成功した場合は 0. そうでない場合はエラーコード \see pico_error_codes
 */
int cyw43_arch_init_with_country(uint32_t country);

/*!
 * \brief CYW43アーキテクチャを解放する
 * \ingroup pico_cyw43_arch
 *
 * この関数は`cyw43_driver`コードを解放し、（ビルド時に有効になっていた場合は）
 * lwIPスタックを解放します。この関数は常に  \ref cyw43_arch_init を実行したのと
 * 同じコア（あるいは、完了によってはRTOSタスク）から呼び出す必要があります。
 *
 * さらに、cyw43_archが独自のasync_contextインスタンスを使用している場合は、
 * そのインスタンスも解放されまう。
 */
void cyw43_arch_deinit(void);

/*!
 * \brief cyw43_archコードで現在使用されているasync_contextを返す
 * \ingroup pico_cyw43_arch
 *
 * \return async_context.
 */
async_context_t *cyw43_arch_async_context(void);

/*!
 * \brief cyw43_arch_initで使用するasync_contextをセットする
 * \ingroup pico_cyw43_arch
 *
 * \note 独自のasync_contextインスタンスを使用したい場合は cyw43_arch_init または
 * cyw43_arch_init_with_country を呼び出す前にこの関数を呼び出す必要があります。
 *
 * \param context 使用する async_context
 */
void cyw43_arch_set_async_context(async_context_t *context);

/*!
 * \brief カレントcyw43_archタイプとしてデフォルトのasync_contextを初期化する
 * \ingroup pico_cyw43_arch
 *
 * この関数はcyw43_archに関連付けられたstaticなasync_contextへのポインタを初期化して
 * 返します。この関数は \ref cyw43_arch_set_async_context によって異なる async_context が
 * 設定されていない場合、自動的に \ref cyw43_arch_init によって呼び出されます。
 *
 * \return context、または、初期化が失敗した場合は NULL
 */
async_context_t *cyw43_arch_init_default_async_context(void);

/*!
 * \brief \c cyw43_driver またはTCP/IPスタックにより要求された任意の処理を実行する
 * \ingroup pico_cyw43_arch
 *
 * この関数は \em polling スタイルの \c pico_cyw43_arch （たとえば \c pico_cyw43_arch_lwip_poll ）を
 * 使用している場合、メインループから定期的に呼び出す必要があります。他のスタイルでも呼び出すことは
 * できますが、呼び出す必要はありません。
 */
void cyw43_arch_poll(void);

/*!
 * \brief 実行するべきcyw43_driver workが現れるまでスリープする
 * \ingroup pico_cyw43_arch
 *
 * この関数はcyw43_driverからのイベントを待っているおり、何もする
 * ことはないがcyw43_driverに関連するバックグラウンドworkをブロック
 * することなくスリープしたいコードによって呼び出すことができます。
 *
 * \param until するべきことが場合にその時まで待つ時間.
 */
void cyw43_arch_wait_for_work_until(absolute_time_t until);

/*!
 * \fn cyw43_arch_lwip_begin
 * \brief lwIPの呼び出しに必要なロックを取得する
 * \ingroup pico_cyw43_arch
 *
 * lwIP APIはスレッドセーフではありません。lwIP APIの呼び出しはこの関数の呼び出しと
 * \ref cyw43_arch_lwip_end の呼び出しで囲む必要があります。lwIP のコールバックから
 * lwIP API にコールバックする場合はこれらの呼び出しは不要なことに注意してください
 * （ただし呼び出しても害はありません）。シングルコアでポーリングだけ (pico_cyw43_arch_poll)
 * を使用している場合はこれらの呼び出しはいずれにせよno-opです。必要な場合はいつでも
 * これらの関数を呼び出すのは良い習慣です。
 *
 * \note SDKリリース1.5.0ではこの関数は cyw43_archとlwIPに関連付けられたasync_contextに対して
 * \ref async_context_acquire_lock_blocking を呼び出すことと同等になりました。
 *
 * \sa cyw43_arch_lwip_end
 * \sa cyw43_arch_lwip_protect
 * \sa async_context_acquire_lock_blocking
 * \sa cyw43_arch_async_context
 */
static inline void cyw43_arch_lwip_begin(void) {
    cyw43_thread_enter();
}

/*!
 * \fn void cyw43_arch_lwip_end(void)
 * \brief lwIPを呼び出すために必要なロックを解放する
 * \ingroup pico_cyw43_arch
 *
 * lwIP APIはスレッドセーフではありません。lwIP APIの呼び出しは  \ref cyw43_arch_lwip_begin と
 * この関数の呼び出しで囲む必要があります。lwIP のコールバックからlwIP API にコールバックする
 * 場合はこれらの呼び出しは不要なことに注意してください（ただし呼び出しても害はありません）。
 * シングルコアでポーリングだけ (pico_cyw43_arch_poll) を使用している場合はこれらの呼び出しは
 * いずれにせよno-opです。必要な場合はいつでもこれらの関数を呼び出すのは良い習慣です。
 *
 * \note SDKリリース1.5.0ではこの関数は cyw43_archとlwIPに関連付けられたasync_contextに対して
 * \ref async_context_release_lock を呼び出すことと同等になりました。
 *
 * \sa cyw43_arch_lwip_begin
 * \sa cyw43_arch_lwip_protect
 * \sa async_context_release_lock
 * \sa cyw43_arch_async_context
 */
static inline void cyw43_arch_lwip_end(void) {
    cyw43_thread_exit();
}

/*!
 * \fn int cyw43_arch_lwip_protect(int (*func)(void *param), void *param)
 * \brief lwIPの呼び出しをするロックが必要な関数を保護する
 * \ingroup pico_cyw43_arch
 *
 * lwIP APIはスレッドセーフではありません。この関数を使用してlwIP APIの
 * 呼び出しにロックが必要な関数をラップすることができます。
 * シングルコアでポーリングだけ (pico_cyw43_arch_poll) を使用している場合はロックは
 * 不要ですが、それでもこの関数を使用するのは良い習慣です。
 *
 * \param func ロックの取得が必要な呼び出しをする関数
 * \param param \c func にわたす引数
 * \return \c func からの返り値
 * \sa cyw43_arch_lwip_begin
 * \sa cyw43_arch_lwip_end
 */
static inline int cyw43_arch_lwip_protect(int (*func)(void *param), void *param) {
    cyw43_arch_lwip_begin();
    int rc = func(param);
    cyw43_arch_lwip_end();
    return rc;
}

/*!
 * \fn void cyw43_arch_lwip_check(void)
 * \brief CallerがlwIPの呼び出しに必要なロックを保持しているかチェックする
 * \ingroup pico_cyw43_arch
 *
 * この関数はデバッグモードでは上記の条件を満たした（すなわち、lwIP APIの呼び出しが
 * 安全でない）場合アサートします。
 *
 * \note \ref cyw43_thread_lock_check に #define されている。
 *
 * \note SDKリリース1.5.0ではこの関数は cyw43_archとlwIPに関連付けられたasync_contextに対して
 * \ref async_context_lock_check を呼び出すことと同等になりました。
 *
 * \sa cyw43_arch_lwip_begin
 * \sa cyw43_arch_lwip_protect
 * \sa async_context_lock_check
 * \sa cyw43_arch_async_context
 */

/*!
 * \brief cyw43_archの初期化に使用された国コードを返す
 * \ingroup pico_cyw43_arch
 *
 * \return the country code (see \ref CYW43_COUNTRY_)
 */
uint32_t cyw43_arch_get_country_code(void);

/*!
 * \brief Wi-Fi STA (Station) モードを有効にする.
 * \ingroup pico_cyw43_arch
 *
 * Wi-Fi \em Station モードを有効にします。これにより他のWi-Fiアクセスポイントへの
 * 接続が可能になります。
 */
void cyw43_arch_enable_sta_mode(void);

/*!
 * \brief Wi-Fi STA (Station) モードを無効にする.
 * \ingroup pico_cyw43_arch
 *
 * Wi-Fi \em Station モードを無効にし、アクティブなすべての接続を切断します。
 * この関数に続いて \ref cyw43_wifi_link_status を呼び出してステータスを
 * チェックする必要があります。
 */
void cyw43_arch_disable_sta_mode(void);

/*!
 * \brief  Wi-Fi AP (Access point) モードを有効にする.
 * \ingroup pico_cyw43_arch
 *
 * Wi-Fi \em Access \em Point  モードを有効にします。これにより他のWi-Fi
 * クライアントからデバイスへの接続が可能になります。
 *
 * \param ssid アクセスポイントの名前
 * \param password 使用するパスワード、パスワードが設定されていない場合は NULL.
 * \param auth パスワードが有効な場合に使用する認証タイプ. 値は  \ref CYW43_AUTH_WPA_TKIP_PSK,
 *             \ref CYW43_AUTH_WPA2_AES_PSK, \ref CYW43_AUTH_WPA2_MIXED_PSK ( \ref CYW43_AUTH_ を参照)
 */
void cyw43_arch_enable_ap_mode(const char *ssid, const char *password, uint32_t auth);

/*!
 * \brief Wi-Fi AP (Access point) モードを無効にする.
 * \ingroup pico_cyw43_arch
 *
 * Wi-Fi \em Access \em Point モードを無効にします。
 */
void cyw43_arch_disable_ap_mode(void);

/*!
 * \brief ワイヤレスアクセスポイントへの接続を試み、ネットワークに参加するか、障害が検出されるまでブロックする.
 * \ingroup pico_cyw43_arch
 *
 * \param ssid 接続するネットワークの名前
 * \param pw 使用するネットワークのパスワード、パスワードが設定されていない場合は NULL.
 * \param auth パスワードが有効な場合に使用する認証タイプ. 値は  \ref CYW43_AUTH_WPA_TKIP_PSK,
 *             \ref CYW43_AUTH_WPA2_AES_PSK, \ref CYW43_AUTH_WPA2_MIXED_PSK ( \ref CYW43_AUTH_ を参照)
 *
 * \return 初期化が成功した場合は 0, それ以外はエラーコード. \see pico_error_codes
 */
int cyw43_arch_wifi_connect_blocking(const char *ssid, const char *pw, uint32_t auth);

/*!
 * \brief SSIDとBSSIDを指定してワイヤレスアクセスポイントへの接続を試み、ネットワークに参加するか、障害が検出されるまでブロックする.
 * \ingroup pico_cyw43_arch
 *
 * \param ssid 接続するネットワークの名前
 * \param bssid 接続するネットワックBSSID, 無視する場合は NULL
 * \param pw ネットワークパスワード、パスワードが不要な場合は NULL.
 * \param auth パスワードが有効な場合に使用する認証タイプ. 値は  \ref CYW43_AUTH_WPA_TKIP_PSK,
 *             \ref CYW43_AUTH_WPA2_AES_PSK, \ref CYW43_AUTH_WPA2_MIXED_PSK ( \ref CYW43_AUTH_ を参照)
 *
 * \return 初期化が成功した場合は 0, それ以外はエラーコード. \see pico_error_codes
 */
int cyw43_arch_wifi_connect_bssid_blocking(const char *ssid, const uint8_t *bssid, const char *pw, uint32_t auth);

/*!
 * \brief ワイヤレスアクセスポイントへの接続を試み、ネットワークに参加するか、障害が検出されるか、タイムアウトが発生するまでブロックする.
 * \ingroup pico_cyw43_arch
 *
 * \param ssid 接続するネットワークの名前
 * \param pw ネットワークパスワード、パスワードが不要な場合は NULL.
 * \param auth パスワードが有効な場合に使用する認証タイプ. 値は  \ref CYW43_AUTH_WPA_TKIP_PSK,
 *             \ref CYW43_AUTH_WPA2_AES_PSK, \ref CYW43_AUTH_WPA2_MIXED_PSK ( \ref CYW43_AUTH_ を参照)
 * \param timeout 接続が成功するまでどれだけ待つかをミリ秒単位で指定
 *
 * \return 初期化が成功した場合は 0, それ以外はエラーコード. \see pico_error_codes
 */
int cyw43_arch_wifi_connect_timeout_ms(const char *ssid, const char *pw, uint32_t auth, uint32_t timeout);

/*!
 * \brief SSIDとBSSIDを指定してワイヤレスアクセスポイントへの接続を試み、ネットワークに参加するか、障害が検出されるか、タイムアウトが発生するまでブロックする.
 * \ingroup pico_cyw43_arch
 *
 * \param ssid 接続するネットワークの名前
 * \param bssid 接続するネットワックBSSID, 無視する場合は NULL
 * \param pw ネットワークパスワード、パスワードが不要な場合は NULL.
 * \param auth パスワードが有効な場合に使用する認証タイプ. 値は  \ref CYW43_AUTH_WPA_TKIP_PSK,
 *             \ref CYW43_AUTH_WPA2_AES_PSK, \ref CYW43_AUTH_WPA2_MIXED_PSK ( \ref CYW43_AUTH_ を参照)
 * \param timeout 接続が成功するまでどれだけ待つかをミリ秒単位で指定
 *
 * \return 初期化が成功した場合は 0, それ以外はエラーコード. \see pico_error_codes
 */
int cyw43_arch_wifi_connect_bssid_timeout_ms(const char *ssid, const uint8_t *bssid, const char *pw, uint32_t auth, uint32_t timeout);

/*!
 * \brief ワイヤレスアクセスポイントへの接続を開始する
 * \ingroup pico_cyw43_arch
 *
 * この関数はCYW43ドライバにアクセスポイントへの接続を開始するように指示します。続けて
 * \ref cyw43_wifi_link_status を呼び出してステータスを確認する必要があります。
 *
 * \param ssid 接続するネットワークの名前
 * \param pw ネットワークパスワード、パスワードが不要な場合は NULL.
 * \param auth パスワードが有効な場合に使用する認証タイプ. 値は  \ref CYW43_AUTH_WPA_TKIP_PSK,
 *             \ref CYW43_AUTH_WPA2_AES_PSK, \ref CYW43_AUTH_WPA2_MIXED_PSK ( \ref CYW43_AUTH_ を参照)
 *
 * \return 初期化が成功した場合は 0, それ以外はエラーコード. \see pico_error_codes
 */
int cyw43_arch_wifi_connect_async(const char *ssid, const char *pw, uint32_t auth);

/*!
 * \brief SSIDとBSSIDを指定してワイヤレスアクセスポイントへの接続を開始する.
 * \ingroup pico_cyw43_arch
 *
 * この関数はCYW43ドライバにアクセスポイントへの接続を開始するように指示します。続けて
 * \ref cyw43_wifi_link_status を呼び出してステータスを確認する必要があります。
 *
 * \param ssid 接続するネットワークの名前
 * \param bssid 接続するネットワックBSSID, 無視する場合は NULL
 * \param pw ネットワークパスワード、パスワードが不要な場合は NULL.
 * \param auth パスワードが有効な場合に使用する認証タイプ. 値は  \ref CYW43_AUTH_WPA_TKIP_PSK,
 *             \ref CYW43_AUTH_WPA2_AES_PSK, \ref CYW43_AUTH_WPA2_MIXED_PSK ( \ref CYW43_AUTH_ を参照)
 * \param timeout 接続が成功するまでどれだけ待つかをミリ秒単位で指定
 *
 * \return 初期化が成功した場合は 0, それ以外はエラーコード. \see pico_error_codes
 */
int cyw43_arch_wifi_connect_bssid_async(const char *ssid, const uint8_t *bssid, const char *pw, uint32_t auth);

/*!
 * \brief ワイヤレスチップのGPIOピンに指定の値をセットする.
 * \ingroup pico_cyw43_arch
 * \note この関数はGPIOのセットエラーをチェックしません。エラーのチェックを行いたい場合は低水準関数
 * \ref cyw43_gpio_set を使用できます。
 *
 * \param wl_gpio ワイヤレスチップのGPIO番号
 * \param value GPIOをセットする場合は true, クリアする場合は false to clear it.
 */
void cyw43_arch_gpio_put(uint wl_gpio, bool value);

/*!
 * \brief ワイヤレスチップのGPIOピンの値を読み取る
 * \ingroup pico_cyw43_arch
 * \note この関数はGPIOのセットエラーをチェックしません。エラーのチェックを行いたい場合は低水準関数
 * \ref cyw43_gpio_get を使用できます。
 *
 * \param wl_gpio ワイヤレスチップのGPIO番号
 * \return GPIOがHighの場合は true, そうでなければ false
 */
bool cyw43_arch_gpio_get(uint wl_gpio);

#ifdef __cplusplus
}
#endif

#endif
