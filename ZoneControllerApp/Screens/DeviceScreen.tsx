import React, { useState } from 'react';
import { View, StyleSheet } from 'react-native';
import { Text, Switch, IconButton, Button, Surface } from 'react-native-paper';
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
    setDevice((prev) => ({ ...prev, mode: !prev.mode }));
  };

  const toggleDisplay = () => {
    setDevice((prev) => ({ ...prev, display: !prev.display }));
  };

  return (
    <Surface style={styles.container}>
        <Text style={styles.title}>{device.name}</Text>
        
        {/* Controls Section */}
        <View style={styles.section}>
            <Text style={styles.subtitle}>Controls</Text>
            <View style={styles.twoColumnRow}>
                {/* Mode Control */}
                <View style={styles.toggleControl}>
                    <IconButton
                        icon={device.mode ? 'power' : 'power-off'}
                        size={24}
                        iconColor={device.mode ? 'white' : 'grey'}
                        onPress={toggleMode}
                    />
                    <Text>Mode</Text>
                    <Switch value={device.mode} onValueChange={toggleMode} />
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
            
            {/* Temperature Details */}
            <View style={styles.row}>
                <Text style={styles.label}>Set Temperature:</Text>
                <Text style={styles.temperatureText}>{device.setTemperature}°C</Text>
            </View>
            <View style={styles.row}>
                <Text style={styles.label}>Current Temperature:</Text>
                <Text style={styles.temperatureText}>{device.currentTemperature}°C</Text>
            </View>
            
            {/* Airflow Status */}
            <View style={styles.row}>
                <Text style={styles.label}>Airflow:</Text>
                <Text style={styles.temperatureText}>{device.airflow ? 'Active' : 'Inactive'}</Text>
            </View>
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
        marginBottom: 20,
        color: '#e0e0e0',
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
    row: {
        flexDirection: 'row',
        alignItems: 'center',
        marginBottom: 20,
        justifyContent: 'space-between',
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
});

