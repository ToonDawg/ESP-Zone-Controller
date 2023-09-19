#include "BluetoothManager.h"

#define TEMPERATURE_CHARACTERISTIC_UUID "fec680e1-bf30-47b7-b678-80de71b58d1c"
#define WIFI_CHARACTERISTIC_UUID "b8c9373a-bbfb-4ead-bc40-cf4f59d11364"
#define SERVICE_UUID "d11b21ac-d2cb-4022-922b-312323dffb9a"

NimBLEServer* pServer = NULL;
BluetoothManager::BluetoothManager(TemperatureController& controller) : tempController(controller) {}


void BluetoothManager::init() {
    NimBLEDevice::init("Zone Controller");
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
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->start();
}

void BluetoothManager::stopAdvertising() {
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->stop();
}

class CharacteristicCallbacks : public NimBLECharacteristicCallbacks {
private:
    BluetoothManager& btManager;

public:
    CharacteristicCallbacks(BluetoothManager& manager) : btManager(manager) {}

    void onWrite(NimBLECharacteristic* pCharacteristic) override {
        btManager.onWrite(pCharacteristic);
    }
    
    void onRead(NimBLECharacteristic* pCharacteristic) override {
        btManager.onRead(pCharacteristic);
    }
};

void BluetoothManager::setupCharacteristic() {
    NimBLEService* pService = pServer->createService(SERVICE_UUID);

    pWifiDetailsCharacteristic = pService->createCharacteristic(
        WIFI_CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::READ |
        NIMBLE_PROPERTY::WRITE
    );
    pWifiDetailsCharacteristic->setCallbacks(new CharacteristicCallbacks(*this));
    pTemperatureCharacteristic = pService->createCharacteristic(
        TEMPERATURE_CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::READ
    );
    pTemperatureCharacteristic->setCallbacks(new CharacteristicCallbacks(*this));

    pService->start();
}

void BluetoothManager::onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();

    if (pCharacteristic->getUUID().toString() == WIFI_CHARACTERISTIC_UUID) {
        Serial.println("Received Wi-Fi details:");
        Serial.println(value.c_str());
        // Parsing WiFi details here
    }

    if (pCharacteristic->getUUID().toString() == TEMPERATURE_CHARACTERISTIC_UUID && value.length() == sizeof(float)) {
        float setTemp;
        memcpy(&setTemp, value.c_str(), sizeof(setTemp));
        tempController.adjustTemperature(setTemp);  // Directly accessing tempController
        Serial.print("Setting temperature to: ");
        Serial.println(setTemp);
    }
}

void BluetoothManager::onRead(NimBLECharacteristic* pCharacteristic) {
    if (pCharacteristic->getUUID().toString() == TEMPERATURE_CHARACTERISTIC_UUID) {
        float currentTemp = tempController.getCurrentTemperature();
        float setTemp = tempController.getSetTemperature();
        uint8_t modeAndMotorState = 0;

        if (strcmp(tempController.getMode(), "Heat") == 0) {
            modeAndMotorState |= 1; // Set the least significant bit
        }
        if (strcmp(tempController.getMotorState(), "Open") == 0) {
            modeAndMotorState |= 2; // Set the second least significant bit
        }

        uint8_t data[2*sizeof(float) + 1] = {0};

        memcpy(data, &currentTemp, sizeof(float));
        memcpy(data + sizeof(float), &setTemp, sizeof(float));
        data[2*sizeof(float)] = modeAndMotorState;

        pCharacteristic->setValue(data, sizeof(data));
    }
}



