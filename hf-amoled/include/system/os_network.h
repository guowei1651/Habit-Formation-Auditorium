#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <WebServer.h>
#include "system/os_config_manager.h"

class Network {
public:
    Network(ConfigManager& configManager);
    void begin();
    bool connectStoredWiFi();
    void startConfigPortal();
    bool isConnected();
    bool confirmIsConnected(); // 确认网络的连接状态
    
private:
    ConfigManager& _configManager;
    WebServer _server;
    void handleRoot();
    void handleSave();
    void handleScan();
    void sendHtmlPage();
    const char* AP_SSID = "HABIT_FORMATION";
    const int WIFI_TIMEOUT = 10000; // 10秒超时
};

#endif