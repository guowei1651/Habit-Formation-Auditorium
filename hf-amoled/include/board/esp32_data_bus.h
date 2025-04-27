#pragma once

#include <Arduino.h>
#include <ESP_IOExpander_Library.h>
#include <Arduino_GFX_Library.h>

#include <lvgl.h>
#include <Wire.h>
#include <Arduino.h>
#include "esp32_pins.h"
#include "XPowersLib.h"
#include "lv_conf.h"
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include <ESP_IOExpander_Library.h>
#include "HWCDC.h"

class DataBus {
public:
    DataBus();
    void begin();
    Arduino_DataBus *getBus() { return bus; }
private:
    Arduino_DataBus *bus;
};