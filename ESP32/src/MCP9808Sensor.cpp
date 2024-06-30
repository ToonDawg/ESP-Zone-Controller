#include "MCP9808Sensor.h"
#include <DallasTemperature.h>

MCP9808Sensor::MCP9808Sensor(uint8_t address, TwoWire *wire) : sensor(), wire(wire)
{
}

void MCP9808Sensor::begin()
{
    if (sensor.begin(wire))
    {
        Serial.println("MCP9808 sensor found!");
        sensor.wake(); // Wake up MCP9808 - power consumption ~200 μA
    }
    else
    {
        Serial.println("Couldn't find MCP9808! Check your connections and verify the address.");
    }
}

float MCP9808Sensor::readTemperature()
{
    float tempC = sensor.readTempC();
    if (tempC != DEVICE_DISCONNECTED_C)
    {
        return tempC;
    }
    else
    {
        Serial.println("MCP9808: Error reading temperature!");
        return -273.15;
    }
}