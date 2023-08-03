#ifndef APP_STATE_MANAGER_H
#define APP_STATE_MANAGER_H

#include "DisplayManager.h"

class AppStateManager {
public:
    enum class AppState {
      CURRENT_TEMPERATURE,
      SET_TEMPERATURE,
    };

private:
    AppState currentState;
    DisplayManager& displayManager;

public:
    AppStateManager(DisplayManager& displayManager);
    void setAppState(AppState state);
    void display(float temperature);
};

#endif // APP_STATE_MANAGER_H
