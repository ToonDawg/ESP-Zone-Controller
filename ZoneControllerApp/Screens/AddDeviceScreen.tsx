import React, { useState, useEffect } from 'react';
import { View, FlatList, StyleSheet } from 'react-native';
import { TextInput, Button, List, Surface, IconButton, ActivityIndicator } from 'react-native-paper';
import { useDevices, DeviceDetail } from '../contexts/DeviceContext'
import { useBluetooth, BluetoothDevice } from '../hooks/useBluetooth'

const transformToDeviceDetail = (device: BluetoothDevice): DeviceDetail => {
  return {
    id: device.id,
    name: device.name || "Unknown Device",  // Default to "Unknown Device" if name is null
    mode: false,  // Default value
    display: false,  // Default value
    setTemperature: 20,  // Default value
    currentTemperature: 20,  // Default value
    airflow: false,  // Default value
    isEditing: false,  // Default value
  };
};


const AddDeviceScreen: React.FC<{ navigation: any }> = ({ navigation }) => {
  const [availableDevices, setAvailableDevices] = useState<BluetoothDevice[]>([]);
  const [selectedDevice, setSelectedDevice] = useState<string | null>(null);
  const { addDevice } = useDevices();
  const [loading, setLoading] = useState(false);

  const { 
    devices, 
    isScanning, 
    startScan, 
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
      const connectedDevice = await connectToDevice(item.id);

      // // Assuming you have the UUIDs for your service and characteristics
      // const temperatureChar = await readCharacteristic(connectedDevice, "YOUR_SERVICE_UUID", "YOUR_TEMPERATURE_CHARACTERISTIC_UUID");
      // deviceDetail.currentTemperature = parseFloat(temperatureChar.value!); // Convert and assign

      // // Now, send WiFi credentials
      // const wifiCredentials = "SSID:PASSWORD";  // Modify this according to your app's logic
      // await writeCharacteristic(connectedDevice, "YOUR_SERVICE_UUID", "YOUR_WIFI_CHARACTERISTIC_UUID", wifiCredentials);

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
