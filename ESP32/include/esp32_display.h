#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <GxEPD2_3C.h>
#include <epd3c/GxEPD2_750c_Z08.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "esp32_pins.h"

#define NORMAL_FONT u8g2_font_wqy16_t_gb2312a // 设置NORMAL_FONT默认字体

struct DetailInfo {
    int year; // 年，其值等于实际年份减去 1900；
    int month; // 月，取值 0~11；
    int dayOfyear; // 年中的日期，取值 0~365，0 代表 1 月 1 日，1 代表 1 月 2 日，依此类推；
    int dayOfMonth; // 月中的日期，取值 1~31；
    int dayOfWeek; // 星期，取值 0~6，0 为周日，1 为周一，依此类推；
    int hour; // 时，取值 0~23；
    int minute; // 分，取值 0~59；
    int second; // 秒，取值 0~59；
    float temperature; // 温度
    float humidity; // 湿度
    int batteryPercentage; // 电量
};

class Display {
public:
    Display();
    void begin();
    void prompt(String z);
    void showStartupScreen();
    void displayClock(DetailInfo* info);
    void displayWeater();

    void testSetup();
    void testLoop();
    GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT>* getDEP() {return _display;};

private:
    SPIClass* _hspi;
    GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT>* _display;
    U8G2_FOR_ADAFRUIT_GFX* _u8g2Fonts;
    int _partialRefreshCnt = 0;
};

#endif
