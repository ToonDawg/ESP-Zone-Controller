#ifndef TEMPERATURE_CONTROLLER_H
#define TEMPERATURE_CONTROLLER_H

#include "TemperatureSensor.h"
#include "Icons.h"

enum class Mode {
    Heat,
    Cool,
};

enum class MotorState {
    Open,
    Closed,
};

struct TelemetryData {
    float setTemperature;
    float currentTemperature;
    Mode currentMode;
    MotorState currentMotorState;
};

class TemperatureController {
private:
    float setTemperature;
    float currentTemperature;
    Mode currentMode = Mode::Cool;
    MotorState currentMotorState = MotorState::Open;
    TemperatureSensor& temperatureSensor;
    unsigned long lastTempCheckTime;
    static constexpr unsigned long TEMPERATURE_INTERVAL = 2000;
    static constexpr float TEMPERATURE_THRESHOLD = 0.5;

    bool shouldToggleForHeat(float currentTemp) const;
    bool shouldToggleForCool(float currentTemp) const;

public:
    TemperatureController(float initialTemp, TemperatureSensor& sensor);
    void toggleMode();
    const char* getMode();
    const char* getMotorState() const;
    void toggleMotorState();
    void adjustTemperature(float amount);
    void setSetTemperature(float amount);
    void setMode(Mode mode);
    void setMotorState(MotorState motorState);
    float getSetTemperature() const;
    float getCurrentTemperature();
    void regulateTemperature();
    const tImage& getModeIcon() const;
    const tImage& getMotorStateIcon() const;
    TelemetryData getTelemetryData();
};

#endif // TEMPERATURE_CONTROLLER_H
