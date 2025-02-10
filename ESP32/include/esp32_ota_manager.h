#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include <Arduino.h>
#include <HTTPUpdate.h>
#include <functional>
#include <HTTPClient.h>
#include <Update.h>

class OTAManager {
public:
    void begin();
    void onProgress(UpdateClass::THandlerFunction_Progress fn);
    bool checkUpdate(const char* updateUrl);
    
private:
    static void updateProgress(int progress, int total);
    const int CHECK_INTERVAL = 24 * 60 * 60 * 1000; // 24小时检查一次
    unsigned long lastCheck = 0;
    const char* _currentVersion = "v0.1";
};

#endif