#ifndef IOT_H
#define IOT_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

struct IoTDevice {
  IoTClient iotClient;
};

enum DeviceState {
    INIT,           // 初始化状态
    REGISTERING,    // 注册中
    REGISTERED,     // 已注册
    CONNECTING,     // 连接中
    CONNECTED,      // 已连接
    ERROR          // 错误状态
};

class IoTClient {
public:
    IoTClient();
    void begin(const char* serverUrl, const char* deviceId);
    void loop();
    bool reportButtonPress(int buttonId);
    bool reportStatus(float temperature, float humidity);
    String getDeviceConfig();
    
private:
    DeviceState _state;
    String _serverUrl;
    String _deviceId;
    String _token;
    unsigned long _lastHeartbeat;
    const unsigned long HEARTBEAT_INTERVAL = 60000; // 60秒心跳间隔
    
    bool registerDevice();
    bool sendHeartbeat();
    bool queryDeviceInfo();
    void setState(DeviceState newState);
    void handleState();
    bool sendRequest(const char* endpoint, const char* method, const String& payload);
};

#endif