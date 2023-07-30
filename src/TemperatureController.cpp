#include "TemperatureController.h"
#include <Arduino.h>

TemperatureController::TemperatureController(float initialTemp, TemperatureSensor& sensor)
    : setTemperature(initialTemp), temperatureSensor(sensor), currentTemperature(0.0f), lastTempCheckTime(0) {}

void TemperatureController::adjustTemperature(float amount)
{
    setTemperature += amount;
    Serial.println(setTemperature);
}

float TemperatureController::getSetTemperature() const
{
    return setTemperature;
}

float TemperatureController::getCurrentTemperature()
{
    unsigned long currentTime = millis();
    if (currentTime - lastTempCheckTime >= TEMPERATURE_INTERVAL)
    {
        lastTempCheckTime = currentTime;
        currentTemperature = temperatureSensor.readTemperature();
    }
    return currentTemperature;
}
