// LoaderAnimation.h
#pragma once

#include <Adafruit_GFX.h>

class LoaderAnimation {
private:
    Adafruit_GFX& display;
    int16_t centerX;
    int16_t centerY;
    int16_t radius;
    int16_t thickness;
    uint16_t color;
    int16_t startAngle;
    int16_t endAngle;
    int16_t stepSize;

public:
    LoaderAnimation(Adafruit_GFX& disp, int16_t cx, int16_t cy, int16_t r, int16_t thick, uint16_t col);
    void update();
    void draw();
    void setColor(uint16_t col);
    void setStepSize(int16_t step);
    void setArcLength(int16_t length);
};