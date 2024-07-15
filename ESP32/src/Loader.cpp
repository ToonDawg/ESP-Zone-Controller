#include "Loader.h"
#include <cmath>
#include <Adafruit_GrayOLED.h>

LoaderAnimation::LoaderAnimation(Adafruit_GFX& disp, int16_t cx, int16_t cy, int16_t r, int16_t thick, uint16_t col)
    : display(disp), centerX(cx), centerY(cy), radius(r), thickness(thick), color(col),
      startAngle(0), endAngle(90), stepSize(10) {}

void LoaderAnimation::update() {
    startAngle = (startAngle + stepSize) % 360;
    endAngle = (endAngle + stepSize) % 360;
}

void LoaderAnimation::draw() {
    display.fillCircle(centerX, centerY, radius, MONOOLED_BLACK);
    for (int16_t i = startAngle; i != endAngle; i = (i + 1) % 360) {
        float angle = i * M_PI / 180;
        int16_t x1 = centerX + (radius - thickness) * cos(angle);
        int16_t y1 = centerY + (radius - thickness) * sin(angle);
        int16_t x2 = centerX + radius * cos(angle);
        int16_t y2 = centerY + radius * sin(angle);
        display.drawLine(x1, y1, x2, y2, color);
    }
}

void LoaderAnimation::setColor(uint16_t col) { color = col; }
void LoaderAnimation::setStepSize(int16_t step) { stepSize = step; }
void LoaderAnimation::setArcLength(int16_t length) { endAngle = (startAngle + length) % 360; }