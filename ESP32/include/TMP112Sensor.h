#ifndef TMP112_SENSOR_H
#define TMP112_SENSOR_H

#include "TemperatureSensor.h"
#include <Wire.h>

class TMP112Sensor : public TemperatureSensor {
public:
    /**
     * @brief Construct a new TMP112Sensor object
     * 
     * @param address I2C address of the sensor
     * @param wire Pointer to the TwoWire object for I2C communication
     */
    TMP112Sensor(uint8_t address, TwoWire *wire = &Wire);

    /**
     * @brief Initialize the sensor
     */
    void begin() override;

    /**
     * @brief Read the temperature from the sensor
     * 
     * @return float The temperature in Celsius, or INVALID_TEMPERATURE if reading fails
     */
    float readTemperature() override;

    /**
     * @brief Set a temperature offset for calibration
     * 
     * @param offset The offset in Celsius to be applied to the temperature readings
     */
    void setTemperatureOffset(float offset);

    /**
     * @brief Get the current temperature offset
     * 
     * @return float The current temperature offset in Celsius
     */
    float getTemperatureOffset() const;

private:
    uint8_t i2cAddress;
    TwoWire *wire;
    float temperatureOffset;

    /**
     * @brief Read a single temperature measurement from the sensor
     * 
     * @return float The temperature in Celsius, or INVALID_TEMPERATURE if reading fails
     */
    float readSingleTemperature();
};

#endif // TMP112_SENSOR_H