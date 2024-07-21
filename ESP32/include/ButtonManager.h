#pragma once


#include <OneButton.h>
#include "TemperatureController.h"
#include "AppStateManager.h"
#include "OTAUpdater.h"

class ButtonManager {
public:
    ButtonManager(TemperatureController& tempController, AppStateManager& appStateManager, OTAUpdater& updater);
    void setupButtons();
    void tick();

private:
    TemperatureController& tempController;
    AppStateManager& appStateManager;
    OTAUpdater& updater;
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
    static void handleALongPressStart();
    static void handleBLongPressStart();
    static void handleDecreaseLongPressStart();
    static void handleIncreaseLongPress();
    static void handleDecreaseLongPress();

    void handleIncreaseInCurrentState();
    void handleDecreaseInCurrentState();
    void handleButtonAInCurrentState();
    void handleButtonBInCurrentState();
    void handleIncreaseLongPressInCurrentState();
    void handleDecreaseLongPressInCurrentState();

    static constexpr unsigned int INCREASE_BUTTON_PIN = 4;
    static constexpr unsigned int DECREASE_BUTTON_PIN = 7;
    static constexpr unsigned int BUTTON_A_PIN = 2;
    static constexpr unsigned int BUTTON_B_PIN = 3;
};