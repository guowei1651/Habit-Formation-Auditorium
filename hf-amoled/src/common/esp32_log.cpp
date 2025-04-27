#include <stdarg.h>
#include <map>
#include <string>
#include <HardwareSerial.h>

#include "../include/esp32_log.h"

std::map<std::string, esp_log_level_t> log_levels;

void esp32_log_level_set(const char* tag, esp_log_level_t level) {
    log_levels[tag] = level;
}

esp_log_level_t esp32_log_level_get(const char* tag) {
    if (log_levels.find(tag) != log_levels.end()) {
        return log_levels[tag];
    }
    if (log_levels.find("*") != log_levels.end()) {
        return log_levels["*"];
    }
    return ESP_LOG_NONE; // Default log level if tag is not found
}

