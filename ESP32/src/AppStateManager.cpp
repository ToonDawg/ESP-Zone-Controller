#include "AppStateManager.h"
#include <Arduino.h>
#include "AppStateManager.h"
#include "Settings.h"

const std::vector<std::pair<String, unsigned long>> AppStateManager::AUTO_SLEEP_OPTIONS = {
    {"Off", 0},
    {"10s", 10000},
    {"30s", 30000},
    {"1min", 60000},
    {"2min", 120000},
    {"5min", 300000}};

AppStateManager::AppStateManager(DisplayManager &displayManager, TemperatureController &tempController, Settings &settings, OTAUpdater &updater)
    : currentState(AppState::CURRENT_TEMPERATURE),
      displayManager(displayManager),
      temperatureController(tempController),
      settings(settings),
      updater(updater),
      lastSetTempAdjustmentTime(0),
      lastActivityTime(0),
      settingsMenu("Settings", {"Temp. Calibration", "Motor Direction", "Temp. Unit", "Auto Sleep", "About"}),
      motorDirectionMenu("Motor Direction", {"Normal", "Reversed"}),
      tempUnitMenu("Temp. Unit", {"Celsius", "Fahrenheit"}),
      aboutMenu("About", {"Check for Updates", "Device Details", "Update"}),
      autoSleepMenu("Auto Sleep", {})
{
    for (const auto &option : AUTO_SLEEP_OPTIONS)
    {
        autoSleepMenu.addItem(option.first);
    }
    autoSleepMenu.setSelectedIndex(settings.getAutoSleepOption());
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
    case AppState::SETTINGS:
        displaySettings();
        break;
    case AppState::ABOUT:
        displayWiFiProvisioning();
        break;
    case AppState::MOTOR_DIRECTION:
        displayMotorDirectionSetting();
        break;
    case AppState::TEMPERATURE_CALIBRATION:
        displayTemperatureCalibration();
        break;
    case AppState::TEMPERATURE_UNIT:
        displayTemperatureUnitSetting();
        break;
    case AppState::CHECK_FOR_UPDATES:
        displayCheckForUpdates();
        break;
    case AppState::UPDATE:
        displayUpdate();
        break;
    case AppState::DEVICE_DETAILS:
        displayDeviceDetails();
        break;
    case AppState::UPDATING:
        displayUpdating();
        break;
    case AppState::AUTO_SLEEP:
        displayAutoSleepSetting();
        break;
    }
    displayManager.render();
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

void AppStateManager::menuNavigateUp()
{
    if (currentState == AppState::SETTINGS)
    {
        settingsMenu.navigateUp();
    }
    else if (currentState == AppState::MOTOR_DIRECTION)
    {
        motorDirectionMenu.navigateUp();
    }
    else if (currentState == AppState::TEMPERATURE_UNIT)
    {
        tempUnitMenu.navigateUp();
    }
    else if (currentState == AppState::ABOUT)
    {
        aboutMenu.navigateUp();
    }
    else if (currentState == AppState::AUTO_SLEEP)
    {
        autoSleepMenu.navigateUp();
    }
    display();
}

void AppStateManager::menuNavigateDown()
{
    if (currentState == AppState::SETTINGS)
    {
        settingsMenu.navigateDown();
    }
    else if (currentState == AppState::MOTOR_DIRECTION)
    {
        motorDirectionMenu.navigateDown();
    }
    else if (currentState == AppState::TEMPERATURE_UNIT)
    {
        tempUnitMenu.navigateDown();
    }
    else if (currentState == AppState::ABOUT)
    {
        aboutMenu.navigateDown();
    }
    else if (currentState == AppState::AUTO_SLEEP)
    {
        autoSleepMenu.navigateDown();
    }
    display();
}

