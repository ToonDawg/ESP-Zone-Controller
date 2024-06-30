#ifndef MCP9808_SENSOR_H
#define MCP9808_SENSOR_H

#include "TemperatureSensor.h"
#include <Wire.h>
#include <Adafruit_MCP9808.h>

class MCP9808Sensor : public TemperatureSensor {
public:
    MCP9808Sensor(uint8_t address = 0x18, TwoWire *wire = &Wire);
    void begin() override;
    float readTemperature() override;

private:
    Adafruit_MCP9808 sensor;
    TwoWire *wire;
};

#endif // MCP9808_SENSOR_H