#include "../include/esp32_ota_manager.h"

void OTAManager::begin(const char* currentVersion) {
    _currentVersion = String(currentVersion);
    
    httpUpdate.setLedPin(LED_BUILTIN, LOW);
    
    // 证书验证回调
    httpUpdate.onProgress([](size_t progress, size_t total) {
        Serial.printf("Progress: %d%%\r", (progress / (total / 100)));
    });
}

bool OTAManager::checkUpdate(const char* updateUrl) {
    if (millis() - lastCheck < CHECK_INTERVAL) {
        return false;
    }
    
    lastCheck = millis();
    
    WiFiClient client;
    
    t_httpUpdate_return ret = httpUpdate.update(client, updateUrl);
    
    switch (ret) {
        case HTTP_UPDATE_FAILED:
            Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", 
                httpUpdate.getLastError(),
                httpUpdate.getLastErrorString().c_str());
            return false;
            
        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("No updates available");
            return false;
            
        case HTTP_UPDATE_OK:
            Serial.println("Update successful");
            ESP.restart();
            return true;
    }
    
    return false;
}

void OTAManager::onProgress(THandlerFunction_Progress fn) {
    httpUpdate.onProgress(fn);
}