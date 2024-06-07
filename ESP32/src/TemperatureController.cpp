#include "TemperatureController.h"
#include <Arduino.h>
#include "Icons.h"


TemperatureController::TemperatureController(float initialTemp, TemperatureSensor& sensor, int relayPin)
    : setTemperature(initialTemp), temperatureSensor(sensor), relayPin(relayPin), lastTempCheckTime(0) {
    pinMode(relayPin, OUTPUT); 
}

void TemperatureController::adjustTemperature(float amount)
{
    setTemperature += amount;
}

float TemperatureController::getSetTemperature() const
{
    return setTemperature;
}

void TemperatureController::setSetTemperature(float amount)
{
    setTemperature = amount;
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

void TemperatureController::setMode(Mode mode)
{
    currentMode = mode;
}

void TemperatureController::toggleMode()
{
    currentMode = (currentMode == Mode::Heat) ? Mode::Cool : Mode::Heat;
}

void TemperatureController::toggleMotorState()
{
    currentMotorState == MotorState::Open ? setMotorState(MotorState::Closed) : setMotorState(MotorState::Open);
}

const char *TemperatureController::getMode()
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

TelemetryData TemperatureController::getTelemetryData()
{
    TelemetryData data;
    data.setTemperature = setTemperature;
    data.currentTemperature = currentTemperature;
    data.currentMode = currentMode;
    data.currentMotorState = currentMotorState;
    return data;
}


void TemperatureController::regulateTemperature() {
    unsigned long currentTime = millis();
    if (currentTime - lastTempCheckTime >= TEMPERATURE_INTERVAL) {
        lastTempCheckTime = currentTime;
        currentTemperature = temperatureSensor.readTemperature();
        
        if (currentMode == Mode::Heat && shouldToggleForHeat(currentTemperature)) {
            toggleMotorState();
        } else if (currentMode == Mode::Cool && shouldToggleForCool(currentTemperature)) {
            toggleMotorState();
        }
    }
}

void TemperatureController::setMotorState(MotorState motorState)
{
    currentMotorState = motorState;
    updateRelayState();
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

void TemperatureController::updateRelayState() {
    if (currentMotorState == MotorState::Open) {
        digitalWrite(relayPin, LOW);
    } else {
        digitalWrite(relayPin, HIGH); 
    }
}
