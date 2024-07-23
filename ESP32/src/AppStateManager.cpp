#include "AppStateManager.h"
#include <Arduino.h>
#include "AppStateManager.h"
#include "Settings.h"

AppStateManager::AppStateManager(DisplayManager& displayManager, TemperatureController& tempController, Settings& settings, OTAUpdater& updater, MenuRouter& menuRouter)
    : currentState(AppState::CURRENT_TEMPERATURE),
      displayManager(displayManager),
      temperatureController(tempController),
      settings(settings),
      updater(updater),
      menuRenderer(displayManager),
      lastActivityTime(0),
      lastSetTempAdjustmentTime(0)
{
    initializeMenus();
}

void AppStateManager::initializeMenus()
{
    menuRouter.createMenu("main", "Main Menu");
    menuRouter.addMenuItem("main", MenuItem("Settings", ActionType::OPEN_SUBMENU, "settings"));
    menuRouter.addMenuItem("main", MenuItem("Turn Off", ActionType::CHANGE_APP_STATE, [this]() { setAppState(AppState::OFF); }));

    menuRouter.createMenu("settings", "Settings", "main");
    menuRouter.addMenuItem("settings", MenuItem("Temp. Calibration", ActionType::OPEN_SUBMENU, "temp_cal"));
    menuRouter.addMenuItem("settings", MenuItem("Motor Direction", ActionType::OPEN_SUBMENU, "motor_dir"));
    menuRouter.addMenuItem("settings", MenuItem("Temp. Unit", ActionType::OPEN_SUBMENU, "temp_unit"));
    menuRouter.addMenuItem("settings", MenuItem("Auto Sleep", ActionType::OPEN_SUBMENU, "auto_sleep"));
    menuRouter.addMenuItem("settings", MenuItem("About", ActionType::OPEN_SUBMENU, "about"));



    menuRouter.navigateToMenu("main");
}

void AppStateManager::setAppState(AppState state)
{
    currentState = state;
    lastSetTempAdjustmentTime = millis();
}

AppStateManager::AppState AppStateManager::getAppState() const
{
    return currentState;
}

void AppStateManager::display()
{
    switch (currentState)
    {
    case AppState::CURRENT_TEMPERATURE:
        displayCurrentTemperature();
        break;
    case AppState::SET_TEMPERATURE:
        displaySetTemperature();
        break;
    case AppState::OFF:
        displayOff();
        break;
    case AppState::MENU:
        displayMenu();
        break;
    case AppState::UPDATING:
        displayUpdating();
        break;
    }
    displayManager.render();
}

void AppStateManager::displayMenu()
{
    menuRenderer.renderMenu(*menuRouter.getCurrentMenu());
}

void AppStateManager::tick()
{
    handleSetTemperatureTimeout();
    checkAutoSleep();
}

void AppStateManager::recordAdjustmentTime()
{
    lastSetTempAdjustmentTime = millis();
}


void AppStateManager::displayCurrentTemperature()
{
    tImage tempIcon = settings.getTemperatureUnit() ? celciusIcon : fahrenheitIcon;
    displayManager.displayTemperature(temperatureController.getStatus().currentTemperature, tempIcon);
    displayManager.displayIconBottomLeft(temperatureController.getModeIcon());
    displayManager.displayIconBottomRight(temperatureController.getMotorStateIcon());
}

void AppStateManager::displayCheckForUpdates()
{
    displayManager.displayMenuTitle("Check for updates");

    if (shouldCheckForUpdates())
    {
        displayManager.showLoaderWithText("Checking...");
        String latestVersion = updater.getLatestVersion();
        settings.setLastUpdateCheck(millis());
        updateLatestVersionInSettings(latestVersion);

        if (latestVersion.isEmpty())
        {
            displayManager.showLoaderWithText("No update available");
        }
        else
        {
            displayManager.showLoaderWithText("Update available: " + latestVersion);
        }
    }
    else
    {
        displayManager.displayLabelAndContent("Latest version", settings.getLatestAvailableVersion(), 16);
        displayManager.displayLabelAndContent("Current version", settings.getCurrentSoftwareVersion(), 40);
    }
}


