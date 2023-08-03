#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DisplayManager.h"
#include "TemperatureSensor.h"
#include "TemperatureController.h"
#include <OneButton.h>
#include "LargeFont.h"
#include "SmallFont.h"

constexpr int OLED_SCREEN_WIDTH = 128;
constexpr int OLED_SCREEN_HEIGHT = 64;
constexpr int OLED_I2C_ADDRESS = 0x3C;
constexpr unsigned int INCREASE_TEMPERATURE_BUTTON_PIN = 15;
constexpr unsigned int DECREASE_TEMPERATURE_BUTTON_PIN = 3;

Adafruit_SSD1306 oledDisplay(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &Wire);
TemperatureSensor temperatureSensor;
TemperatureController tempController(20.0, temperatureSensor);
OneButton increaseTemperatureButton(INCREASE_TEMPERATURE_BUTTON_PIN, true);
OneButton decreaseTemperatureButton(DECREASE_TEMPERATURE_BUTTON_PIN, true);
DisplayManager displayManager(oledDisplay);


constexpr int16_t TEMPERATURE_FONT_HEIGHT = 35;
constexpr int16_t FONT_VERTICAL_PADDING = 6;
constexpr int16_t DISPLAY_SIDE_MARGIN = 4;


void setup() {
  Serial.begin(115200);
  Wire.begin();
  if (!oledDisplay.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  oledDisplay.clearDisplay();
  oledDisplay.display();

  temperatureSensor.begin();

  //Buttons
  increaseTemperatureButton.attachClick([] { tempController.adjustTemperature(.5); });
  decreaseTemperatureButton.attachClick([] { tempController.adjustTemperature(-.5); });
  increaseTemperatureButton.attachDuringLongPress([] {
    static unsigned long lastChangeTime = 0;
    if (millis() - lastChangeTime > 300) {
      tempController.adjustTemperature(.5);
      lastChangeTime = millis();
    }
  });
  decreaseTemperatureButton.attachDuringLongPress([] {
    static unsigned long lastChangeTime = 0;
    if (millis() - lastChangeTime > 300) {
      tempController.adjustTemperature(-.5);
      lastChangeTime = millis();
    }
  });

}

void loop() {
  increaseTemperatureButton.tick();
  decreaseTemperatureButton.tick();

  float temperature = tempController.getCurrentTemperature();

  // Update the OLED display
  oledDisplay.clearDisplay();
  oledDisplay.setTextSize(1);
  oledDisplay.setTextColor(WHITE);

  displayManager.displayTemperature(temperature);
  displayManager.displayBottomLeft("Cool");
  displayManager.displayBottomRight("Open");
  displayManager.displayBottomCentre("XO");

  oledDisplay.display();
}
