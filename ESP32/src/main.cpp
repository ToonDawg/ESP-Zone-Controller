#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "DisplayManager.h"
#include "TMP112Sensor.h"
#include "TemperatureController.h"
#include "ButtonManager.h"
#include "AppStateManager.h"
#include "WiFiProvisionManager.h"
#include "Settings.h"

constexpr int SCREEN_WIDTH = 128;
constexpr int SCREEN_HEIGHT = 64;
constexpr uint8_t DISPLAY_I2C_ADDRESS = 0x3C;
constexpr uint8_t TMP112_I2C_ADDRESS = 0x48;
constexpr uint8_t I2C_SDA_PIN = 5;
constexpr uint8_t I2C_SCL_PIN = 6;
constexpr int8_t OLED_RESET = -1;
constexpr uint8_t RELAY_PIN = 10;

TwoWire i2cBus(0);
Settings settings;
TMP112Sensor tmp112Sensor(TMP112_I2C_ADDRESS, &i2cBus);
TemperatureController tempController(tmp112Sensor, RELAY_PIN, settings);
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &i2cBus, OLED_RESET);
DisplayManager displayManager(display);
WiFiProvisionManager wifiManager;
AppStateManager appStateManager(displayManager, tempController, settings);
ButtonManager buttonManager(tempController, appStateManager);

void setup()
{
  Serial.begin(115200);
  i2cBus.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  i2cBus.setClock(100000);
  while (!Serial)
  {
    ;
  }

  if (!display.begin(DISPLAY_I2C_ADDRESS, true))
  {
    Serial.println(F("SSD1306 allocation failed"));
  }
  settings.begin();

  tmp112Sensor.begin();
  tmp112Sensor.setTemperatureOffset(-5.0);
  buttonManager.setupButtons();
  wifiManager.begin();

  Serial.println("Serial communication initialized.");

  float setTemp = settings.getSetTemperature();

  Serial.print("Set Temperature: ");
  Serial.print(setTemp);
  Serial.println(" °C"); // Assuming the temperature is in Celsius

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.display();
}

void loop()
{
  buttonManager.tick();
  appStateManager.tick();
  tempController.update();

  display.clearDisplay();
  appStateManager.display();
}