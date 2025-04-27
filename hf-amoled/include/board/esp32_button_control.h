#ifndef BUTTON_CONTROL_H
#define BUTTON_CONTROL_H

#include <Arduino.h>
#include <Wire.h>
#include <ESP_IOExpander_Library.h>

class ButtonControl {
public:
    ButtonControl();
    void begin();
};

#endif