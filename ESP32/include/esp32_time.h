#ifndef NTP_CLIENT_H
#define NTP_CLIENT_H

#include <Arduino.h>
#include <time.h>

class NTPClient {
public:
    void begin();
    bool syncTime();
    bool updateIfNeeded();
    String getFormattedTime();
    
private:
    const char* NTP_SERVER = "pool.ntp.org";
    const char* TZ_INFO = "CST-8";  // 中国时区
    unsigned long lastSync = 0;
    const unsigned long SYNC_INTERVAL = 24 * 60 * 60 * 1000; // 24小时
};

#endif