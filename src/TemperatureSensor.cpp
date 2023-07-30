#include "TemperatureSensor.h"

TemperatureSensor::TemperatureSensor() {}

void TemperatureSensor::begin() {
  tempsensor.begin();
}

float TemperatureSensor::readTemperature() {
  return tempsensor.readTempC();
}
