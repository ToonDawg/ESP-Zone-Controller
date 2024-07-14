// Settings.cpp

#include "Settings.h"
#include <TemperatureController.h>

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
    return preferences.getFloat("set_temp", 24.0);
}

void Settings::setMotorDirection(MotorDirection direction) {
    preferences.putInt("motor_dir", static_cast<int>(direction));
}

MotorDirection Settings::getMotorDirection() {
    return static_cast<MotorDirection>(preferences.getInt("motor_dir", static_cast<int>(MotorDirection::Normal)));
}

void Settings::setTemperatureCalibration(float calibration) {
    preferences.putFloat("temp_cal", calibration);
}

float Settings::getTemperatureCalibration() {
    return preferences.getFloat("temp_cal", 0.0); 
}

void Settings::setCurrentSoftwareVersion(String version) {
    preferences.putString("current_ver", version);
}

String Settings::getCurrentSoftwareVersion() {
    return preferences.getString("current_ver", "v0.0.2"); 
}

void Settings::updateAllSettings(bool isCelsius, bool isNormal, float calibration) {
    preferences.putBool("temp_unit", isCelsius);
    preferences.putBool("motor_dir", isNormal);
    preferences.putFloat("temp_cal", calibration);
}