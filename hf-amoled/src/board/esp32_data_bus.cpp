#include <Arduino_GFX_Library.h>
#include "esp32_log.h"
#include "board/esp32_data_bus.h"

#define tag "Data Bus"

#define _EXAMPLE_CHIP_CLASS(name, ...) ESP_IOExpander_##name(__VA_ARGS__)
#define EXAMPLE_CHIP_CLASS(name, ...) _EXAMPLE_CHIP_CLASS(name, ##__VA_ARGS__)

DataBus::DataBus() {
    int coreID = xPortGetCoreID();
    ESP32_LOGI(tag, "DataBus 构造函数, coreID: %d", coreID);

    Wire.begin(IIC_SDA, IIC_SCL);

    ESP_IOExpander *expander = new EXAMPLE_CHIP_CLASS(TCA95xx_8bit,
        (i2c_port_t)0, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000,
        IIC_SCL, IIC_SDA);
    expander->init();
    expander->begin();
    expander->pinMode(0, OUTPUT);
    expander->pinMode(1, OUTPUT);
    expander->pinMode(2, OUTPUT);
    expander->digitalWrite(0, LOW);
    expander->digitalWrite(1, LOW);
    expander->digitalWrite(2, LOW);
    delay(20);
    expander->digitalWrite(0, HIGH);
    expander->digitalWrite(1, HIGH);
    expander->digitalWrite(2, HIGH);

    this->bus = new Arduino_ESP32QSPI(
        LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_SDIO0 /* SDIO0 */, LCD_SDIO1 /* SDIO1 */,
        LCD_SDIO2 /* SDIO2 */, LCD_SDIO3 /* SDIO3 */);
}

void DataBus::begin() {
    ESP32_LOGI(tag, "DataBus 初始化");
}
