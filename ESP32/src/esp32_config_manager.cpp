#include "../include/esp32_config_manager.h"

ConfigManager::ConfigManager() : dirty(false) {}

void ConfigManager::begin() {
    preferences.begin(NVS_NAMESPACE, false);
}

bool ConfigManager::saveWiFiConfig(const char* ssid, const char* password) {
    if (strlen(ssid) > 0 && strlen(password) > 0) {
        preferences.putString("wifi_ssid", ssid);
        preferences.putString("wifi_pass", password);
        return true;
    }
    return false;
}

bool ConfigManager::saveAPIConfig(const char* api1_url, const char* api2_url) {
    if (strlen(api1_url) > 0 && strlen(api2_url) > 0) {
        preferences.putString("api1_url", api1_url);
        preferences.putString("api2_url", api2_url);
        return true;
    }
    return false;
}

bool ConfigManager::setValue(const char* key, const String& value) {
    if (configCache[key] != value) {
        configCache[key] = value;
        dirty = true;
        return true;
    }
    return false;
}

String ConfigManager::getValue(const char* key, const String& defaultValue) {
    if (configCache.find(key) == configCache.end()) {
        loadFromNVS(key);
    }
    auto it = configCache.find(key);
    return it != configCache.end() ? it->second : defaultValue;
}

void ConfigManager::loadFromNVS(const char* key) {
    String value = preferences.getString(key, "");
    if (value.length() > 0) {
        configCache[key] = value;
    }
}

void ConfigManager::saveToNVS(const char* key, const String& value) {
    preferences.putString(key, value.c_str());
}

void ConfigManager::commit() {
    if (dirty) {
        for (const auto& pair : configCache) {
            saveToNVS(pair.first.c_str(), pair.second);
        }
        dirty = false;
    }
}

void ConfigManager::clearAll() {
    preferences.clear();
    configCache.clear();
    dirty = false;
}

bool ConfigManager::isDirty() const {
    return dirty;
}