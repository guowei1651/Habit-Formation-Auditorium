#include "business/business_main.h"

// 实现构造函数
Business::Business(ESP32Board *esp32) : _esp32(esp32), _clock(nullptr) {
}

void Business::begin() {
    // 初始化代码
}

void Business::loop() {
    if (_clock) {
        _clock->update();
    }
}

void Business::displayAuxiliary() {
    // 显示辅助信息的代码
}

// 创建全局实例
Business *business = new Business(&board);  // 然后创建business实例
