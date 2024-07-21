#include "DS18B20Sensor.h"
#include <Arduino.h>

DS18B20Sensor::DS18B20Sensor(uint8_t pin)
    : oneWire(pin), sensors(&oneWire), temperatureOffset(0.0f),
      lastValidTemperature(0), lastRequestTime(0),
      conversionTime(750), conversionRequested(false)
{
}

void DS18B20Sensor::begin()
{
    sensors.begin();
    if (sensors.getAddress(deviceAddress, 0))
    {
        sensors.setResolution(deviceAddress, 11);
        Serial.println("DS18B20 sensor initialized!");
        sensors.setWaitForConversion(false); // Enable non-blocking mode
        conversionTime = sensors.millisToWaitForConversion(sensors.getResolution());
        lastValidTemperature = sensors.getTempCByIndex(0) + temperatureOffset;
    }
    else
    {
        Serial.println("DS18B20 sensor initialization failed. No sensors found!");
    }
}

float DS18B20Sensor::readTemperature()
{
    return lastValidTemperature + temperatureOffset;
}

float DS18B20Sensor::readTemperatureF()
{
    return (lastValidTemperature * 1.8f + 32.0f) + temperatureOffset;
}

void DS18B20Sensor::update()
{
    unsigned long currentTime = millis();

    if (!conversionRequested)
    {
        sensors.requestTemperatures();
        lastRequestTime = currentTime;
        conversionRequested = true;
        Serial.println("Temperature conversion requested");
    }
    else if (currentTime - lastRequestTime >= conversionTime)
    {
        float tempC = sensors.getTempCByIndex(0);
        if (tempC != DEVICE_DISCONNECTED_C)
        {
            lastValidTemperature = tempC + temperatureOffset;
            Serial.printf("Temperature read successfully: %.2f°C\n", lastValidTemperature);
        }
        else
        {
            Serial.println("Failed to read temperature");
        }
        conversionRequested = false;
    }
}

bool DS18B20Sensor::isReady() const
{
    return !conversionRequested;
}

void DS18B20Sensor::setTemperatureOffset(float offset)
{
    temperatureOffset = offset;
}

float DS18B20Sensor::getTemperatureOffset() const
{
    return temperatureOffset;
}