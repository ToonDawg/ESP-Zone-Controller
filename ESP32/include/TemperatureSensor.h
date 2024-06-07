#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

class TemperatureSensor {
public:
    virtual void begin() = 0;
    virtual float readTemperature() = 0;

    virtual ~TemperatureSensor() = default; 
};

#endif // TEMPERATURE_SENSOR_H
