import React, { useState } from 'react';
import { View, FlatList, StyleSheet } from 'react-native';
import { List, FAB, TextInput, IconButton, Portal, Dialog, Button, Text } from 'react-native-paper'
import { StackNavigationProp } from '@react-navigation/stack';
import { useDevices, DeviceDetail } from '../contexts/DeviceContext'
import Swipeable from 'react-native-gesture-handler/Swipeable';
import { TouchableOpacity } from 'react-native-gesture-handler';
import { RootStackParamList } from '../App';


type HomeScreenNavigationProp = StackNavigationProp<
    RootStackParamList,
    'Home'
>;

type Props = {
    navigation: HomeScreenNavigationProp;
};

const HomeScreen: React.FC<Props> = ({ navigation }) => {
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
    const deleteDevice = (id: string) => {
        const updatedDevices = devices.filter(device => device.id !== id);
        updateDevices(updatedDevices);
        setDeleteDialogVisible(false);
    };

    const renderRightActions = (id: string) => {
        return (
            <TouchableOpacity 
                style={styles.centeredButton}
                onPress={() => {
                    setDeviceToDelete(devices.find(device => device.id === id) || null);
                    setDeleteDialogVisible(true);
                }}
            >
                <IconButton
                    icon="trash-can"
                    size={24}
                />
            </TouchableOpacity>
        );
    };
    

    return (
        <View style={styles.container}>
            <FlatList
                data={devices}
                renderItem={({ item }) => (
                    <Swipeable renderRightActions={() => renderRightActions(item.id)}>
                        {item.isEditing ? (
                            <>
                                <TextInput
                                    label="Device Name"
                                    value={tempName}
                                    onChangeText={setTempName}
                                    mode="outlined"
                                    onEndEditing={() => saveName(item.id)}
                                />
                            </>
                        ) : (
                            <List.Item
                                title={item.name}
                                description={`${item.setTemperature}°C`}
                                right={() => (
                                    <IconButton
                                        icon="pencil"
                                        size={24}
                                        style={{ marginRight: -20 }}
                                        onPress={(e) => {
                                            e.stopPropagation();
                                            startEditing(item.id, item.name);
                                        }}
                                    />
                                )}
                                onPress={() => navigation.navigate('Device', { deviceDetail: item })}
                            />
                        )}
                    </Swipeable>
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
                    <Dialog.Title>Confirm Deletion</Dialog.Title>
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
        padding: 20,
        backgroundColor: '#121212',
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
        margin: 20,
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