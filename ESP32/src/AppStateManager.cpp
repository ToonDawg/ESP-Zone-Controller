#include "AppStateManager.h"
#include <Arduino.h>
#include "AppStateManager.h"
#include "Settings.h"

AppStateManager::AppStateManager(DisplayManager &displayManager, TemperatureController &tempController, Settings &settings, OTAUpdater &updater, MenuRouter &menuRouter)
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
    menuRouter.createMenu("settings", "Settings");
    menuRouter.addMenuItem("settings", MenuItem("Temp. Calibration", ActionType::CHANGE_APP_STATE, [this]()
                                                { setAppState(AppState::CALIBRATE_TEMPERATURE); }));
    menuRouter.addMenuItem("settings", MenuItem("Motor Direction", ActionType::OPEN_SUBMENU, "motor_dir"));
    menuRouter.addMenuItem("motor_dir", MenuItem("Normal", ActionType::EXECUTE_CALLBACK, [this]()
                                                 { settings.setMotorDirection(MotorDirection::Normal); }));
    menuRouter.addMenuItem("motor_dir", MenuItem("Reversed", ActionType::EXECUTE_CALLBACK, [this]()
                                                 { settings.setMotorDirection(MotorDirection::Reversed); }));

    menuRouter.addMenuItem("settings", MenuItem("Temp. Unit", ActionType::OPEN_SUBMENU, "temp_unit"));
    menuRouter.addMenuItem("temp_unit", MenuItem("Celsius", ActionType::EXECUTE_CALLBACK, [this]()
                                                 { settings.setTemperatureUnit(TemperatureUnit::Celsius); }));
    menuRouter.addMenuItem("temp_unit", MenuItem("Fahrenheit", ActionType::EXECUTE_CALLBACK, [this]()
                                                 { settings.setTemperatureUnit(TemperatureUnit::Fahrenheit); }));
    menuRouter.addMenuItem("settings", MenuItem("Auto Sleep", ActionType::OPEN_SUBMENU, "auto_sleep"));
    menuRouter.addMenuItem("auto_sleep", MenuItem("Off", ActionType::EXECUTE_CALLBACK, [this]()
                                                  { settings.setAutoSleepOption(0); }));
    menuRouter.addMenuItem("auto_sleep", MenuItem("10s", ActionType::EXECUTE_CALLBACK, [this]()
                                                  { settings.setAutoSleepOption(10000); }));
    menuRouter.addMenuItem("auto_sleep", MenuItem("30s", ActionType::EXECUTE_CALLBACK, [this]()
                                                  { settings.setAutoSleepOption(30000); }));
    menuRouter.addMenuItem("auto_sleep", MenuItem("1m", ActionType::EXECUTE_CALLBACK, [this]()
                                                  { settings.setAutoSleepOption(60000); }));
    menuRouter.addMenuItem("auto_sleep", MenuItem("5m", ActionType::EXECUTE_CALLBACK, [this]()
                                                  { settings.setAutoSleepOption(300000); }));
    menuRouter.addMenuItem("auto_sleep", MenuItem("30m", ActionType::EXECUTE_CALLBACK, [this]()
                                                  { settings.setAutoSleepOption(1800000); }));

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
    case AppState::CALIBRATE_TEMPERATURE:
        displayTemperatureCalibration();
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
    tImage tempIcon = settings.getTemperatureUnit() == TemperatureUnit::Celsius ? celciusIcon : fahrenheitIcon;
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
    tImage tempIcon = settings.getTemperatureUnit() == TemperatureUnit::Celsius ? celciusIcon : fahrenheitIcon;
    displayManager.displayTemperature(temperatureController.getStatus().setTemperature, tempIcon);
    displayManager.displayBottomCenterText("Set Temp");
}

void AppStateManager::displayTemperatureCalibration()
{
    tImage tempIcon = settings.getTemperatureUnit() == TemperatureUnit::Celsius ? celciusIcon : fahrenheitIcon;
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

void AppStateManager::handleInput(ButtonInput input)
{
    resetSleepTimer();

    switch (currentState)
    {
    case AppState::CURRENT_TEMPERATURE:
        if (input == ButtonInput::SELECT)
            setAppState(AppState::MENU);
        break;
    case AppState::SET_TEMPERATURE:
        switch (input)
        {
        case ButtonInput::UP:
            temperatureController.adjustSetTemperature(0.5);
            break;
        case ButtonInput::DOWN:
            temperatureController.adjustSetTemperature(-0.5);
            break;
        default:
            setAppState(AppState::CURRENT_TEMPERATURE);
            break;
        }
        break;
    case AppState::MENU:
        switch (input)
        {
        case ButtonInput::UP:
            menuRouter.navigateUp();
            break;
        case ButtonInput::DOWN:
            menuRouter.navigateDown();
            break;
        case ButtonInput::SELECT:
            menuRouter.selectCurrentItem();
            break;
        case ButtonInput::BACK:
            if (menuRouter.isAtRootMenu())
            {
                setAppState(AppState::CURRENT_TEMPERATURE);
            }
            menuRouter.navigateToParentMenu();
            break;
        }
    case AppState::OFF:
        setAppState(AppState::CURRENT_TEMPERATURE);
        break;
    case AppState::UPDATING:
        break;
    }

    display();
}
