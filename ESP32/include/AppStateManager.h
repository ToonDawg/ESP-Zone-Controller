#pragma once

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

    AppStateManager(DisplayManager& displayManager, TemperatureController& tempController, BluetoothManager& btManager);
    
    void setAppState(AppState state);
    AppState getAppState() const;
    void display();
    void tick();
    void recordAdjustmentTime();

private:
    AppState currentState;
    DisplayManager& displayManager;
    TemperatureController& temperatureController;
    BluetoothManager& btManager;

    unsigned long lastAdjustmentTime;

    void displayCurrentTemperature();
    void displaySetTemperature();
    void displayOff();
    void displayConnecting();
    void manageBluetoothStatus();
    void handleStateTimeouts();
};