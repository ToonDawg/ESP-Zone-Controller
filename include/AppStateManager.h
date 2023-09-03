#ifndef APP_STATE_MANAGER_H
#define APP_STATE_MANAGER_H

#include "DisplayManager.h"
#include "TemperatureController.h"

class AppStateManager {
public:
    enum class AppState {
      CURRENT_TEMPERATURE,
      SET_TEMPERATURE,
      OFF,
      CONNECTING
    };

private:
    AppState currentState;
    DisplayManager& displayManager;
    TemperatureController& temperatureController;
    unsigned long lastAdjustmentTime = 0;

public:
    AppStateManager(DisplayManager& displayManager, TemperatureController& tempController);
    void setAppState(AppState state);
    const AppState& getAppState() const;
    void display();
    void tick();
    void recordAdjustmentTime();
};

#endif // APP_STATE_MANAGER_H
