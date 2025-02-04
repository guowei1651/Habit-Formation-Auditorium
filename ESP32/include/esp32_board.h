#ifndef ESP32_BOARD_H
#define ESP32_BOARD_H

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
    OTAManager *otaManager;
    ConfigManager *configManager;
    Network *network;
    NTPClient *ntpClient;
    Display *display;
    Battery *battery;
};

String getUniqueID()

#endif