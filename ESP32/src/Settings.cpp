// Settings.cpp

#include "Settings.h"

Settings::Settings() : preferences() {}

void Settings::begin() {
    preferences.begin("core", false); 
}

void Settings::setTemperatureUnit(bool isCelsius) {
    preferences.putBool("temp_unit", isCelsius);
}

bool Settings::getTemperatureUnit() {
    return preferences.getBool("temp_unit", true);
}

void Settings::setSetTemperature(float setTemp) {
    preferences.putFloat("set_temp", setTemp);
}

float Settings::getSetTemperature() {
    return preferences.getFloat("set_temp", 10.0);
}

void Settings::setMotorDirection(bool isNormal) {
    preferences.putBool("motor_dir", isNormal);
}

bool Settings::getMotorDirection() {
    return preferences.getBool("motor_dir", true); 
}

void Settings::setTemperatureCalibration(float calibration) {
    preferences.putFloat("temp_cal", calibration);
}

float Settings::getTemperatureCalibration() {
    return preferences.getFloat("temp_cal", 0.0); 
}

void Settings::updateAllSettings(bool isCelsius, bool isNormal, float calibration) {
    preferences.putBool("temp_unit", isCelsius);
    preferences.putBool("motor_dir", isNormal);
    preferences.putFloat("temp_cal", calibration);
}