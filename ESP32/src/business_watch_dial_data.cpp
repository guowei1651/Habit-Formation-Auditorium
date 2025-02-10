#include <Arduino.h>
/*
#include "../include/esp32_log.h"
#include "../include/utils.h"

const char* busDataTag = "busData";

void GetData()
{
    String weatherKey_s = "SqRA77zdL0dt8H5Vf"; //秘钥
    String city_s = "西安";             //TODO: 用geoip获取城市
    String language="zh-Hans";

    String url_ActualWeather = "http://api.seniverse.com/v3/weather/now.json?key=" + weatherKey_s + "&location=" + city_s + "&language=" + language + "&unit=c";
    String url_FutureWeather = "http://api.seniverse.com/v3/weather/daily.json?key=" + weatherKey_s + "&location=" + city_s + "&language=" + language + "&start=0&days=3";
    String url_LifeIndex = "http://api.seniverse.com/v3/life/suggestion.json?key=" + weatherKey_s + "&location=" + city_s;

    uint8_t cs_max = 2;   //重试次数

    ESP_LOGI(busDataTag, "获取生活指数");
    if (!retryHttpRequest(url_LifeIndex, ParseLifeIndex, &life_index, cs_max)) return;

    ESP_LOGI(busDataTag, "获取天气实况数据中");
    if (!retryHttpRequest(url_ActualWeather, ParseActualWeather, &actual, cs_max)) return;

    ESP_LOGI(busDataTag, "获取未来天气数据中");
    if (!retryHttpRequest(url_FutureWeather, ParseFutureWeather, &future, cs_max)) return;

    handleInAWord(cs_max);

    ESP_LOGI(busDataTag, "获取时间");
    if (!updateTime()) {
        fallbackToWeatherTime();
    }
}

bool retryHttpRequest(const String& url, bool (*parseFunc)(const String&, void*), void* data, uint8_t maxRetries)
{
    uint8_t cs_count = 0;
    while (parseFunc(callHttp(url), data) == 0 && cs_count < maxRetries) {
        cs_count++;
    }
    return cs_count < maxRetries;
}

void handleInAWord(uint8_t cs_max)
{
    switch (eepUserSet.inAWord_mod) {
        case 0:
            ESP_LOGI(busDataTag, "获取一言数据");
            retryHttpRequest(url_yiyan, ParseHitokoto, &yiyan, cs_max);
            break;
        case 1:
            ESP_LOGI(busDataTag, "使用自定义句子");
            break;
        case 2:
            ESP_LOGI(busDataTag, "启用天数倒计时");
            break;
        case 3:
            String BUID = getBUID();
            String url_Bfen = "http://api.bilibili.com/x/relation/stat?vmid=" + BUID + "&jsonp=jsonp";
            ESP_LOGI(busDataTag, "获取B站粉丝");
            retryHttpRequest(url_Bfen, ParseBliBli, nullptr, cs_max);
            break;
    }
}

String getBUID()
{
    String BUID = "";
    for (uint8_t i = 4; i < strlen(eepUserSet.inAWord); i++) {
        BUID += eepUserSet.inAWord[i];
    }
    return BUID;
}

void fallbackToWeatherTime()
{
    String a = actual.last_update[11];
    String b = actual.last_update[12];
    RTC_hour = atoi(a.c_str()) * 10 + atoi(b.c_str()) + 1;
    ESP.rtcUserMemoryWrite(RTCdz_hour, &RTC_hour, sizeof(RTC_hour));
    Serial.print("获取时间："); Serial.println(RTC_hour);
    display_partialLine(7, "获取NTP时间失败,改用天气时间");
}


// 天气数据获取
//使用Json解析天气数据，天气实况
bool ParseActualWeather(String content, struct ActualWeather* jgt)
{
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, content);
  //serializeJson(json, Serial);//构造序列化json,将内容从串口输出
  if (error)
  {
    Serial.print("天气实况加载json配置失败:");
    Serial.println(error.f_str());
    Serial.println(" ");
    String z = "天气实况json配置失败:" + String(error.f_str()) + " " + content;
    if (String(error.f_str()) == "EmptyInput")
    {
      display_partialLine(7, "天气实况json 重试中");
      return false;
    }
    else
    {
      display_partialLine(7, z);
      esp_sleep(0);
    }
  }

  //检查API是否有返回错误信息，有返回则进入休眠
  if (doc["status_code"].isNull() == 0) //检查到不为空
  {
    strcpy(jgt->status_code, doc["status_code"]);
    String z;
    if (String(actual.status_code) == "AP010001") z = "API 请求参数错误" ;
    else if (String(actual.status_code) == "AP010002") z = "没有权限访问这个 API 接口" ;
    else if (String(actual.status_code) == "AP010003") z = "API 密钥 key 错误" ;
    else if (String(actual.status_code) == "AP010004") z = "签名错误" ;
    else if (String(actual.status_code) == "AP010005") z = "你请求的 API 不存在" ;
    else if (String(actual.status_code) == "AP010006") z = "没有权限访问这个地点: " + String(eepUserSet.city);
    else if (String(actual.status_code) == "AP010007") z = "JSONP 请求需要使用签名验证方式" ;
    else if (String(actual.status_code) == "AP010008") z = "没有绑定域名" ;
    else if (String(actual.status_code) == "AP010009") z = "API 请求的 user-agent 与你设置的不一致" ;
    else if (String(actual.status_code) == "AP010010") z = "没有这个地点" + String(eepUserSet.city);
    else if (String(actual.status_code) == "AP010011") z = "无法查找到指定 IP 地址对应的城市" ;
    else if (String(actual.status_code) == "AP010012") z = "你的服务已经过期" ;
    else if (String(actual.status_code) == "AP010013") z = "访问量余额不足" ;
    else if (String(actual.status_code) == "AP010014") z = "访问频率超过限制" ;
    else if (String(actual.status_code) == "AP010015") z = "暂不支持该城市的车辆限行信息" ;
    else if (String(actual.status_code) == "AP010016") z = "暂不支持该城市的潮汐数据" ;
    else if (String(actual.status_code) == "AP010017") z = "请求的坐标超出支持的范围" ;
    else if (String(actual.status_code) == "AP100001") z = "心知系统内部错误：数据缺失" ;
    else if (String(actual.status_code) == "AP100002") z = "心知系统内部错误：数据错误" ;
    else if (String(actual.status_code) == "AP100003") z = "心知系统内部错误：服务内部错误" ;
    else if (String(actual.status_code) == "AP100004") z = "心知系统内部错误：网关错误" ;
    else z = "天气实况异常:" + String(actual.status_code);
    display_partialLine(7, z);
    Serial.print("天气实况异常:"); Serial.println(actual.status_code);
    const char* zf_t = z.c_str();//String转换char
    strcpy(jgt->weather_name, zf_t);
    return true;
  }
  JsonObject results_0 = doc["results"][0];

  JsonObject results_0_now = results_0["now"];

  if (results_0["location"]["name"].isNull() == 0)
    strcpy(jgt->city, results_0["location"]["name"]);

  if (results_0_now["text"].isNull() == 0)
    strcpy(jgt->weather_name, results_0_now["text"]);

  if (results_0_now["code"].isNull() == 0)
    strcpy(jgt->weather_code, results_0_now["code"]);

  if (results_0_now["temperature"].isNull() == 0)
    strcpy(jgt->temp, results_0_now["temperature"]);

  if (results_0["last_update"].isNull() == 0)
    strcpy(jgt->last_update, results_0["last_update"]);

  // 这不是强制复制，你可以使用指针，因为他们是指向"内容"缓冲区内，所以你需要确保
  // 当你读取字符串时它仍在内存中
  return true;
}

//使用Json解析天气数据，今天和未来2天
bool ParseFutureWeather(String content, struct FutureWeather* jgt)
{
  DynamicJsonDocument doc(2048); //分配内存,动态
  DeserializationError error = deserializeJson(doc, content); //解析json
  //serializeJson(doc, Serial);//构造序列化json,将内容从串口输出
  if (error)
  {
    Serial.print("未来天气json配置失败:");
    Serial.println(error.f_str());
    Serial.println(" ");
    String z = "未来天气加载json配置失败:" + String(error.f_str()) + " " + content;
    if (String(error.f_str()) == "EmptyInput")
    {
      display_partialLine(7, "未来天气json 重试中");
      return false;
    }
    else
    {
      display_partialLine(7, z);
      esp_sleep(0);
    }
  }

  //检查API是否有返回错误信息，有返回则进入休眠
  if (doc["status_code"].isNull() == 0) //检查到不为空
  {
    strcpy(jgt->status_code, doc["status_code"]);
    String z;
    if (String(future.status_code) == "AP010001") z = "API 请求参数错误" ;
    else if (String(future.status_code) == "AP010002") z = "没有权限访问这个 API 接口" ;
    else if (String(future.status_code) == "AP010003") z = "API 密钥 key 错误" ;
    else if (String(future.status_code) == "AP010004") z = "签名错误" ;
    else if (String(future.status_code) == "AP010005") z = "你请求的 API 不存在" ;
    else if (String(future.status_code) == "AP010006") z = "没有权限访问这个地点: " + String(eepUserSet.city);
    else if (String(future.status_code) == "AP010007") z = "JSONP 请求需要使用签名验证方式" ;
    else if (String(future.status_code) == "AP010008") z = "没有绑定域名" ;
    else if (String(future.status_code) == "AP010009") z = "API 请求的 user-agent 与你设置的不一致" ;
    else if (String(future.status_code) == "AP010010") z = "没有这个地点" + String(eepUserSet.city);
    else if (String(future.status_code) == "AP010011") z = "无法查找到指定 IP 地址对应的城市" ;
    else if (String(future.status_code) == "AP010012") z = "你的服务已经过期" ;
    else if (String(future.status_code) == "AP010013") z = "访问量余额不足" ;
    else if (String(future.status_code) == "AP010014") z = "访问频率超过限制" ;
    else if (String(future.status_code) == "AP010015") z = "暂不支持该城市的车辆限行信息" ;
    else if (String(future.status_code) == "AP010016") z = "暂不支持该城市的潮汐数据" ;
    else if (String(future.status_code) == "AP010017") z = "请求的坐标超出支持的范围" ;
    else if (String(future.status_code) == "AP100001") z = "心知系统内部错误：数据缺失" ;
    else if (String(future.status_code) == "AP100002") z = "心知系统内部错误：数据错误" ;
    else if (String(future.status_code) == "AP100003") z = "心知系统内部错误：服务内部错误" ;
    else if (String(future.status_code) == "AP100004") z = "心知系统内部错误：网关错误" ;
    else z = "未来天气异常:" + String(future.status_code);
    display_partialLine(7, z);
    Serial.print("未来天气异常:"); Serial.println(future.status_code);
    const char* zf_t = z.c_str();//String转换char
    strcpy(jgt->date0_text_day, zf_t);
    return true;
  }

  // 复制我们感兴趣的字符串，先检查是否为空，空会复制失败导致系统无限重启
  if (doc["results"][0]["daily"][0]["date"].isNull() == 0)        //日期
    strcpy(jgt->date0, doc["results"][0]["daily"][0]["date"]);
  if (doc["results"][0]["daily"][1]["date"].isNull() == 0)
    strcpy(jgt->date1, doc["results"][0]["daily"][1]["date"]);
  if (doc["results"][0]["daily"][2]["date"].isNull() == 0)
    strcpy(jgt->date2, doc["results"][0]["daily"][2]["date"]);

  if (doc["results"][0]["daily"][0]["text_day"].isNull() == 0)    //白天天气现象
    strcpy(jgt->date0_text_day, doc["results"][0]["daily"][0]["text_day"]);
  if (doc["results"][0]["daily"][1]["text_day"].isNull() == 0)
    strcpy(jgt->date1_text_day, doc["results"][0]["daily"][1]["text_day"]);
  if (doc["results"][0]["daily"][2]["text_day"].isNull() == 0)
    strcpy(jgt->date2_text_day, doc["results"][0]["daily"][2]["text_day"]);

  if (doc["results"][0]["daily"][0]["text_night"].isNull() == 0)    //晚间天气现象
    strcpy(jgt->date0_text_night, doc["results"][0]["daily"][0]["text_night"]);
  if (doc["results"][0]["daily"][1]["text_night"].isNull() == 0)
    strcpy(jgt->date1_text_night, doc["results"][0]["daily"][1]["text_night"]);
  if (doc["results"][0]["daily"][2]["text_night"].isNull() == 0)
    strcpy(jgt->date2_text_night, doc["results"][0]["daily"][2]["text_night"]);

  if (doc["results"][0]["daily"][0]["high"].isNull() == 0)
    strcpy(jgt->date0_high, doc["results"][0]["daily"][0]["high"]);  //最高温度
  if (doc["results"][0]["daily"][1]["high"].isNull() == 0)
    strcpy(jgt->date1_high, doc["results"][0]["daily"][1]["high"]);
  if (doc["results"][0]["daily"][2]["high"].isNull() == 0)
    strcpy(jgt->date2_high, doc["results"][0]["daily"][2]["high"]);

  if (doc["results"][0]["daily"][0]["low"].isNull() == 0)             //最低温度
    strcpy(jgt->date0_low, doc["results"][0]["daily"][0]["low"]);
  if (doc["results"][0]["daily"][1]["low"].isNull() == 0)
    strcpy(jgt->date1_low, doc["results"][0]["daily"][1]["low"]);
  if (doc["results"][0]["daily"][2]["low"].isNull() == 0)
    strcpy(jgt->date2_low, doc["results"][0]["daily"][2]["low"]);

  if (doc["results"][0]["daily"][0]["humidity"].isNull() == 0)                //湿度
    strcpy(jgt->date0_humidity, doc["results"][0]["daily"][0]["humidity"]);

  if (doc["results"][0]["daily"][0]["wind_scale"].isNull() == 0)        //风力等级
    strcpy(jgt->date0_wind_scale, doc["results"][0]["daily"][0]["wind_scale"]);
  // 这不是强制复制，你可以使用指针，因为他们是指向"内容"缓冲区内，所以你需要确保
  // 当你读取字符串时它仍在内存中
  return true;
}


//使用Json解析天气数据，天气指数
bool ParseLifeIndex(String content, struct LifeIndex* jgt)
{
  StaticJsonDocument<96> filter;
  filter["results"][0]["suggestion"]["uv"]["brief"] = true;
  filter["status_code"] = true;
  
  StaticJsonDocument<192> doc;

  DeserializationError error = deserializeJson(doc, content, DeserializationOption::Filter(filter));
  if (error)
  {
    Serial.print("天气指数加载json配置失败:");
    Serial.println(error.f_str());
    Serial.println(" ");
    String z = "天气指数json配置失败:" + String(error.f_str()) + " " + content;
    if (String(error.f_str()) == "EmptyInput")
    {
      display_partialLine(7, "天气指数json 重试中");
      return false;
    }
    else
    {
      display_partialLine(7, z);
      esp_sleep(0);
    }
  }

  //检查API是否有返回错误信息，有返回则进入休眠
  if (doc["status_code"].isNull() == 0) //检查到不为空
  {
    strcpy(jgt->status_code, doc["status_code"]);
    String z;
    if (String(actual.status_code) == "AP010001") z = "API 请求参数错误" ;
    else if (String(actual.status_code) == "AP010002") z = "没有权限访问这个 API 接口" ;
    else if (String(actual.status_code) == "AP010003") z = "API 密钥 key 错误" ;
    else if (String(actual.status_code) == "AP010004") z = "签名错误" ;
    else if (String(actual.status_code) == "AP010005") z = "你请求的 API 不存在" ;
    else if (String(actual.status_code) == "AP010006") z = "没有权限访问这个地点: " + String(eepUserSet.city);
    else if (String(actual.status_code) == "AP010007") z = "JSONP 请求需要使用签名验证方式" ;
    else if (String(actual.status_code) == "AP010008") z = "没有绑定域名" ;
    else if (String(actual.status_code) == "AP010009") z = "API 请求的 user-agent 与你设置的不一致" ;
    else if (String(actual.status_code) == "AP010010") z = "没有这个地点" + String(eepUserSet.city);
    else if (String(actual.status_code) == "AP010011") z = "无法查找到指定 IP 地址对应的城市" ;
    else if (String(actual.status_code) == "AP010012") z = "你的服务已经过期" ;
    else if (String(actual.status_code) == "AP010013") z = "访问量余额不足" ;
    else if (String(actual.status_code) == "AP010014") z = "访问频率超过限制" ;
    else if (String(actual.status_code) == "AP010015") z = "暂不支持该城市的车辆限行信息" ;
    else if (String(actual.status_code) == "AP010016") z = "暂不支持该城市的潮汐数据" ;
    else if (String(actual.status_code) == "AP010017") z = "请求的坐标超出支持的范围" ;
    else if (String(actual.status_code) == "AP100001") z = "心知系统内部错误：数据缺失" ;
    else if (String(actual.status_code) == "AP100002") z = "心知系统内部错误：数据错误" ;
    else if (String(actual.status_code) == "AP100003") z = "心知系统内部错误：服务内部错误" ;
    else if (String(actual.status_code) == "AP100004") z = "心知系统内部错误：网关错误" ;
    else z = "天气指数异常:" + String(actual.status_code);
    display_partialLine(7, z);
    Serial.print("天气指数异常:"); Serial.println(actual.status_code);
    return true;
  }

  // 复制我们感兴趣的字符串 ,先检查是否为空，空会导致系统无限重启
  // isNull()检查是否为空 空返回1 非空0
  if (doc["results"][0]["suggestion"]["uv"]["brief"].isNull() == 0)    //紫外线指数
    strcpy(jgt->uvi, doc["results"][0]["suggestion"]["uv"]["brief"]);

  //Serial.print("紫外线:"); Serial.println(data->uvi);
  // 这不是强制复制，你可以使用指针，因为他们是指向"内容"缓冲区内，所以你需要确保
  // 当你读取字符串时它仍在内存中
  return true;
}
*/