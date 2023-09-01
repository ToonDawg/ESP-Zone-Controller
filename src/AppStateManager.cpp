#include "AppStateManager.h"
#include "icons.c"

AppStateManager::AppStateManager(DisplayManager& displayManager, TemperatureController& tempController)
    : currentState(AppState::CURRENT_TEMPERATURE), displayManager(displayManager), temperatureController(tempController) {}


void AppStateManager::setAppState(AppState state) {
    currentState = state;
}


void AppStateManager::display() {
    switch (currentState) {
      case AppState::CURRENT_TEMPERATURE:
        displayManager.displayTemperature(temperatureController.getCurrentTemperature());
        displayManager.displayIconBottomLeft(temperatureController.getModeIcon());
        displayManager.displayIconBottomRight(temperatureController.getMotorStateIcon());
        break;
      case AppState::SET_TEMPERATURE:
        displayManager.displayTemperature(temperatureController.getSetTemperature());
        displayManager.displayBottomCentre("Set Temp");
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
