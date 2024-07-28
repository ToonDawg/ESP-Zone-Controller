#include "WifiManager.h"

WiFiManager::WiFiManager(Settings &settings)
    : state(State::IDLE), settings(settings), connectionAttempts(0)
{
}

void WiFiManager::begin()
{
    if (!settings.getWiFiSSID().isEmpty() && !settings.getWiFiPassword().isEmpty())
    {
        connectToStoredNetwork();
    }
}

void WiFiManager::update()
{
    handleConnection();
    
    if (state == State::SMARTCONFIG && WiFi.smartConfigDone())
    {
        setState(State::CONNECTED, "SmartConfig successful");
        settings.setWiFiSSID(WiFi.SSID());
        settings.setWiFiPassword(WiFi.psk());
    }
}

void WiFiManager::setState(State newState, const char *message)
{
    state = newState;
    statusMessage = message;
    Serial.println(message);
}

void WiFiManager::connectToStoredNetwork()
{
    WiFi.begin(settings.getWiFiSSID().c_str(), settings.getWiFiPassword().c_str());
    connectionStartTime = millis();
    connectionAttempts = 1;
    setState(State::CONNECTING, "Connecting to stored network...");
}

void WiFiManager::handleConnection()
{
    if (state == State::CONNECTING)
    {
        if (isConnected())
        {
            setState(State::CONNECTED, "WiFi connected successfully!");
            Serial.printf("Connected to %s with IP address: %s\n", getSSID().c_str(), getIPAddress().c_str());
        }
        else if (millis() - connectionStartTime > CONNECTION_TIMEOUT)
        {
            if (connectionAttempts < MAX_CONNECTION_ATTEMPTS)
            {
                connectionAttempts++;
                WiFi.begin(settings.getWiFiSSID().c_str(), settings.getWiFiPassword().c_str());
                connectionStartTime = millis();
                char attemptMsg[32];
                snprintf(attemptMsg, sizeof(attemptMsg), "Connecting... Attempt %d", connectionAttempts);
                setState(State::CONNECTING, attemptMsg);
            }
            else
            {
                startSmartConfig();
            }
        }
    }
    else if (state == State::CONNECTED && !isConnected())
    {
        setState(State::CONNECTING, "WiFi disconnected. Reconnecting...");
        connectToStoredNetwork();
    }
}

void WiFiManager::startSmartConfig()
{
    setState(State::SMARTCONFIG, "Starting SmartConfig...");
    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();
}

void WiFiManager::disconnect()
{
    WiFi.disconnect(true, true);
    settings.setWiFiSSID("");
    settings.setWiFiPassword("");
    setState(State::IDLE, "WiFi disconnected");
}