#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Wire.h>
#include "icons.h"

class DisplayManager {
public:
    DisplayManager(Adafruit_SH1106G& display): display(display) {}

    void displayTemperature(float temperature);
    void displayBottomLeft(const String& message);
    void displayIconBottomLeft(const tImage& icon);
    void displayIconBottomRight(const tImage& icon);
    void displayIconBottomMiddle(const tImage& icon);
    void displayBottomRight(const String& message);
    void displayBottomCentre(const String& message);
    void displayOff();
    void render();
    void displayCentre(const String lines[], int numLines);
protected:
    Adafruit_SH1106G& display;

    int16_t calculateTextCenterX(const String& text, const GFXfont* font);
    int16_t calculateTextWidth(const String& text, const GFXfont* font);
    int16_t calculateTextHeight(const String& text, const GFXfont* font);
    void draw8BitImage(int16_t x, int16_t y, const tImage& image);
    void displayChunkCentre(const String& chunk, int16_t startY);

};

#endif
