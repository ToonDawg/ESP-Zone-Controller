#include "AppStateManager.h"
#include <Arduino.h>
#include "AppStateManager.h"
#include "Settings.h"

AppStateManager::AppStateManager(DisplayManager &displayManager, TemperatureController &tempController, Settings &settings, OTAUpdater &updater, MenuRouter &menuRouter, WiFiManager &wifiManager)
    : currentState(AppState::CURRENT_TEMPERATURE),
      displayManager(displayManager),
      temperatureController(tempController),
      settings(settings),
      updater(updater),
      menuRenderer(displayManager),
      wifiManager(wifiManager),
      lastActivityTime(0),
      lastSetTempAdjustmentTime(0),
      lastUpdateCheckTime(0)
{
    initializeMenus();
}

void AppStateManager::initializeMenus()
{
    menuRouter.createMenu("settings", "Settings");
    menuRouter.addMenuItem("settings", MenuItem("Temp. Calibration", ActionType::CHANGE_APP_STATE, [this]()
                                                { setAppState(AppState::CALIBRATE_TEMPERATURE); }));
    menuRouter.addMenuItem("settings", MenuItem("Motor Direction", ActionType::OPEN_SUBMENU, "motor_dir"));
    menuRouter.createMenu("motor_dir", "Motor Direction");
    menuRouter.addMenuItem("motor_dir", MenuItem("Normal", ActionType::SELECT_OPTION, [this]()
                                                 { settings.setMotorDirection(MotorDirection::Normal); }));
    menuRouter.addMenuItem("motor_dir", MenuItem("Reversed", ActionType::SELECT_OPTION, [this]()
                                                 { settings.setMotorDirection(MotorDirection::Reversed); }));
    menuRouter.addMenuItem("settings", MenuItem("Temp. Unit", ActionType::OPEN_SUBMENU, "temp_unit"));
    menuRouter.createMenu("temp_unit", "Temperature Unit");
    menuRouter.addMenuItem("temp_unit", MenuItem("Celsius", ActionType::SELECT_OPTION, [this]()
                                                 { settings.setTemperatureUnit(TemperatureUnit::Celsius); }));
    menuRouter.addMenuItem("temp_unit", MenuItem("Fahrenheit", ActionType::SELECT_OPTION, [this]()
                                                 { settings.setTemperatureUnit(TemperatureUnit::Fahrenheit); }));
    menuRouter.addMenuItem("settings", MenuItem("Auto Sleep", ActionType::OPEN_SUBMENU, "auto_sleep"));
    menuRouter.createMenu("auto_sleep", "Auto Sleep");
    menuRouter.addMenuItem("auto_sleep", MenuItem("Off", ActionType::SELECT_OPTION, [this]()
                                                  { settings.setAutoSleepOption(0); }));
    menuRouter.addMenuItem("auto_sleep", MenuItem("10s", ActionType::SELECT_OPTION, [this]()
                                                  { settings.setAutoSleepOption(10000); }));
    menuRouter.addMenuItem("auto_sleep", MenuItem("30s", ActionType::SELECT_OPTION, [this]()
                                                  { settings.setAutoSleepOption(30000); }));
    menuRouter.addMenuItem("auto_sleep", MenuItem("1m", ActionType::SELECT_OPTION, [this]()
                                                  { settings.setAutoSleepOption(60000); }));
    menuRouter.addMenuItem("auto_sleep", MenuItem("5m", ActionType::SELECT_OPTION, [this]()
                                                  { settings.setAutoSleepOption(300000); }));
    menuRouter.addMenuItem("auto_sleep", MenuItem("30m", ActionType::SELECT_OPTION, [this]()
                                                  { settings.setAutoSleepOption(1800000); }));
    menuRouter.addMenuItem("settings", MenuItem("About", ActionType::OPEN_SUBMENU, "about"));
    menuRouter.createMenu("about", "About");
    menuRouter.addMenuItem("about", MenuItem("Check for updates", ActionType::CHANGE_APP_STATE, [this]()
                                             { setAppState(AppState::CHECK_FOR_UPDATES); }));
    menuRouter.addMenuItem("about", MenuItem("Update", ActionType::CHANGE_APP_STATE, [this]()
                                             { setAppState(AppState::UPDATE); }));
    menuRouter.addMenuItem("about", MenuItem("WiFi", ActionType::OPEN_SUBMENU, "wifi"));
    menuRouter.addMenuItem("about", MenuItem("Device Details", ActionType::CHANGE_APP_STATE, [this]()
                                             { settings.printAllSettings(); }));

    menuRouter.createMenu("wifi", "Wifi Settings");
    menuRouter.addMenuItem("wifi", MenuItem("Connect", ActionType::CHANGE_APP_STATE, [this]()
                                            { setAppState(AppState::WIFI_PROVISIONING); }));
    menuRouter.addMenuItem("wifi", MenuItem("Disconnect", ActionType::EXECUTE_CALLBACK, [this]()
                                            { wifiManager.disconnectAndClearCredentials(); }));
    menuRouter.addMenuItem("wifi", MenuItem("Details", ActionType::OPEN_SUBMENU, "wifi_details"));
    menuRouter.createMenu("wifi_details", "WiFi Details");
    menuRouter.addMenuItem("wifi_details", MenuItem(wifiManager.getIPAddress(), ActionType::DISPLAY_VALUE, [this]() {}));
    menuRouter.addMenuItem("wifi_details", MenuItem(WiFi.macAddress(), ActionType::DISPLAY_VALUE, [this]() {}));
    menuRouter.addMenuItem("wifi_details", MenuItem(wifiManager.getSSID(), ActionType::DISPLAY_VALUE, [this]() {}));

    menuRouter.navigateToMenu("main");
}

