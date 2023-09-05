#include "BluetoothManager.h"

NimBLEServer* pServer = NULL;
BluetoothManager::BluetoothManager() {}

void BluetoothManager::init() {
    NimBLEDevice::init("ESP32_NimBLE");
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new TempControlBTCallbacks());
}

bool BluetoothManager::isConnected() {
    return pServer->getConnectedCount() > 0;
}

void TempControlBTCallbacks::onConnect(NimBLEServer* pServer) {
    Serial.println("Client connected");
}

void TempControlBTCallbacks::onDisconnect(NimBLEServer* pServer) {
    Serial.println("Client disconnected");
}

void BluetoothManager::startAdvertising() {
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->start();
}

void BluetoothManager::stopAdvertising() {
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->stop();
}

class CharacteristicCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic) {
        BluetoothManager::onWrite(pCharacteristic);
    }
};

void BluetoothManager::setupCharacteristic() {
    // Create a service for the Bluetooth device
    NimBLEService* pService = pServer->createService("YOUR_SERVICE_UUID");

    // Create a characteristic for the service
    pWifiDetailsCharacteristic = pService->createCharacteristic(
        "YOUR_CHARACTERISTIC_UUID",
        NIMBLE_PROPERTY::READ |
        NIMBLE_PROPERTY::WRITE
    );

    // Set a callback for when the characteristic is written to
    pWifiDetailsCharacteristic->setCallbacks(new CharacteristicCallbacks());

    // Start the service
    pService->start();
}

// Callback when the characteristic is written to
void BluetoothManager::onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0) {
        Serial.println("Received Wi-Fi details:");
        Serial.println(value.c_str());

        // Here, you can parse the received value and use it to connect to Wi-Fi
    }
}

