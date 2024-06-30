#include "BluetoothManager.h"
#include <Arduino.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define WIFI_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define TEMP_CHARACTERISTIC_UUID "cba1d466-344c-4be3-ab3f-189f80dd7518"

BluetoothManager::BluetoothManager(TemperatureController& controller)
    : tempController(controller), deviceConnected(false) {}

void BluetoothManager::init() {
    NimBLEDevice::init("TempControl");
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(this);
    
    setupService();
    setupCharacteristics();
    
    NimBLEAdvertising* pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
}

void BluetoothManager::update() {
    if (deviceConnected) {
        updateCharacteristics();
    }
}

bool BluetoothManager::isConnected() const {
    return deviceConnected;
}

void BluetoothManager::setupService() {
    pService = pServer->createService(SERVICE_UUID);
}

void BluetoothManager::setupCharacteristics() {
    pWifiDetailsCharacteristic = pService->createCharacteristic(
        WIFI_CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::WRITE
    );
    pWifiDetailsCharacteristic->setCallbacks(new NimBLECharacteristicCallbacks());

    pTemperatureCharacteristic = pService->createCharacteristic(
        TEMP_CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    pTemperatureCharacteristic->setCallbacks(new NimBLECharacteristicCallbacks());

    pService->start();
}

void BluetoothManager::updateCharacteristics() {
    TemperatureController::Status status = tempController.getStatus();
    String tempData = String(status.currentTemperature, 2) + "," + 
                      String(status.setTemperature, 2) + "," +
                      String(static_cast<int>(status.mode)) + "," +
                      String(static_cast<int>(status.motorState));
    pTemperatureCharacteristic->setValue(tempData);
    pTemperatureCharacteristic->notify();
}

void BluetoothManager::onConnect(NimBLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Device connected");
}

void BluetoothManager::onDisconnect(NimBLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Device disconnected");
    pServer->startAdvertising();
}

void BluetoothManager::handleWriteWifiDetails(NimBLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0) {
        // Process WiFi details
        // This is where you would handle the WiFi credentials
        // For example, splitting the string into SSID and password
        Serial.println("Received WiFi details");
    }
}

void BluetoothManager::handleReadTemperature(NimBLECharacteristic* pCharacteristic) {
    // This method is called when a client reads the temperature characteristic
    // The value is already set in updateCharacteristics(), so we don't need to do anything here
}