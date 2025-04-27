#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <Fonts/FreeSerif12pt7b.h>

#include "../include/esp32_log.h"
#include "../include/utils.h"
#include "../include/esp32_pins.h"
#include "../include/esp32_display.h"

// 感谢yanyuandi的快刷
// 快刷 https://github.com/yanyuandi/FastFreshBWOnColor

// https://github.com/ZinggJM/GxEPD2/blob/master/examples/GxEPD2_WS_ESP32_Driver/GxEPD2_WS_ESP32_Driver.ino
// https://gitee.com/corogoo/epaper-7.5-weather-station-plus

const char *logDisplayTag = "display";

Display::Display()
{
  _hspi = new SPIClass(HSPI);
  _display = new GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT>(GxEPD2_750c_Z08(/*CS=*/CS_Pin, /*DC=*/DC_Pin, /*RST=*/RES_Pin, /*BUSY=*/BUSY_Pin)); // GDEW075Z08  800x480, EK79655 (GD7965), (WFT0583CZ61)

  _u8g2Fonts = new U8G2_FOR_ADAFRUIT_GFX();
}

void Display::begin()
{
  // 优化显示驱动配置
  // _display->epd2.setPowerDuration(0, 10); // 减少上电时间
  // _display->epd2.setPowerDuration(1, 10); // 减少断电时间
  // _display->epd2.setPowerDuration(2, 5);  // 减少刷新时间
  // _display->epd2.setPaged

  // *** special handling for Waveshare ESP32 Driver board *** //
  // begin(int8_t sck, int8_t miso, int8_t mosi, int8_t ss)
  // hspi.begin(18, 19, 23, 5); // remap hspi for EPD (swap pins)
  _hspi->begin(SCK_Pin, DC_Pin, SDI_Pin, CS_Pin); // remap hspi for EPD (swap pins)
  _display->epd2.selectSPI(*_hspi, SPISettings(4000000, MSBFIRST, SPI_MODE0));
  // *** end of special handling for Waveshare ESP32 Driver board *** //

  _display->init(115200, true, 2, false);
  _display->setRotation(0); //  对这块 4.2 寸屏幕而言，2 是横向（排线在上方）

  _u8g2Fonts->begin(*_display);                // 将u8g2过程连接到Adafruit GFX
  _u8g2Fonts->setFontMode(1);                  // 使用u8g2透明模式（这是默认设置）
  _u8g2Fonts->setFontDirection(0);             // 从左到右（这是默认设置）
  _u8g2Fonts->setForegroundColor(GxEPD_BLACK); // 设置前景色
  _u8g2Fonts->setBackgroundColor(GxEPD_WHITE); // 设置背景色
  _u8g2Fonts->setFont(NORMAL_FONT);
}

void Display::showStartupScreen()
{
  const char *startupText = "Wales Kuo";

  _display->setFullWindow(); // 设置全屏刷新窗口
  _display->firstPage();
  do
  {
    _display->fillScreen(GxEPD_WHITE);

    _display->setTextSize(5);

    int16_t tbx, tby; uint16_t tbw, tbh;
    _display->getTextBounds(startupText, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t x = ((_display->width() - tbw) / 2) - tbx;
    uint16_t y = ((_display->height() - tbh) / 2) - tby - 100;
    
    _display->setTextColor(GxEPD_BLACK);
    _display->setCursor(x, y);
    _display->print(startupText);
  } while (_display->nextPage());

  espSleep(5);
}

void Display::prompt(String z)
{
  if (nullptr == z)
  {
    ESP32_LOGE(logDisplayTag, "调用prompt时参数为空");
    return;
  }

  U8G2_FOR_ADAFRUIT_GFX &u8g2Fonts = *_u8g2Fonts;
  GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> &display = *_display;

  u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312);
  int16_t width = u8g2Fonts.getUTF8Width(z.c_str());
  int16_t x = (display.width() / 2) - (width / 2);

  display.setPartialWindow(x, 329, width, 120);
  display.firstPage();
  do
  {

    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
    u8g2Fonts.setCursor(347, 376);
    u8g2Fonts.print(z);
  } while (display.nextPageBW());
}