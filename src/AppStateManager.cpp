#include "AppStateManager.h"
#include "icons.c"

AppStateManager::AppStateManager(DisplayManager& displayManager, TemperatureController& tempController)
    : currentState(AppState::CURRENT_TEMPERATURE), displayManager(displayManager), temperatureController(tempController) {}


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
            displayManager.render();
            break;
        case AppState::SET_TEMPERATURE:
            displayManager.displayTemperature(temperatureController.getSetTemperature());
            displayManager.displayBottomCentre("Set Temp");
            displayManager.render();
            break;
        case AppState::OFF:
            displayManager.displayOff();
            displayManager.render();
            break;
    }
}
void AppStateManager::recordAdjustmentTime() {
    lastAdjustmentTime = millis();
}

void AppStateManager::tick() {
    if (currentState == AppState::SET_TEMPERATURE && millis() - lastAdjustmentTime > 3000) {
        setAppState(AppState::CURRENT_TEMPERATURE);
    }
}
