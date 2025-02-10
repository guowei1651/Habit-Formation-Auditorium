#ifndef BATTERY_H
#define BATTERY_H

#include <Arduino.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>

#include "esp32_pins.h"

class Battery {
public:
    Battery(int batteryPin = BATTERY_PIN, int weakIOPin = WEAKIO_PIN); // VP引脚用于电池电压监测
    void begin();
    float getVoltage();      // 获取电压
    int getPercentage();     // 获取电量百分比
    bool isLow();           // 电量是否过低
    
private:
    int _batteryPin;
    int _weakioPin;
    esp_adc_cal_characteristics_t _adc_chars;
    const float BATTERY_MIN_V = 3.3;  // 最低工作电压
    const float BATTERY_MAX_V = 4.2;  // 满电电压
    const float LOW_BATTERY_THRESHOLD = 3.5; // 低电量警告阈值
    
    void initADC();
    uint32_t readRawVoltage();
};

#endif