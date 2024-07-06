#include "AppStateManager.h"
#include <Arduino.h>
#include "AppStateManager.h"

#include "AppStateManager.h"

AppStateManager::AppStateManager(DisplayManager &displayManager, TemperatureController &tempController)
    : displayManager(displayManager), temperatureController(tempController),
      currentState(AppState::CURRENT_TEMPERATURE), lastAdjustmentTime(0),
      settingsMenu("Settings", {"Temp. Unit", "Motor Direction", "WiFi", "Temp. Calibration"}),
      motorDirectionMenu("Motor Direction", {"Normal", "Reversed"})
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
    display();
}

void AppStateManager::selectMenuItem()
{
    if (currentState == AppState::SETTINGS)
    {
        switch (settingsMenu.getSelectedIndex())
        {
        case 0: /* Handle Temp Unit */
            break;
        case 1:
            setAppState(AppState::MOTOR_DIRECTION);
            break;
        case 2:
            setAppState(AppState::WIFI_PROVISIONING);
            break;
        case 3: /* Handle Temp Calibration */
            break;
        }
    }
    else if (currentState == AppState::MOTOR_DIRECTION)
    {
        // Handle motor direction selection
        setAppState(AppState::SETTINGS);
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

void AppStateManager::displayOff()
{
    displayManager.displayOff();
}

void AppStateManager::displaySettings()
{
    displayManager.displaySettingsMenu(
        settingsMenu.getTitle(),
        settingsMenu.getItems(),
        settingsMenu.getItems().size(),
        settingsMenu.getSelectedIndex());
}

void AppStateManager::displayMotorDirectionSetting()
{
    displayManager.displaySettingsMenu(
        motorDirectionMenu.getTitle(),
        motorDirectionMenu.getItems(),
        motorDirectionMenu.getItems().size(),
        motorDirectionMenu.getSelectedIndex());
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