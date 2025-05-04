#include "business/business_main.h"

#define TAG "Business"


// 实现构造函数
Business::Business(ESP32Board *esp32) : _esp32(esp32), _clock(nullptr) {
}

void Business::begin() {
    // 初始化代码
}

void Business::loop() {
    ESP32_LOGI(TAG, "Running business logic...");
    if (_clock) {
        ESP32_LOGI(TAG, "Running business clock logic...");
        _clock->update();
    }
}

void Business::displayAuxiliary() {
    // 显示辅助信息的代码
}

// 创建全局实例
Business *business = new Business(&board);  // 然后创建business实例
