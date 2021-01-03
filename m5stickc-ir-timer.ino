#include <M5StickC.h>
#include <WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "wifi-ssid.h"

// -----------------------------------------------------------------------------
// 定数
// -----------------------------------------------------------------------------
// 赤外線LEDのピン番号
// M5StickC内蔵の赤外線LEDを使用する場合は9
// Grove接続のIRユニットを使用する場合は32
const uint8_t IR_SEND_PIN = 9;
// 赤外線送信するデータ
const uint64_t IR_SEND_DATA = 0x41B6659A;
// GMTからの時間差(秒)
const long JST = 9 * 60 * 60;
// NTPサーバ
const char *NTP_SERVER = "ntp.nict.jp";
// 電源ボタンが1秒未満押された
const uint8_t AXP_WAS_PRESSED = 2;
// ボタンが長押しされたと判定する時間(ms)
const uint32_t BUTTON_PRESSED_MS = 500;
// 最後にボタンを押してから画面を省電力にするまでの時間(ms)
const unsigned long SCREEN_OFF_MS = 3000;
// 通常時の画面輝度
const uint8_t SCREEN_ON_BRIGHTNESS = 12;
// 省電力時の画面輝度
const uint8_t SCREEN_OFF_BRIGHTNESS = 8;

// -----------------------------------------------------------------------------
// 変数
// -----------------------------------------------------------------------------
// IRremoteESP8266のIRsendクラス
IRsend irsend(IR_SEND_PIN);
// 現在時刻
struct tm now;
// 赤外線送信する時刻の時間
int timer_hour = 7;
// 赤外線送信する時刻の分
int timer_min = 0;
// 赤外線送信済みならtrue
bool ir_sent = false;
// 最後にボタンが押された時間
unsigned long button_pressed_millis = 0;

// -----------------------------------------------------------------------------
// 関数
// -----------------------------------------------------------------------------
// 現在時刻を表示
void showCurrentTime() {
    char message[50];

    sprintf(
        message,
        " now:\n  %04d/%02d/%02d %02d:%02d:%02d\n timer:\n  %02d:%02d",
        now.tm_year + 1900,
        now.tm_mon + 1,
        now.tm_mday,
        now.tm_hour,
        now.tm_min,
        now.tm_sec,
        timer_hour,
        timer_min);

    M5.Lcd.setCursor(0, 0);
    M5.Lcd.print(message);
}

// 無線LAN接続
void connectWiFi(const char *ssid, const char *passphrase) {
    M5.Lcd.printf("Connecting to %s", ssid);
    WiFi.begin(ssid, passphrase);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        M5.Lcd.print(".");
    }
    M5.Lcd.print("\nconnected!");
    delay(500);
    M5.Lcd.fillScreen(BLACK);
}

// 画面の輝度を上げる
void screenOn() {
    M5.Axp.ScreenBreath(SCREEN_ON_BRIGHTNESS);
    button_pressed_millis = millis();
}

// 画面の輝度を下げる
void screenOff() {
    M5.Axp.ScreenBreath(SCREEN_OFF_BRIGHTNESS);
}

void setup() {
    M5.begin();

    // 画面の表示設定
    M5.Lcd.setRotation(1);
    M5.Lcd.setTextFont(2);
    // ピンモードの設定
    pinMode(IR_SEND_PIN, OUTPUT);
    // 無線LANへ接続
    connectWiFi(WIFI_SSID, WIFI_PASSPHRASE);
    // NTPの設定
    configTime(JST, 0, NTP_SERVER);
}

void loop() {
    M5.update();

    // ボタンAが押されたらアラーム時刻の時間を変更
    if (M5.BtnA.wasPressed() || M5.BtnA.pressedFor(BUTTON_PRESSED_MS)) {
        timer_hour++;
        if (timer_hour > 23) timer_hour = 0;

        screenOn();
    }
    // ボタンBが押されたらアラーム時刻の分を変更
    if (M5.BtnB.wasPressed() || M5.BtnB.pressedFor(BUTTON_PRESSED_MS)) {
        timer_min++;
        if (timer_min > 59) timer_min = 0;

        screenOn();
    }
    // 電源ボタンが押されたらリセット
    if (M5.Axp.GetBtnPress() == AXP_WAS_PRESSED) {
        esp_restart();
    }

    // 現在時刻を取得
    getLocalTime(&now);
    showCurrentTime();

    // 最後にボタンを押してから時間が経過している場合は画面を省電力化
    if ((millis() - button_pressed_millis) > SCREEN_OFF_MS) {
        screenOff();
    }

    if (now.tm_hour == timer_hour && now.tm_min == timer_min && now.tm_sec == 0) {
        // 赤外線送信していなければ電源ON信号を送信
        if (!ir_sent) {
            irsend.sendNEC(IR_SEND_DATA);
            ir_sent = true;
        }
    } else {
        ir_sent = false;
    }

    delay(100);
}
