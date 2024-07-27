#pragma once

#include <Preferences.h>
#include "Enums.h"

class Settings
{
public:
    Settings();

    void begin();

    void setTemperatureUnit(TemperatureUnit unit);
    TemperatureUnit getTemperatureUnit();

    void setSetTemperature(float setTemp);
    float getSetTemperature();

    void setMotorDirection(MotorDirection direction);
    MotorDirection getMotorDirection();

    void setTemperatureCalibration(float calibration);
    float getTemperatureCalibration();

    void setAutoSleepOption(int option);
    int getAutoSleepOption();

    void printAllSettings();

    void setCurrentSoftwareVersion(String version);
    String getCurrentSoftwareVersion();

    void updateAllSettings(bool isCelsius, bool isNormal, float calibration);

    void setLastUpdateCheck(unsigned long time);
    unsigned long getLastUpdateCheck();

    void setLatestAvailableVersion(const String &version);
    String getLatestAvailableVersion();

    void setWiFiSSID(const String& ssid);
    String getWiFiSSID();

    void setWiFiPassword(const String& password);
    String getWiFiPassword();

private:
    Preferences preferences;
};