import React, { useState, useEffect } from 'react';
import { View, FlatList, StyleSheet } from 'react-native';
import { TextInput, Button, List, Surface, IconButton, ActivityIndicator } from 'react-native-paper';
import { useDevices, DeviceDetail } from '../contexts/DeviceContext'
import { useBluetooth, BluetoothDevice, CharacteristicUUIDs } from '../hooks/useBluetooth'
import { Buffer } from 'buffer'

const transformToDeviceDetail = (device: BluetoothDevice): DeviceDetail => {
  return {
    id: device.id,
    name: device.name || "Unknown Device",
    mode: 'Cool',
    display: false,
    setTemperature: 20,
    currentTemperature: 20,
    airflow: 'Open',
    isEditing: false,
  };
};

function decodeBluetoothData(base64Value: string) {
  const decodedData = Buffer.from(base64Value, 'base64');
  const dataView = new DataView(decodedData.buffer);

  const currentTemperature = parseFloat(dataView.getFloat32(0, true).toFixed(1));
  const setTemperature = parseFloat(dataView.getFloat32(4, true).toFixed(1));

  const modeAndMotorState = decodedData[8]; // Ninth byte
  const mode = (modeAndMotorState & 1) === 1; // True for "Heat", false for "Cool"
  const airflow = (modeAndMotorState & 2) === 2; // True for "Open", false for "Closed"

  return {
    currentTemperature: currentTemperature,
    setTemperature: setTemperature,
    mode: mode,
    airflow: airflow,
  };
}

const AddDeviceScreen: React.FC<{ navigation: any }> = ({ navigation }) => {
  const [availableDevices, setAvailableDevices] = useState<BluetoothDevice[]>([]);
  const [selectedDevice, setSelectedDevice] = useState<string | null>(null);
  const { addDevice } = useDevices();
  const [loading, setLoading] = useState(false);

  const {
    devices,
    isScanning,
    startScan,
    stopScan,
    connectToDevice,
    readCharacteristic,
    writeCharacteristic
  } = useBluetooth();

  useEffect(() => {
    if (isScanning) {
      setLoading(true);
    } else {
      setLoading(false);
    }
    setAvailableDevices(devices);
  }, [isScanning, devices]);

  const scanForDevices = () => {
    startScan();
  };

  const handleDeviceInteraction = async (item: BluetoothDevice) => {
    const deviceDetail = transformToDeviceDetail(item);
    try {
      let connectedDevice = await connectToDevice(item.id);

      let attempts = 0;
      const MAX_ATTEMPTS = 10;
      while (!connectedDevice.isConnected() && attempts < MAX_ATTEMPTS) {
        await new Promise(resolve => setTimeout(resolve, 100));
        connectedDevice = await connectToDevice(item.id);
        attempts++;
      }

      if (!connectedDevice.isConnected()) {
        throw new Error("Max connection attempts reached");
      }

      const temperatureChar = await readCharacteristic(connectedDevice, CharacteristicUUIDs.SERVICE_UUID, CharacteristicUUIDs.TEMPERATURE_CHARACTERISTIC_UUID);
      const decodedData = decodeBluetoothData(temperatureChar.value || '');

      deviceDetail.currentTemperature = decodedData.currentTemperature;
      deviceDetail.setTemperature = decodedData.setTemperature;
      deviceDetail.mode = decodedData.mode ? 'Heat' : 'Cool';
      deviceDetail.airflow = decodedData.airflow ? 'Open' : 'Closed';

      const wifiCredentials = "SSID:PASSWORD";
      await writeCharacteristic(connectedDevice, CharacteristicUUIDs.SERVICE_UUID, CharacteristicUUIDs.WIFI_CHARACTERISTIC_UUID, wifiCredentials);

      addDevice(deviceDetail);
      navigation.goBack();
    } catch (error) {
      console.error("Error interacting with the device:", error);
      // Handle errors, show error messages, etc.
    }
  };


  return (
    <Surface style={styles.container}>
      <Button mode="outlined" onPress={scanForDevices} style={styles.scanButton}>
        Scan for Devices
      </Button>
      <List.Section>
        <List.Subheader>Available Bluetooth Devices:</List.Subheader>
        <FlatList
          data={availableDevices}
          renderItem={({ item }) => (
            <List.Item
              title={item.name}
              onPress={() => handleDeviceInteraction(item)}
              style={selectedDevice === item.id ? styles.selectedDevice : {}}
            />
          )}
          keyExtractor={item => item.id}
        />
      </List.Section>
      {loading && <ActivityIndicator size="large" />}
    </Surface>
  );
};


const styles = StyleSheet.create({
  container: {
    flex: 1,
    padding: 20,
    backgroundColor: '#111',
  },
  textInput: {
    marginBottom: 20,
  },
  scanButton: {
    marginBottom: 20,
    backgroundColor: '#444',
  },
  selectedDevice: {
    backgroundColor: '#555',
  },
  listItemRight: {
    flexDirection: 'row',
    alignItems: 'center',
  }
});

export default AddDeviceScreen;
