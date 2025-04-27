#ifndef ESP32_BOARD_H
#define ESP32_BOARD_H

#include "board/esp32_pins.h"
#include "board/esp32_data_bus.h"
#include "board/esp32_power_manager.h"
#include "board/esp32_display.h"
#include "board/esp32_button_control.h"

#define FIRMWARE_VERSION "1.0.0"
#define UPDATE_URL "http://your-server.com/firmware.bin"

struct ESP32Board {
    String uniqueID;
    DataBus *dataBus;
    PowerManager *powerManager;
    Display *display;
    ButtonControl *button;
};

String getUniqueID();

extern ESP32Board board;

#endif