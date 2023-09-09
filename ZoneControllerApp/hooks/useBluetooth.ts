import { useState, useEffect, useMemo } from 'react';
import { BleManager, Device, Characteristic } from 'react-native-ble-plx';

export const CharacteristicUUIDs = {
    TEMPERATURE_CHARACTERISTIC_UUID: "fec680e1-bf30-47b7-b678-80de71b58d1c",
    WIFI_CHARACTERISTIC_UUID: "b8c9373a-bbfb-4ead-bc40-cf4f59d11364",
    SERVICE_UUID: "d11b21ac-d2cb-4022-922b-312323dffb9a"
} as const;

export type BluetoothDevice = {
    id: string;
    name: string | null;
    rssi: number | null;
    isConnectable: boolean | null;
};

export const useBluetooth = () => {
    const [devices, setDevices] = useState<BluetoothDevice[]>([]);
    const [isScanning, setIsScanning] = useState(false);
    
    // Ensure BleManager is instantiated only once.
    const bleManager = useMemo(() => new BleManager(), []);

    const addScannedDevice = (scannedDevice: any) => {
        if (scannedDevice && scannedDevice.name && scannedDevice.name.trim() !== "") {
            setDevices(prevDevices => {
                if (prevDevices.some(device => device.id === scannedDevice.id)) {
                    return prevDevices;
                }
                return [
                    ...prevDevices,
                    {
                        id: scannedDevice.id,
                        name: scannedDevice.name,
                        rssi: scannedDevice.rssi,
                        isConnectable: scannedDevice.isConnectable
                    }
                ];
            });
        }
    };

    const startScan = () => {
        setIsScanning(true);

        bleManager.startDeviceScan(null, null, (error, scannedDevice) => {
            if (error) {
                console.error("Error scanning devices:", error);
                return;
            }
            addScannedDevice(scannedDevice);
        });

        setTimeout(stopScan, 5000);
    };

    const stopScan = () => {
        bleManager.stopDeviceScan();
        setIsScanning(false);
    };

    const connectToDevice = async (deviceId: string): Promise<Device> => {
        try {
            const device = await bleManager.connectToDevice(deviceId);
            await device.discoverAllServicesAndCharacteristics();
            return device;
        } catch (error) {
            console.error("Error connecting to device:", error);
            throw error;
        }
    };

    const disconnectFromDevice = async (deviceId: string) => {
        try {
            await bleManager.cancelDeviceConnection(deviceId);
        } catch (error) {
            console.error("Error disconnecting from device:", error);
            throw error;
        }
    };

    const readCharacteristic = async (
        device: Device,
        serviceUUID: string,
        characteristicUUID: string
    ): Promise<Characteristic> => {
        try {
            return await device.readCharacteristicForService(serviceUUID, characteristicUUID);
        } catch (error) {
            console.error("Error reading characteristic:", error);
            throw error;
        }
    };

    const writeCharacteristic = async (
        device: Device,
        serviceUUID: string,
        characteristicUUID: string,
        value: string
    ): Promise<Characteristic> => {
        try {
            return await device.writeCharacteristicWithResponseForService(serviceUUID, characteristicUUID, value);
        } catch (error) {
            console.error("Error writing to characteristic:", error);
            throw error;
        }
    };

    return {
        devices,
        isScanning,
        startScan,
        stopScan,
        connectToDevice,
        disconnectFromDevice, // added the disconnect function
        readCharacteristic,
        writeCharacteristic
    };
};
