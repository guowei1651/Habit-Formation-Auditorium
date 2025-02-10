#ifndef NTP_CLIENT_H
#define NTP_CLIENT_H

#include <Arduino.h>
#include <time.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "../include/esp32_network.h"

class CustomNTPClient
{
public:
    CustomNTPClient(Network *network);
    void begin();
    bool syncTime();
    bool updateIfNeeded();
    String getFormattedTime();

private:
    Network *_network;
    WiFiUDP *_ntpUDP;
    NTPClient *_timeClient;
    int _completeCnt = 0;
    const char *NTP_SERVER = "pool.ntp.org";
    const char *TZ_INFO = "CST-8"; // 中国时区
    unsigned long lastSync = 0;
    const unsigned long SYNC_INTERVAL = 10 * 60 * 60 * 1000; // 24小时
};

#endif