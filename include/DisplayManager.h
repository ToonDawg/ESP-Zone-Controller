#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

class DisplayManager {
public:
    DisplayManager(Adafruit_SSD1306& display): display(display) {}

    void displayTemperature(float temperature);
    void displayBottomLeft(const String& message);
    void displayBottomRight(const String& message);
    void displayBottomCentre(const String& message);

protected:
    Adafruit_SSD1306& display;

    int16_t calculateTextCenterX(const String& text, const GFXfont* font);
    int16_t calculateTextWidth(const String& text, const GFXfont* font);
    int16_t calculateTextHeight(const String& text, const GFXfont* font);
};

#endif
