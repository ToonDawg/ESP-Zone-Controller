#include "ButtonManager.h"

ButtonManager *ButtonManager::instance = nullptr;

ButtonManager::ButtonManager(TemperatureController &tempController, AppStateManager &appStateManager, OTAUpdater &updater)
    : tempController(tempController), appStateManager(appStateManager), updater(updater),
      increaseButton(INCREASE_BUTTON_PIN, true),
      decreaseButton(DECREASE_BUTTON_PIN, true),
      buttonA(BUTTON_A_PIN, true),
      buttonB(BUTTON_B_PIN, true)
{
    instance = this;
}

void ButtonManager::setupButtons()
{
    increaseButton.attachClick(handleIncreaseClick);
    decreaseButton.attachClick(handleDecreaseClick);
    buttonA.attachClick(handleButtonAClick);
    buttonB.attachClick(handleButtonBClick);

    buttonA.attachLongPressStart(handleALongPressStart);
    buttonB.attachLongPressStart(handleDecreaseLongPressStart);

    increaseButton.attachLongPressStart(handleIncreaseLongPressStart);
    decreaseButton.attachLongPressStart(handleDecreaseLongPressStart);
    increaseButton.attachDuringLongPress(handleIncreaseLongPress);
    decreaseButton.attachDuringLongPress(handleDecreaseLongPress);
}

void ButtonManager::tick()
{
    increaseButton.tick();
    decreaseButton.tick();
    buttonA.tick();
    buttonB.tick();
}

void ButtonManager::handleIncreaseClick()
{
    if (instance)
        instance->handleIncreaseInCurrentState();
}

void ButtonManager::handleDecreaseClick()
{
    if (instance)
        instance->handleDecreaseInCurrentState();
}

void ButtonManager::handleButtonAClick()
{
    if (instance)
        instance->handleButtonAInCurrentState();
}

void ButtonManager::handleButtonBClick()
{
    if (instance)
        instance->handleButtonBInCurrentState();
}

void ButtonManager::handleIncreaseLongPressStart()
{
    if (instance)
        instance->handleIncreaseLongPressInCurrentState();
}

void ButtonManager::handleALongPressStart()
{
    if (instance)
        instance->appStateManager.setAppState(AppStateManager::AppState::SETTINGS);
}
void ButtonManager::handleBLongPressStart()
{
    if (instance)
        instance->handleIncreaseLongPressInCurrentState();
}

void ButtonManager::handleDecreaseLongPressStart()
{
    if (instance)
        instance->handleDecreaseLongPressInCurrentState();
}

void ButtonManager::handleIncreaseLongPress()
{
    if (instance)
        instance->handleIncreaseLongPressInCurrentState();
}

void ButtonManager::handleDecreaseLongPress()
{
    if (instance)
        instance->handleDecreaseLongPressInCurrentState();
}

void ButtonManager::handleIncreaseInCurrentState()
{
    AppStateManager::AppState currentState = appStateManager.getAppState();
    switch (currentState)
    {
    case AppStateManager::AppState::SETTINGS:
    case AppStateManager::AppState::MOTOR_DIRECTION:
    case AppStateManager::AppState::TEMPERATURE_UNIT:
    case AppStateManager::AppState::ABOUT:
        appStateManager.menuNavigateUp();
        break;
    case AppStateManager::AppState::TEMPERATURE_CALIBRATION:
        tempController.adjustCalibrationTemperature(0.5);
        break;
    default:
        tempController.adjustTemperature(0.5);
        appStateManager.setAppState(AppStateManager::AppState::SET_TEMPERATURE);
        appStateManager.recordAdjustmentTime();
    }
}

void ButtonManager::handleDecreaseInCurrentState()
{
    AppStateManager::AppState currentState = appStateManager.getAppState();
    switch (currentState)
    {
    case AppStateManager::AppState::SETTINGS:
    case AppStateManager::AppState::MOTOR_DIRECTION:
    case AppStateManager::AppState::TEMPERATURE_UNIT:
    case AppStateManager::AppState::ABOUT:
        appStateManager.menuNavigateDown();
        break;
    case AppStateManager::AppState::TEMPERATURE_CALIBRATION:
        tempController.adjustCalibrationTemperature(-0.5);
        break;
    default:
        tempController.adjustTemperature(-0.5);
        appStateManager.setAppState(AppStateManager::AppState::SET_TEMPERATURE);
        appStateManager.recordAdjustmentTime();
    }
}

void ButtonManager::handleButtonAInCurrentState()
{
    AppStateManager::AppState currentState = appStateManager.getAppState();
    switch (currentState)
    {
    case AppStateManager::AppState::SETTINGS:
    case AppStateManager::AppState::MOTOR_DIRECTION:
    case AppStateManager::AppState::TEMPERATURE_UNIT:
    case AppStateManager::AppState::ABOUT:
        appStateManager.selectMenuItem();
        break;
    case AppStateManager::AppState::TEMPERATURE_CALIBRATION:
        appStateManager.setAppState(AppStateManager::AppState::CURRENT_TEMPERATURE);
        break;
    case AppStateManager::AppState::UPDATE:
        appStateManager.setAppState(AppStateManager::AppState::UPDATING);
        updater.performUpdate();
        break;
    default:
        appStateManager.setAppState(AppStateManager::AppState::CURRENT_TEMPERATURE);
        tempController.toggleMode();
        break;
    }
}

void ButtonManager::handleButtonBInCurrentState()
{
    AppStateManager::AppState currentState = appStateManager.getAppState();
    switch (currentState)
    {
    case AppStateManager::AppState::CURRENT_TEMPERATURE:
        appStateManager.setAppState(AppStateManager::AppState::OFF);
        break;
    default:
        appStateManager.setAppState(AppStateManager::AppState::CURRENT_TEMPERATURE);
    }
}

void ButtonManager::handleIncreaseLongPressInCurrentState()
{
    AppStateManager::AppState currentState = appStateManager.getAppState();
    if (currentState == AppStateManager::AppState::CURRENT_TEMPERATURE ||
        currentState == AppStateManager::AppState::SET_TEMPERATURE)
    {
        static unsigned long lastChangeTime = 0;
        if (millis() - lastChangeTime > 200)
        {
            tempController.adjustTemperature(0.5);
            appStateManager.setAppState(AppStateManager::AppState::SET_TEMPERATURE);
            appStateManager.recordAdjustmentTime();
            lastChangeTime = millis();
        }
    }
}

void ButtonManager::handleDecreaseLongPressInCurrentState()
{
    AppStateManager::AppState currentState = appStateManager.getAppState();
    if (currentState == AppStateManager::AppState::CURRENT_TEMPERATURE ||
        currentState == AppStateManager::AppState::SET_TEMPERATURE)
    {
        static unsigned long lastChangeTime = 0;
        if (millis() - lastChangeTime > 200)
        {
            tempController.adjustTemperature(-0.5);
            appStateManager.setAppState(AppStateManager::AppState::SET_TEMPERATURE);
            appStateManager.recordAdjustmentTime();
            lastChangeTime = millis();
        }
    }
}