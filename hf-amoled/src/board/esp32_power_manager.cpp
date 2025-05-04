#include "board/esp32_power_manager.h"

#define TAG "PowerManager"

bool PowerManager::pmu_irq_flag = false;

PowerManager::PowerManager() : adc_enabled(false), expander(nullptr) {
    pmu = new XPowersPMU();
    if (pmu == nullptr) {
        ESP32_LOGI(TAG, "Failed to allocate memory for PMU");
        return;
    }
    pmu_irq_flag = false;
}

void PowerManager::begin() {
    // 初始化PMU
    if (!pmu->begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
        ESP32_LOGI(TAG, "PMU initialization failed!");
        return;
    }
    
    initPMU();
    initExpander();
    enableADC();
}

void PowerManager::initPMU() {
    pmu->disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
    pmu->setChargeTargetVoltage(3);
    pmu->clearIrqStatus();
    pmu->enableIRQ(XPOWERS_AXP2101_PKEY_SHORT_IRQ);
}

void PowerManager::initExpander() {
    expander = new ESP_IOExpander_TCA95xx_8bit(
        (i2c_port_t)0, 
        ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000,
        IIC_SCL, 
        IIC_SDA
    );
    
    expander->init();
    expander->begin();
    
    // 配置IO
    expander->pinMode(5, INPUT);  // PMU IRQ
    expander->pinMode(1, OUTPUT); // Power control 1
    expander->pinMode(2, OUTPUT); // Power control 2
    
    // 初始化电源控制
    expander->digitalWrite(1, LOW);
    expander->digitalWrite(2, LOW);
    delay(20);
    expander->digitalWrite(1, HIGH);
    expander->digitalWrite(2, HIGH);
    
    if (expander->digitalRead(5)) {
        setIRQFlag();
    }
}

void PowerManager::enableADC() {
    pmu->enableTemperatureMeasure();
    pmu->enableBattDetection();
    pmu->enableVbusVoltageMeasure();
    pmu->enableBattVoltageMeasure();
    pmu->enableSystemVoltageMeasure();
    adc_enabled = true;
}

void PowerManager::disableADC() {
    pmu->disableTemperatureMeasure();
    pmu->disableBattDetection();
    pmu->disableVbusVoltageMeasure();
    pmu->disableBattVoltageMeasure();
    pmu->disableSystemVoltageMeasure();
    adc_enabled = false;
}

void PowerManager::handleIRQ() {
    if (pmu_irq_flag) {
        pmu_irq_flag = false;
        uint32_t status = pmu->getIrqStatus();
        if (pmu->isPekeyShortPressIrq()) {
            if (adc_enabled) {
                disableADC();
                ESP32_LOGI(TAG, "ADC disabled");
            } else {
                enableADC();
                ESP32_LOGI(TAG, "ADC enabled");
            }
        }
        pmu->clearIrqStatus();
    }
}

void PowerManager::restart() {
    delay(5000);   // 延时 5 秒
    esp_restart(); // 软件重启
}

void restartCallback(void* arg) {
    esp_restart();
}

void PowerManager::asyncRestart() {
    esp_timer_handle_t timer;
    esp_timer_create_args_t timer_args = {
      .callback = &restartCallback,
      .name = "restart_timer"
    };
    esp_timer_create(&timer_args, &timer);
    esp_timer_start_once(timer, 5000000); // 5,000,000微秒 = 5秒
}

void PowerManager::shutdown() {
    delay(5000);   // 延时 5 秒
    pmu->shutdown(); // 正确关机指令
}

void shutdownCallback(void* arg) {
    XPowersPMU *pmu = (XPowersPMU *)arg;
    pmu->shutdown(); // 正确关机指令
}

void PowerManager::asyncShutdown() {
    esp_timer_handle_t timer;
    esp_timer_create_args_t timer_args = {
        .callback = &shutdownCallback,
        .arg = pmu,
        .name = "shutdown_timer"
    };
    esp_timer_create(&timer_args, &timer);
    esp_timer_start_once(timer, 5000000); // 5,000,000微秒 = 5秒
}

const char* PowerManager::getChargeStatusStr() {
    switch (pmu->getChargerStatus()) {
        case XPOWERS_AXP2101_CHG_TRI_STATE:
            return "Tri-state charging";
        case XPOWERS_AXP2101_CHG_PRE_STATE:
            return "Pre-charging";
        case XPOWERS_AXP2101_CHG_CC_STATE:
            return "Constant current charging";
        case XPOWERS_AXP2101_CHG_CV_STATE:
            return "Constant voltage charging";
        case XPOWERS_AXP2101_CHG_DONE_STATE:
            return "Charging complete";
        case XPOWERS_AXP2101_CHG_STOP_STATE:
            return "Not charging";
        default:
            return "Unknown status";
    }
}

// 实现其他getter方法
float PowerManager::getTemperature() { return pmu->getTemperature(); }
uint16_t PowerManager::getBatteryVoltage() { return pmu->getBattVoltage(); }
uint16_t PowerManager::getVbusVoltage() { return pmu->getVbusVoltage(); }
uint16_t PowerManager::getSystemVoltage() { return pmu->getSystemVoltage(); }
uint8_t PowerManager::getBatteryPercent() { return pmu->getBatteryPercent(); }
bool PowerManager::isCharging() { return pmu->isCharging(); }
bool PowerManager::isDischarging() { return pmu->isDischarge(); }
bool PowerManager::isStandby() { return pmu->isStandby(); }
bool PowerManager::isVbusPresent() { return pmu->isVbusIn(); }
bool PowerManager::isVbusGood() { return pmu->isVbusGood(); }
bool PowerManager::isBatteryConnected() { return pmu->isBatteryConnect(); }