#pragma once

#include <NimBLEDevice.h>
#include "TemperatureController.h"

class BluetoothManager : public NimBLEServerCallbacks {
public:
    BluetoothManager(TemperatureController& controller);
    
    void init();
    void update();
    bool isConnected() const;

private:
    TemperatureController& tempController;
    NimBLEServer* pServer;
    NimBLEService* pService;
    NimBLECharacteristic* pWifiDetailsCharacteristic;
    NimBLECharacteristic* pTemperatureCharacteristic;
    bool deviceConnected;

    void setupService();
    void setupCharacteristics();
    void updateCharacteristics();

    // NimBLEServerCallbacks
    void onConnect(NimBLEServer* pServer) override;
    void onDisconnect(NimBLEServer* pServer) override;

    static void handleWriteWifiDetails(NimBLECharacteristic* pCharacteristic);
    static void handleReadTemperature(NimBLECharacteristic* pCharacteristic);
};