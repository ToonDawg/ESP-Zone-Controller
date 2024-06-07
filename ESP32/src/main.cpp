#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "DisplayManager.h"
#include "DS18B20Sensor.h"
#include "TemperatureSensor.h"
#include "TemperatureController.h"
#include "ButtonManager.h"
#include "LargeFont.h"
#include "SmallFont.h"
#include "AppStateManager.h"
#include "BluetoothManager.h"
#include <TMP112Sensor.h>
#include <AzureIoTManager.h>

constexpr int SCREEN_WIDTH = 128;
constexpr int SCREEN_HEIGHT = 64;
#define Display_i2c_Address 0x3c
#define TMP112_I2C_ADDRESS 0x48
#define Display_SDA_PIN 5
#define Display_SCL_PIN 6
#define OLED_RESET -1
#define ONE_WIRE_BUS_PIN 0
#define RELAY_PIN 10

TMP112Sensor temperatureSensor(TMP112_I2C_ADDRESS);
// DS18B20Sensor temperatureSensor(ONE_WIRE_BUS_PIN);
TemperatureController tempController(20.0, temperatureSensor, RELAY_PIN);
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DisplayManager displayManager(display);
BluetoothManager btManager(tempController);
AppStateManager appStateManager(displayManager, tempController, btManager);
AzureIoTManager azureIoTManager(tempController);
ButtonManager buttonManager(tempController, appStateManager);

void setup() {
  Serial.begin(115200);
  Serial.println("Serial communication started");

  Wire.setPins(Display_SDA_PIN, Display_SCL_PIN);
  Wire.begin();
  display.begin(Display_i2c_Address, true);
  Serial.println(F("Setup"));


  if(!display.begin()) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  btManager.init();
  btManager.setupCharacteristic();

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.drawPixel(10, 10, SH110X_WHITE);
  display.display();

  temperatureSensor.begin();
  buttonManager.setupButtons();
}

void loop() {
  buttonManager.tick();
  appStateManager.tick(); 
  tempController.regulateTemperature();

  display.clearDisplay();
  appStateManager.display();
}
