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
#include <WiFi.h>
#include <ArduinoOTA.h>
#include "OTAUpdater.h"

constexpr int SCREEN_WIDTH = 128;
constexpr int SCREEN_HEIGHT = 64;
constexpr uint8_t DISPLAY_I2C_ADDRESS = 0x3C;
constexpr uint8_t TMP112_I2C_ADDRESS = 0x48;
constexpr uint8_t I2C_SDA_PIN = 5;
constexpr uint8_t I2C_SCL_PIN = 6;
constexpr int8_t OLED_RESET = -1;
constexpr uint8_t RELAY_PIN = 10;

const char *ssid = "Asus";
const char *password = "REDACTED";
const char* firmware_url = "https://test-esp32-firmware-updates.s3.amazonaws.com/AC%20Mate/v0.0.0/firmware.bin";
const char* metadata_url = "https://test-esp32-firmware-updates.s3.amazonaws.com/AC%20Mate/v0.0.0/metadata.json";

OTAUpdater updater(firmware_url, metadata_url);
TwoWire i2cBus(0);
Settings settings;
TMP112Sensor *tmp112Sensor = nullptr;
TemperatureController *tempController = nullptr;
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &i2cBus, OLED_RESET);
DisplayManager *displayManager = nullptr;
WiFiProvisionManager *wifiManager = nullptr;
AppStateManager *appStateManager = nullptr;
ButtonManager *buttonManager = nullptr;

void setup()
{
  Serial.begin(115200);
  i2cBus.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  i2cBus.setClock(100000);

  if (!display.begin(DISPLAY_I2C_ADDRESS, true))
  {
    Serial.println(F("SSD1306 allocation failed"));
  }

  settings.begin();
  tmp112Sensor = new TMP112Sensor(TMP112_I2C_ADDRESS, &i2cBus);
  tmp112Sensor->begin();
  tmp112Sensor->setTemperatureOffset(settings.getTemperatureCalibration());

  tempController = new TemperatureController(*tmp112Sensor, RELAY_PIN, settings);
  displayManager = new DisplayManager(display);
  wifiManager = new WiFiProvisionManager();
  appStateManager = new AppStateManager(*displayManager, *tempController, settings);
  buttonManager = new ButtonManager(*tempController, *appStateManager, updater);

  buttonManager->setupButtons();
  wifiManager->begin();

  Serial.println("Serial communication initialized.");

  float setTemp = settings.getSetTemperature();
  Serial.print("Set Temperature: ");
  Serial.print(setTemp);
  Serial.println(" °C");

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.display();
}

void loop()
{
  buttonManager->tick();
  appStateManager->tick();
  tempController->update();

  display.clearDisplay();
  appStateManager->display();
}

void cleanup()
{
  delete tmp112Sensor;
  delete tempController;
  delete displayManager;
  delete wifiManager;
  delete appStateManager;
  delete buttonManager;
}