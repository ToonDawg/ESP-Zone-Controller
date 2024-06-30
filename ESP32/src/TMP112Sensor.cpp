#include "TMP112Sensor.h"
#include <HardwareSerial.h>

constexpr float INVALID_TEMPERATURE = -273.15f;

TMP112Sensor::TMP112Sensor(uint8_t address, TwoWire *wire) 
    : i2cAddress(address), wire(wire), temperatureOffset(0.0f) {}

void TMP112Sensor::begin() {
    wire->beginTransmission(i2cAddress);
    wire->write(0x01);  // Configuration register
    wire->write(0x60);  // 12-bit resolution, continuous conversion
    wire->write(0x00);  // Standard mode, other settings default
    uint8_t error = wire->endTransmission();
    if (error == 0) {
        Serial.println("TMP112 sensor initialized!");
    } else {
        Serial.printf("TMP112 initialization failed. Error code: %d\n", error);
    }
}

float TMP112Sensor::readTemperature() {
    wire->beginTransmission(i2cAddress);
    wire->write(0x00);  // Temperature register
    if (wire->endTransmission() != 0) {
        Serial.println("TMP112: Error setting temperature register");
        return INVALID_TEMPERATURE;
    }
    
    if (wire->requestFrom(i2cAddress, (uint8_t)2) != 2) {
        Serial.println("TMP112: Error requesting data");
        return INVALID_TEMPERATURE;
    }
    
    uint8_t msb = wire->read();
    uint8_t lsb = wire->read();
    
    int16_t rawTemperature = ((msb << 8) | lsb);
    rawTemperature >>= 4;  // Right-shift by 4 for 12-bit resolution
    
    float temperature = rawTemperature * 0.0625f;
    
    return temperature + temperatureOffset;
}

void TMP112Sensor::setTemperatureOffset(float offset) {
    temperatureOffset = offset;
}

float TMP112Sensor::getTemperatureOffset() const {
    return temperatureOffset;
}