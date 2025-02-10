#ifndef ESP32_BOARD_H
#define ESP32_BOARD_H

#include "esp32_battery.h"
#include "esp32_button_interrupts.h"
#include "esp32_sht30.h"
#include "esp32_ota_manager.h"
#include "esp32_config_manager.h"
#include "esp32_network.h"
#include "esp32_time.h"
#include "esp32_display.h"

#define FIRMWARE_VERSION "1.0.0"
#define UPDATE_URL "http://your-server.com/firmware.bin"

struct ESP32Board {
    String uniqueID;
    SHT30 *sht30;
    ButtonManager *buttonManager;
    OTAManager *otaManager;
    ConfigManager *configManager;
    Network *network;
    CustomNTPClient *ntpClient;
    Display* display;
    Battery *battery;
};

String getUniqueID();

extern ESP32Board esp32;

#endif