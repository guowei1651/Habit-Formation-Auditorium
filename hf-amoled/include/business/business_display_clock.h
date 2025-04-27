#ifndef CLOCK_H
#define CLOCK_H

#include <Arduino.h>

#include "board/esp32_board.h"

class Clock {
public:
    Clock(Display* display);
    void begin();
    void update();
    void setBrightness(uint8_t brightness);
    void turnOff();
    void turnOn();

private:
    Display* _display;
    void drawClockFace();
    void drawHands(unsigned long now);
    void redrawHandsCachedDrawAndErase();
    void drawRoundClockMark(int16_t innerR1, int16_t outerR1, 
                           int16_t innerR2, int16_t outerR2,
                           int16_t innerR3, int16_t outerR3);
    void drawAndEraseCachedLine(int16_t x0, int16_t y0, 
        int16_t x1, int16_t y1, int16_t color, int16_t *cache, int16_t cache_len, bool cross_check_second, bool cross_check_hour);
    void writeCachePixel(int16_t x, int16_t y, int16_t color, bool cross_check_second, bool cross_check_hour);

    // 时钟参数
    static const uint16_t BACKGROUND = 0x0000;  // BLACK
    static const uint16_t MARK_COLOR = 0xFFFF;  // WHITE 
    static const uint16_t SUBMARK_COLOR = 0x7BEF; // DARKGREY
    static const uint16_t HOUR_COLOR = 0xFFFF;   // WHITE
    static const uint16_t MINUTE_COLOR = 0x001F;  // BLUE
    static const uint16_t SECOND_COLOR = 0xF800;  // RED
    
    int16_t w, h, center;
    int16_t hHandLen, mHandLen, sHandLen, markLen;
    int16_t hh, mm, ss;
    unsigned long targetTime; // next action time

    float sdeg, mdeg, hdeg;
    int16_t osx = 0, osy = 0, omx = 0, omy = 0, ohx = 0, ohy = 0; // Saved H, M, S x & y coords
    int16_t nsx, nsy, nmx, nmy, nhx, nhy;                         // H, M, S x & y coords
    int16_t xMin, yMin, xMax, yMax;                               // redraw range
    
    int16_t *cached_points;
    uint16_t cached_points_idx = 0;
    int16_t *last_cached_point;

    bool isDisplayOn;
};

#endif