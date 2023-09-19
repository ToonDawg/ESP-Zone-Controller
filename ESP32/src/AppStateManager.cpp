#include "AppStateManager.h"
#include "icons.c"
#include <WiFi.h>
#include <BluetoothSerial.h>

AppStateManager::AppStateManager(DisplayManager &displayManager, TemperatureController &tempController, BluetoothManager &btMgr)
    : displayManager(displayManager), temperatureController(tempController), btManager(btMgr), advertisingStarted(false), lastAdjustmentTime(0) {}

void AppStateManager::setAppState(AppState state)
{
    currentState = state;
}

const AppStateManager::AppState &AppStateManager::getAppState() const
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

void AppStateManager::displayCurrentTemperature()
{
    displayManager.displayTemperature(temperatureController.getCurrentTemperature());
    displayManager.displayIconBottomLeft(temperatureController.getModeIcon());
    displayManager.displayIconBottomRight(temperatureController.getMotorStateIcon());
    if (btManager.isConnected())
    {
        displayManager.displayIconBottomMiddle(bluetoothIcon);
    }
}

void AppStateManager::displaySetTemperature()
{
    displayManager.displayTemperature(temperatureController.getSetTemperature());
    displayManager.displayBottomCentre("Set Temp");
}

void AppStateManager::displayOff()
{
    displayManager.displayOff();
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

void AppStateManager::recordAdjustmentTime() {
    lastAdjustmentTime = millis();
}

void AppStateManager::manageBluetoothStatus() {
    if (getAppState() == AppState::CONNECTING) {
        if (!btManager.isConnected() && !advertisingStarted) {
            btManager.startAdvertising();
            advertisingStarted = true;
        } else if (btManager.isConnected()) {
            btManager.stopAdvertising();
            advertisingStarted = false;
            setAppState(AppState::CURRENT_TEMPERATURE);
        }
    } else {
        advertisingStarted = false;
    }
}

void AppStateManager::tick()
{
    manageBluetoothStatus();
    handleStateTimeouts();
}

void AppStateManager::handleStateTimeouts() {
    unsigned long currentTime = millis();

    if (currentState == AppState::SET_TEMPERATURE && currentTime - lastAdjustmentTime > 3000) {
        setAppState(AppState::CURRENT_TEMPERATURE);
    }

    if (getAppState() == AppState::CONNECTING && currentTime - lastAdjustmentTime > 60000) {
        btManager.stopAdvertising();
        setAppState(AppState::CURRENT_TEMPERATURE);
    }
}
