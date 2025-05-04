#include "esp32_log.h"
#include "board/esp32_display.h"

#define tag "Display"

bool backlight_on = true;

Display::Display(DataBus *bus) {
    Arduino_DataBus *dataBus = bus->getBus();
    this->bus = bus;
    ESP32_LOGI(tag, "Display bus : %p", dataBus);
    Arduino_GFX *gfx = new Arduino_SH8601(bus->getBus(), (int8_t) -1 /* RST */, (int8_t) 0 /* rotation */, false /* IPS */, (int16_t) LCD_WIDTH, (int16_t) LCD_HEIGHT);
    this->gfx = gfx;
    ESP32_LOGI(tag, "Display gfx : %p", gfx);
}

void Display::begin() {
    Arduino_GFX *gfx = this->gfx;
    if (!gfx->begin()) {
        ESP32_LOGI(tag, "Display init failed!");
        delay(5000);
        ESP32_LOGI(tag, "Display init failed!");
        return;
    }
    gfx->fillScreen(WHITE);
    delay(1000);
}

// 切换亮暗
void Display::toggleBacklight() {
    ESP32_LOGI(tag, "Toggle backlight : %d", backlight_on);
    Arduino_GFX *gfx = this->gfx;
    if (backlight_on) {
      for (int i = 255; i >= 0; i--) {
        gfx->Display_Brightness(i);
        delay(3);
      }
    } else {
      for (int i = 0; i <= 255; i++) {
        gfx->Display_Brightness(i);
        delay(3);
      }
    }
    backlight_on = !backlight_on;
}