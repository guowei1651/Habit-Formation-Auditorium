#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>

#include "../include/esp32_pins.h"

// Define the display type and size
#define MAX_W 800 // Change this according to your display size
#define MAX_HEIGHT 480

Display::Display()
{
  // Initialize the display
  hspi.begin(18, 19, 23, 5);
  display = GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT>(GxEPD2_750c_Z08(/*CS=*/CS_Pin, /*DC=*/DC_Pin, /*RST=*/RES_Pin, /*BUSY=*/BUSY_Pin)); // GDEW075Z08 800x480, GD7965
}

void Display::init()
{
  display.init();
  display.setRotation(1);
  display.setTextColor(GxEPD_BLACK);
}

void Display::showText(const String &text)
{
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(0, 0);
    display.print(text);
  } while (display.nextPage());
}

void Display::refresh()
{
  display.display();
}

void Display::sleep()
{
  display.hibernate();
}

void Display::displayImage(const uint8_t *bwImage, const uint8_t *redImage)
{
  display.firstPage();
  do
  {
    display.drawBitmap(0, 0, bwImage, display.width(), display.height(), GxEPD_BLACK);
    display.drawBitmap(0, 0, redImage, display.width(), display.height(), GxEPD_RED);
  } while (display.nextPage());
}

void Display::clear()
{
  display.fillScreen(GxEPD_WHITE);
  display.display();
}

void Display::checkStatus()
{
  while (display.epd2.isBusy())
  {
    delay(100);
  }
}
