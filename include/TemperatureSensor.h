#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <Adafruit_MCP9808.h>

class TemperatureSensor {
public:
  TemperatureSensor();
  void begin();
  float readTemperature();

private:
  Adafruit_MCP9808 tempSensor;
};

#endif
