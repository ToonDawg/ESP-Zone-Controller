// ButtonManager.h
#pragma once

#include <OneButton.h>
#include "TemperatureController.h"
#include "AppStateManager.h"

class ButtonManager {
public:
    ButtonManager(TemperatureController& tempController, AppStateManager& appStateManager);
    void setupButtons();
    void tick();

private:
    TemperatureController& tempController;
    AppStateManager& appStateManager;
    OneButton increaseButton;
    OneButton decreaseButton;
    OneButton buttonA;
    OneButton buttonB;

    static ButtonManager* instance;

    static void handleIncreaseClick();
    static void handleDecreaseClick();
    static void handleButtonAClick();
    static void handleButtonBClick();

    static void handleIncreaseLongPressStart();
    static void handleDecreaseLongPressStart();
    static void handleIncreaseLongPress();
    static void handleDecreaseLongPress();
    static void handleALongPressStart();

    void handleIncreaseButtonEvents();
    void handleDecreaseButtonEvents();
    void handleButtonAEvents();
    void handleButtonBEvents();

    static constexpr unsigned int INCREASE_BUTTON_PIN = 4;
    static constexpr unsigned int DECREASE_BUTTON_PIN = 7;
    static constexpr unsigned int BUTTON_A_PIN = 2;
    static constexpr unsigned int BUTTON_B_PIN = 3;
};