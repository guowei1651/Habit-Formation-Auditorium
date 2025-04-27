#pragma once

#include "board/esp32_board.h"
#include "business_display_clock.h"

class Business
{
public:
    Business(ESP32Board *esp32);
    void begin();
    void loop();
    void displayAuxiliary();
    void setClock(Clock *clock){ _clock = clock; };
    Clock* getClock(){ return _clock; };

private:
    ESP32Board *_esp32;
    Clock *_clock;
};

extern Business *business;