bool AppStateManager::shouldCheckForUpdates()
{
    unsigned long lastCheck = settings.getLastUpdateCheck();
    unsigned long currentTime = millis();

    return (currentTime - lastCheck) > 600000 || lastCheck == 0;
}

void AppStateManager::displaySetTemperature()
{
    tImage tempIcon = settings.getTemperatureUnit() ? celciusIcon : fahrenheitIcon;
    displayManager.displayTemperature(temperatureController.getStatus().setTemperature, tempIcon);
    displayManager.displayBottomCenterText("Set Temp");
}

void AppStateManager::displayTemperatureCalibration()
{
    tImage tempIcon = settings.getTemperatureUnit() ? celciusIcon : fahrenheitIcon;
    displayManager.displayTemperature(settings.getTemperatureCalibration(), tempIcon);
    displayManager.displayBottomCenterText("Temp. Calibration");
}

void AppStateManager::displayOff()
{
    displayManager.displayOff();
}

void AppStateManager::handleSetTemperatureTimeout()
{
    unsigned long currentTime = millis();

    if (currentState == AppState::SET_TEMPERATURE && currentTime - lastSetTempAdjustmentTime > 3000)
    {
        setAppState(AppState::CURRENT_TEMPERATURE);
    }
}

void AppStateManager::displayUpdate()
{
    String latestVersion = settings.getLatestAvailableVersion();
    String currentVersion = settings.getCurrentSoftwareVersion();

    if (shouldCheckForUpdates() || latestVersion.isEmpty())
    {
        displayManager.showLoaderWithText("Checking for updates...");
        latestVersion = updater.getLatestVersion();
        settings.setLastUpdateCheck(millis());
        updateLatestVersionInSettings(latestVersion);
    }

    if (latestVersion.isEmpty())
    {
        displayManager.displayCenteredWrappedText("No update available");
    }
    else if (updater.isNewerVersion(currentVersion, latestVersion))
    {
        displayManager.displayCenteredWrappedText(latestVersion + " Ready. Confirm update?");
    }
    else
    {
        displayManager.displayCenteredWrappedText("You have the latest version");
    }
}


void AppStateManager::displayUpdating()
{
    displayManager.displayCenteredWrappedText("Updating...");
}



void AppStateManager::checkAutoSleep()
{
    if (currentState != AppState::OFF && millis() - lastActivityTime > SLEEP_TIMEOUT)
    {
        setAppState(AppState::OFF);
    }
}

void AppStateManager::resetSleepTimer()
{
    lastActivityTime = millis();
}

void AppStateManager::updateLatestVersionInSettings(const String &version)
{
    if (!version.isEmpty())
    {
        settings.setLatestAvailableVersion(version);
    }
}

// AppStateManager.cpp

void AppStateManager::handleInput(ButtonInput input)
{
    resetSleepTimer();

    switch (currentState)
    {
    case AppState::CURRENT_TEMPERATURE:
        if (input == ButtonInput::SELECT) // Enter menu
            setAppState(AppState::MENU);
        // Handle other inputs for CURRENT_TEMPERATURE state
        break;
    case AppState::SET_TEMPERATURE:
        // Handle inputs for SET_TEMPERATURE state
        if (input == ButtonInput::UP) // Increase temperature
            temperatureController.adjustSetTemperature(0.5);
        else if (input == ButtonInput::DOWN) // Decrease temperature
            temperatureController.adjustSetTemperature(-0.5);
        else if (input == ButtonInput::SELECT) // Confirm and return to CURRENT_TEMPERATURE
            setAppState(AppState::CURRENT_TEMPERATURE);
        break;
    case AppState::MENU:
        if (input == ButtonInput::UP) // Up
            menuRouter.navigateUp();
        else if (input == ButtonInput::DOWN) // Down
            menuRouter.navigateDown();
        else if (input == ButtonInput::SELECT) // Select
            menuRouter.selectCurrentItem();
        else if (input == ButtonInput::BACK) // Back
            menuRouter.navigateToParentMenu();
        break;
    case AppState::OFF:
        if (input != ButtonInput::BACK) // Any input turns the device on
            setAppState(AppState::CURRENT_TEMPERATURE);
        break;
    case AppState::UPDATING:
        // Normally, we don't handle inputs during update
        break;
    }

    // After handling input, update the display
    display();
}
