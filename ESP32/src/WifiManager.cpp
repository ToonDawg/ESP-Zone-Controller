#include "WiFiManager.h"

const char PROGMEM WiFiManager::SERVICE_NAME[] = "TempControl_PROV";
const char PROGMEM WiFiManager::POP[] = "abcd1234";

WiFiManager* WiFiManager::instance = nullptr;

WiFiManager::WiFiManager(Settings& settings) 
    : _settings(settings), _isProvisioned(false), _isConnected(false), _isProvisioning(false) {
    instance = this;
}

void WiFiManager::begin() {
    WiFi.onEvent(wifiEventCallback);
    _isProvisioned = checkStoredCredentials();
}

void WiFiManager::update() {
    if (_isProvisioned && !_isConnected) {
        connectToWiFi();
    }
    _isConnected = WiFi.isConnected();

    static unsigned long lastReconnectAttempt = 0;
    const unsigned long reconnectInterval = 30000;

    if (!_isConnected && millis() - lastReconnectAttempt > reconnectInterval) {
        Serial.println("Attempting to reconnect to WiFi...");
        connectToWiFi();
        lastReconnectAttempt = millis();
    }
}

void WiFiManager::startProvisioning() {
    if (!_isProvisioning) {
        Serial.println("Starting WiFi provisioning");
        WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, 
                                WIFI_PROV_SECURITY_1, POP, SERVICE_NAME);
        
        WiFi.onEvent([](arduino_event_t *event) {
            provisionEventCallback(event, instance);
        }, ARDUINO_EVENT_PROV_START);

        _isProvisioning = true;
    }
}

void WiFiManager::stopProvisioning() {
    if (_isProvisioning) {
        Serial.println("Stopping WiFi provisioning");
        WiFiProv.stopProvision();
        _isProvisioning = false;
    }
}

bool WiFiManager::connectToStoredNetwork() {
    if (checkStoredCredentials()) {
        connectToWiFi();
        return true;
    }
    return false;
}

void WiFiManager::wifiEventCallback(arduino_event_t *event) {
    if (instance) {
        switch (event->event_id) {
            case ARDUINO_EVENT_WIFI_STA_GOT_IP:
                instance->_isConnected = true;
                instance->updateWiFiDetails();
                Serial.println("Connected to WiFi. IP: " + WiFi.localIP().toString());
                break;
            case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
                instance->_isConnected = false;
                Serial.println("Disconnected from WiFi");
                break;
            default:
                break;
        }
    }
}

void WiFiManager::provisionEventCallback(arduino_event_t *event, WiFiManager *instance) {
    if (instance) {
        instance->handleProvisionEvent(event);
    }
}

void WiFiManager::handleProvisionEvent(arduino_event_t *event) {
    switch (event->event_id) {
        case ARDUINO_EVENT_PROV_CRED_RECV:
            _isProvisioned = true;
            Serial.println("Provisioning credentials received");
            break;
        case ARDUINO_EVENT_PROV_END:
            _isProvisioning = false;
            Serial.println("Provisioning ended");
            break;
        default:
            break;
    }
}

void WiFiManager::connectToWiFi() {
    Serial.println("Attempting to connect to WiFi");
    WiFi.begin();
}

void WiFiManager::updateWiFiDetails() {
    _settings.setWiFiSSID(WiFi.SSID());
    _settings.setWiFiPassword(WiFi.psk());
    Serial.println("WiFi details updated in settings");
}

bool WiFiManager::checkStoredCredentials() {
    String ssid = _settings.getWiFiSSID();
    String password = _settings.getWiFiPassword();
    
    if (ssid.length() > 0 && password.length() > 0) {
        Serial.println("Stored WiFi credentials found");
        WiFi.begin(ssid.c_str(), password.c_str());
        return true;
    }
    
    Serial.println("No stored WiFi credentials found");
    return false;
}
