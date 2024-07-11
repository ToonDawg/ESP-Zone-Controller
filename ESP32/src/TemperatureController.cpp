#include "TemperatureController.h"
#include <Arduino.h>
#include "Icons.h"

TemperatureController::TemperatureController(TemperatureSensor &sensor, int relayPin, Settings &settings)
    : temperatureSensor(sensor), relayPin(relayPin), settings(settings), lastUpdateTime(0)
{
    currentStatus = {
        settings.getSetTemperature(),
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

void TemperatureController::adjustTemperature(float delta)
{
    currentStatus.setTemperature += delta;
    settings.setSetTemperature(currentStatus.setTemperature);
}

void TemperatureController::adjustCalibrationTemperature(float delta)
{
    float temperatureOffset = settings.getTemperatureCalibration() + delta;
    settings.setTemperatureCalibration(temperatureOffset);
    temperatureSensor.setTemperatureOffset(temperatureOffset);
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
    digitalWrite(relayPin, currentStatus.motorState == MotorState::Open ? HIGH : LOW);
}

bool TemperatureController::shouldActivateMotor() const
{
    float diff = currentStatus.currentTemperature - currentStatus.setTemperature;
    bool isHeating = currentStatus.mode == Mode::Heat;
    bool motorOpen = currentStatus.motorState ==
                     (settings.getMotorDirection() ? MotorState::Open : MotorState::Closed);

    bool shouldOpen = isHeating ? (diff < -TEMPERATURE_THRESHOLD)
                                : (diff > TEMPERATURE_THRESHOLD);

    return shouldOpen != motorOpen;
}