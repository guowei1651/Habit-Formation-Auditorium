#include "../include/esp32_log.h"
#include "../include/esp32_time.h"
#include <WiFi.h>

const char *ntpTag = "ntp";

CustomNTPClient::CustomNTPClient(Network *network)
{
    _network = network;
    _ntpUDP = new WiFiUDP();
    _timeClient = new NTPClient(*_ntpUDP, NTP_SERVER, 8 * 3600, 5); // udp，服务器地址，时间偏移量，更新间隔
}

void CustomNTPClient::begin()
{
    syncTime();
}

bool CustomNTPClient::syncTime()
{
    if (!_network->confirmIsConnected()) {
        ESP32_LOGI(ntpTag, "确认网络连接失败");
        return false;
    }

    _completeCnt = 0;

    char *timeServers[] = {
        // Windows系统上自带的两个：
        "time.windows.com",
        "time.nist.gov",
        // aliyun
        "ntp.aliyun.com",
        // tencent
        "time1.cloud.tencent.com",
        "time2.cloud.tencent.com",
        "time3.cloud.tencent.com",
        "time4.cloud.tencent.com",
        "time5.cloud.tencent.com",
        // NTP授时快速域名服务：
        "cn.ntp.org.cn",
        // 中国科学院国家授时中心：
        "ntp.ntsc.ac.cn",
        // 开源NTP服务器：
        "cn.pool.ntp.org",
        // MacOS上自带的两个：
        "time.apple.com",
        "time.asia.apple.com",
    };

    uint8_t update_count = 0;
    _timeClient->begin();
    while (_timeClient->update() == 0 && update_count < 13)
    {
        delay(1000);

        _timeClient->setPoolServerName(timeServers[update_count]);
        ESP32_LOGI(ntpTag, "ntp设置server进行跟新 cnt:%d server:%s", update_count, timeServers[update_count]);

        update_count++;
    }

    if (update_count >= 13)
    {
        ESP32_LOGE(ntpTag, "ntp更新时间未成功");
        return false;
    }
    
    _completeCnt ++;
    time_t epochTime = _timeClient->getEpochTime();
    struct timeval tv = { epochTime, 0 };
    settimeofday(&tv, nullptr);
    // setenv("TZ", "CST-8", 1);
    // tzset();
    ESP32_LOGI(ntpTag, "ntp时间同步成功: %s", getFormattedTime().c_str());

    return true;
}

bool CustomNTPClient::updateIfNeeded()
{
    if (millis() - lastSync >= SYNC_INTERVAL || _completeCnt == 0)
    {
        return syncTime();
    }
    return true;
}

String CustomNTPClient::getFormattedTime()
{
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return String(buffer);
}
