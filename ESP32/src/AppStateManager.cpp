#include "AppStateManager.h"
#include <Arduino.h>
#include "AppStateManager.h"
#include "Settings.h"

AppStateManager::AppStateManager(DisplayManager &displayManager, TemperatureController &tempController, Settings &settings)
    : displayManager(displayManager),
      temperatureController(tempController),
      settings(settings),
      currentState(AppState::CURRENT_TEMPERATURE),
      lastAdjustmentTime(0),
      settingsMenu("Settings", {"Temp. Calibration", "Motor Direction", "Temp. Unit", "WiFi"}),
      motorDirectionMenu("Motor Direction", {"Normal", "Reversed"}),
      tempUnitMenu("Temp. Unit", {"Celsius", "Fahrenheit"})
{
}

void AppStateManager::setAppState(AppState state)
{
    currentState = state;
    lastAdjustmentTime = millis();
}

AppStateManager::AppState AppStateManager::getAppState() const
{
    return currentState;
}

void AppStateManager::display()
{
    switch (currentState)
    {
    case AppState::CURRENT_TEMPERATURE:
        displayCurrentTemperature();
        break;
    case AppState::SET_TEMPERATURE:
        displaySetTemperature();
        break;
    case AppState::OFF:
        displayOff();
        break;
    case AppState::SETTINGS:
        displaySettings();
        break;
    case AppState::WIFI_PROVISIONING:
        displayWiFiProvisioning();
        break;
    case AppState::MOTOR_DIRECTION:
        displayMotorDirectionSetting();
        break;
    case AppState::TEMPERATURE_CALIBRATION:
        displayTemperatureCalibration();
        break;
    case AppState::TEMPERATURE_UNIT:
        displayTemperatureUnitSetting();
        break;
    }
    displayManager.render();
}

void AppStateManager::tick()
{
    manageBluetoothStatus();
    handleStateTimeouts();
}

void AppStateManager::recordAdjustmentTime()
{
    lastAdjustmentTime = millis();
}

void AppStateManager::menuNavigateUp()
{
    if (currentState == AppState::SETTINGS)
    {
        settingsMenu.navigateUp();
    }
    else if (currentState == AppState::MOTOR_DIRECTION)
    {
        motorDirectionMenu.navigateUp();
    }
    else if (currentState == AppState::TEMPERATURE_UNIT)
    {
        tempUnitMenu.navigateUp();
    }
    display();
}

void AppStateManager::menuNavigateDown()
{
    if (currentState == AppState::SETTINGS)
    {
        settingsMenu.navigateDown();
    }
    else if (currentState == AppState::MOTOR_DIRECTION)
    {
        motorDirectionMenu.navigateDown();
    }
    else if (currentState == AppState::TEMPERATURE_UNIT)
    {
        tempUnitMenu.navigateDown();
    }
    display();
}

void AppStateManager::selectMenuItem()
{
    switch (currentState)
    {
    case AppState::SETTINGS:
        switch (settingsMenu.getSelectedIndex())
        {
        case 0:
            setAppState(AppState::TEMPERATURE_CALIBRATION);
            break;
        case 1:
            setAppState(AppState::MOTOR_DIRECTION);
            break;
        case 2:
            setAppState(AppState::TEMPERATURE_UNIT);
            break;
        case 3:
            setAppState(AppState::WIFI_PROVISIONING);
            break;
        }
        break;

    case AppState::MOTOR_DIRECTION:
        switch (motorDirectionMenu.getSelectedIndex())
        {
        case 0:
            settings.setMotorDirection(true);
            setAppState(AppState::CURRENT_TEMPERATURE);
            break;
        case 1:
            settings.setMotorDirection(false);
            setAppState(AppState::CURRENT_TEMPERATURE);
            break;
        default:
            setAppState(AppState::CURRENT_TEMPERATURE);
            break;
        }
        break;

    case AppState::TEMPERATURE_UNIT:
        switch (tempUnitMenu.getSelectedIndex())
        {
        case 0:
            settings.setTemperatureUnit(true);
            setAppState(AppState::CURRENT_TEMPERATURE);
            break;
        case 1:
            settings.setTemperatureUnit(false);
            setAppState(AppState::CURRENT_TEMPERATURE);
            break;
        default:
            setAppState(AppState::CURRENT_TEMPERATURE);
            break;
        }
        break;

    default:
        // Handle any other states if necessary
        break;
    }
}

void AppStateManager::displayCurrentTemperature()
{
    displayManager.displayTemperature(temperatureController.getStatus().currentTemperature);
    displayManager.displayIconBottomLeft(temperatureController.getModeIcon());
    displayManager.displayIconBottomRight(temperatureController.getMotorStateIcon());
}

void AppStateManager::displaySetTemperature()
{
    displayManager.displayTemperature(temperatureController.getStatus().setTemperature);
    displayManager.displayBottomCenterText("Set Temp");
}

void AppStateManager::displayTemperatureCalibration()
{
    displayManager.displayTemperature(settings.getTemperatureCalibration());
    displayManager.displayBottomCenterText("Temp. Calibration");
}

void AppStateManager::displayOff()
{
    displayManager.displayOff();
}

void AppStateManager::displaySettings()
{
    displayManager.displaySettingsMenu(settingsMenu);
}

void AppStateManager::displayMotorDirectionSetting()
{
    bool currentDirection = settings.getMotorDirection();
    motorDirectionMenu.setActiveIndex(currentDirection ? 0 : 1);
    displayManager.displaySettingsMenu(motorDirectionMenu);
}

void AppStateManager::displayTemperatureUnitSetting()
{
    bool tempUnit = settings.getTemperatureUnit();
    tempUnitMenu.setActiveIndex(tempUnit ? 0 : 1);
    displayManager.displaySettingsMenu(tempUnitMenu);
}

void AppStateManager::displayWiFiProvisioning()
{
    // Implement WiFi provisioning display
    String lines[2] = {"WiFi", "Setup"};
    displayManager.displayCentre(lines, 2);
}

void AppStateManager::manageBluetoothStatus()
{
    // Implement Bluetooth status management
}

void AppStateManager::handleStateTimeouts()
{
    unsigned long currentTime = millis();

    if (currentState == AppState::SET_TEMPERATURE && currentTime - lastAdjustmentTime > 3000)
    {
        setAppState(AppState::CURRENT_TEMPERATURE);
    }
}

void AppStateManager::saveSettings()
{
    // settings.setTemperatureUnit(/* get current temperature unit */);
    // settings.setTemperatureCalibration(/* get current temperature calibration */);
}

void AppStateManager::loadSettings()
{
    bool isCelsius = settings.getTemperatureUnit();
    bool isNormalMotorDirection = settings.getMotorDirection();
    float tempCalibration = settings.getTemperatureCalibration();
}

void AppStateManager::beginSettings()
{
    settings.begin();
}