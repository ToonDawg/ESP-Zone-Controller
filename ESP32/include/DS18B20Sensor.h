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
    float readTemperatureF() override;
    void setTemperatureOffset(float offset) override;
    float getTemperatureOffset() const;
    void update();
    bool isReady() const;

private:
    OneWire oneWire;
    DallasTemperature sensors;
    DeviceAddress deviceAddress;
    float temperatureOffset;
    float lastValidTemperature;
    unsigned long lastRequestTime;
    unsigned long conversionTime;
    bool conversionRequested;
    static constexpr float INVALID_TEMPERATURE = -273.15f;
};

#endif