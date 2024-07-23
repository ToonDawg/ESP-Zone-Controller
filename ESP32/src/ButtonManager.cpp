// ButtonManager.cpp
#include "ButtonManager.h"

ButtonManager *ButtonManager::instance = nullptr;

ButtonManager::ButtonManager(TemperatureController &tempController, AppStateManager &appStateManager)
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
    buttonA.attachLongPressStart(handleALongPressStart);

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
        instance->handleIncreaseButtonEvents();
}
void ButtonManager::handleDecreaseClick()
{
    if (instance)
        instance->handleDecreaseButtonEvents();
}
void ButtonManager::handleButtonAClick()
{
    if (instance)
        instance->handleButtonAEvents();
}
void ButtonManager::handleButtonBClick()
{
    if (instance)
        instance->handleButtonBEvents();
}

void ButtonManager::handleIncreaseLongPressStart()
{
    if (instance)
        instance->handleIncreaseButtonEvents();
}
void ButtonManager::handleDecreaseLongPressStart()
{
    if (instance)
        instance->handleDecreaseButtonEvents();
}
void ButtonManager::handleIncreaseLongPress()
{
    if (instance)
        instance->handleIncreaseButtonEvents();
}
void ButtonManager::handleDecreaseLongPress()
{
    if (instance)
        instance->handleDecreaseButtonEvents();
}

void ButtonManager::handleALongPressStart()
{
    if (instance)
        instance->appStateManager.setAppState(AppStateManager::AppState::MENU);
}

void ButtonManager::handleIncreaseButtonEvents()
{
    appStateManager.handleInput(AppStateManager::ButtonInput::UP);
}

void ButtonManager::handleDecreaseButtonEvents()
{
    appStateManager.handleInput(AppStateManager::ButtonInput::DOWN);
}

void ButtonManager::handleButtonAEvents()
{
    appStateManager.handleInput(AppStateManager::ButtonInput::SELECT);
}

void ButtonManager::handleButtonBEvents()
{
    appStateManager.handleInput(AppStateManager::ButtonInput::BACK);
}
