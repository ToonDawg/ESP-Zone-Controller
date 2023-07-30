// Make sure to include guards to prevent double inclusion of this header file
#ifndef TEMPERATURE_CONTROLLER_H
#define TEMPERATURE_CONTROLLER_H

#include "TemperatureSensor.h"

class TemperatureController {
  private:
    float setTemperature;
    float currentTemperature;
    TemperatureSensor& temperatureSensor;
    unsigned long lastTempCheckTime;
    static constexpr unsigned long TEMPERATURE_INTERVAL = 2000;

  public:
    TemperatureController(float initialTemp, TemperatureSensor& sensor);
    void adjustTemperature(float amount);
    float getSetTemperature() const;
    float getCurrentTemperature();
};

#endif // TEMPERATURE_CONTROLLER_H
