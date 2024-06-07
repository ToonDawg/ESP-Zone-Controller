#ifndef DS18B20_SENSOR_H
#define DS18B20_SENSOR_H

#include "TemperatureSensor.h"
#include <OneWire.h>
#include <DallasTemperature.h>

class DS18B20Sensor : public TemperatureSensor {
public:
    DS18B20Sensor(uint8_t pin); 
    void begin() override;
    float readTemperature() override;

private:
    OneWire oneWire;
    DallasTemperature sensors;
    DeviceAddress deviceAddress;
};

#endif // DS18B20_SENSOR_H
