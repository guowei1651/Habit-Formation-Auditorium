#include "../include/esp32_battery.h"

// https://github.com/HwzLoveDz/ESP32-ADC-sense-battery-voltage/blob/main/ESP32_ADC_VBAT.ino

Battery::Battery(int batteryPin) : _batteryPin(batteryPin) {}

void Battery::begin() {
    initADC();
}

void Battery::initADC() {
    // 配置ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
    
    // 特性曲线校准
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 
                            1100, &_adc_chars);
}

uint32_t Battery::readRawVoltage() {
    uint32_t adc_reading = 0;
    
    // 多次采样取平均值
    for (int i = 0; i < 64; i++) {
        adc_reading += adc1_get_raw(ADC1_CHANNEL_0);
    }
    adc_reading /= 64;
    
    // 转换为实际电压(mV)
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, &_adc_chars);
    return voltage;
}

float Battery::getVoltage() {
    uint32_t mV = readRawVoltage();
    // 根据分压电路计算实际电压
    // 假设使用100K和100K的分压电阻
    return (float)mV * 2.0 / 1000.0; 
}

int Battery::getPercentage() {
    float voltage = getVoltage();
    if (voltage >= BATTERY_MAX_V) return 100;
    if (voltage <= BATTERY_MIN_V) return 0;
    
    // 线性插值计算百分比
    return (int)((voltage - BATTERY_MIN_V) / 
                 (BATTERY_MAX_V - BATTERY_MIN_V) * 100.0);
}

bool Battery::isLow() {
    return getVoltage() < LOW_BATTERY_THRESHOLD;
}