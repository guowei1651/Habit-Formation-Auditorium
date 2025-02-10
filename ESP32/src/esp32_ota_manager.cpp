#include "../include/esp32_log.h"
#include "../include/esp32_ota_manager.h"

const char* otaTag = "ota";

void OTAManager::begin() {
    esp32_log_level_set(otaTag, ESP_LOG_DEBUG);
    
    ESP32_LOGD(otaTag, "OTAManager begin");

    // 证书验证回调
    httpUpdate.onProgress([](size_t progress, size_t total) {
        ESP32_LOGI(otaTag, "Progress: %d%%\r", (progress / (total / 100)));
    });
}

bool OTAManager::checkUpdate(const char* updateUrl) {
    ESP32_LOGD(otaTag, "checkUpdate start");
    if (millis() - lastCheck < CHECK_INTERVAL) {
        return false;
    }
    
    lastCheck = millis();

    ESP32_LOGD(otaTag, "开始进行升级, ");
    WiFiClient client;
    t_httpUpdate_return ret = httpUpdate.update(client, updateUrl);
    ESP32_LOGD(otaTag, "升级结果: %d ", ret);

    switch (ret) {
        case HTTP_UPDATE_FAILED:
            ESP32_LOGD(otaTag, "HTTP_UPDATE_FAILED Error (%d): %s\n", 
                httpUpdate.getLastError(),
                httpUpdate.getLastErrorString().c_str());
            return false;
            
        case HTTP_UPDATE_NO_UPDATES:
            ESP32_LOGD(otaTag, "No updates available");
            return false;
            
        case HTTP_UPDATE_OK:
            ESP32_LOGD(otaTag, "Update successful");
            // ESP.restart();
            return true;
    }
    
    return false;
}

void OTAManager::onProgress(UpdateClass::THandlerFunction_Progress fn) {
    httpUpdate.onProgress(fn);
}