#include <stdio.h>
#include <Arduino.h>
#include <esp_task_wdt.h>
#include <HardwareSerial.h>

#include "../include/esp32_log.h"
#include "../include/utils.h"
#include "../include/esp32_board.h"
#include "../include/business_main.h"

const char *logTag = "main";

DetailInfo info;
SemaphoreHandle_t xDisplayMutex; // 显示屏的互斥锁
// 定义 FreeRTOS 任务句柄
TaskHandle_t taskUpdateTimeHandle = NULL;
TaskHandle_t taskReadSensorHandle = NULL;
TaskHandle_t taskReadBatteryHandle = NULL;
TaskHandle_t taskDisplayImageHandle = NULL;

// 更新 NTP 时间
void taskUpdateTime(void *pvParameters);
void taskReadSensor(void *pvParameters);
void taskReadBattery(void *pvParameters);
void taskDisplayImage(void *pvParameters);

void setup()
{
  Serial.begin(115200);

  esp32_log_level_set("*", ESP_LOG_INFO);
  ESP32_LOGI(logTag, "Starting setup...");

  // Get the unique ID of the ESP32
  esp32.uniqueID = getUniqueID();

  // Initialize IoT client with the unique ID
  ESP32_LOGI(logTag, "Initializing battery...");
  esp32.battery = new Battery();
  esp32.battery->begin();

  // Initialize button interrupts
  ESP32_LOGI(logTag, "Initializing button interrupts...");
  esp32.buttonManager = new ButtonManager(BUTTON1_PIN, BUTTON2_PIN, BUTTON3_PIN);
  esp32.buttonManager->begin();

  // Initialize sensor
  ESP32_LOGI(logTag, "Initializing SHT30 sensor...");
  esp32.sht30 = new SHT30();
  esp32.sht30->begin();

  // Initialize ConfigManager
  ESP32_LOGI(logTag, "Initializing SHT30 sensor...");
  esp32.configManager = new ConfigManager();
  esp32.configManager->begin();

  // Initialize display
  ESP32_LOGI(logTag, "Initializing display...");
  esp32.display = new Display();
  esp32.display->begin();
  esp32.display->showStartupScreen();

  // ********************************纯硬件初始化结束***************************************
  // Initialize network 必须在ntpclient之前
  ESP32_LOGI(logTag, "Initializing network...");
  esp32.network = new Network(*esp32.configManager, *esp32.display);
  esp32.network->begin();

  // Initialize NTPClient
  ESP32_LOGI(logTag, "Initializing NTPClient...");
  esp32.ntpClient = new CustomNTPClient(esp32.network);
  esp32.ntpClient->begin();

  ESP32_LOGI(logTag, "Initializing ota...");
  esp32.otaManager = new OTAManager();
  esp32.otaManager->begin();
  // *********************************业务初始化开始****************************************

  ESP32_LOGI(logTag, "setup done");

  // *********************************创建 FreeRTOS 任务****************************************
  ESP32_LOGI(logTag, "Initializing FreeRTOS task...");
  xDisplayMutex = xSemaphoreCreateMutex(); // 创建互斥锁
  memset(&info, 0, sizeof(DetailInfo));
  xTaskCreatePinnedToCore(taskUpdateTime, "UpdateTime", 4096, NULL, 2, &taskUpdateTimeHandle, 0);       // 核心 0，优先级 2
  xTaskCreatePinnedToCore(taskReadSensor, "ReadSensor", 4096, NULL, 1, &taskReadSensorHandle, 1);       // 核心 1，优先级 1
  xTaskCreatePinnedToCore(taskReadBattery, "ReadBattery", 4096, NULL, 1, &taskReadBatteryHandle, 1);    // 核心 1，优先级 1
  xTaskCreatePinnedToCore(taskDisplayImage, "DisplayImage", 4096, NULL, 3, &taskDisplayImageHandle, 0); // 核心 0，优先级 3
  
}

// 更新 NTP 时间
void taskUpdateTime(void *pvParameters)
{
  uint64_t delay = 12 * 60 * 60 * 1000; // 每 12 小时读取一次
  while (1)
  {
    ESP32_LOGI(logTag, "开始更新时间");
    bool isSuccess = esp32.ntpClient->updateIfNeeded();
    if (!isSuccess)
    {
      delay = 1 * 60 * 1000; // 失败后每1分钟重试
    }
    else
    {
      ESP32_LOGI(logTag, "当前时间为: %s", esp32.ntpClient->getFormattedTime().c_str());
    }
    ESP32_LOGI(logTag, "更新时间结束");

    vTaskDelay(pdMS_TO_TICKS(delay));
  }
}

