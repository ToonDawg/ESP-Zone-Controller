#ifndef APP_STATE_MANAGER_H
#define APP_STATE_MANAGER_H

#include "DisplayManager.h"
#include "TemperatureController.h"
#include "BluetoothManager.h"

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
    BluetoothManager& btManager; 

    bool advertisingStarted;
    unsigned long lastAdjustmentTime;

    void displayCurrentTemperature();
    void displaySetTemperature();
    void displayOff();
    void displayConnecting();
    void manageBluetoothStatus();
    void handleStateTimeouts();

public:
    AppStateManager(DisplayManager& displayManager, TemperatureController& tempController, BluetoothManager& btMgr);
    void setAppState(AppState state);
    const AppState& getAppState() const;
    void display();
    void tick();
    void recordAdjustmentTime();
};

#endif // APP_STATE_MANAGER_H
