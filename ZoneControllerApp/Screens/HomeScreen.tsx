import React, { useState } from 'react';
import { View, StyleSheet, TouchableOpacity, Text } from 'react-native';
import { FlatGrid } from 'react-native-super-grid';
import { Card, IconButton, TextInput, Portal, Dialog, Button, FAB } from 'react-native-paper';
import { RootStackParamList } from '../App';
import { StackNavigationProp } from '@react-navigation/stack';
import {  useDevices } from '../contexts/DeviceContext';
import { useBluetooth } from '../hooks/useBluetooth';
import { DeviceCard } from '../components/DeviceCard';
import { DeviceDetail } from '../types/types';

type HomeScreenNavigationProp = StackNavigationProp<
    RootStackParamList,
    'Home'
>;

type Props = {
    navigation: HomeScreenNavigationProp;
};

const HomeScreen: React.FC<Props> = ({ navigation }) => {
    const { disconnectFromDevice } = useBluetooth();
    const { devices, updateDevices } = useDevices();
    const [tempName, setTempName] = useState<string>('');
    const [deleteDialogVisible, setDeleteDialogVisible] = useState(false);
    const [deviceToDelete, setDeviceToDelete] = useState<DeviceDetail | null>(null);

    const startEditing = (id: string, name: string) => {
        setTempName(name);
        const updatedDevices = devices.map((device: DeviceDetail) => {
            if (device.id === id) {
                return { ...device, isEditing: true };
            }
            return device;
        });
        updateDevices(updatedDevices);
    };

    const saveName = (id: string) => {
        const updatedDevices = devices.map((device: DeviceDetail) => {
            if (device.id === id) {
                return { ...device, name: tempName, isEditing: false };
            }
            return device;
        });
        updateDevices(updatedDevices);
    };

    const deleteDevice = async (id: string) => {
        try {
            await disconnectFromDevice(id);
        } catch (error) {
            console.error("Error disconnecting device:", error);
        }
        const updatedDevices = devices.filter(device => device.id !== id);
        updateDevices(updatedDevices);
        setDeleteDialogVisible(false);
    };

    return (
        <View style={styles.container}>
            <FlatGrid
                itemDimension={160}
                data={devices}
                renderItem={({ item }) => (
                    <DeviceCard
                        device={item}
                        onEdit={(device) => startEditing(device.id, device.name)}
                        onPress={(device) => navigation.navigate('Device', { deviceDetail: device })}
                        onLongPress={(device) => {
                            setDeviceToDelete(device);
                            setDeleteDialogVisible(true);
                        }}
                    />
                )}
                keyExtractor={(item) => item.id}
            />

            <FAB
                style={styles.fab}
                icon="plus"
                onPress={() => navigation.navigate('AddNewDevice')}
            />
            <Portal>
                <Dialog visible={deleteDialogVisible} onDismiss={() => setDeleteDialogVisible(false)}>
                    <Dialog.Title>Confirm</Dialog.Title>
                    <Dialog.Content>
                        <Text>Are you sure you want to delete {deviceToDelete?.name}?</Text>
                    </Dialog.Content>
                    <Dialog.Actions>
                        <Button onPress={() => setDeleteDialogVisible(false)}>Cancel</Button>
                        <Button onPress={() => deleteDevice(deviceToDelete?.id || '')}>Delete</Button>
                    </Dialog.Actions>
                </Dialog>
            </Portal>
        </View>
    );
};

const styles = StyleSheet.create({
    container: {
        flex: 1,
        backgroundColor: '#121212',
    },
    card: {
        margin: 10,
    },
    listItem: {
        flexDirection: 'row',
        justifyContent: 'space-between',
        alignItems: 'center',
        padding: 10,
        borderBottomWidth: 1,
        borderBottomColor: '#666',
    },
    temperatureText: {
        marginRight: 10,
        color: '#f5f5f5',
    },
    temperatureContainer: {
        flexDirection: 'row',
        alignItems: 'center',
    },
    deviceName: {
        marginRight: 20,
        color: '#f5f5f5',
    },
    fab: {
        position: 'absolute',
        margin: 20,  // The margin has been updated to 20 as it was different between the two styles
        right: 0,
        bottom: 0,
    },
    centeredButton: {
        flex: 1,
        justifyContent: 'center',
        alignItems: 'center',
    },
});


export default HomeScreen;