void AppStateManager::setAppState(AppState state)
{
    currentState = state;
}

AppStateManager::AppState AppStateManager::getAppState() const
{
    return currentState;
}

void AppStateManager::display()
{
    displayManager.clearDisplay();
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
    case AppState::CHECK_FOR_UPDATES:
        displayCheckForUpdates();
        break;
    case AppState::UPDATE:
        displayUpdate();
        break;
    case AppState::WIFI_PROVISIONING:
        displayWifiProvisioning();
        break;
    }
    displayManager.render();
}

void AppStateManager::displayMenu()
{
    String menuTitle = menuRouter.getCurrentMenu()->getTitle();
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

    if (wifiManager.isWiFiConnected())
    {
        displayManager.displayIconBottomMiddle(wifiIcon);
    }
}

void AppStateManager::displayCheckForUpdates()
{
    displayManager.displayMenuTitle("Check for updates");
    if (shouldCheckForUpdates())
    {
        displayManager.showLoaderWithText("Checking...");
        String latestVersion = updater.getLatestVersion();
        lastUpdateCheckTime = millis();
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
    unsigned long currentTime = millis();

    return (currentTime - lastUpdateCheckTime) > 30000 || lastUpdateCheckTime == 0;
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
    displayManager.clearDisplay();
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
        lastUpdateCheckTime = millis();
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
    unsigned long autoSleepOption = settings.getAutoSleepOption();
    unsigned long currentTime = millis();
    unsigned long timeSinceLastActivity = currentTime - lastActivityTime;

    if (currentState != AppState::OFF && autoSleepOption > 0 && timeSinceLastActivity > autoSleepOption)
    {
        Serial.println(F("Auto sleep activated"));
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
    Serial.print(F("Current state: "));
    Serial.print(static_cast<int>(currentState));
    Serial.print(F(", input: "));
    Serial.println(static_cast<int>(input));

    switch (currentState)
    {
    case AppState::CURRENT_TEMPERATURE:
        switch (input)
        {
        case ButtonInput::UP:
        case ButtonInput::DOWN:
            lastSetTempAdjustmentTime = millis();
            setAppState(AppState::SET_TEMPERATURE);
            break;
        case ButtonInput::SELECT:
            temperatureController.toggleMode();
            break;
        case ButtonInput::BACK:
            setAppState(AppState::OFF);
            break;
        }
        break;
    case AppState::SET_TEMPERATURE:
        switch (input)
        {
        case ButtonInput::UP:
            lastSetTempAdjustmentTime = millis();
            temperatureController.adjustSetTemperature(0.5);
            break;
        case ButtonInput::DOWN:
            lastSetTempAdjustmentTime = millis();
            temperatureController.adjustSetTemperature(-0.5);
            break;
        case ButtonInput::SELECT:
        case ButtonInput::BACK:
            setAppState(AppState::CURRENT_TEMPERATURE);
            break;
        }
        break;
    case AppState::CALIBRATE_TEMPERATURE:
        switch (input)
        {
        case ButtonInput::UP:
            temperatureController.adjustCalibrationTemperature(0.5);
            break;
        case ButtonInput::DOWN:
            temperatureController.adjustCalibrationTemperature(-0.5);
            break;
        case ButtonInput::SELECT:
        case ButtonInput::BACK:
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
            else
            {
                menuRouter.navigateToParentMenu();
            }
            break;
        }
        break;
    case AppState::OFF:
        setAppState(AppState::CURRENT_TEMPERATURE);
        break;
    case AppState::CHECK_FOR_UPDATES:
        switch (input)
        {
        case ButtonInput::BACK:
            setAppState(AppState::MENU);
            break;
        }
        break;
    case AppState::UPDATE:
        switch (input)
        {
        case ButtonInput::SELECT:
            updater.performUpdate();
            break;
        case ButtonInput::BACK:
            setAppState(AppState::MENU);
            break;
        }
        break;

    case AppState::WIFI_PROVISIONING:
        switch (input)
        {
        case ButtonInput::BACK:
            setAppState(AppState::MENU);
            break;
        }
        break;
    }

    display();
}

void AppStateManager::displayWifiProvisioning()
{
    WiFiManagerState currentState = wifiManager.getCurrentConnectionState();
    const char *latestStatus = wifiManager.getLatestStatusMessage();

    switch (currentState)
    {
    case WiFiManagerState::IDLE:
        wifiManager.initiateSmartConfig();
        wifiManager.updateStateAndNotify(WiFiManagerState::SMARTCONFIG, "Initialising...");
        break;
    case WiFiManagerState::CONNECTED:
        if (WiFi.status() != WL_CONNECTED)
        {
            wifiManager.updateStateAndNotify(WiFiManagerState::CONNECTION_FAILED, "WiFi lost. Retrying...");
            wifiManager.connectUsingStoredCredentials();
        }
        break;
    case WiFiManagerState::SMARTCONFIG:
        displayManager.displayCenteredWrappedText("Scanning for APP...");
        if (WiFi.smartConfigDone())
        {
            wifiManager.updateStateAndNotify(WiFiManagerState::CONNECTING, "Details received. Connecting...");
            wifiManager.setConnectionStartTime(millis());
            wifiManager.setConnectionAttempts(1);
        }
        break;
    case WiFiManagerState::CONNECTING:
        displayManager.showLoaderWithText(latestStatus);
        wifiManager.handleConnectionAttempt();
        break;
    case WiFiManagerState::CONNECTION_FAILED:
    case WiFiManagerState::DISCONNECTED:
        displayManager.displayCenteredWrappedText("Disconnected");
        wifiManager.restartSmartConfig();
        break;
    }
}