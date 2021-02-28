#ifndef SCREENS_H
#define SCREENS_H
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "statics.h"

void screenSelector(Adafruit_SSD1306 &display);
void welcomeScreen(Adafruit_SSD1306 &display);
void statusScreen(Adafruit_SSD1306 &display);
void drawCentreString(Adafruit_SSD1306 &display, const char *buf, int x, int y);
void uptimeScreen(Adafruit_SSD1306 &display);
void upsScreen(Adafruit_SSD1306 &display);
void webScreen(Adafruit_SSD1306 &display);

#endif