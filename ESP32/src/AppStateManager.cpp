#include "AppStateManager.h"
#include <Arduino.h>

AppStateManager::AppStateManager(DisplayManager &displayManager, TemperatureController &tempController, BluetoothManager &btManager)
    : displayManager(displayManager), temperatureController(tempController), btManager(btManager),
      currentState(AppState::CURRENT_TEMPERATURE), lastAdjustmentTime(0)
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
    case AppState::CONNECTING:
        displayConnecting();
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

void AppStateManager::displayCurrentTemperature()
{
    displayManager.displayTemperature(temperatureController.getStatus().currentTemperature);
    displayManager.displayIconBottomLeft(temperatureController.getModeIcon());
    displayManager.displayIconBottomRight(temperatureController.getMotorStateIcon());
}

void AppStateManager::displaySetTemperature()
{
    displayManager.displayTemperature(temperatureController.getStatus().setTemperature);
    displayManager.displayBottomCentre("Set Temp");
}

void AppStateManager::displayOff()
{
    displayManager.displayOff();
    displayManager.render();
}

void AppStateManager::displayConnecting()
{
    String lines[2];

    if (btManager.isConnected())
    {
        lines[0] = "Connected to";
        lines[1] = "Bluetooth!";
    }
    else
    {
        lines[0] = "Connecting to";
        lines[1] = "Bluetooth...";
    }

    displayManager.displayCentre(lines, 2);
}

void AppStateManager::manageBluetoothStatus()
{
    if (getAppState() == AppState::CONNECTING)
    {
        if (btManager.isConnected())
        {
            setAppState(AppState::CURRENT_TEMPERATURE);
        }
    }
}

void AppStateManager::handleStateTimeouts()
{
    unsigned long currentTime = millis();

    if (currentState == AppState::SET_TEMPERATURE && currentTime - lastAdjustmentTime > 3000)
    {
        setAppState(AppState::CURRENT_TEMPERATURE);
    }

    if (getAppState() == AppState::CONNECTING && currentTime - lastAdjustmentTime > 60000)
    {
        setAppState(AppState::CURRENT_TEMPERATURE);
    }
}
