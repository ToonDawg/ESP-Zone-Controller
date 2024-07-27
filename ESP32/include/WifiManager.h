#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "Settings.h"

enum class WifiState {
    IDLE,
    SMART_CONFIG,
    CONNECTING,
    CONNECTED,
    CONNECTION_FAILED
};

enum class WifiStatus {
    NOT_CONNECTED,
    WAITING_FOR_SMARTCONFIG,
    SMARTCONFIG_RECEIVED,
    CONNECTING,
    CONNECTION_ATTEMPT_FAILED,
    CONNECTED,
    CONNECTION_LOST
};

class WiFiManager {
public:
    WiFiManager(Settings& settings);
    void begin();
    void update();
    WifiStatus getStatus() const;
    String getStatusMessage() const;
    int getConnectionAttempts() const;

private:
    static const unsigned long CONNECTION_TIMEOUT = 30000; // 30 seconds
    static const int MAX_CONNECTION_ATTEMPTS = 3;

    WifiState state;
    WifiStatus status;
    Settings& settings;
    unsigned long connectionStartTime;
    int connectionAttempts;
    String statusMessage;

    void startSmartConfig();
    void updateSmartConfig();
    void updateConnection();
    bool connectWithStoredCredentials();
    void saveWifiCredentials(const String& ssid, const String& password);
    void setStatus(WifiStatus newStatus, const String& message);
};

#endif // WIFI_MANAGER_H