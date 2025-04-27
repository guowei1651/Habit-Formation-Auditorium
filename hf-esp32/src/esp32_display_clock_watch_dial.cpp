#include "../include/esp32_log.h"
#include "../include/utils.h"
#include "../include/esp32_display.h"

#define DISPLAY_WIDTH 800
#define DISPLAY_HEIGHT 480
#define PARTIAL_WATCH_DIAL_WIDTH 230
#define PARTIAL_WATCH_DIAL_HEIGHT 103
#define FULL_WATCH_DIAL_WIDTH 296
#define FULL_WATCH_DIAL_HEIGHT 128

#define LogTag "display"

const char *WEEKDAY_CN[] = {"周日", "周一", "周二", "周三", "周四", "周五", "周六"};
const char *WEEKDAY_EN[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

uint16_t getXAbsolutePosition(uint16_t x)
{
  // 右下角的起始位置
  const uint16_t StartingPosition = DISPLAY_WIDTH - FULL_WATCH_DIAL_WIDTH;

  return StartingPosition + x;
}

uint16_t getYAbsolutePosition(uint16_t y)
{
  // 右下角的起始位置
  const uint16_t StartingPosition = DISPLAY_HEIGHT - FULL_WATCH_DIAL_HEIGHT;

  return StartingPosition + y;
}

uint16_t getCenterX(U8G2_FOR_ADAFRUIT_GFX &u8g2Fonts, String z) // 计算字符居中的X位置
{
  uint16_t zf_width = u8g2Fonts.getUTF8Width(z.c_str()); // 获取字符的像素长度
  uint16_t x = (416 / 2) - (zf_width / 2);               // 计算字符居中的X位置
  return x;
}
uint16_t getCenterX(U8G2_FOR_ADAFRUIT_GFX &u8g2Fonts, const char *z) // 计算字符居中的X位置
{
  uint16_t zf_width = u8g2Fonts.getUTF8Width(z); // 获取字符的像素长度
  uint16_t x = (416 / 2) - (zf_width / 2);       // 计算字符居中的X位置
  return x;
}

//************************ 显示设备信息 ************************ 电池电量0-电压 1-百分比
void displayDeviceInfo(GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> &display, U8G2_FOR_ADAFRUIT_GFX &u8g2Fonts, DetailInfo *info)
{
  uint16_t batVol_length;
  const int16_t xOffset = -20;

  u8g2Fonts.setFont(chinese_gb2312);

  //************************ 温度显示 ************************
  display.drawInvertedBitmap(getXAbsolutePosition(0), getYAbsolutePosition(125 - 13), Bitmap_tempSHT30, 16, 16, GxEPD_BLACK);
  u8g2Fonts.setCursor(getXAbsolutePosition(16), getYAbsolutePosition(125));
  u8g2Fonts.print(info->temperature, 1);

  //************************ 湿度显示 ************************
  display.drawInvertedBitmap(getXAbsolutePosition(46), getYAbsolutePosition(125 - 13), Bitmap_humiditySHT30, 16, 16, GxEPD_BLACK);
  u8g2Fonts.setCursor(getXAbsolutePosition(48 + 16), getYAbsolutePosition(125));
  u8g2Fonts.print(info->humidity, 1);

  //************************ 电量显示 ************************
  String batVol = String(info->batteryPercentage) + "%";
  batVol_length = u8g2Fonts.getUTF8Width(batVol.c_str()); // 获取字符的长度
  int x = 48 + 16 + 30;
  ESP32_LOGD(LogTag, "电量图标的位置: %d", x);
  display.drawInvertedBitmap(getXAbsolutePosition(x), getYAbsolutePosition(125 - 13), Bitmap_dlxtb, 11, 16, GxEPD_BLACK); // 显示电池小图标
  u8g2Fonts.setCursor(getXAbsolutePosition(x + 16), getYAbsolutePosition(125));
  u8g2Fonts.print(batVol);
}

void displayTime(GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> &display, U8G2_FOR_ADAFRUIT_GFX &u8g2Fonts, DetailInfo *info)
{
  // 拼装时间 小时和分,不够两位数需要补0
  String hour, minute, assembleTime;
  if (info->hour < 10)
    hour = "0" + String(info->hour);
  else
    hour = String(info->hour);
  if (info->minute < 10)
    minute = "0" + String(info->minute);
  else
    minute = String(info->minute);
  assembleTime = hour + ":" + minute;

  int8_t sz_x = 0; // 显示位置X轴的偏移量
  if (info->hour >= 10 && info->hour <= 19)
    sz_x = -3; // 10-19点
  else
    sz_x = 2;

  // 显示时间
  u8g2Fonts.setFont(u8g2_font_logisoso78_tn);
  u8g2Fonts.setCursor(getXAbsolutePosition(sz_x), getYAbsolutePosition(95));
  u8g2Fonts.print(assembleTime);
}

void displayRightDate(GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> &display, U8G2_FOR_ADAFRUIT_GFX &u8g2Fonts, DetailInfo *info)
{
  // 显示最上一行, 年份
  String year = String(info->year) + "年"; // 事件里的时间
  u8g2Fonts.setFont(chinese_gb2312);
  u8g2Fonts.setCursor(getXAbsolutePosition(getCenterX(u8g2Fonts, year) + 55), getYAbsolutePosition(24));
  u8g2Fonts.print(year);

  // 显示第二行, 月份
  String month = String(info->month);
  u8g2Fonts.setFont(u8g2_font_logisoso16_tn);
  uint16_t xMonth = getXAbsolutePosition(getCenterX(u8g2Fonts, month) + 50);
  u8g2Fonts.setCursor(xMonth, getYAbsolutePosition(48));
  u8g2Fonts.print(month);

  String monthWord = String("月");
  uint16_t monthLength = u8g2Fonts.getUTF8Width(month.c_str());
  uint16_t xMonthWord = xMonth + monthLength + 1;
  ESP32_LOGD(LogTag, "月的x位置: %d 数字长度: %d 月字的x位置: %d", xMonth, monthLength, xMonthWord);
  u8g2Fonts.setFont(chinese_gb2312);
  u8g2Fonts.setCursor(xMonthWord, getYAbsolutePosition(48));
  u8g2Fonts.print(monthWord);

  // 显示第三行，天数
  String day = String(info->dayOfMonth);
  u8g2Fonts.setFont(u8g2_font_logisoso20_tn);
  uint16_t xDay = getXAbsolutePosition(getCenterX(u8g2Fonts, day) + 50);
  u8g2Fonts.setCursor(xDay, getYAbsolutePosition(80));
  u8g2Fonts.print(day);

  String dayWord = String("日");
  uint16_t dayLength = u8g2Fonts.getUTF8Width(day.c_str());
  uint16_t xDayWord = xDay + dayLength + 1;
  ESP32_LOGD(LogTag, "日的x位置: %d 数字长度: %d 日字的x位置: %d", xDay, dayLength, xDayWord);
  u8g2Fonts.setFont(chinese_gb2312);
  u8g2Fonts.setCursor(xDayWord, getYAbsolutePosition(80));
  u8g2Fonts.print(dayWord);

  // 显示周几
  String dayOfWeek = String(WEEKDAY_CN[info->dayOfWeek]);
  u8g2Fonts.setFont(chinese_gb2312);
  u8g2Fonts.setCursor(getXAbsolutePosition(getCenterX(u8g2Fonts, dayOfWeek) + 55), getYAbsolutePosition(100));
  u8g2Fonts.print(dayOfWeek);
}

void displayAuthor(GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> &display, U8G2_FOR_ADAFRUIT_GFX &u8g2Fonts, DetailInfo *info) {
  String author = String("By Wales Kuo");
  u8g2Fonts.setFont(chinese_gb2312);
  uint16_t length = u8g2Fonts.getUTF8Width(author.c_str());
  u8g2Fonts.setCursor(getXAbsolutePosition(FULL_WATCH_DIAL_WIDTH - length - 3), getYAbsolutePosition(125));
  u8g2Fonts.print(author);
}

void partialRefresh(GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> &display, U8G2_FOR_ADAFRUIT_GFX &u8g2Fonts, DetailInfo *info)
{
  display.setPartialWindow(getXAbsolutePosition(0), getYAbsolutePosition(0), FULL_WATCH_DIAL_WIDTH, FULL_WATCH_DIAL_HEIGHT); // 设置局部刷新窗口
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    //************************ 时间显示 ************************
    displayTime(display, u8g2Fonts, info);
    // 画一条垂直线
    display.drawLine(getXAbsolutePosition(230), getYAbsolutePosition(7), getXAbsolutePosition(230), getYAbsolutePosition(103), 0);

    //************************ 日期显示 ************************
    displayRightDate(display, u8g2Fonts, info);

    // 画水平线
    display.drawLine(getXAbsolutePosition(0), getYAbsolutePosition(110), getXAbsolutePosition(295), getYAbsolutePosition(110), 0);

    //************************ 显示设备信息 ************************
    displayDeviceInfo(display, u8g2Fonts, info);

    //************************ 显示作者信息 ************************
    displayAuthor(display, u8g2Fonts, info);
  } while (display.nextPageBW());
}

static int lastMinute = -1;
bool checkMinuteChange(int minute)
{
  int currentMinute = minute;
  if (currentMinute != lastMinute)
  {
    lastMinute = currentMinute;
    return true;
  }
  return false;
}

void Display::displayClock(DetailInfo *info) // 时钟显示界面
{
  GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> &display = *getDEP();
  U8G2_FOR_ADAFRUIT_GFX &u8g2Fonts = *_u8g2Fonts;

  if (checkMinuteChange(info->minute))
  {
    ESP32_LOGI(LogTag, "开始刷新时钟: %d", _partialRefreshCnt);
    partialRefresh(display, u8g2Fonts, info);
  }

  // 局刷计数
  _partialRefreshCnt = ((_partialRefreshCnt + 1) % 3600); // 一个小时
}