#include "WiFiManager.h"

WiFiManager* WiFiManager::instance = nullptr;

WiFiManager::WiFiManager(Settings& settings) 
    : state(WiFiManagerState::IDLE), settings(settings), connectionAttempts(0) {
    instance = this;
}

void WiFiManager::begin() {
    WiFi.onEvent(handleWiFiStateChange);
    
    if (settings.getWiFiSSID()[0] != '\0' && settings.getWiFiPassword()[0] != '\0') {
        connectUsingStoredCredentials();
    } else {
        initiateSmartConfig();
    }
}

void WiFiManager::processConnectionState() {
    switch (state) {
        case WiFiManagerState::CONNECTING:
            handleConnectionAttempt();
            break;
        case WiFiManagerState::CONNECTED:
            if (WiFi.status() != WL_CONNECTED) {
                updateStateAndNotify(WiFiManagerState::CONNECTION_FAILED, "WiFi lost. Retrying...");
                connectUsingStoredCredentials();
            }
            break;
        case WiFiManagerState::SMARTCONFIG:
            if (WiFi.smartConfigDone()) {
                updateStateAndNotify(WiFiManagerState::CONNECTING, "SmartConfig received. Connecting...");
                connectionStartTime = millis();
                connectionAttempts = 1;
            }
            break;
        default:
            break;
    }
}

void WiFiManager::initiateSmartConfig() {
    updateStateAndNotify(WiFiManagerState::SMARTCONFIG, "Starting SmartConfig...");
    WiFi.beginSmartConfig();
}

void WiFiManager::connectUsingStoredCredentials() {
    updateStateAndNotify(WiFiManagerState::CONNECTING, "Connecting to stored network...");
    WiFi.begin(settings.getWiFiSSID(), settings.getWiFiPassword());
    connectionStartTime = millis();
    connectionAttempts = 1;
}

void WiFiManager::handleConnectionAttempt() {
    if (WiFi.status() == WL_CONNECTED) {
        updateStateAndNotify(WiFiManagerState::CONNECTED, "WiFi connected successfully!");
        settings.setWiFiSSID(WiFi.SSID().c_str());
        settings.setWiFiPassword(WiFi.psk().c_str());
    } else if (millis() - connectionStartTime > CONNECTION_TIMEOUT) {
        if (connectionAttempts < MAX_CONNECTION_ATTEMPTS) {
            connectionAttempts++;
            WiFi.begin(settings.getWiFiSSID(), settings.getWiFiPassword());
            connectionStartTime = millis();
            char attemptMsg[32];
            snprintf_P(attemptMsg, sizeof(attemptMsg), PSTR("Connecting... Attempt %d"), connectionAttempts);
            updateStateAndNotify(WiFiManagerState::CONNECTING, attemptMsg);
        } else {
            updateStateAndNotify(WiFiManagerState::CONNECTION_FAILED, "Failed to connect. Starting SmartConfig...");
            initiateSmartConfig();
        }
    }
}

void WiFiManager::updateStateAndNotify(WiFiManagerState newState, const char* message) {
    state = newState;
    statusMessage = message;
    Serial.println(message);
}

void WiFiManager::handleWiFiStateChange(WiFiEvent_t event) {
    if (!instance) {
        Serial.println(F("Error: WiFiManager instance not set"));
        return;
    }

    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.print(F("Connected IP address: "));
            Serial.println(WiFi.localIP());
            instance->updateStateAndNotify(WiFiManagerState::CONNECTED, "WiFi connected successfully!");
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println(F("Disconnected from WiFi network"));
            instance->updateStateAndNotify(WiFiManagerState::CONNECTING, "Reconnecting...");
            break;
        case ARDUINO_EVENT_SC_SEND_ACK_DONE:
            Serial.println(F("SmartConfig: Ack sent"));
            break;
        default:
            break;
    }
}

void WiFiManager::disconnectAndClearCredentials() {
    WiFi.disconnect(false, true);
    
    settings.setWiFiSSID("");
    settings.setWiFiPassword("");
    
    updateStateAndNotify(WiFiManagerState::DISCONNECTED, "WiFi disconnected");
}

void WiFiManager::restartSmartConfig() {
    disconnectAndClearCredentials();
    initiateSmartConfig();
}

String WiFiManager::getSSID() const {
    return WiFi.SSID();
}

String WiFiManager::getIPAddress() const {
    return WiFi.localIP().toString();
}