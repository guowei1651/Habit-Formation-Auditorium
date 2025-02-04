#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "../include/esp32_pins.h"
#include "../include/esp32_button_interrupts.h"
#include "../include/esp32_display.h"

// 防抖延时
const unsigned long DEBOUNCE_DELAY = 200;
unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
unsigned long lastDebounceTime3 = 0;

// 外部声明显示对象
extern Display display;

void buttonBegin() {
    // 按钮初始化
    pinMode(BUTTON1_PIN, INPUT_PULLUP);
    pinMode(BUTTON2_PIN, INPUT_PULLUP);
    pinMode(BUTTON3_PIN, INPUT_PULLUP);

    // 设置中断
    attachInterrupt(digitalPinToInterrupt(BUTTON1_PIN), handleButton1, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON2_PIN), handleButton2, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON3_PIN), handleButton3, FALLING);
}

// 按钮1处理函数 - 调用远程API 1
void IRAM_ATTR handleButton1() {
    unsigned long currentTime = millis();
    if (currentTime - lastDebounceTime1 > DEBOUNCE_DELAY) {
        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;
            http.begin("http://your-api-endpoint-1.com/api");
            int httpCode = http.GET();
            if (httpCode > 0) {
                String payload = http.getString();
                Serial.println(payload);
            }
            http.end();
        }
        lastDebounceTime1 = currentTime;
    }
}

// 按钮2处理函数 - 调用远程API 2
void IRAM_ATTR handleButton2() {
    unsigned long currentTime = millis();
    if (currentTime - lastDebounceTime2 > DEBOUNCE_DELAY) {
        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;
            http.begin("http://your-api-endpoint-2.com/api");
            int httpCode = http.GET();
            if (httpCode > 0) {
                String payload = http.getString();
                Serial.println(payload);
            }
            http.end();
        }
        lastDebounceTime2 = currentTime;
    }
}

// 按钮3处理函数 - 显示配置页面
void IRAM_ATTR handleButton3() {
    unsigned long currentTime = millis();
    if (currentTime - lastDebounceTime3 > DEBOUNCE_DELAY) {
        display.clearScreen();
        display.downloadAndDisplayImage("http://your-config-page-image.com/config.bmp");
        lastDebounceTime3 = currentTime;
    }
}