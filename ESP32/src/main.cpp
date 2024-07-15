#include <Arduino.h>
#include "DisplayManager.h"
#include "TMP112Sensor.h"
#include "TemperatureController.h"
#include "ButtonManager.h"
#include "AppStateManager.h"
#include "Settings.h"
#include "OTAUpdater.h"

// Use PROGMEM for constant strings
const char PROGMEM SSID[] = "Asus";
const char PROGMEM PASSWORD[] = "REDACTED";
const char UPDATE_URL[] = "https://test-esp32-firmware-updates.s3.amazonaws.com";
const char DEVICE_NAME[] = "ACMate";

// Use namespaces to group related constants
namespace Pins
{
  constexpr uint8_t I2C_SDA = 5;
  constexpr uint8_t I2C_SCL = 6;
  constexpr uint8_t RELAY = 10;
}

namespace I2CAddresses
{
  constexpr uint8_t DISPLAYADDR = 0x3C;
  constexpr uint8_t TMP112 = 0x48;
}

namespace DisplayConfig
{
  constexpr int WIDTH = 128;
  constexpr int HEIGHT = 64;
  constexpr int8_t RESET = -1;
}

// Use forward declarations and pointers to reduce header dependencies
class OTAUpdater;
class TMP112Sensor;
class TemperatureController;
class DisplayManager;
class AppStateManager;
class ButtonManager;

OTAUpdater *updater = nullptr;
TMP112Sensor *tmp112Sensor = nullptr;
TemperatureController *tempController = nullptr;
DisplayManager *displayManager = nullptr;
AppStateManager *appStateManager = nullptr;
ButtonManager *buttonManager = nullptr;

TwoWire i2cBus(0);
Settings settings;
Adafruit_SH1106G display(DisplayConfig::WIDTH, DisplayConfig::HEIGHT, &i2cBus, DisplayConfig::RESET);

void setup()
{
  Serial.begin(115200);
  i2cBus.begin(Pins::I2C_SDA, Pins::I2C_SCL);
  i2cBus.setClock(100000);

  if (!display.begin(I2CAddresses::DISPLAYADDR, true))
  {
    Serial.println(F("SSD1306 allocation failed"));
    return;
  }

  WiFi.begin(FPSTR(SSID), FPSTR(PASSWORD));
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println(F("Connecting to WiFi..."));
  }
  Serial.println(F("Connected to WiFi"));
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());

  settings.begin();
  tmp112Sensor = new TMP112Sensor(I2CAddresses::TMP112, &i2cBus);
  tmp112Sensor->begin();
  tmp112Sensor->setTemperatureOffset(settings.getTemperatureCalibration());

  tempController = new TemperatureController(*tmp112Sensor, Pins::RELAY, settings);
  displayManager = new DisplayManager(display);

  updater = new OTAUpdater(UPDATE_URL, DEVICE_NAME, settings);
  appStateManager = new AppStateManager(*displayManager, *tempController, settings, *updater);

  buttonManager = new ButtonManager(*tempController, *appStateManager, *updater);
  buttonManager->setupButtons();

  Serial.println(F("Initialization complete."));

  float setTemp = settings.getSetTemperature();
  Serial.print(F("Set Temperature: "));
  Serial.print(setTemp);
  Serial.println(F(" °C"));

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
