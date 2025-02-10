#include "../include/esp32_log.h"
#include "../include/esp32_pins.h"
#include "../include/esp32_battery.h"

// https://github.com/HwzLoveDz/ESP32-ADC-sense-battery-voltage/blob/main/ESP32_ADC_VBAT.ino

const char *batteryTag = "battery";

Battery::Battery(int batteryPin, int weakIOPin) : _batteryPin(batteryPin), _weakioPin(weakIOPin) {}

void Battery::begin()
{
    esp32_log_level_set(batteryTag, ESP_LOG_INFO);

    pinMode(_weakioPin, OUTPUT);
    initADC();
}

void Battery::initADC()
{
    // 配置ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);

    // 特性曲线校准
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12,
                             1100, &_adc_chars);
}

/*
另一种写法：
  digitalWrite(25, HIGH); // 拉高针脚25
  delay(10); // 等待一段时间以确保电压稳定
//   analogSetPinAttenuation(36, ADC_ATTENDB_MAX); // 设置针脚36的衰减为12db
  float adc_reading = 0;

  // 多次采样取平均值
  for (int i = 0; i < 20; i++) {
      delay(10);
      // uint32_t voltage = analogReadMilliVolts(36);
      // uint16_t voltage = analogReadRaw(36);
      uint16_t voltage = analogRead(36);
      ESP32_LOGI(logTag, "采样过程中的采样次数: %d 电压: %d", i, voltage);
      adc_reading += voltage * 0.001015;
  }
  ESP32_LOGI(logTag, "采样结果中的电压: %f", adc_reading);
  // adc_reading /= 20;
  adc_reading = (adc_reading * 6.6 / 81.9 + 0.27);
*/

uint32_t Battery::readRawVoltage()
{
    uint32_t adc_reading = 0;

    // 睡眠前禁用GPIO39中断
    gpio_intr_disable(GPIO_NUM_39);

    digitalWrite(_weakioPin, HIGH); // 拉高针脚25
    delay(10);                      // 等待一段时间以确保电压稳定

    // 多次采样取平均值
    for (int i = 0; i < 64; i++)
    {
        delay(1);
        int rawValue = adc1_get_raw(ADC1_CHANNEL_0);
        ESP32_LOGD(batteryTag, "使用adc采样的值。i: %d raw value:%d", i, rawValue);
        adc_reading += adc1_get_raw(ADC1_CHANNEL_0);
    }
    ESP32_LOGD(batteryTag, "使用adc采样合计值。raw value:%d", adc_reading);
    adc_reading /= 64;
    ESP32_LOGD(batteryTag, "使用adc采样平均值。raw value:%d", adc_reading);

    // 转换为实际电压(mV)
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, &_adc_chars);
    ESP32_LOGD(batteryTag, "使用adc采样转换出的电压值。raw value:%d", voltage);

    digitalWrite(_weakioPin, LOW); // 拉低针脚25

    // 唤醒后重新启用GPIO39中断
    gpio_intr_enable(GPIO_NUM_39);

    return voltage;
}

float Battery::getVoltage()
{
    uint32_t mV = readRawVoltage();
    // 根据分压电路计算实际电压
    // 假设使用10K和10K的分压电阻
    return (float)mV * 2.0 / 1000.0;
}

int Battery::getPercentage()
{
    float voltage = getVoltage();
    if (voltage >= BATTERY_MAX_V)
        return 100;
    if (voltage <= BATTERY_MIN_V)
        return 0;

    // 线性插值计算百分比
    return (int)((voltage - BATTERY_MIN_V) /
                 (BATTERY_MAX_V - BATTERY_MIN_V) * 100.0);
}

bool Battery::isLow()
{
    return getVoltage() < LOW_BATTERY_THRESHOLD;
}