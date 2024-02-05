**host** ディレクトリにはコンピュータ（Raspberry Pi OSやLinux、macOS、Cygwinや
Windows Subsystem for Linuxを使用したWindows）上で簡単なアプリケーションを実行
できるようにするための基本的な代替ライブラリの実装が含まれています。CMakeビルドで`PICO_PLATFORM=host`とすることで選択されます。

より高度なアプリケーションコードのテストやデバッグ、あるいは、RP2040で実行できる
ほど小さくはないコードの移植に非常に便利です。

この基本レベルのホストライブラリはプログラムをコンパイルするための最小限の環境を
提供しますが、ハードウェアに直接アクセスしないプログラムには十分だと思われます。

しかし、より完全な機能を提供するために他のSDKライブラリの実装やシミュレーションを注入
することが可能です。この例としては、pico_multicore、pico-timeのタイマー/アラーム、
[pico-extras](https://github.com/raspberrypi/pico-extras)のpico-audio/pico-scanvideoに
SDL2ライブラリのサポートを追加した[pico-host-sdl](https://github.com/raspberrypi/pico-host-sdl)を
参照してください。
