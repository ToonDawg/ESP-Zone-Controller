// AppStateManager.h
#pragma once
#include "DisplayManager.h"
#include "TemperatureController.h"
#include "Menu.h"
#include "Settings.h"

class AppStateManager {
public:
    enum class AppState {
        CURRENT_TEMPERATURE,
        SET_TEMPERATURE,
        OFF,
        SETTINGS,
        WIFI_PROVISIONING,
        MOTOR_DIRECTION
    };

    AppStateManager(DisplayManager& displayManager, TemperatureController& tempController, Settings& settings);
    
    void setAppState(AppState state);
    AppState getAppState() const;
    void display();
    void tick();
    void recordAdjustmentTime();
    void menuNavigateUp();
    void menuNavigateDown();
    void selectMenuItem();
    void beginSettings();
    void saveSettings();
    void loadSettings();

private:
    AppState currentState;
    DisplayManager& displayManager;
    TemperatureController& temperatureController;
    Settings& settings;
    unsigned long lastAdjustmentTime;
    
    Menu settingsMenu;
    Menu motorDirectionMenu;

    void displayCurrentTemperature();
    void displaySetTemperature();
    void displayOff();
    void displaySettings();
    void displayWiFiProvisioning();
    void displayMotorDirectionSetting();
    void manageBluetoothStatus();
    void handleStateTimeouts();
};