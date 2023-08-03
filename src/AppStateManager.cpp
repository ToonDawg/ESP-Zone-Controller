#include "AppStateManager.h"

AppStateManager::AppStateManager(DisplayManager& displayManager)
    : currentState(AppState::CURRENT_TEMPERATURE), displayManager(displayManager) {}

void AppStateManager::setAppState(AppState state) {
    currentState = state;
}

void AppStateManager::display(float temperature) {
    switch (currentState) {
      case AppState::CURRENT_TEMPERATURE:
        displayManager.displayTemperature(temperature);
        displayManager.displayBottomLeft("Cool");
        displayManager.displayBottomRight("Open");
        displayManager.displayBottomCentre("XO");
        break;
      case AppState::SET_TEMPERATURE:
        // Display something different
        displayManager.displayTemperature(temperature);
        displayManager.displayBottomLeft("Button");
        displayManager.displayBottomRight("Pressed");
        displayManager.displayBottomCentre("XX");
        break;
      // add more cases as needed
    }
}
