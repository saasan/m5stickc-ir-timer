# m5stickc-ir-timer

M5StickCで設定時刻に赤外線信号を送信する

## Wi-FiのSSIDとパスフレーズ

起動時にWi-Fiへ接続しNTPで時刻を合わせるため、
wifi-ssid.hという名前でファイルを作成し
以下の内容を書き込んでおく必要があります。

    // Wi-FiのSSID
    const char *WIFI_SSID = "Wi-FiのSSID";
    // Wi-Fiのパスフレーズ
    const char *WIFI_PASSPHRASE = "Wi-Fiのパスフレーズ";

## IRユニットを使用する場合

Grove接続のIRユニットを使用する場合はIR_SEND_PINを32へ変更してください。

## 時刻設定

デフォルトのタイマー設定は午前7時になっていますが、
ボタンA(正面の「M5」ボタン)で時、
ボタンB(側面の小さいボタン)で分を変更できます。
