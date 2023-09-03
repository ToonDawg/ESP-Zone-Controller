#include "DisplayManager.h"
#include "Icons.h"
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>

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

void DisplayManager::draw8BitImage(int16_t x, int16_t y, const tImage& image) {
    for (int16_t j = 0; j < image.height; j++) {
        for (int16_t i = 0; i < image.width; i++) {
            if (image.data[j * image.width + i] == 0xff) {
                display.drawPixel(x + i, y + j, MONOOLED_WHITE);
            } else {
                display.drawPixel(x + i, y + j, MONOOLED_BLACK);
            }
        }
    }
}

void DisplayManager::displayTemperature(float temperature) {
  String temperatureString = String(temperature, 1);

  int16_t temperatureStartX = calculateTextCenterX(temperatureString, &FreeSansBold24pt7b);
  display.setFont(&FreeSansBold24pt7b);
  display.setCursor(temperatureStartX, TEMPERATURE_FONT_HEIGHT);
  display.print(temperatureString);

  String unitString = "C";
  int16_t unitStartY = TEMPERATURE_FONT_HEIGHT - calculateTextHeight(temperatureString, &FreeSansBold24pt7b) + calculateTextHeight(unitString, &FreeSans9pt7b);
  int16_t unitStartX = temperatureStartX + calculateTextWidth(temperatureString, &FreeSansBold24pt7b) + 5; 
  display.setFont(&FreeSans9pt7b);
  display.setCursor(unitStartX, unitStartY);
  display.print(unitString);

  display.drawFastHLine(30, TEMPERATURE_FONT_HEIGHT + FONT_VERTICAL_PADDING, display.width() - 60, MONOOLED_WHITE);
}


void DisplayManager::displayBottomLeft(const String& message) {
  int16_t textBoxX, textBoxY; 
  uint16_t textBoxWidth, textBoxHeight;

  display.setFont(&FreeSans9pt7b);
  display.getTextBounds(message, 0, 0, &textBoxX, &textBoxY, &textBoxWidth, &textBoxHeight);

  display.setCursor(5, display.height() - DISPLAY_SIDE_MARGIN);
  display.print(message);
}

void DisplayManager::displayIconBottomLeft(const tImage& icon) {
    int16_t iconX = 5;  
    int16_t iconY = display.height() - icon.height;

    draw8BitImage(iconX, iconY, icon);
}

void DisplayManager::displayIconBottomRight(const tImage& icon) {
    int16_t iconX = display.width() - icon.width;  
    int16_t iconY = display.height() - icon.height;

    draw8BitImage(iconX, iconY, icon);
}
void DisplayManager::displayBottomRight(const String& message) {
  int16_t textBoxX, textBoxY;
  uint16_t textBoxWidth, textBoxHeight;

  display.setFont(&FreeSans9pt7b);
  display.getTextBounds(message, 0, 0, &textBoxX, &textBoxY, &textBoxWidth, &textBoxHeight);

  display.setCursor(display.width() - textBoxWidth - 5, display.height() - DISPLAY_SIDE_MARGIN);
  display.print(message);
}

void DisplayManager::displayBottomCentre(const String& message) {
    int16_t temperatureStartX = calculateTextCenterX(message, &FreeSans9pt7b);
    display.setFont(&FreeSans9pt7b);
    display.setCursor(temperatureStartX, display.height() - DISPLAY_SIDE_MARGIN);
    display.print(message);
}

void DisplayManager::displayOff() {
  display.clearDisplay();
}

void DisplayManager::render() {
  display.display();
}

void DisplayManager::displayCentre(const String lines[], int numLines) {
  int16_t textHeight = calculateTextHeight("W", &FreeSans9pt7b); // Assuming all characters have roughly the same height

  int16_t totalHeight = textHeight * numLines;

  
  int16_t startY = (display.height() - totalHeight) / 2 + textHeight; 

  for (int i = 0; i < numLines; i++) {
    displayChunkCentre(lines[i], startY);
    startY += textHeight + 5; 
  }
}

void DisplayManager::displayChunkCentre(const String& chunk, int16_t startY) {
  int16_t textWidth = calculateTextWidth(chunk, &FreeSans9pt7b);
  int16_t textStartX = (display.width() - textWidth) / 2;

  display.setCursor(textStartX, startY);
  display.print(chunk);
}