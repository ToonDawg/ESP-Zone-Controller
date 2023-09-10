import React, { useState } from 'react';
import { View, StyleSheet } from 'react-native';
import { Text, Switch, IconButton, Surface, Button } from 'react-native-paper';
import { RouteProp, NavigationProp } from '@react-navigation/native';
import { RootStackParamList } from '../App';
import { DeviceDetail } from '../contexts/DeviceContext';


type DeviceScreenProps = {
    route: RouteProp<RootStackParamList, 'Device'>;
    navigation: NavigationProp<RootStackParamList, 'Device'>;
};

export const DeviceScreen: React.FC<DeviceScreenProps> = ({ route }) => {
    const { deviceDetail } = route.params;

    const [device, setDevice] = useState<DeviceDetail>(deviceDetail);

    // Handlers for different device properties
    const toggleMode = () => {
        setDevice((prev) => ({ ...prev, mode: prev.mode === 'Cool' ? 'Heat' : 'Cool' }));
    };

    const toggleDisplay = () => {
        setDevice((prev) => ({ ...prev, display: !prev.display }));
    };

    const changeTemperature = (value: number) => {
        setDevice((prev) => ({ ...prev, setTemperature: value }));
    };

    const buttonColor = device.mode === 'Heat' ? '#FFDDC1' : '#C1D8FF'; 
    const iconName = device.mode === 'Heat' ? 'weather-sunny' : 'snowflake';

    return (
        <Surface style={styles.container}>
            <Text style={styles.title}>{device.name}</Text>
            <Text style={styles.area}>Living Room</Text> 

            {/* Temperature Control */}
            <View style={styles.temperatureControl}>
                <IconButton icon="minus" onPress={() => changeTemperature(device.setTemperature - 1)} />
                <Text style={styles.temperatureText}>{device.setTemperature}°C</Text>
                <IconButton icon="plus" onPress={() => changeTemperature(device.setTemperature + 1)} />
            </View>

            {/* Controls Section */}
            <View style={styles.section}>
                <Text style={styles.subtitle}>Controls</Text>
                <View style={styles.twoColumnRow}>
                    {/* Mode Control */}
                    <View style={styles.toggleControl}>
                        <Text>Mode</Text>
                        <IconButton
                            icon={iconName}
                            mode="contained"
                            onPress={toggleMode}
                            style={{
                                backgroundColor: buttonColor,
                                flexDirection: 'row',
                                alignItems: 'center',
                                borderRadius: 5
                            }}
                        />
                    </View>

                    {/* Display Control */}
                    <View style={styles.toggleControl}>
                        <IconButton
                            icon={device.display ? 'monitor' : 'monitor-off'}
                            size={24}
                            iconColor={device.display ? 'white' : 'gray'}
                            onPress={toggleDisplay}
                        />
                        <Text>Display</Text>
                        <Switch value={device.display} onValueChange={toggleDisplay} />
                    </View>
                </View>
                
                {/* Add other device details if needed */}
                
            </View>

            {/* Bottom Buttons */}
            <View style={styles.buttonContainer}>
                <Button style={styles.smallButton}>Button 1</Button>
                <Button style={styles.smallButton}>Button 2</Button>
                <Button style={styles.smallButton}>Button 3</Button>
            </View>
            <View style={styles.buttonContainer}>
                <Button mode="contained" style={styles.largeButton}>Button 4</Button>
                <Button mode="contained" style={styles.largeButton}>Button 5</Button>
            </View>
        </Surface>
    )
};

const styles = StyleSheet.create({
    container: {
        flex: 1,
        padding: 20,
        backgroundColor: '#121212',
    },
    title: {
        fontSize: 24,
        marginBottom: 10,
        color: '#e0e0e0',
    },
    area: {
        fontSize: 18,
        marginBottom: 20,
        color: '#bbb',
    },
    temperatureControl: {
        flexDirection: 'row',
        alignItems: 'center',
        justifyContent: 'center',
        marginBottom: 20,
    },
    subtitle: {
        fontSize: 20,
        marginBottom: 15,
        color: '#bbb',
    },
    section: {
        marginBottom: 20,
        padding: 15,
        backgroundColor: '#2c2c2c',
        borderRadius: 10,
    },
    twoColumnRow: {
        flexDirection: 'row',
        justifyContent: 'space-between',
        marginBottom: 20,
    },
    toggleControl: {
        flexDirection: 'row',
        alignItems: 'center',
        justifyContent: 'center',
    },
    label: {
        color: '#e0e0e0',
        marginRight: 10,
        fontSize: 16,
    },
    temperatureText: {
        color: '#f5f5f5',
        fontSize: 18,
    },
    buttonContainer: {
        flexDirection: 'row',
        justifyContent: 'space-between',
        marginBottom: 10,
    },
    smallButton: {
        width: '30%',
        borderRadius: 10,
    },
    largeButton: {
        width: '48%',
        paddingVertical: 15,
        borderRadius: 10,
    },
});
