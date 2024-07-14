#pragma once

#include <Preferences.h>
#include "Enums.h"

class Settings
{
public:
    Settings();

    void begin();

    void setTemperatureUnit(bool isCelsius);
    bool getTemperatureUnit();

    void setSetTemperature(float setTemp);
    float getSetTemperature();

    void setMotorDirection(MotorDirection direction);
    MotorDirection getMotorDirection();

    void setTemperatureCalibration(float calibration);
    float getTemperatureCalibration();

    void updateAllSettings(bool isCelsius, bool isNormal, float calibration);

private:
    Preferences preferences;
};