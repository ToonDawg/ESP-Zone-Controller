#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "DisplayManager.h"
#include "TemperatureSensor.h"
#include "TemperatureController.h"
#include <OneButton.h>
#include "LargeFont.h"
#include "SmallFont.h"
#include "AppStateManager.h"
#include <SPI.h>
#include "BluetoothManager.h"

constexpr int SCREEN_WIDTH = 128;
constexpr int SCREEN_HEIGHT = 64;
constexpr unsigned int INCREASE_TEMPERATURE_BUTTON_PIN = 1;
constexpr unsigned int DECREASE_TEMPERATURE_BUTTON_PIN = 3;
constexpr unsigned int TOGGLE_MODE_BUTTON_PIN = 16;
constexpr unsigned int TOGGLE_DISPLAY_BUTTON_PIN = 17;

#define OLED_MOSI   23
#define OLED_CLK   18
#define OLED_DC    4
#define OLED_CS    5
#define OLED_RESET 19

TemperatureSensor temperatureSensor;
TemperatureController tempController(20.0, temperatureSensor);
OneButton increaseTemperatureButton(INCREASE_TEMPERATURE_BUTTON_PIN, true);
OneButton decreaseTemperatureButton(DECREASE_TEMPERATURE_BUTTON_PIN, true);
OneButton toggleModeButton(TOGGLE_MODE_BUTTON_PIN, true);
OneButton toggleDisplayButton(TOGGLE_DISPLAY_BUTTON_PIN, true);
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64,OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
DisplayManager displayManager(display);
BluetoothManager btManager(tempController);
AppStateManager appStateManager(displayManager, tempController, btManager);

constexpr int16_t TEMPERATURE_FONT_HEIGHT = 35;
constexpr int16_t FONT_VERTICAL_PADDING = 6;
constexpr int16_t DISPLAY_SIDE_MARGIN = 4;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  btManager.init();
  btManager.setupCharacteristic();

  if(!display.begin()) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();
  display.setRotation(2);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);

  display.display();

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
      appStateManager.setAppState(AppStateManager::AppState::CURRENT_TEMPERATURE);
  });

  toggleModeButton.attachLongPressStart([] { 
    appStateManager.setAppState(AppStateManager::AppState::CONNECTING);
    appStateManager.recordAdjustmentTime();

});


  toggleDisplayButton.attachClick([] {
    if(appStateManager.getAppState() == AppStateManager::AppState::OFF) {
      appStateManager.setAppState(AppStateManager::AppState::CURRENT_TEMPERATURE);
    } else {
      appStateManager.setAppState(AppStateManager::AppState::OFF);
    }
  });

  increaseTemperatureButton.attachDuringLongPress([] {
    appStateManager.setAppState(AppStateManager::AppState::SET_TEMPERATURE);
    static unsigned long lastChangeTime = 0;
    if (millis() - lastChangeTime > 200) {
      tempController.adjustTemperature(.5);
      appStateManager.recordAdjustmentTime();
      lastChangeTime = millis();
    }
  });
  decreaseTemperatureButton.attachDuringLongPress([] {
    appStateManager.setAppState(AppStateManager::AppState::SET_TEMPERATURE);
    static unsigned long lastChangeTime = 0;
    if (millis() - lastChangeTime > 200) {
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
  toggleDisplayButton.tick();
  appStateManager.tick(); 
  tempController.regulateTemperature();
  // Update the OLED display
  display.clearDisplay();


  appStateManager.display();
}
