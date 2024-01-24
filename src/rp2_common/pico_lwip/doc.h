/**
 * \defgroup pico_lwip pico_lwip
 * \brief <a href="https://savannah.nongnu.org/projects/lwip/lwIP">lwIP</a>用の
 * 統合/ラッパーライブラリ。ドキュメントは<a href="https://www.nongnu.org/lwip/2_1_x/index.html">こちら</a>にあります.
 *
 * 主たる \c \b pico_lwip ライブラリ自体は次の lwIP RAW API を集約したものです:
 * \c \b pico_lwip_core, \c \b pico_lwip_core4, \c \b pico_lwip_core6,
 * \c \b pico_lwip_api, \c \b pico_lwip_netif, \c \b pico_lwip_sixlowpan,
 * \c \b pico_lwip_ppp.
 *
 * NO_SYS=1モードで動作させたい場合は \c \b pico_lwip とともに \ref pico_lwip_nosys
 * をリンクします。
 *
 * NO_SYS=0モードで動作させたい場合は \c \b pico_lwip とともに （たとえば）
 * \ref pico_lwip_freertos をリンクし、さらにブロッキング/スレッドセーフAPIを
 * 追加するために \ref pico_lwip_api もリンクします。
 *
 * さらに、lwIPに対する独自のコンパイラバインディングを提供しない限り、
 * \ref pico_lwip_arch をリンクする必要があります。
 *
 * その他のlwIP機能は以下のライブラリを個別にリンクすることでお好みで利用できます。
 *
 * 以下のライブラリはlwIPの機能と同等の機能を提供します。
 *
 * * \c \b pico_lwip_core -
 * * \c \b pico_lwip_core4 -
 * * \c \b pico_lwip_core6 -
 * * \c \b pico_lwip_netif -
 * * \c \b pico_lwip_sixlowpan -
 * * \c \b pico_lwip_ppp -
 * * \c \b pico_lwip_api -
 *
 * 以下のライブラリはlwIPがサポートしているアプリケーションと同等の機能を提供します。
 *
 * * \c \b pico_lwip_snmp -
 * * \c \b pico_lwip_http -
 * * \c \b pico_lwip_makefsdata -
 * * \c \b pico_lwip_iperf -
 * * \c \b pico_lwip_smtp -
 * * \c \b pico_lwip_sntp -
 * * \c \b pico_lwip_mdns -
 * * \c \b pico_lwip_netbios -
 * * \c \b pico_lwip_tftp -
 * * \c \b pico_lwip_mbedtls -
 * * \c \b pico_lwip_mqtt -
 *
 */

/** \defgroup pico_lwip_arch pico_lwip_arch
 * \ingroup pico_lwip
 * \brief lwIPのコンパイルアダプタです. これは、独自に実装したい場合に備えて存在します。
 * デフォルトでは \c \b pico_lwip には含まれていません.
 */
