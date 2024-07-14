// #pragma once

// #include <WiFi.h>
// #include <WiFiProv.h>

// class WiFiProvisionManager {
// public:
//     WiFiProvisionManager();
//     void begin();
//     void update();
//     bool isProvisioned() const { return _isProvisioned; }
//     bool isConnected() const { return _isConnected; }
//     void startProvisioning();
//     bool isProvisioning() const { return _isProvisioning; }

// private:
//     static void wifiEventCallback(arduino_event_t *event);
//     static void provisionEventCallback(arduino_event_t *event, WiFiProvisionManager *instance);
//     void handleProvisionEvent(arduino_event_t *event);
//     void connectToWiFi();

//     bool _isProvisioned;
//     bool _isConnected;
//     bool _isProvisioning;

//     static WiFiProvisionManager* instance;
// };