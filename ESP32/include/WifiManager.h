#pragma once

#include <WiFi.h>
#include <WiFiProv.h>
#include "Settings.h"

class WiFiManager {
public:
    WiFiManager(Settings& settings);
    void begin();
    void update();
    bool isProvisioned() const { return _isProvisioned; }
    bool isConnected() const { return _isConnected; }
    void startProvisioning();
    void stopProvisioning();
    bool isProvisioning() const { return _isProvisioning; }
    bool connectToStoredNetwork();

private:
    static void wifiEventCallback(arduino_event_t *event);
    static void provisionEventCallback(arduino_event_t *event, WiFiManager *instance);
    void handleProvisionEvent(arduino_event_t *event);
    void connectToWiFi();
    void updateWiFiDetails();
    bool checkStoredCredentials();

    Settings& _settings;
    bool _isProvisioned;
    bool _isConnected;
    bool _isProvisioning;

    static WiFiManager* instance;

    static const char PROGMEM SERVICE_NAME[];
    static const char PROGMEM POP[];
};