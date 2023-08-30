#ifndef TEMPERATURE_CONTROLLER_H
#define TEMPERATURE_CONTROLLER_H

#include "TemperatureSensor.h"

enum class Mode {
  Heat,
  Cool,
};
enum class MotorState {
  Open,
  Closed,
};

class TemperatureController {
  private:
    float setTemperature;
    float currentTemperature;
    private:
    Mode currentMode = Mode::Cool;
    MotorState currentMotorState = MotorState::Open;
    TemperatureSensor& temperatureSensor;
    unsigned long lastTempCheckTime;
    static constexpr unsigned long TEMPERATURE_INTERVAL = 2000;

  public:
    TemperatureController(float initialTemp, TemperatureSensor& sensor);
    void toggleMode();
    const char* getMode() const;
    const char* getMotorState() const;
    void toggleMotorState();
    void adjustTemperature(float amount);
    float getSetTemperature() const;
    float getCurrentTemperature();
    
};

#endif // TEMPERATURE_CONTROLLER_H
