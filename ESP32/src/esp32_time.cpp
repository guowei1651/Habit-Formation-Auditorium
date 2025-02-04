#include "../include/esp32_time.h"

void NTPClient::begin() {
    configTime(0, 0, NTP_SERVER);
    setenv("TZ", TZ_INFO, 1);
    tzset();
}

bool NTPClient::syncTime() {
    time_t now = time(nullptr);
    int retry = 0;
    while(now < 24 * 3600 && retry < 10) {
        delay(500);
        now = time(nullptr);
        retry++;
    }
    
    if (now > 24 * 3600) {
        lastSync = millis();
        return true;
    }
    return false;
}

bool NTPClient::updateIfNeeded() {
    if (millis() - lastSync >= SYNC_INTERVAL) {
        return syncTime();
    }
    return true;
}

String NTPClient::getFormattedTime() {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return String(buf);
}