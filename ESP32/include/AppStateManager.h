// AppStateManager.h
#pragma once

#include "DisplayManager.h"
#include "TemperatureController.h"
#include "Settings.h"
#include "OTAUpdater.h"
#include "MenuRouter.h"
#include "MenuRenderer.h"

class AppStateManager
{
public:
    enum class AppState
    {
        CURRENT_TEMPERATURE,
        SET_TEMPERATURE,
        OFF,
        MENU,
        UPDATING,
        CALIBRATE_TEMPERATURE
    };

    AppStateManager(DisplayManager& displayManager, TemperatureController& tempController, Settings& settings, OTAUpdater& updater, MenuRouter& menuRouter);

    void setAppState(AppState state);
    AppState getAppState() const;
    void display();
    void tick();
    enum class ButtonInput {
        UP,
        DOWN,
        SELECT,
        BACK
    };

    void handleInput(ButtonInput input);
    void resetSleepTimer();

private:
    AppState currentState;
    DisplayManager &displayManager;
    TemperatureController &temperatureController;
    Settings &settings;
    OTAUpdater &updater;
    MenuRouter menuRouter;
    MenuRenderer menuRenderer;

    unsigned long lastActivityTime;
    unsigned long lastSetTempAdjustmentTime;
    static const unsigned long SLEEP_TIMEOUT = 10000;

    void initializeMenus();
    void displayCurrentTemperature();
    void displaySetTemperature();
    void displayOff();
    void updateLatestVersionInSettings(const String &version);
    void displayMenu();
    void displayUpdating();
    void checkAutoSleep();
    bool shouldCheckForUpdates();
    void handleSetTemperatureTimeout();
    void displayCheckForUpdates();
    void recordAdjustmentTime();
    void displayUpdate();
    void displayTemperatureCalibration();

};