// 读取温湿度
void taskReadSensor(void *pvParameters)
{
  while (1)
  {
    float temp, hum;
    ESP32_LOGI(logTag, "开始获取温湿度");
    esp32.sht30->readTempHum(temp, hum);
    ESP32_LOGI(logTag, "获取温湿度结束");

    info.temperature = temp;
    info.humidity = hum;

    vTaskDelay(pdMS_TO_TICKS(30 * 60 * 1000)); // 每 30 分钟读取一次
  }
}

// 读取电量
void taskReadBattery(void *pvParameters)
{
  while (1)
  {
    ESP32_LOGI(logTag, "开始获取电量");
    float voltagePercentage = esp32.battery->getPercentage();
    ESP32_LOGI(logTag, "获取电量结束");

    info.batteryPercentage = voltagePercentage;

    vTaskDelay(pdMS_TO_TICKS(1 * 60 * 60 * 1000)); // 每 1小时读取一次
  }
}

// 处理图片显示
void taskDisplayImage(void *pvParameters)
{
  uint64_t delayTime = 1 * 60 * 60 * 1000; // 每 2 分钟更新图片
  while (true)
  {
    ESP32_LOGI(logTag, "开始显示图片");
    bool isSuccessDisplay = false;

    esp_task_wdt_reset(); // 喂狗，防止 WDT 复位

    // 获取锁
    if (xSemaphoreTake(xDisplayMutex, pdMS_TO_TICKS(500))) // 最多等 500ms
    {
      isSuccessDisplay = downloadAndDisplayImage(esp32);
      xSemaphoreGive(xDisplayMutex); // 释放锁
    } else {
      ESP32_LOGE(logTag, "获取墨水屏控制权失败");
    }

    if (!isSuccessDisplay)
    {
      isSuccessDisplay = 5 * 1000; // 如果更新失败则5秒后重试
    }
    
    esp_task_wdt_reset(); // 喂狗，防止 WDT 复位

    ESP32_LOGI(logTag, "显示图片结束，结果为: %d", isSuccessDisplay);

    vTaskDelay(pdMS_TO_TICKS(delayTime));
  }
}

void fillTime(DetailInfo *info)
{
  if (nullptr == info)
  {
    ESP32_LOGE(logTag, "fillTime 参数错误");
    return;
  }

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    ESP32_LOGE(logTag, "获取本地时间失败");
    esp32.ntpClient->updateIfNeeded();
    return;
  }

  info->year = timeinfo.tm_year + 1900; // 年，其值等于实际年份减去 1900；
  info->month = timeinfo.tm_mon + 1;    // 月，取值 0~11；
  info->dayOfyear = timeinfo.tm_yday;   // 年中的日期，取值 0~365，0 代表 1 月 1 日，1 代表 1 月 2 日，依此类推；
  info->dayOfMonth = timeinfo.tm_mday;  // 月中的日期，取值 1~31；
  info->dayOfWeek = timeinfo.tm_wday;   // 星期，取值 0~6，0 为周日，1 为周一，依此类推；
  info->hour = timeinfo.tm_hour;        // 时，取值 0~23；
  info->minute = timeinfo.tm_min;       // 分，取值 0~59；
  info->second = timeinfo.tm_sec;       // 秒，取值 0~59；
}

// **主循环只处理时钟显示**
void loop()
{
  ESP32_LOGD(logTag, "开始获取时间");
  fillTime(&info);
  ESP32_LOGD(logTag, "时间： %d-%d-%d %d:%d:%d", info.year, info.month, info.dayOfMonth, info.hour, info.minute, info.second);
  ESP32_LOGD(logTag, "获取时间结束");

  ESP32_LOGD(logTag, "开始显示时钟");
  // 获取锁
  if (xSemaphoreTake(xDisplayMutex, pdMS_TO_TICKS(10)))
  {
    esp32.display->displayClock(&info);
    xSemaphoreGive(xDisplayMutex); // 释放锁
  }
  ESP32_LOGD(logTag, "显示时钟结束");

  vTaskDelay(pdMS_TO_TICKS(1000)); // 改为 FreeRTOS 任务调度
}
