#pragma once

#include "TemperatureSensor.h"
#include "Icons.h"
#include "Settings.h"

class TemperatureController {
public:
    enum class Mode { Heat, Cool };
    enum class MotorState { Open, Closed };

    struct Status {
        float setTemperature;
        float currentTemperature;
        Mode mode;
        MotorState motorState;
    };

    TemperatureController(TemperatureSensor& sensor, int relayPin, Settings& settings);

    void update();
    Status getStatus() const;

    void adjustTemperature(float delta);
    void adjustCalibrationTemperature(float delta);
    void toggleMode();
    void toggleMotorState();

    const tImage& getModeIcon() const;
    const tImage& getMotorStateIcon() const;

private:
    static constexpr float TEMPERATURE_THRESHOLD = 0.5f;
    static constexpr unsigned long UPDATE_INTERVAL = 2000; // ms

    TemperatureSensor& temperatureSensor;
    Settings& settings;
    int relayPin;
    Status currentStatus;
    unsigned long lastUpdateTime;

    void updateTemperature();
    void regulateTemperature();
    void updateRelayState();
    bool shouldActivateMotor() const;
};