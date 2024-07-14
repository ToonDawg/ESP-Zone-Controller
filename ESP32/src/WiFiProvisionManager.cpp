// #include "WiFiProvisionManager.h"
// #include <esp_wifi.h>

// static const char PROGMEM service_name[] = "TempControl_PROV";
// static const char PROGMEM pop[] = "abcd1234";

// WiFiProvisionManager* WiFiProvisionManager::instance = nullptr;

// WiFiProvisionManager::WiFiProvisionManager() 
//     : _isProvisioned(false), _isConnected(false), _isProvisioning(false) {
//     instance = this;
// }

// void WiFiProvisionManager::begin() {
//     WiFi.onEvent(wifiEventCallback);

//     if (WiFi.isConnected()) {
//         _isProvisioned = true;
//         _isConnected = true;
//     }
// }

// void WiFiProvisionManager::update() {
//     if (_isProvisioned && !_isConnected) {
//         connectToWiFi();
//     }
//     _isConnected = WiFi.isConnected();
// }

// void WiFiProvisionManager::startProvisioning() {
//     if (!_isProvisioning) {
//         WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, 
//                                 WIFI_PROV_SECURITY_1, pop, service_name);
        
//         WiFi.onEvent([](arduino_event_t *event) {
//             provisionEventCallback(event, instance);
//         }, ARDUINO_EVENT_PROV_START);

//         _isProvisioning = true;
//     }
// }

// void WiFiProvisionManager::wifiEventCallback(arduino_event_t *event) {
//     if (instance) {
//         switch (event->event_id) {
//             case ARDUINO_EVENT_WIFI_STA_GOT_IP:
//                 instance->_isConnected = true;
//                 break;
//             case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
//                 instance->_isConnected = false;
//                 break;
//             default:
//                 break;
//         }
//     }
// }

// void WiFiProvisionManager::provisionEventCallback(arduino_event_t *event, WiFiProvisionManager *instance) {
//     if (instance) {
//         instance->handleProvisionEvent(event);
//     }
// }

// void WiFiProvisionManager::handleProvisionEvent(arduino_event_t *event) {
//     switch (event->event_id) {
//         case ARDUINO_EVENT_PROV_CRED_RECV:
//             _isProvisioned = true;
//             break;
//         case ARDUINO_EVENT_PROV_END:
//             _isProvisioning = false;
//             break;
//         default:
//             break;
//     }
// }

// void WiFiProvisionManager::connectToWiFi() {
//     WiFi.begin();
// }