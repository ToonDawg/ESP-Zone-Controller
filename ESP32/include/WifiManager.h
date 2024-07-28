#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "Settings.h"

class WiFiManager
{
public:
    enum class State : uint8_t
    {
        IDLE,
        CONNECTING,
        CONNECTED,
        SMARTCONFIG
    };

    WiFiManager(Settings &settings);
    void begin();
    void update();
    State getState() const { return state; }
    const char *getStatusMessage() const { return statusMessage; }
    bool isConnected() const { return WiFi.status() == WL_CONNECTED; }
    void disconnect();
    void startSmartConfig();
    String getSSID() const { return WiFi.SSID(); }
    String getIPAddress() const { return WiFi.localIP().toString(); }

private:
    static constexpr unsigned long CONNECTION_TIMEOUT = 20000;
    static constexpr int MAX_CONNECTION_ATTEMPTS = 3;

    State state;
    Settings &settings;
    unsigned long connectionStartTime;
    uint8_t connectionAttempts;
    const char *statusMessage;

    void setState(State newState, const char *message);
    void connectToStoredNetwork();
    void handleConnection();
};

#endif // WIFI_MANAGER_H
