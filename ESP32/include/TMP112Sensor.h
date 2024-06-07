#ifndef TMP112_SENSOR_H
#define TMP112_SENSOR_H

#include "TemperatureSensor.h"
#include <Wire.h>

class TMP112Sensor : public TemperatureSensor {
public:
    TMP112Sensor(uint8_t address); // Constructor to initialize the sensor address
    void begin() override;
    float readTemperature() override;

private:
    uint8_t i2cAddress;
};

#endif // TMP112_SENSOR_H
