#include "business/business_display_clock.h"

#define TAG "Clock"

#define SIXTIETH 0.016666667
#define TWELFTH 0.08333333
#define SIXTIETH_RADIAN 0.10471976
#define TWELFTH_RADIAN 0.52359878
#define RIGHT_ANGLE_RADIAN 1.5707963

Clock::Clock(Display *display) : _display(display), isDisplayOn(true) {
}

void Clock::begin() {
    ESP32_LOGI(TAG, "Initializing clock...");
    Arduino_GFX *gfx = _display->getGFX();

    // 初始化显示屏参数
    w = gfx->width();
    h = gfx->height();
    center = (w < h) ? w/2 : h/2;
    ESP32_LOGI(TAG, "Display size: %d x %d, Center: %d", w, h, center);

    hHandLen = center * 3 / 8;
    mHandLen = center * 2 / 3;
    sHandLen = center * 5 / 6;
    markLen = sHandLen / 6;

    // 初始显示
    ESP32_LOGI(TAG, "Filling screen with background color...");
    gfx->fillScreen(BACKGROUND);
    drawClockFace();
    
    // 设置初始时间
    ESP32_LOGI(TAG, "Setting initial time...");
    hh = 12;
    mm = 0;
    ss = 0;
    targetTime = millis() + 1000;
}

void Clock::update() {
    if (!isDisplayOn) return;
    
    ESP32_LOGI(TAG, "Updating clock...");

    unsigned long now = millis();
    if (now >= targetTime) {
        targetTime += 1000;
        ss++;
        if (ss == 60) {
            ss = 0;
            mm++;
            if (mm == 60) {
                mm = 0;
                hh++;
                if (hh == 24) hh = 0;
            }
        }
        drawHands(now);
    }
}

void Clock::setBrightness(uint8_t brightness) {
    Arduino_GFX *gfx = _display->getGFX();
    gfx->Display_Brightness(brightness);
}

void Clock::turnOff() {
    Arduino_GFX *gfx = _display->getGFX();
    isDisplayOn = false;
    gfx->Display_Brightness(0);
}

void Clock::turnOn() {
    Arduino_GFX *gfx = _display->getGFX();
    isDisplayOn = true;
    gfx->Display_Brightness(255);
}

void Clock::drawClockFace() {
    Arduino_GFX *gfx = _display->getGFX();
    // init LCD constant
    w = gfx->width();
    h = gfx->height();
    if (w < h)
    {
      center = w / 2;
    }
    else
    {
      center = h / 2;
    }
    hHandLen = center * 3 / 8;
    mHandLen = center * 2 / 3;
    sHandLen = center * 5 / 6;
    markLen = sHandLen / 6;
    cached_points = (int16_t *)malloc((hHandLen + 1 + mHandLen + 1 + sHandLen + 1) * 2 * 2);

    // Draw 60 clock marks
    drawRoundClockMark(
        // draw_square_clock_mark(
        center - markLen, center,
        center - (markLen * 2 / 3), center,
        center - (markLen / 2), center);
}

void Clock::drawHands(unsigned long now) {
  // Pre-compute hand degrees, x & y coords for a fast screen update
  sdeg = SIXTIETH_RADIAN * ((0.001 * (now % 1000)) + ss); // 0-59 (includes millis)
  nsx = cos(sdeg - RIGHT_ANGLE_RADIAN) * sHandLen + center;
  nsy = sin(sdeg - RIGHT_ANGLE_RADIAN) * sHandLen + center;
  if ((nsx != osx) || (nsy != osy))
  {
    mdeg = (SIXTIETH * sdeg) + (SIXTIETH_RADIAN * mm); // 0-59 (includes seconds)
    hdeg = (TWELFTH * mdeg) + (TWELFTH_RADIAN * hh);   // 0-11 (includes minutes)
    mdeg -= RIGHT_ANGLE_RADIAN;
    hdeg -= RIGHT_ANGLE_RADIAN;
    nmx = cos(mdeg) * mHandLen + center;
    nmy = sin(mdeg) * mHandLen + center;
    nhx = cos(hdeg) * hHandLen + center;
    nhy = sin(hdeg) * hHandLen + center;

    // redraw hands
    redrawHandsCachedDrawAndErase();

    ohx = nhx;
    ohy = nhy;
    omx = nmx;
    omy = nmy;
    osx = nsx;
    osy = nsy;

    delay(1);
  }
}

