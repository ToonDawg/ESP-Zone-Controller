// AppStateManager.h
#pragma once
#include "DisplayManager.h"
#include "TemperatureController.h"
#include "Menu.h"
#include "Settings.h"
#include "OTAUpdater.h"

class AppStateManager
{
public:
    enum class AppState
    {
        CURRENT_TEMPERATURE,
        SET_TEMPERATURE,
        OFF,
        SETTINGS,
        WIFI_PROVISIONING,
        AUTO_SLEEP,
        MOTOR_DIRECTION,
        TEMPERATURE_CALIBRATION,
        TEMPERATURE_UNIT,
        ABOUT,
        CHECK_FOR_UPDATES,
        UPDATE,
        DEVICE_DETAILS,
        UPDATING
    };

    enum class TemperatureUnit
    {
        CELSIUS,
        FAHRENHEIT
    };

    AppStateManager(DisplayManager &displayManager, TemperatureController &tempController, Settings &settings, OTAUpdater &updater);

    void setAppState(AppState state);
    AppState getAppState() const;
    void display();
    void tick();
    void recordAdjustmentTime();
    void menuNavigateUp();
    void menuNavigateDown();
    void selectMenuItem();
    void checkAutoSleep();
    void resetSleepTimer();
    void selectAutoSleepOption();

private:
    AppState currentState;
    DisplayManager &displayManager;
    TemperatureController &temperatureController;
    Settings &settings;
    unsigned long lastSetTempAdjustmentTime;
    unsigned long lastActivityTime;
    static const unsigned long SLEEP_TIMEOUT = 10000;
    static const std::vector<std::pair<String, unsigned long>> AUTO_SLEEP_OPTIONS;

    Menu settingsMenu;
    Menu motorDirectionMenu;
    Menu tempUnitMenu;
    Menu aboutMenu;
    Menu autoSleepMenu;

    OTAUpdater &updater;

    void displayCurrentTemperature();
    void displaySetTemperature();
    void displayTemperatureCalibration();
    void displayOff();
    void displaySettings();
    void displayWiFiProvisioning();
    void displayMotorDirectionSetting();
    void displayTemperatureUnitSetting();
    void displayCheckForUpdates();
    void displayUpdate();
    void displayDeviceDetails();
    void displayAutoSleepSetting();
    void updateLatestVersionInSettings(const String &version);
    void handleSetTemperatureTimeout();
    void displayUpdating();
    bool shouldCheckForUpdates();
};