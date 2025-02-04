#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include <Arduino.h>
#include <HTTPUpdate.h>
#include <HTTPClient.h>
#include <Update.h>

class OTAManager {
public:
    void begin(const char* currentVersion);
    bool checkUpdate(const char* updateUrl);
    void onProgress(THandlerFunction_Progress fn);
    
private:
    String _currentVersion;
    static void updateProgress(int progress, int total);
    const int CHECK_INTERVAL = 24 * 60 * 60 * 1000; // 24小时检查一次
    unsigned long lastCheck = 0;
};

#endif