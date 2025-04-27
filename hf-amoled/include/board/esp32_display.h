#pragma once

#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "esp32_data_bus.h"

class Display {
public:
    Display(DataBus *bus);
    void begin();
    void toggleBacklight();
    DataBus *getBus() { return bus; }
    Arduino_GFX *getGFX() { return gfx; }
private:
    DataBus *bus;
    Arduino_GFX *gfx;
};