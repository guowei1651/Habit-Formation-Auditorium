#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>

#include "../include/pins.h"

class Display {
public:
    Display();
    void begin();
    void clearScreen();
    void displayImage(const uint8_t* blackImage, const uint8_t* redImage);
    void sleep();
    void wakeUp();
    
private:
    GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> display;
};

#endif