#ifndef BUSINESS_MAIN_H
#define BUSINESS_MAIN_H

#include "esp32_board.h"
#include "business_iot.h"

//****** 天气数据
struct ActualWeather
{
  char status_code[64];  // 错误代码
  char city[16];         // 城市名称
  char weather_name[16]; // 天气现象名称
  char weather_code[4];  // 天气现象代码
  char temp[5];          // 温度
  char last_update[25];  // 最后更新时间
};

struct FutureWeather
{
  char status_code[64];       // 错误代码

  char date0[14];             // 今天日期
  char date0_text_day[20];    // 白天天气现象名称
  char date0_code_day[4];     // 白天天气现象代码
  char date0_text_night[16];  // 晚上天气现象名称
  char date0_code_night[4];   // 晚上天气现象代码
  char date0_high[5];         // 最高温度
  char date0_low[5];          // 最低温度
  char date0_humidity[5];     // 相对湿度
  char date0_wind_scale[5];   // 风力等级

  char date1[14];             // 明天日期
  char date1_text_day[20];    // 白天天气现象名称
  char date1_code_day[4];     // 白天天气现象代码
  char date1_text_night[16];  // 晚上天气现象名称
  char date1_code_night[4];   // 晚上天气现象代码
  char date1_high[5];         // 最高温度
  char date1_low[5];          // 最低温度

  char date2[14];             // 后天日期
  char date2_text_day[20];    // 白天天气现象名称
  char date2_code_day[4];     // 白天天气现象代码
  char date2_text_night[16];  // 晚上天气现象名称
  char date2_code_night[4];   // 晚上天气现象代码
  char date2_high[5];         // 最高温度
  char date2_low[5];          // 最低温度
};

struct RiQi       // 日期 https://api.xygeng.cn/day
{
  int code;       // 错误代码
  char year[5];   // 年
  char month[3];  // 月
  char date[3];   // 日
  char day[12];   // 星期几
  char festival[64]; // 节日名
}; 

struct LifeIndex // 生活指数
{
  char status_code[64];  // 错误代码
  char uvi[10];          // 紫外线指数
}; 

struct Hitokoto  // 一言API
{
  char status_code[64]; //错误代码
  char hitokoto[70];
  //https://v1.hitokoto.cn?encode=json&charset=utf-8&min_length=1&max_length=21
  //https://pa-1251215871.cos-website.ap-chengdu.myqcloud.com/
};

bool downloadAndDisplayImage(ESP32Board &board);

class Business
{
public:
    Business(ESP32Board esp32);
    void begin();
    void loop();
    void displayAuxiliary();

private:
    ESP32Board _esp32;
    void display_tbpd();
    // TODO: 填充业务内容
};

#endif