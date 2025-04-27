#include <Arduino_GFX_Library.h>
#include "esp32_log.h"
#include "board/esp32_data_bus.h"

#define tag "Data Bus"

DataBus::DataBus() {
    bus = new Arduino_ESP32QSPI(
        LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1, LCD_SDIO2, LCD_SDIO3
    );
}

void DataBus::begin() {
    ESP32_LOGI(tag, "DataBus 初始化");
}
