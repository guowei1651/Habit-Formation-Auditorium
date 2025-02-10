#ifndef ESP32_LOG
#define ESP32_LOG

#include <HardwareSerial.h>
#include <esp_log.h>

#define LOG32_FORMAT(format)  "(%u) [%s] <%s> [%s:%u] %s(): " format " \n"

void esp32_log_level_set(const char* tag, esp_log_level_t level);
esp_log_level_t esp32_log_level_get(const char* tag);

#define ESP32_LOG_LEVEL(level, tag, format, ...) do {                     \
        esp_log_level_t setLevel = esp32_log_level_get(tag);              \
        if (level <= setLevel) {                                          \
            if (level==ESP_LOG_ERROR )          { Serial.printf(LOG32_FORMAT(format), esp_log_timestamp(), "ERR", tag, pathToFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); } \
            else if (level==ESP_LOG_WARN )      { Serial.printf(LOG32_FORMAT(format), esp_log_timestamp(), "WAR", tag, pathToFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); } \
            else if (level==ESP_LOG_DEBUG )     { Serial.printf(LOG32_FORMAT(format), esp_log_timestamp(), "DEB", tag, pathToFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); } \
            else if (level==ESP_LOG_VERBOSE )   { Serial.printf(LOG32_FORMAT(format), esp_log_timestamp(), "VER", tag, pathToFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); } \
            else                                { Serial.printf(LOG32_FORMAT(format), esp_log_timestamp(), "INF", tag, pathToFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); } \
            Serial.flush();                                               \
        }          \
    } while(0)

#define ESP32_LOGE( tag, format, ... ) ESP32_LOG_LEVEL(ESP_LOG_ERROR,   tag, format, ##__VA_ARGS__)
#define ESP32_LOGW( tag, format, ... ) ESP32_LOG_LEVEL(ESP_LOG_WARN,    tag, format, ##__VA_ARGS__)
#define ESP32_LOGI( tag, format, ... ) ESP32_LOG_LEVEL(ESP_LOG_INFO,    tag, format, ##__VA_ARGS__)
#define ESP32_LOGD( tag, format, ... ) ESP32_LOG_LEVEL(ESP_LOG_DEBUG,   tag, format, ##__VA_ARGS__)
#define ESP32_LOGV( tag, format, ... ) ESP32_LOG_LEVEL(ESP_LOG_VERBOSE, tag, format, ##__VA_ARGS__)



#endif