void AppStateManager::selectMenuItem()
{
    switch (currentState)
    {
    case AppState::SETTINGS:
        switch (settingsMenu.getSelectedIndex())
        {
        case 0:
            setAppState(AppState::TEMPERATURE_CALIBRATION);
            break;
        case 1:
            setAppState(AppState::MOTOR_DIRECTION);
            break;
        case 2:
            setAppState(AppState::TEMPERATURE_UNIT);
            break;
        case 3:
            setAppState(AppState::AUTO_SLEEP);
            break;
        case 4:
            setAppState(AppState::ABOUT);
            break;
        }
        break;
    case AppState::MOTOR_DIRECTION:
        switch (motorDirectionMenu.getSelectedIndex())
        {
        case 0:
            settings.setMotorDirection(MotorDirection::Normal);
            setAppState(AppState::CURRENT_TEMPERATURE);
            break;
        case 1:
            settings.setMotorDirection(MotorDirection::Reversed);
            setAppState(AppState::CURRENT_TEMPERATURE);
            break;
        default:
            setAppState(AppState::CURRENT_TEMPERATURE);
            break;
        }
        break;

    case AppState::TEMPERATURE_UNIT:
        switch (tempUnitMenu.getSelectedIndex())
        {
        case 0:
            settings.setTemperatureUnit(true);
            setAppState(AppState::CURRENT_TEMPERATURE);
            break;
        case 1:
            settings.setTemperatureUnit(false);
            setAppState(AppState::CURRENT_TEMPERATURE);
            break;
        default:
            setAppState(AppState::CURRENT_TEMPERATURE);
            break;
        }
        break;

    case AppState::ABOUT:
        switch (aboutMenu.getSelectedIndex())
        {
        case 0:
            setAppState(AppState::CHECK_FOR_UPDATES);
            break;
        case 1:
            setAppState(AppState::DEVICE_DETAILS);
            break;
        case 2:
            setAppState(AppState::UPDATE);
            break;
        default:
            setAppState(AppState::CURRENT_TEMPERATURE);
            break;
        }
        break;

    case AppState::AUTO_SLEEP:
        int selectedIndex = autoSleepMenu.getSelectedIndex();
        if (selectedIndex >= 0 && selectedIndex < AUTO_SLEEP_OPTIONS.size())
        {
            settings.setAutoSleepOption(selectedIndex);
            Serial.print("Auto Sleep set to: ");
            Serial.println(AUTO_SLEEP_OPTIONS[selectedIndex].first);
        }
        setAppState(AppState::SETTINGS);
        break;
    }
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

void AppStateManager::updateLatestVersionInSettings(const String &version)
{
    if (!version.isEmpty())
    {
        settings.setLatestAvailableVersion(version);
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

void AppStateManager::displaySettings()
{
    displayManager.displaySettingsMenu(settingsMenu);
}

void AppStateManager::displayMotorDirectionSetting()
{
    MotorDirection currentDirection = settings.getMotorDirection();
    motorDirectionMenu.setActiveIndex(currentDirection == MotorDirection::Normal ? 0 : 1);
    displayManager.displaySettingsMenu(motorDirectionMenu);
}

void AppStateManager::displayTemperatureUnitSetting()
{
    bool tempUnit = settings.getTemperatureUnit();
    tempUnitMenu.setActiveIndex(tempUnit ? 0 : 1);
    displayManager.displaySettingsMenu(tempUnitMenu);
}

void AppStateManager::displayWiFiProvisioning()
{
    displayManager.displaySettingsMenu(aboutMenu);
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

void AppStateManager::displayDeviceDetails()
{
    displayManager.displayMenuTitle("Device Details");
}

void AppStateManager::displayUpdating()
{
    displayManager.displayCenteredWrappedText("Updating...");
}

void AppStateManager::displayAutoSleepSetting()
{
    displayManager.displaySettingsMenu(autoSleepMenu);
}

void AppStateManager::checkAutoSleep()
{
    int optionIndex = settings.getAutoSleepOption();
    if (optionIndex > 0 && optionIndex < AUTO_SLEEP_OPTIONS.size())
    {
        unsigned long sleepTimeout = AUTO_SLEEP_OPTIONS[optionIndex].second;
        if (currentState != AppState::OFF && millis() - lastActivityTime > sleepTimeout)
        {
            setAppState(AppState::OFF);
        }
    }
}

void AppStateManager::resetSleepTimer()
{
    lastActivityTime = millis();
}

void AppStateManager::selectAutoSleepOption()
{
    int selectedIndex = autoSleepMenu.getSelectedIndex();
    settings.setAutoSleepOption(selectedIndex);
    setAppState(AppState::SETTINGS);
}