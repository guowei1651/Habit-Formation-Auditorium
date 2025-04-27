#include "board/esp32_button_control.h"
#include "board/esp32_pins.h"

#define EXAMPLE_LVGL_TICK_PERIOD_MS 2
#define _EXAMPLE_CHIP_CLASS(name, ...) ESP_IOExpander_##name(__VA_ARGS__)
#define EXAMPLE_CHIP_CLASS(name, ...) _EXAMPLE_CHIP_CLASS(name, ##__VA_ARGS__)

ButtonControl::ButtonControl() {
}

void ButtonControl::begin() {

}

