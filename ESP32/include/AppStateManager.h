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
        MOTOR_DIRECTION,
        TEMPERATURE_CALIBRATION,
        TEMPERATURE_UNIT,
        OTA_UPDATE
    };

    enum class TemperatureUnit {
        CELSIUS,
        FAHRENHEIT
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

private:
    AppState currentState;
    DisplayManager& displayManager;
    TemperatureController& temperatureController;
    Settings& settings;
    unsigned long lastAdjustmentTime;
    
    Menu settingsMenu;
    Menu motorDirectionMenu;
    Menu tempUnitMenu;
    Menu updatesMenu;

    void displayCurrentTemperature();
    void displaySetTemperature();
    void displayTemperatureCalibration();
    void displayOff();
    void displaySettings();
    void displayWiFiProvisioning();
    void displayMotorDirectionSetting();
    void displayTemperatureUnitSetting();
    void manageBluetoothStatus();
    void handleStateTimeouts();
};