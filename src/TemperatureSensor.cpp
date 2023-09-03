#include "TemperatureSensor.h"

TemperatureSensor::TemperatureSensor() {}

void TemperatureSensor::begin() {
  tempSensor.begin();
}

float TemperatureSensor::readTemperature() {
  return tempSensor.readTempC();
}
