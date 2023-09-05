#ifndef BluetoothManager_h
#define BluetoothManager_h

#include <NimBLEDevice.h>

class BluetoothManager {
    NimBLECharacteristic* pWifiDetailsCharacteristic;

public:
    BluetoothManager();
    void init();
    bool isConnected();
    void startAdvertising();
    void stopAdvertising();
    void setupCharacteristic();
    static void onWrite(NimBLECharacteristic* pCharacteristic);
};

class TempControlBTCallbacks: public NimBLEServerCallbacks {
public:
    void onConnect(NimBLEServer* pServer);
    void onDisconnect(NimBLEServer* pServer);
};

#endif
