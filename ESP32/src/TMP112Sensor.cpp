#include "TMP112Sensor.h"
#include <HardwareSerial.h>

TMP112Sensor::TMP112Sensor(uint8_t address) : i2cAddress(address) {
}

void TMP112Sensor::begin() {
    // Initialization code for TMP112
    Wire.begin();
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x01);
    Wire.write(0x60); 
    Wire.endTransmission();
    Serial.println("TMP112 sensor initialized!");
}

float TMP112Sensor::readTemperature() {
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x00);
    Wire.endTransmission();
    
    Wire.requestFrom(i2cAddress, (uint8_t)2);
    if (Wire.available() == 2) {
        uint16_t rawTemperature = (Wire.read() << 8) | Wire.read();
        rawTemperature >>= 4;
        
        if (rawTemperature & 0x800) {
            rawTemperature |= 0xF000;
        }
        
        return rawTemperature * 0.0625;
    } else {
        Serial.println("Error reading temperature!");
        return -00.0;
    }
}
