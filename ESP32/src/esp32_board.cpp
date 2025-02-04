#include <esp_efuse.h>
#include <esp_efuse_table.h>
#include <esp_system.h>

#include "../include/esp32_board.h"

String bytesToHex(uint8_t* bytes, int len) {
    String result = "";
    for (int i = 0; i < len; i++) {
        if (bytes[i] < 16) result += "0";
        result += String(bytes[i], HEX);
    }
    return result;
}

String getUniqueID() {
    uint8_t efuseData[8] = {0};
    esp_efuse_read_block(EFUSE_BLK0, efuseData, 0, 8 * 8);

    bool isValid = false;
    for (int i = 0; i < 8; i++) {
        if (efuseData[i] != 0) {
            isValid = true;
            break;
        }
    }

    if (isValid) {
        return bytesToHex(efuseData, 6);
    }

    uint8_t chipid[6] = {0};
    esp_efuse_mac_get_default(chipid);
    return bytesToHex(chipid, 6);
}