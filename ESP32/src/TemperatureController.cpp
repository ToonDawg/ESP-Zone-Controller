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
        temperatureSensor.update();
        currentStatus.currentTemperature = settings.getTemperatureUnit()
                                               ? temperatureSensor.readTemperature()
                                               : temperatureSensor.readTemperatureF();
    }
    regulateTemperature();
}

TemperatureStatus TemperatureController::getStatus() const
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

void TemperatureController::regulateTemperature()
{
    if (shouldChangeMotorState())
    {
        toggleMotorState();
    }
}

void TemperatureController::updateRelayState()
{
    digitalWrite(relayPin, currentStatus.motorState == MotorState::Open ? HIGH : LOW);
}

bool TemperatureController::shouldChangeMotorState() const
{
    float currentTemp = currentStatus.currentTemperature;
    float setTemp = currentStatus.setTemperature;
    bool isCooling = currentStatus.mode == Mode::Cool;
    bool motorOpen = currentStatus.motorState == MotorState::Open;
    MotorDirection direction = settings.getMotorDirection();

    float upperThreshold = setTemp + TEMPERATURE_THRESHOLD;
    float lowerThreshold = setTemp - TEMPERATURE_THRESHOLD;

    bool shouldBeOpen;

    if (isCooling)
    {
        if (currentTemp >= upperThreshold)
            shouldBeOpen = true;
        else if (currentTemp <= lowerThreshold)
            shouldBeOpen = false;
        else
            return false;
    }
    else
    {
        if (currentTemp <= lowerThreshold)
            shouldBeOpen = true;
        else if (currentTemp >= upperThreshold)
            shouldBeOpen = false;
        else
            return false;
    }

    bool desiredState = (direction == MotorDirection::Normal) ? shouldBeOpen : !shouldBeOpen;

    return desiredState != motorOpen;
}