void Clock::redrawHandsCachedDrawAndErase()
{
  Arduino_GFX *gfx = _display->getGFX();
  gfx->startWrite();
  drawAndEraseCachedLine(center, center, nsx, nsy, SECOND_COLOR, cached_points, sHandLen + 1, false, false);
  drawAndEraseCachedLine(center, center, nhx, nhy, HOUR_COLOR, cached_points + ((sHandLen + 1) * 2), hHandLen + 1, true, false);
  drawAndEraseCachedLine(center, center, nmx, nmy, MINUTE_COLOR, cached_points + ((sHandLen + 1 + hHandLen + 1) * 2), mHandLen + 1, true, true);
  gfx->endWrite();
}

void Clock::drawRoundClockMark(int16_t innerR1, int16_t outerR1, int16_t innerR2, int16_t outerR2, int16_t innerR3, int16_t outerR3)
{
  float x, y;
  int16_t x0, x1, y0, y1, innerR, outerR;
  uint16_t c;

  for (uint8_t i = 0; i < 60; i++)
  {
    if ((i % 15) == 0)
    {
      innerR = innerR1;
      outerR = outerR1;
      c = MARK_COLOR;
    }
    else if ((i % 5) == 0)
    {
      innerR = innerR2;
      outerR = outerR2;
      c = MARK_COLOR;
    }
    else
    {
      innerR = innerR3;
      outerR = outerR3;
      c = SUBMARK_COLOR;
    }

    mdeg = (SIXTIETH_RADIAN * i) - RIGHT_ANGLE_RADIAN;
    x = cos(mdeg);
    y = sin(mdeg);
    x0 = x * outerR + center;
    y0 = y * outerR + center;
    x1 = x * innerR + center;
    y1 = y * innerR + center;

    Arduino_GFX *gfx = _display->getGFX();
    gfx->drawLine(x0, y0, x1, y1, c);
  }
}

void Clock::drawAndEraseCachedLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t color, int16_t *cache, int16_t cache_len, bool cross_check_second, bool cross_check_hour)
{
#if defined(ESP8266)
  yield();
#endif
  bool steep = _diff(y1, y0) > _diff(x1, x0);
  if (steep)
  {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  int16_t dx, dy;
  dx = _diff(x1, x0);
  dy = _diff(y1, y0);

  int16_t err = dx / 2;
  int8_t xstep = (x0 < x1) ? 1 : -1;
  int8_t ystep = (y0 < y1) ? 1 : -1;
  x1 += xstep;
  int16_t x, y, ox, oy;
  for (uint16_t i = 0; i <= dx; i++)
  {
    if (steep)
    {
      x = y0;
      y = x0;
    }
    else
    {
      x = x0;
      y = y0;
    }
    ox = *(cache + (i * 2));
    oy = *(cache + (i * 2) + 1);
    if ((x == ox) && (y == oy))
    {
      if (cross_check_second || cross_check_hour)
      {
        writeCachePixel(x, y, color, cross_check_second, cross_check_hour);
      }
    }
    else
    {
      writeCachePixel(x, y, color, cross_check_second, cross_check_hour);
      if ((ox > 0) || (oy > 0))
      {
        writeCachePixel(ox, oy, BACKGROUND, cross_check_second, cross_check_hour);
      }
      *(cache + (i * 2)) = x;
      *(cache + (i * 2) + 1) = y;
    }
    if (err < dy)
    {
      y0 += ystep;
      err += dx;
    }
    err -= dy;
    x0 += xstep;
  }
  for (uint16_t i = dx + 1; i < cache_len; i++)
  {
    ox = *(cache + (i * 2));
    oy = *(cache + (i * 2) + 1);
    if ((ox > 0) || (oy > 0))
    {
      writeCachePixel(ox, oy, BACKGROUND, cross_check_second, cross_check_hour);
    }
    *(cache + (i * 2)) = 0;
    *(cache + (i * 2) + 1) = 0;
  }
}

void Clock::writeCachePixel(int16_t x, int16_t y, int16_t color, bool cross_check_second, bool cross_check_hour)
{
  Arduino_GFX *gfx = _display->getGFX();
  int16_t *cache = cached_points;

  if (cross_check_second)
  {
    for (uint16_t i = 0; i <= sHandLen; i++)
    {
      if ((x == *(cache++)) && (y == *(cache)))
      {
        return;
      }
      cache++;
    }
  }
  if (cross_check_hour)
  {
    cache = cached_points + ((sHandLen + 1) * 2);
    for (uint16_t i = 0; i <= hHandLen; i++)
    {
      if ((x == *(cache++)) && (y == *(cache)))
      {
        return;
      }
      cache++;
    }
  }
  gfx->writePixel(x, y, color);
}