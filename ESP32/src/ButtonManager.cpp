// ButtonManager.cpp
#include "ButtonManager.h"

ButtonManager* ButtonManager::instance = nullptr;

ButtonManager::ButtonManager(TemperatureController& tempController, AppStateManager& appStateManager)
    : tempController(tempController), appStateManager(appStateManager),
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

void ButtonManager::handleIncreaseClick() { if (instance) instance->handleIncreaseButtonEvents(); }
void ButtonManager::handleDecreaseClick() { if (instance) instance->handleDecreaseButtonEvents(); }
void ButtonManager::handleButtonAClick() { if (instance) instance->handleButtonAEvents(); }
void ButtonManager::handleButtonBClick() { if (instance) instance->handleButtonBEvents(); }

void ButtonManager::handleIncreaseLongPressStart() { if (instance) instance->handleIncreaseButtonEvents(); }
void ButtonManager::handleDecreaseLongPressStart() { if (instance) instance->handleDecreaseButtonEvents(); }
void ButtonManager::handleIncreaseLongPress() { if (instance) instance->handleIncreaseButtonEvents(); }
void ButtonManager::handleDecreaseLongPress() { if (instance) instance->handleDecreaseButtonEvents(); }

void ButtonManager::handleIncreaseButtonEvents()
{
    switch (appStateManager.getAppState())
    {
        case AppStateManager::AppState::CURRENT_TEMPERATURE:
            appStateManager.setAppState(AppStateManager::AppState::SET_TEMPERATURE);
            tempController.adjustSetTemperature(0.5);
            break;
        case AppStateManager::AppState::SET_TEMPERATURE:
            tempController.adjustSetTemperature(0.5);
            break;
        case AppStateManager::AppState::MENU:
            appStateManager.handleInput(1); // Up
            break;
        default:
            break;
    }
}

void ButtonManager::handleDecreaseButtonEvents()
{
    switch (appStateManager.getAppState())
    {
        case AppStateManager::AppState::CURRENT_TEMPERATURE:
            appStateManager.setAppState(AppStateManager::AppState::SET_TEMPERATURE);
            tempController.adjustSetTemperature(-0.5);
            break;
        case AppStateManager::AppState::SET_TEMPERATURE:
            tempController.adjustSetTemperature(-0.5);
            break;
        case AppStateManager::AppState::MENU:
            appStateManager.handleInput(2); // Down
            break;
        default:
            break;
    }
}

void ButtonManager::handleButtonAEvents()
{
    switch (appStateManager.getAppState())
    {
        case AppStateManager::AppState::CURRENT_TEMPERATURE:
            appStateManager.setAppState(AppStateManager::AppState::MENU);
            break;
        case AppStateManager::AppState::SET_TEMPERATURE:
            appStateManager.setAppState(AppStateManager::AppState::CURRENT_TEMPERATURE);
            break;
        case AppStateManager::AppState::MENU:
            appStateManager.handleInput(3); // Select
            break;
        default:
            break;
    }
}

void ButtonManager::handleButtonBEvents()
{
    switch (appStateManager.getAppState())
    {
        case AppStateManager::AppState::CURRENT_TEMPERATURE:
            tempController.toggleMode();
            break;
        case AppStateManager::AppState::SET_TEMPERATURE:
            appStateManager.setAppState(AppStateManager::AppState::CURRENT_TEMPERATURE);
            break;
        case AppStateManager::AppState::MENU:
            appStateManager.handleInput(4); // Back
            break;
        case AppStateManager::AppState::OFF:
            appStateManager.setAppState(AppStateManager::AppState::CURRENT_TEMPERATURE);
            break;
        default:
            break;
    }
}