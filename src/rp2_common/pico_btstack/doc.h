/**
 * \defgroup pico_btstack pico_btstack
 * \brief Integration/wrapper libraries for <a href="https://github.com/bluekitchen/btstack">BTstack</a>用の統合/ラッパーライブラリ.
 * BTstackのドキュメントは<a href="https://bluekitchen-gmbh.com/btstack/">ここ</a>にあります。
 *
 * BTstackの補足ライセンス（純正BTstackライセンス条項に追加）は<a href="https://github.com/raspberrypi/pico-sdk/blob/master/src/rp2_common/pico_btstack/LICENSE.RP">ここ</a>にあります。
 *
 * \c \b pico_btstack_ble ライブラリはBluetoothLowEnergy (BLE) に必要なサポートを
 * 追加します。\c \b pico_btstack_classic ライブラリは Bluetooth Classic に必要な
 * サポートを追加します。いずれかのライブラリに個別にリンクすることもできますし、
 * 両方のライブラリをリンクすることでBTstackが提供するデュアルモードサポートも
 * 実現できます。
 *
 * BTstackを使用するには、ソースツリーに \c btstack_config.h ファイルを用意し、
 * その場所をインクルードパスに追加する必要があります。
 *  \c pico_btstack_classic と \c pico_btstack_ble BTstack をリンクすると各々
 * 構成マクロである \c ENABLE_CLASSIC と \c ENABLE_BLE が定義されますので
 * 自分で定義する必要はありません。
 *
 * 詳細は、<a href="https://bluekitchen-gmbh.com/btstack/develop/#how_to/">BTstackの構成法</a>と
 * 関連する <a href="https://github.com/raspberrypi/pico-examples#pico-w-bluetooth">pico-examples</a> を参照してください。
 *
 * 以下のライブラリが提供されており、リンクすることができます。
 * * \c \b pico_btstack_ble - Bluetooth Low Energy (LE) サポートを追加します.
 * * \c \b pico_btstack_classic - Bluetooth Classic サポートを追加します.
 * * \c \b pico_btstack_sbc_encoder - Bluetooth Sub Band Coding (SBC) エンコーダサポートを追加します.
 * * \c \b pico_btstack_sbc_decoder - Bluetooth Sub Band Coding (SBC) デコーダサポートを追加します.
 * * \c \b pico_btstack_bnep_lwip - lwIPを使ってBluetooth Network Encapsulation Protocol (BNEP) サポートを追加します.
 * * \c \b pico_btstack_bnep_lwip_sys_freertos - NO_SYS=0のFreeRtOSでlwIPを使ってBluetooth Network Encapsulation Protocol (BNEP) サポートを追加します.
 *
 * \note CMake関数 pico_btstack_make_gatt_header を使用してBTstack compile_gatt ツールを実行し、BTstack GATTファイルからGATTヘッダーファイルを作成することができます。
 *
 * \sa pico_cyw43_driver の pico_btstack_cyw43 , これはBTstack実行ループサポートなどの
 * BTstackに必要なcyw43ドライバのサポートを追加します。
 */
