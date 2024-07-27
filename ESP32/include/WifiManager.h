#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "Settings.h"

enum class WiFiManagerState : uint8_t
{
    IDLE,
    SMARTCONFIG,
    CONNECTING,
    CONNECTED,
    CONNECTION_FAILED,
    DISCONNECTED
};

class WiFiManager
{
public:
    WiFiManager(Settings &settings);
    void begin();
    void processConnectionState();
    WiFiManagerState getCurrentConnectionState() const { return state; }
    const char *getLatestStatusMessage() const { return statusMessage; }
    bool isWiFiConnected() const { return state == WiFiManagerState::CONNECTED; }
    void disconnectAndClearCredentials();
    void restartSmartConfig();

private:
    static WiFiManager *instance;
    static constexpr unsigned long CONNECTION_TIMEOUT = 30000; // 30 seconds
    static constexpr int MAX_CONNECTION_ATTEMPTS = 3;
    WiFiManagerState state;
    Settings &settings;
    unsigned long connectionStartTime;
    uint8_t connectionAttempts;
    const char *statusMessage;

    void initiateSmartConfig();
    void connectUsingStoredCredentials();
    void handleConnectionAttempt();
    void updateStateAndNotify(WiFiManagerState newState, const char *message);

    static void handleWiFiStateChange(WiFiEvent_t event);
};

#endif // WIFI_MANAGER_H