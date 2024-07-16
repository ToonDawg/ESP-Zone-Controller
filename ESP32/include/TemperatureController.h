#pragma once

#include "TemperatureSensor.h"
#include "Icons.h"
#include "Enums.h"
#include "Settings.h"

class TemperatureController
{
public:
    TemperatureController(TemperatureSensor &sensor, int relayPin, Settings &settings);

    void update();
    TemperatureStatus getStatus() const;

    void adjustTemperature(float delta);
    void adjustCalibrationTemperature(float delta);
    void toggleMode();
    void toggleMotorState();

    const tImage &getModeIcon() const;
    const tImage &getMotorStateIcon() const;

private:
    static constexpr float TEMPERATURE_THRESHOLD = 0.5f;
    static constexpr unsigned long UPDATE_INTERVAL = 5000; // ms

    TemperatureSensor &temperatureSensor;
    Settings &settings;
    int relayPin;
    TemperatureStatus currentStatus;
    unsigned long lastUpdateTime;
    bool shouldChangeMotorState() const;
    void updateTemperature();
    void regulateTemperature();
    void updateRelayState();
};