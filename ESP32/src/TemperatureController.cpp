#include "TemperatureController.h"
#include <Arduino.h>
#include "Icons.h"

TemperatureController::TemperatureController(float initialTemp, TemperatureSensor &sensor)
    : setTemperature(initialTemp), temperatureSensor(sensor), currentTemperature(0.0f), lastTempCheckTime(0) {}

void TemperatureController::adjustTemperature(float amount)
{
    setTemperature += amount;
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

void TemperatureController::toggleMode()
{
    currentMode = (currentMode == Mode::Heat) ? Mode::Cool : Mode::Heat;
}

void TemperatureController::toggleMotorState()
{
    currentMotorState = (currentMotorState == MotorState::Open) ? MotorState::Closed : MotorState::Open;
}

const char *TemperatureController::getMode() const
{
    return (currentMode == Mode::Heat) ? "Heat" : "Cool";
}

const char *TemperatureController::getMotorState() const
{
    return (currentMotorState == MotorState::Open) ? "Open" : "Closed";
}

const tImage& TemperatureController::getModeIcon() const
{
    return (currentMode == Mode::Heat) ? heatIcon : coolIcon;
}

const tImage& TemperatureController::getMotorStateIcon() const
{
    return (currentMotorState == MotorState::Open) ? windFlow : noWindFlow;
}


void TemperatureController::regulateTemperature()
{
    float currentTemp = getCurrentTemperature();
    bool shouldToggle = false;

    switch (currentMode)
    {
    case Mode::Heat:
        shouldToggle = shouldToggleForHeat(currentTemp);
        break;
    case Mode::Cool:
        shouldToggle = shouldToggleForCool(currentTemp);
        break;
    }

    if (shouldToggle)
    {
        toggleMotorState();
    }
}

bool TemperatureController::shouldToggleForHeat(float currentTemp) const
{
    if (currentTemp < (setTemperature - TEMPERATURE_THRESHOLD) && currentMotorState == MotorState::Closed)
    {
        return true;
    }
    if (currentTemp > (setTemperature + TEMPERATURE_THRESHOLD) && currentMotorState == MotorState::Open)
    {
        return true;
    }
    return false;
}

bool TemperatureController::shouldToggleForCool(float currentTemp) const
{
    if (currentTemp > (setTemperature + TEMPERATURE_THRESHOLD) && currentMotorState == MotorState::Closed)
    {
        return true;
    }
    if (currentTemp < (setTemperature - TEMPERATURE_THRESHOLD) && currentMotorState == MotorState::Open)
    {
        return true;
    }
    return false;
}

TelemetryData TemperatureController::getTelemetryData() const {
    TelemetryData data;
    data.setTemperature = setTemperature;
    data.currentTemperature = currentTemperature;
    data.currentMode = currentMode;
    data.currentMotorState = currentMotorState;
    return data;
}

