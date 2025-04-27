#include <GxEPD2_3C.h>
#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
// #include "shuma.c"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <time.h>

#include "../include/esp32_display.h"

// #include "jpg.h"



const char *ssid = "";
const char *password = "";

// NTP服务器相关信息
#define TZ 8     // 时区偏移值，以 UTC+8 为例
#define DST_MN 0 // 夏令时持续时间（分钟）

#define TZ_SEC ((TZ) * 3600)    // 时区偏移值（秒）
#define DST_SEC ((DST_MN) * 60) // 夏令时持续时间（秒）

void Display::testSetup()
{
    GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> &display = *_display;
    U8G2_FOR_ADAFRUIT_GFX &u8g2Fonts = *_u8g2Fonts;

    display.setFullWindow();
    display.firstPage();
    do
    {
        display.fillRect(0, 30, 800, 100, GxEPD_RED); // 屏幕顶部画一个红色的矩形
        u8g2Fonts.setFont(u8g2_font_fub42_tf);
        u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
        u8g2Fonts.setCursor(101, 285);
        u8g2Fonts.print("8");

        u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312);
        u8g2Fonts.setBackgroundColor(GxEPD_RED);
        u8g2Fonts.setForegroundColor(GxEPD_WHITE);
        u8g2Fonts.setCursor(268, 84);
        u8g2Fonts.print("此区域为7.5寸三色屏幕的红色刷新");

        u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312);
        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
        u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        u8g2Fonts.setCursor(61, 189);
        u8g2Fonts.print("三色的黑色刷新");

        u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312);
        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
        u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        u8g2Fonts.setCursor(242, 189);
        u8g2Fonts.print("局部白底黑字快刷");

        u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312);
        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
        u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        u8g2Fonts.setCursor(434, 189);
        u8g2Fonts.print("局部黑底白字快刷");

        u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312);
        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
        u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        u8g2Fonts.setCursor(610, 189);
        u8g2Fonts.print("三色屏幕本身局部刷新");

        u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312);
        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
        u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        u8g2Fonts.setCursor(258, 24);
        u8g2Fonts.print("7.5寸三色屏幕局部黑白快刷测试DEMO");

        u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312);
        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
        u8g2Fonts.setForegroundColor(GxEPD_RED);
        u8g2Fonts.setCursor(374, 468);
        u8g2Fonts.print("BY YYD");

    } while (display.nextPage());

    for (int i = 0; i < 3; i++)
    {
        // display.fillScreen(GxEPD_BLACK);
        display.setPartialWindow(248, 204, 120, 120);
        // display.fillScreen(GxEPD_WHITE);
        display.firstPage();
        u8g2Fonts.setFont(u8g2_font_fub42_tf);

        do
        {
            u8g2Fonts.setForegroundColor(GxEPD_BLACK);
            u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
            u8g2Fonts.setCursor(286, 285);
            u8g2Fonts.print(i);
            Serial.println(i);
        } while (display.nextPageBW());
    }

    for (int i = 0; i < 3; i++)
    {

        display.setPartialWindow(434, 204, 120, 120);
        display.firstPage();
        display.fillScreen(GxEPD_BLACK);
        u8g2Fonts.setFont(u8g2_font_fub42_tf);

        do
        {
            u8g2Fonts.setForegroundColor(GxEPD_WHITE);
            u8g2Fonts.setBackgroundColor(GxEPD_BLACK);
            u8g2Fonts.setCursor(474, 285);
            u8g2Fonts.print(i);
            Serial.println(i);
        } while (display.nextPageBW());
    }

    for (int i = 0; i < 1; i++)
    {

        display.setPartialWindow(620, 204, 120, 120);
        display.firstPage();
        // display.fillScreen(GxEPD_WHITE);
        u8g2Fonts.setFont(u8g2_font_fub42_tf);

        do
        {
            u8g2Fonts.setForegroundColor(GxEPD_BLACK);
            u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
            u8g2Fonts.setCursor(660, 285);
            u8g2Fonts.print(i);
            Serial.println(i);
        } while (display.nextPage());
    }
    // 显示“时间获取中...”
    display.setPartialWindow(175, 329, 450, 120);
    display.firstPage();
    do
    {
        u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312);
        u8g2Fonts.setForegroundColor(GxEPD_BLACK);
        u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
        u8g2Fonts.setCursor(347, 376);
        u8g2Fonts.print("时间获取中...");
    } while (display.nextPageBW());

    delay(3000); // 延迟3秒

    display.powerOff();
}

void Display::testLoop()
{
    GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> &display = *_display;
    U8G2_FOR_ADAFRUIT_GFX &u8g2Fonts = *_u8g2Fonts;
    static int prevHour = -1; // 用于存储上一次获取的小时
    static int prevMin = -1;  // 用于存储上一次获取的分钟

    time_t now = time(nullptr);       // 获取当前时间
    struct tm *ltm = localtime(&now); // 将时间转换为本地时间结构体

    // 如果获取时间失败，打印错误信息并返回
    if (now == (time_t)-1)
    {
        Serial.println("Failed to obtain time");
        return;
    }

    // 仅当小时或分钟发生变化时才刷新显示
    if (ltm->tm_hour != prevHour || ltm->tm_min != prevMin)
    {
        // 更新上一次获取的小时和分钟
        prevHour = ltm->tm_hour;
        prevMin = ltm->tm_min;

        // 显示时间
        display.setPartialWindow(175, 329, 450, 120);
        display.firstPage();
        do
        {
            char timeString[20];
            sprintf(timeString, "%02d:%02d", ltm->tm_hour, ltm->tm_min); // 将时间格式化为字符串
            u8g2Fonts.setFont(u8g2_font_logisoso50_tn);
            u8g2Fonts.setForegroundColor(GxEPD_BLACK);
            u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
            u8g2Fonts.setCursor(355, 415);
            u8g2Fonts.print(timeString); // 显示时间字符串
            u8g2Fonts.setForegroundColor(GxEPD_BLACK);
            u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
            u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312);
            u8g2Fonts.setCursor(261, 415);
            u8g2Fonts.print("当前时间："); // 显示时间字符串
        } while (display.nextPageBW());
    }

    // 等待一段时间再继续循环
    delay(1000);
}
