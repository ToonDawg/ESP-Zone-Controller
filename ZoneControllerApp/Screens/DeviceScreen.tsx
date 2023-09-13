import React, { useEffect, useState } from 'react';
import { View, StyleSheet, Animated } from 'react-native';
import { Text, IconButton, Surface } from 'react-native-paper';
import { RouteProp } from '@react-navigation/native';
import CircularSlider from '../components/CircularSlider';
import { DeviceDetail, useDevices } from '../contexts/DeviceContext';
import { RootStackParamList } from '../App';

const MIN_TEMP = 12;
const MAX_TEMP = 36;

type DeviceScreenProps = {
    route: RouteProp<RootStackParamList, 'Device'>;
};

export type Mode = 'Heating' | 'Cooling' | 'Dry';

const modeColors: Record<Mode, string> = {
    'Heating': '#FF5733',
    'Cooling': '#33A6FF',
    'Dry': '#FFD733'
};


export const DeviceScreen: React.FC<DeviceScreenProps> = ({ route }) => {
    const { deviceDetail } = route.params;
    const { devices, updateDevices } = useDevices();

    const device = devices.find(d => d.id === deviceDetail.id)!;

    const [selectedMode, setSelectedMode] = useState<Mode>(device.mode);
    const heatingWidth = React.useRef(new Animated.Value(1)).current;
    const coolingWidth = React.useRef(new Animated.Value(1)).current;
    const dryWidth = React.useRef(new Animated.Value(1)).current;

    useEffect(() => {
        handleModeChange(selectedMode);
    }, []);


    const handleModeChange = (mode: Mode) => {
        setSelectedMode(mode);

        const animations: Record<Mode, [Animated.Value, number]> = {
            'Heating': [heatingWidth, mode === 'Heating' ? 1.1 : 1],
            'Cooling': [coolingWidth, mode === 'Cooling' ? 1.1 : 1],
            'Dry': [dryWidth, mode === 'Dry' ? 1.1 : 1]
        };

        const animatedValues = Object.values(animations).map(([value, toValue]) =>
            Animated.timing(value, { toValue: toValue, duration: 200, useNativeDriver: false })
        );

        Animated.parallel(animatedValues).start();
    };

    const updateDevice = (key: keyof DeviceDetail, value: any) => {
        const updatedDevice = { ...device, [key]: value };
        const updatedDevicesList = devices.map(d => d.id === device.id ? updatedDevice : d);
        updateDevices(updatedDevicesList);
    };

    return (
        <Surface style={styles.container}>
            <Text style={styles.title}>AC Mate</Text>
            <Text style={styles.area}>{device.name}</Text>
            <View style={styles.tempWidget}>
                <CircularSlider
                    step={1}
                    min={MIN_TEMP}
                    max={MAX_TEMP}
                    value={device.setTemperature}
                    onChange={(value: number) => updateDevice('setTemperature', value)}
                    strokeWidth={10}
                    buttonBorderColor={modeColors[selectedMode]}
                    buttonFillColor="#fff"
                    buttonStrokeWidth={2}
                    openingRadian={1}
                    buttonRadius={13}
                    linearGradient={[{ stop: '0%', color: modeColors[selectedMode] }, { stop: '100%', color: modeColors[selectedMode] }]}
                />
                <View style={styles.temperatureControl}>
                    <IconButton
                        icon="minus"
                        onPress={() => device.setTemperature > MIN_TEMP && updateDevice('setTemperature', device.setTemperature - 1)}
                    />
                    <Text style={styles.temperatureText}>{device.setTemperature}°C</Text>
                    <IconButton
                        icon="plus"
                        onPress={() => device.setTemperature < MAX_TEMP && updateDevice('setTemperature', device.setTemperature + 1)}
                    />
                </View>
            </View>
            <View style={styles.buttonContainer}>
                <Animated.View style={[styles.modeButton, { width: heatingWidth.interpolate({ inputRange: [1, 1.1], outputRange: ['25%', '45%'] }) }]}>
                    <IconButton icon="weather-sunny" iconColor={modeColors['Heating']} onPress={() => handleModeChange('Heating')} />
                    {selectedMode === 'Heating' && <Text>Heating</Text>}
                </Animated.View>
                <Animated.View style={[styles.modeButton, { width: coolingWidth.interpolate({ inputRange: [1, 1.1], outputRange: ['25%', '45%'] }) }]}>
                    <IconButton icon="snowflake" iconColor={modeColors['Cooling']} onPress={() => handleModeChange('Cooling')} />
                    {selectedMode === 'Cooling' && <Text>Cooling</Text>}
                </Animated.View>
                <Animated.View style={[styles.modeButton, { width: dryWidth.interpolate({ inputRange: [1, 1.1], outputRange: ['25%', '45%'] }) }]}>
                    <IconButton icon="water" iconColor={modeColors['Dry']} onPress={() => handleModeChange('Dry')} />
                    {selectedMode === 'Dry' && <Text>Dry</Text>}
                </Animated.View>
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
    modeButton: {
        borderRadius: 10,
        elevation: 5,
        shadowColor: '#000',
        shadowOffset: { width: 0, height: 2 },
        shadowOpacity: 0.25,
        shadowRadius: 3.84,
        flexDirection: 'column',
        alignItems: 'center',
        justifyContent: 'center',
        paddingVertical: 5,
        backgroundColor: '#2c2c2c',
    },
    tempWidget: {
        flex: 1,
        padding: 20,
        backgroundColor: '#121212',
        alignItems: 'center',
    },
    title: {
        fontSize: 24,
        color: '#e0e0e0',
    },
    area: {
        fontSize: 16,
        marginBottom: 20,
        color: '#bbb',
    },
    temperatureControl: {
        position: 'absolute',
        flexDirection: 'row',
        alignItems: 'center',
        marginTop: 90
    },
    temperatureText: {
        color: '#f5f5f5',
        fontSize: 30,
    },
    buttonContainer: {
        flexDirection: 'row',
        justifyContent: 'space-between',
        marginBottom: 10,
    },
});
