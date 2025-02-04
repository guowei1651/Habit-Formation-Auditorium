#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <WebServer.h>
#include "esp32_config_manager.h"
#include "esp32_display.h"

class Network {
public:
    Network(ConfigManager& configManager, Display& display);
    void begin();
    bool connectStoredWiFi();
    void startConfigPortal();
    bool isConnected();
    
private:
    ConfigManager& _configManager;
    Display& _display
    WebServer _server;
    void handleRoot();
    void handleSave();
    void sendHtmlPage();
    const char* AP_SSID = "Habit_FORMATION";
    const int WIFI_TIMEOUT = 10000; // 10秒超时
};

#endif