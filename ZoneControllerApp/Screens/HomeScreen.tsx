import React, { useCallback, useEffect, useState } from 'react';
import { View, StyleSheet, TouchableOpacity, Text, RefreshControl } from 'react-native';
import { FlatGrid } from 'react-native-super-grid';
import { Card, IconButton, TextInput, Portal, Dialog, Button, FAB } from 'react-native-paper';
import { RootStackParamList } from '../App';
import { StackNavigationProp } from '@react-navigation/stack';
import { useDevices } from '../contexts/DeviceContext';
import { useBluetooth } from '../hooks/useBluetooth';
import { DeviceCard } from '../components/DeviceCard';
import { DeviceDetail } from '../types/types';
import { DevicePayload, fetchIotDevicesDataByIds } from '../services/AzureIoTHubService';

const MODE_HEATING = 0;
const AIRFLOW_OPEN = 0;

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
    const [deleteDialogVisible, setDeleteDialogVisible] = useState(false);
    const [deviceToDelete, setDeviceToDelete] = useState<DeviceDetail | null>(null);
    const [isRefreshing, setIsRefreshing] = useState(false);

    useEffect(() => {
        fetchAndUpdateDevices();
    }, []);

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

    const parseDeviceData = (data: any): Partial<DeviceDetail> => ({
        setTemperature: data.setTemperature,
        currentTemperature: data.currentTemperature,
        mode: data.currentMode === MODE_HEATING ? 'Heating' : 'Cooling',
        airflow: data.currentMotorState === AIRFLOW_OPEN ? "Open" : "Closed"
    });

    const fetchAndUpdateDevices = async () => {
        try {
            const deviceIds = devices.map(device => device.id);
            const updatedDevicesData: DevicePayload[] = await fetchIotDevicesDataByIds(deviceIds);
            console.log(updatedDevicesData);
            
    
            if (updatedDevicesData) {
                const updatedDevicesList = devices.map(device => {
                    const updatedData = updatedDevicesData.find(d => d.deviceId === device.id);
                    if (updatedData) {
                        const parsedData = parseDeviceData(updatedData);
                        return { ...device, ...parsedData };
                    }
                    return device;
                });
    
                updateDevices(updatedDevicesList);
            }
        } catch (error) {
            console.error("Error fetching and updating devices data:", error);
        }
    };
    

    const onRefresh = useCallback(async () => {
        setIsRefreshing(true);
        await fetchAndUpdateDevices();
        setIsRefreshing(false);
    }, []);

    return (
        <View style={styles.container}>
            <FlatGrid
                itemDimension={160}
                data={devices}
                renderItem={({ item }) => (
                    <DeviceCard
                        device={item}
                        onEdit={(device) => {
                            // handle edit action
                        }}
                        onPress={(device) => navigation.navigate('Device', { deviceDetail: device })}
                        onLongPress={(device) => {
                            // handle long press action
                        }}
                    />
                )}
                keyExtractor={(item) => item.id}
                refreshControl={
                    <RefreshControl
                        refreshing={isRefreshing}
                        onRefresh={onRefresh}
                    />
                }
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