#include "DS18B20Sensor.h"

DS18B20Sensor::DS18B20Sensor(uint8_t pin) : oneWire(pin), sensors(&oneWire) {
}

void DS18B20Sensor::begin() {
    sensors.begin();
    if (sensors.getAddress(deviceAddress, 0)) {
        Serial.println("DS18B20 sensor found!");
    } else {
        Serial.println("No DS18B20 sensors are found!");
    }
}

float DS18B20Sensor::readTemperature() {
    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0);
    return tempC;
}
