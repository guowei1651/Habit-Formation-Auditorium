#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <Wire.h>
#include <ESP_IOExpander_Library.h>

#include "esp32_log.h"
#include "board/esp32_pins.h"
#include "XPowersLib.h"

class PowerManager {
public:
    PowerManager();
    void begin();
    void update();
    
    // 电源状态查询
    float getTemperature();
    uint16_t getBatteryVoltage();
    uint16_t getVbusVoltage();
    uint16_t getSystemVoltage();
    uint8_t getBatteryPercent();
    bool isCharging();
    bool isDischarging();
    bool isStandby();
    bool isVbusPresent();
    bool isVbusGood();
    bool isBatteryConnected();
    
    // ADC控制
    void enableADC();
    void disableADC();
    bool isADCEnabled() const { return adc_enabled; }
    
    // 开关机管理
    void shutdown();
    void asyncShutdown();
    void restart();
    void asyncRestart();

    // 中断处理
    void handleIRQ();
    static void setIRQFlag() { pmu_irq_flag = true; }
    
    // 获取充电状态描述
    const char* getChargeStatusStr();

private:
    XPowersPMU *pmu;
    ESP_IOExpander *expander;
    static bool pmu_irq_flag;
    bool adc_enabled;

    void initPMU();
    void initExpander();
};

#endif