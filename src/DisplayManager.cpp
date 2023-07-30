#include "DisplayManager.h"
#include "LargeFont.h"
#include "SmallFont.h"

constexpr int16_t TEMPERATURE_FONT_HEIGHT = 35;
constexpr int16_t FONT_VERTICAL_PADDING = 6;
constexpr int16_t DISPLAY_SIDE_MARGIN = 4;

int16_t DisplayManager::calculateTextCenterX(const String& text, const GFXfont* font) {
  int16_t x, y;
  uint16_t w, h;
  display.setFont(font);
  display.getTextBounds(text.c_str(), 0, 0, &x, &y, &w, &h);
  return (display.width() - w) / 2;
}

int16_t DisplayManager::calculateTextWidth(const String& text, const GFXfont* font) {
  int16_t x, y;
  uint16_t w, h;
  display.setFont(font);
  display.getTextBounds(text.c_str(), 0, 0, &x, &y, &w, &h);
  return w;
}

int16_t DisplayManager::calculateTextHeight(const String& text, const GFXfont* font) {
  int16_t x, y;
  uint16_t w, h;
  display.setFont(font);
  display.getTextBounds(text.c_str(), 0, 0, &x, &y, &w, &h);
  return h;
}

void DisplayManager::displayTemperature(float temperature) {
  String temperatureString = String(temperature, 1);

  int16_t temperatureStartX = calculateTextCenterX(temperatureString, &FreeSansBold25pt7b);
  display.setFont(&FreeSansBold25pt7b);
  display.setCursor(temperatureStartX, TEMPERATURE_FONT_HEIGHT);
  display.print(temperatureString);

  String unitString = "C";
  int16_t unitStartY = TEMPERATURE_FONT_HEIGHT - calculateTextHeight(temperatureString, &FreeSansBold25pt7b) + calculateTextHeight(unitString, &FreeSans8pt7b);
  int16_t unitStartX = temperatureStartX + calculateTextWidth(temperatureString, &FreeSansBold25pt7b) + 5; 
  display.setFont(&FreeSans8pt7b);
  display.setCursor(unitStartX, unitStartY);
  display.print(unitString);

  display.drawFastHLine(30, TEMPERATURE_FONT_HEIGHT + FONT_VERTICAL_PADDING, display.width() - 60, WHITE);
}


void DisplayManager::displayBottomLeft(const String& message) {
  int16_t textBoxX, textBoxY; 
  uint16_t textBoxWidth, textBoxHeight;

  display.setFont(&FreeSans8pt7b);
  display.getTextBounds(message, 0, 0, &textBoxX, &textBoxY, &textBoxWidth, &textBoxHeight);

  display.setCursor(5, display.height() - DISPLAY_SIDE_MARGIN);
  display.print(message);
}

void DisplayManager::displayBottomRight(const String& message) {
  int16_t textBoxX, textBoxY;
  uint16_t textBoxWidth, textBoxHeight;

  display.setFont(&FreeSans8pt7b);
  display.getTextBounds(message, 0, 0, &textBoxX, &textBoxY, &textBoxWidth, &textBoxHeight);

  display.setCursor(display.width() - textBoxWidth - 5, display.height() - DISPLAY_SIDE_MARGIN);
  display.print(message);
}

void DisplayManager::displayBottomCentre(const String& message) {
    int16_t temperatureStartX = calculateTextCenterX(message, &FreeSans8pt7b);
    display.setFont(&FreeSans8pt7b);
    display.setCursor(temperatureStartX, display.height() - DISPLAY_SIDE_MARGIN);
    display.print(message);
}
