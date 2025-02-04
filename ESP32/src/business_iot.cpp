#include "../include/iot.h"

IoTClient::IoTClient() : _state(INIT), _lastHeartbeat(0) {}

void IoTClient::begin(const char* serverUrl, const char* deviceId) {
    _serverUrl = serverUrl;
    _deviceId = deviceId;
    setState(REGISTERING);
}

void IoTClient::loop() {
    handleState();
    
    if (_state == CONNECTED && millis() - _lastHeartbeat >= HEARTBEAT_INTERVAL) {
        sendHeartbeat();
    }
}

void IoTClient::setState(DeviceState newState) {
    _state = newState;
}

void IoTClient::handleState() {
    switch (_state) {
        case REGISTERING:
            if (registerDevice()) {
                setState(REGISTERED);
            } else {
                setState(ERROR);
            }
            break;
            
        case REGISTERED:
            setState(CONNECTING);
            break;
            
        case CONNECTING:
            if (queryDeviceInfo()) {
                setState(CONNECTED);
            } else {
                setState(ERROR);
            }
            break;
            
        case ERROR:
            delay(5000);  // 错误状态等待5秒后重试
            setState(REGISTERING);
            break;
            
        default:
            break;
    }
}

bool IoTClient::registerDevice() {
    StaticJsonDocument<200> doc;
    doc["deviceId"] = _deviceId;
    doc["type"] = "ESP32";
    
    String payload;
    serializeJson(doc, payload);
    
    return sendRequest("/register", "POST", payload);
}

bool IoTClient::sendHeartbeat() {
    _lastHeartbeat = millis();
    return sendRequest("/heartbeat", "POST", "{\"deviceId\":\"" + _deviceId + "\"}");
}

bool IoTClient::reportButtonPress(int buttonId) {
    StaticJsonDocument<200> doc;
    doc["deviceId"] = _deviceId;
    doc["buttonId"] = buttonId;
    
    String payload;
    serializeJson(doc, payload);
    
    return sendRequest("/event", "POST", payload);
}

bool IoTClient::reportStatus(float temperature, float humidity) {
    StaticJsonDocument<200> doc;
    doc["deviceId"] = _deviceId;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    
    String payload;
    serializeJson(doc, payload);
    
    return sendRequest("/status", "POST", payload);
}

String IoTClient::getDeviceConfig() {
    if (sendRequest("/config", "GET", "")) {
        // 返回配置信息
        return "{}";  // 实际应返回服务器响应
    }
    return "{}";
}

bool IoTClient::sendRequest(const char* endpoint, const char* method, const String& payload) {
    HTTPClient http;
    http.begin(_serverUrl + endpoint);
    http.addHeader("Content-Type", "application/json");
    
    int httpCode;
    if (strcmp(method, "GET") == 0) {
        httpCode = http.GET();
    } else {
        httpCode = http.POST(payload);
    }
    
    bool success = (httpCode == 200);
    http.end();
    return success;
}