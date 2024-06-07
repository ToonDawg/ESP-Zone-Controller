#include "ButtonManager.h"

ButtonManager* ButtonManager::instance = nullptr;

ButtonManager::ButtonManager(TemperatureController& tempController, AppStateManager& appStateManager)
    : tempController(tempController), appStateManager(appStateManager),
      increaseTemperatureButton(INCREASE_TEMPERATURE_BUTTON_PIN, true),
      decreaseTemperatureButton(DECREASE_TEMPERATURE_BUTTON_PIN, true),
      toggleModeButton(TOGGLE_MODE_BUTTON_PIN, true),
      toggleDisplayButton(TOGGLE_DISPLAY_BUTTON_PIN, true) {
    instance = this;
}

void ButtonManager::setupButtons() {
    increaseTemperatureButton.attachClick(handleIncreaseTemperatureClick);
    decreaseTemperatureButton.attachClick(handleDecreaseTemperatureClick);
    toggleModeButton.attachClick(handleToggleModeClick);
    toggleModeButton.attachLongPressStart(handleToggleModeLongPressStart);
    toggleDisplayButton.attachClick(handleToggleDisplayClick);
    increaseTemperatureButton.attachDuringLongPress(handleIncreaseTemperatureDuringLongPress);
    decreaseTemperatureButton.attachDuringLongPress(handleDecreaseTemperatureDuringLongPress);
}

void ButtonManager::tick() {
    increaseTemperatureButton.tick();
    decreaseTemperatureButton.tick();
    toggleModeButton.tick();
    toggleDisplayButton.tick();
}

void ButtonManager::handleIncreaseTemperatureClick() {
    instance->tempController.adjustTemperature(.5);
    instance->appStateManager.setAppState(AppStateManager::AppState::SET_TEMPERATURE);
    instance->appStateManager.recordAdjustmentTime();
}

void ButtonManager::handleDecreaseTemperatureClick() {
    instance->tempController.adjustTemperature(-.5);
    instance->appStateManager.setAppState(AppStateManager::AppState::SET_TEMPERATURE);
    instance->appStateManager.recordAdjustmentTime();
}

void ButtonManager::handleToggleModeClick() {
    instance->tempController.toggleMode();
    instance->appStateManager.setAppState(AppStateManager::AppState::CURRENT_TEMPERATURE);
}

void ButtonManager::handleToggleModeLongPressStart() {
    instance->appStateManager.setAppState(AppStateManager::AppState::CONNECTING);
    instance->appStateManager.recordAdjustmentTime();
}

void ButtonManager::handleToggleDisplayClick() {
    if (instance->appStateManager.getAppState() == AppStateManager::AppState::OFF) {
        instance->appStateManager.setAppState(AppStateManager::AppState::CURRENT_TEMPERATURE);
    } else {
        instance->appStateManager.setAppState(AppStateManager::AppState::OFF);
    }
}

void ButtonManager::handleIncreaseTemperatureDuringLongPress() {
    instance->appStateManager.setAppState(AppStateManager::AppState::SET_TEMPERATURE);
    static unsigned long lastChangeTime = 0;
    if (millis() - lastChangeTime > 200) {
        instance->tempController.adjustTemperature(.5);
        instance->appStateManager.recordAdjustmentTime();
        lastChangeTime = millis();
    }
}

void ButtonManager::handleDecreaseTemperatureDuringLongPress() {
    instance->appStateManager.setAppState(AppStateManager::AppState::SET_TEMPERATURE);
    static unsigned long lastChangeTime = 0;
    if (millis() - lastChangeTime > 200) {
        instance->tempController.adjustTemperature(-.5);
        instance->appStateManager.recordAdjustmentTime();
        lastChangeTime = millis();
    }
}
