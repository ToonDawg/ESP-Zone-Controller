// Settings.cpp

#include "Settings.h"
#include <TemperatureController.h>

Settings::Settings() : preferences() {}

void Settings::begin()
{
    preferences.begin("core", false);
}

void Settings::setTemperatureUnit(TemperatureUnit unit)
{
    preferences.putInt("temp_unit", static_cast<int>(unit));
}

TemperatureUnit Settings::getTemperatureUnit()
{
    return static_cast<TemperatureUnit>(preferences.getInt("temp_unit", static_cast<int>(TemperatureUnit::Celsius)));
}

void Settings::setSetTemperature(float setTemp)
{
    preferences.putFloat("set_temp", setTemp);
}

float Settings::getSetTemperature()
{
    return preferences.getFloat("set_temp", 24.0);
}

void Settings::setMotorDirection(MotorDirection direction)
{
    preferences.putInt("motor_dir", static_cast<int>(direction));
}

MotorDirection Settings::getMotorDirection()
{
    return static_cast<MotorDirection>(preferences.getInt("motor_dir", static_cast<int>(MotorDirection::Normal)));
}

void Settings::setTemperatureCalibration(float calibration)
{
    preferences.putFloat("temp_cal", calibration);
}

float Settings::getTemperatureCalibration()
{
    return preferences.getFloat("temp_cal", 0.0);
}

void Settings::setCurrentSoftwareVersion(String version)
{
    preferences.putString("current_ver", version);
}

String Settings::getCurrentSoftwareVersion()
{
    return preferences.getString("current_ver", "v0.0.2");
}

void Settings::updateAllSettings(bool isCelsius, bool isNormal, float calibration)
{
    preferences.putBool("temp_unit", isCelsius);
    preferences.putBool("motor_dir", isNormal);
    preferences.putFloat("temp_cal", calibration);
}

void Settings::setLastUpdateCheck(unsigned long time)
{
    preferences.putULong("last_update", time);
}

unsigned long Settings::getLastUpdateCheck()
{
    return preferences.getULong("last_update", 0);
}

void Settings::setLatestAvailableVersion(const String &version)
{
    preferences.putString("latest_ver", version);
}

String Settings::getLatestAvailableVersion()
{
    return preferences.getString("latest_ver", "");
}

void Settings::setAutoSleepOption(int option)
{
    preferences.putInt("auto_sleep", option);
}

int Settings::getAutoSleepOption()
{
    return preferences.getInt("auto_sleep", 0);
}

void Settings::setWiFiSSID(const String& ssid)
{
    preferences.putString("wifi_ssid", ssid);
}

String Settings::getWiFiSSID()
{
    return preferences.getString("wifi_ssid", "");
}

void Settings::setWiFiPassword(const String& password)
{
    preferences.putString("wifi_pass", password);
}

String Settings::getWiFiPassword()
{
    return preferences.getString("wifi_pass", "");
}

void Settings::printAllSettings() {
    Serial.println("Current Settings:");
    Serial.println("--------------------");

    // Temperature Unit
    Serial.print("Temperature Unit: ");
    Serial.println(getTemperatureUnit() == TemperatureUnit::Celsius ? "Celsius" : "Fahrenheit");

    // Set Temperature
    Serial.print("Set Temperature: ");
    Serial.println(getSetTemperature());

    // Motor Direction
    Serial.print("Motor Direction: ");
    Serial.println(getMotorDirection() == MotorDirection::Normal ? "Normal" : "Reverse");

    // Temperature Calibration
    Serial.print("Temperature Calibration: ");
    Serial.println(getTemperatureCalibration());

    // Current Software Version
    Serial.print("Current Software Version: ");
    Serial.println(getCurrentSoftwareVersion());

    // Last Update Check
    Serial.print("Last Update Check: ");
    Serial.println(getLastUpdateCheck());

    // Latest Available Version
    Serial.print("Latest Available Version: ");
    Serial.println(getLatestAvailableVersion());

    // Auto Sleep Option
    Serial.print("Auto Sleep Option: ");
    Serial.println(getAutoSleepOption());

    Serial.println("--------------------");
}