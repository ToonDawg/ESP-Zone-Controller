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
#include "AppStateManager.h"

constexpr int OLED_SCREEN_WIDTH = 128;
constexpr int OLED_SCREEN_HEIGHT = 64;
constexpr int OLED_I2C_ADDRESS = 0x3C;
constexpr unsigned int INCREASE_TEMPERATURE_BUTTON_PIN = 15;
constexpr unsigned int DECREASE_TEMPERATURE_BUTTON_PIN = 4;
constexpr unsigned int TOGGLE_MODE_BUTTON_PIN = 5;
constexpr unsigned int TOGGLE_MOTOR_STATE_BUTTON_PIN = 18;



TemperatureSensor temperatureSensor;
TemperatureController tempController(20.0, temperatureSensor);
OneButton increaseTemperatureButton(INCREASE_TEMPERATURE_BUTTON_PIN, true);
OneButton decreaseTemperatureButton(DECREASE_TEMPERATURE_BUTTON_PIN, true);
OneButton toggleModeButton(TOGGLE_MODE_BUTTON_PIN, true);
OneButton toggleMotorStateButton(TOGGLE_MOTOR_STATE_BUTTON_PIN, true);
Adafruit_SSD1306 oledDisplay(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &Wire);
DisplayManager displayManager(oledDisplay);
AppStateManager appStateManager(displayManager, tempController);


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
  increaseTemperatureButton.attachClick([] { 
    tempController.adjustTemperature(.5); 
    appStateManager.setAppState(AppStateManager::AppState::SET_TEMPERATURE);
    appStateManager.recordAdjustmentTime();
}); 

  decreaseTemperatureButton.attachClick([] { 
    tempController.adjustTemperature(-.5);
    appStateManager.setAppState(AppStateManager::AppState::SET_TEMPERATURE);
    appStateManager.recordAdjustmentTime();

  });

  toggleModeButton.attachClick([] { 
      tempController.toggleMode();
      Serial.println(tempController.getMode());
  });

  toggleMotorStateButton.attachClick([] { 
      tempController.toggleMotorState();
      Serial.println(tempController.getMotorState());
  });

  increaseTemperatureButton.attachDuringLongPress([] {
    static unsigned long lastChangeTime = 0;
    if (millis() - lastChangeTime > 300) {
      tempController.adjustTemperature(.5);
      appStateManager.recordAdjustmentTime();
      lastChangeTime = millis();
    }
  });
  decreaseTemperatureButton.attachDuringLongPress([] {
    static unsigned long lastChangeTime = 0;
    if (millis() - lastChangeTime > 300) {
      tempController.adjustTemperature(-.5);
      appStateManager.recordAdjustmentTime();
      lastChangeTime = millis();
    }
  });
}

void loop() {
  increaseTemperatureButton.tick();
  decreaseTemperatureButton.tick();
  toggleModeButton.tick();
  toggleMotorStateButton.tick();
  appStateManager.tick(); 
  tempController.regulateTemperature();

  // Update the OLED display
  oledDisplay.clearDisplay();
  oledDisplay.setTextSize(1);
  oledDisplay.setTextColor(WHITE);


  appStateManager.display();
  oledDisplay.display();
}
