#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

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
    OneButton increaseTemperatureButton;
    OneButton decreaseTemperatureButton;
    OneButton toggleModeButton;
    OneButton toggleDisplayButton;

    static ButtonManager* instance;

    static void handleIncreaseTemperatureClick();
    static void handleDecreaseTemperatureClick();
    static void handleToggleModeClick();
    static void handleToggleModeLongPressStart();
    static void handleToggleDisplayClick();
    static void handleIncreaseTemperatureDuringLongPress();
    static void handleDecreaseTemperatureDuringLongPress();

    static constexpr unsigned int INCREASE_TEMPERATURE_BUTTON_PIN = 4;
    static constexpr unsigned int DECREASE_TEMPERATURE_BUTTON_PIN = 7;
    static constexpr unsigned int TOGGLE_MODE_BUTTON_PIN = 3;
    static constexpr unsigned int TOGGLE_DISPLAY_BUTTON_PIN = 2;
};

#endif // BUTTON_MANAGER_H
