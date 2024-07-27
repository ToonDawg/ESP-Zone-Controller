#include <WiFi.h>
#include "WiFiManager.h"

WiFiManager::WiFiManager(Settings& settings) 
    : state(WifiState::IDLE), status(WifiStatus::NOT_CONNECTED), settings(settings), connectionAttempts(0) {}

void WiFiManager::begin() {
    if (connectWithStoredCredentials()) {
        setStatus(WifiStatus::CONNECTING, "Connecting with stored credentials...");
    } else {
        startSmartConfig();
    }
}

void WiFiManager::update() {
    switch (state) {
        case WifiState::SMART_CONFIG:
            updateSmartConfig();
            break;
        case WifiState::CONNECTING:
            updateConnection();
            break;
        case WifiState::CONNECTED:
            if (WiFi.status() != WL_CONNECTED) {
                setStatus(WifiStatus::CONNECTION_LOST, "WiFi connection lost. Retrying...");
                connectWithStoredCredentials();
            }
            break;
        default:
            break;
    }
}

void WiFiManager::startSmartConfig() {
    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();
    state = WifiState::SMART_CONFIG;
    setStatus(WifiStatus::WAITING_FOR_SMARTCONFIG, "Waiting for SmartConfig...");
}

void WiFiManager::updateSmartConfig() {
    if (WiFi.smartConfigDone()) {
        setStatus(WifiStatus::SMARTCONFIG_RECEIVED, "SmartConfig credentials received!");
        String ssid = WiFi.SSID();
        String password = WiFi.psk();
        saveWifiCredentials(ssid, password);
        state = WifiState::CONNECTING;
        WiFi.begin(ssid.c_str(), password.c_str());
        connectionStartTime = millis();
        connectionAttempts = 1;
        setStatus(WifiStatus::CONNECTING, "Attempting to connect...");
    }
}

void WiFiManager::updateConnection() {
    if (WiFi.status() == WL_CONNECTED) {
        state = WifiState::CONNECTED;
        setStatus(WifiStatus::CONNECTED, "WiFi connected successfully!");
    } else if (millis() - connectionStartTime > CONNECTION_TIMEOUT) {
        if (connectionAttempts < MAX_CONNECTION_ATTEMPTS) {
            connectionAttempts++;
            WiFi.begin(settings.getWiFiSSID().c_str(), settings.getWiFiPassword().c_str());
            connectionStartTime = millis();
            setStatus(WifiStatus::CONNECTING, "Connecting... Attempt " + String(connectionAttempts));
        } else {
            state = WifiState::CONNECTION_FAILED;
            setStatus(WifiStatus::CONNECTION_ATTEMPT_FAILED, "Failed to connect");
            startSmartConfig();
        }
    }
}

bool WiFiManager::connectWithStoredCredentials() {
    String ssid = settings.getWiFiSSID();
    String password = settings.getWiFiPassword();
    
    if (ssid.length() > 0 && password.length() > 0) {
        WiFi.begin(ssid.c_str(), password.c_str());
        connectionStartTime = millis();
        state = WifiState::CONNECTING;
        connectionAttempts = 1;
        setStatus(WifiStatus::CONNECTING, "Connecting with stored credentials...");
        return true;
    }
    return false;
}

void WiFiManager::saveWifiCredentials(const String& ssid, const String& password) {
    settings.setWiFiSSID(ssid);
    settings.setWiFiPassword(password);
}

void WiFiManager::setStatus(WifiStatus newStatus, const String& message) {
    status = newStatus;
    statusMessage = message;
}

WifiStatus WiFiManager::getStatus() const {
    return status;
}

String WiFiManager::getStatusMessage() const {
    return statusMessage;
}

int WiFiManager::getConnectionAttempts() const {
    return connectionAttempts;
}