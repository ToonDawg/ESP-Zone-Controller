#include "AppStateManager.h"
#include "icons.c"
#include <WiFi.h>
#include <BluetoothSerial.h>

AppStateManager::AppStateManager(DisplayManager& displayManager, TemperatureController& tempController, BluetoothManager& btMgr) 
    : displayManager(displayManager), temperatureController(tempController), btManager(btMgr) {}

void AppStateManager::setAppState(AppState state) {
    currentState = state;
}

const AppStateManager::AppState& AppStateManager::getAppState() const {
    return currentState;
}

void AppStateManager::display() {
switch (currentState) {
    case AppState::CURRENT_TEMPERATURE:
        displayManager.displayTemperature(temperatureController.getCurrentTemperature());
        displayManager.displayIconBottomLeft(temperatureController.getModeIcon());
        displayManager.displayIconBottomRight(temperatureController.getMotorStateIcon());
        if (btManager.isConnected()) {
            displayManager.displayIconBottomMiddle(bluetoothIcon);
        }
        break;
    case AppState::SET_TEMPERATURE:
        displayManager.displayTemperature(temperatureController.getSetTemperature());
        displayManager.displayBottomCentre("Set Temp");
        break;
    case AppState::OFF:
        displayManager.displayOff();
        break;
    case AppState::CONNECTING:
        if (btManager.isConnected()) {
            String lines[] = {"Connected to", "Bluetooth!"};
            displayManager.displayCentre(lines, 2);
        } else {
            btManager.startAdvertising();  
            String lines[] = {"Connecting to", "Bluetooth..."};
            displayManager.displayCentre(lines, 2);
        }
        break;
    }
    displayManager.render();
}

void AppStateManager::recordAdjustmentTime() {
    lastAdjustmentTime = millis();
}

void AppStateManager::tick() {
    if (currentState == AppState::SET_TEMPERATURE && millis() - lastAdjustmentTime > 3000) {
        setAppState(AppState::CURRENT_TEMPERATURE);
    }

    if (getAppState() == AppState::CONNECTING && millis() - lastAdjustmentTime > 60000) {
        setAppState(AppState::CURRENT_TEMPERATURE);
    }
}
