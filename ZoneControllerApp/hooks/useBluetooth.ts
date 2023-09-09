import { useState, useEffect } from 'react';
import { BleManager, Device, Characteristic } from 'react-native-ble-plx';

export type BluetoothDevice = {
    id: string;
    name: string | null;
    rssi: number | null;
    isConnectable: boolean | null;
};

export const useBluetooth = () => {
    const [devices, setDevices] = useState<BluetoothDevice[]>([]);
    const [isScanning, setIsScanning] = useState(false);
    const bleManager = new BleManager();

    const startScan = () => {
        setIsScanning(true);

        bleManager.startDeviceScan(null, null, (error, scannedDevice) => {
            if (error) {
                console.log(error);
                return;
            }

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

        });

        setTimeout(() => {
            bleManager.stopDeviceScan();
            setIsScanning(false);
        }, 5000);
    };

    const stopScan = () => {
        bleManager.stopDeviceScan();
        setIsScanning(false);
    };

    const connectToDevice = async (deviceId: string): Promise<Device> => {
        const device = await bleManager.connectToDevice(deviceId);
        await device.discoverAllServicesAndCharacteristics();
        return device;
    };

    const readCharacteristic = async (
        device: Device,
        serviceUUID: string,
        characteristicUUID: string
    ): Promise<Characteristic> => {
        return await device.readCharacteristicForService(serviceUUID, characteristicUUID);
    };

    const writeCharacteristic = async (
        device: Device,
        serviceUUID: string,
        characteristicUUID: string,
        value: string
    ): Promise<Characteristic> => {
        return await device.writeCharacteristicWithResponseForService(serviceUUID, characteristicUUID, value);
    };

    return {
        devices,
        isScanning,
        startScan,
        stopScan,
        connectToDevice,
        readCharacteristic,
        writeCharacteristic
    };
};
