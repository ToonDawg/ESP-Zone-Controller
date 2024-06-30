#include "TemperatureController.h"
#include <Arduino.h>
#include "Icons.h"

TemperatureController::TemperatureController(float initialTemp, TemperatureSensor &sensor, int relayPin)
    : temperatureSensor(sensor), relayPin(relayPin), lastUpdateTime(0)
{
    currentStatus = {
        initialTemp,
        0.0f,
        Mode::Cool,
        MotorState::Open};
    pinMode(relayPin, OUTPUT);
    updateRelayState();
}

void TemperatureController::update()
{
    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime >= UPDATE_INTERVAL)
    {
        lastUpdateTime = currentTime;
        updateTemperature();
        regulateTemperature();
    }
}

TemperatureController::Status TemperatureController::getStatus() const
{
    return currentStatus;
}

void TemperatureController::setTemperature(float temperature)
{
    currentStatus.setTemperature = temperature;
}

void TemperatureController::adjustTemperature(float delta)
{
    currentStatus.setTemperature += delta;
}

void TemperatureController::toggleMode()
{
    currentStatus.mode = (currentStatus.mode == Mode::Heat) ? Mode::Cool : Mode::Heat;
}

void TemperatureController::toggleMotorState()
{
    currentStatus.motorState = (currentStatus.motorState == MotorState::Open) ? MotorState::Closed : MotorState::Open;
    updateRelayState();
}

const tImage &TemperatureController::getModeIcon() const
{
    return (currentStatus.mode == Mode::Heat) ? heatIcon : coolIcon;
}

const tImage &TemperatureController::getMotorStateIcon() const
{
    return (currentStatus.motorState == MotorState::Open) ? windFlow : noWindFlow;
}

void TemperatureController::updateTemperature()
{
    currentStatus.currentTemperature = temperatureSensor.readTemperature();
}

void TemperatureController::regulateTemperature()
{
    if (shouldActivateMotor())
    {
        toggleMotorState();
    }
}

void TemperatureController::updateRelayState()
{
    digitalWrite(relayPin, currentStatus.motorState == MotorState::Open ? LOW : HIGH);
}

bool TemperatureController::shouldActivateMotor() const
{
    float diff = currentStatus.currentTemperature - currentStatus.setTemperature;
    if (currentStatus.mode == Mode::Heat)
    {
        return (diff < -TEMPERATURE_THRESHOLD && currentStatus.motorState == MotorState::Open) ||
               (diff > TEMPERATURE_THRESHOLD && currentStatus.motorState == MotorState::Closed);
    }
    else // Mode::Cool
    {
        return (diff > TEMPERATURE_THRESHOLD && currentStatus.motorState == MotorState::Open) ||
               (diff < -TEMPERATURE_THRESHOLD && currentStatus.motorState == MotorState::Closed);
    }
}