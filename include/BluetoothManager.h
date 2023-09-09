#ifndef BluetoothManager_h
#define BluetoothManager_h

#include <NimBLEDevice.h>
#include "TemperatureController.h"

class BluetoothManager {
private:
    NimBLECharacteristic* pWifiDetailsCharacteristic = nullptr;
    NimBLECharacteristic* pTemperatureCharacteristic = nullptr;
    TemperatureController& tempController;

public:
    BluetoothManager(TemperatureController& controller);
    void init();
    bool isConnected();
    void startAdvertising();
    void stopAdvertising();
    void setupCharacteristic();
    void onWrite(NimBLECharacteristic* pCharacteristic);
    void onRead(NimBLECharacteristic* pCharacteristic);
};

class TempControlBTCallbacks: public NimBLEServerCallbacks {
public:
    void onConnect(NimBLEServer* pServer);
    void onDisconnect(NimBLEServer* pServer);
};

#endif
