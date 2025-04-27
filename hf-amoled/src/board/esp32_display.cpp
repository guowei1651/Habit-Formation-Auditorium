#include "esp32_log.h"
#include "board/esp32_display.h"

#define tag "Display"

bool backlight_on = true;

Display::Display(DataBus *bus) {
    Arduino_DataBus *dataBus = bus->getBus();
    this->bus = bus;
    Arduino_GFX *gfx = new Arduino_SH8601(
        dataBus, -1, 0, false, LCD_WIDTH, LCD_HEIGHT
    );
    this->gfx = gfx;
}

void Display::begin() {
    Arduino_GFX *gfx = this->gfx;
    if (!gfx->begin()) {
        ESP32_LOGI(tag, "Display init failed!");
        return;
    }
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