import { useEffect, useState, useDeferredValue, useRef } from 'react';
import { View, StyleSheet, Animated, TouchableWithoutFeedback } from 'react-native';
import { Text, IconButton, Surface } from 'react-native-paper';
import { RouteProp } from '@react-navigation/native';
import CircularSlider from '../components/CircularSlider';
import { useDevices } from '../contexts/DeviceContext';
import { RootStackParamList } from '../App';
import ModeButton from '../components/ModeButton';
import { DeviceDetail, Mode, modeColors, modeToNumber } from '../types/types';
import { TemperatureInfo } from '../components/TemperatureInfo';
import { updateIotDeviceData } from '../services/AzureIoTHubService';
import { DisplayInfo } from '../components/DisplayInfo';

const MIN_TEMP = 12;
const MAX_TEMP = 36;

type DeviceScreenProps = {
    route: RouteProp<RootStackParamList, 'Device'>;
};

export const DeviceScreen: React.FC<DeviceScreenProps> = ({ route }) => {
    const { deviceDetail } = route.params;
    const { devices, updateDevices } = useDevices();
    const [device, setDevice] = useState(devices.find(d => d.id === deviceDetail.id)!);
    const [apiCallTimeout, setApiCallTimeout] = useState<NodeJS.Timeout | null>(null);


    const heatingWidth = useRef(new Animated.Value(1)).current;
    const coolingWidth = useRef(new Animated.Value(1)).current;
    const dryWidth = useRef(new Animated.Value(1)).current;

    useEffect(() => {
        handleModeChange(device.mode);
    }, []);

    const callApiToUpdateIoTDevice = async () => {
        if (!device || !device.id) {
            console.error("Device or device ID is missing");
            return;
        }

        try {
            const response = await updateIotDeviceData({
                deviceId: device.id,
                setTemperature: device.setTemperature,
                currentTemperature: device.currentTemperature,
                currentMode: modeToNumber(device.mode),
                currentMotorState: device.airflow === 'Open' ? 0 : 1,
                displayPowerState: device.display ? 1 : 0
            });
            console.log(response);
        } catch (error) {
            console.error("Error updating IoT device:", error);
        }
    };

    useEffect(() => {
        return () => {
            if (apiCallTimeout) {
                clearTimeout(apiCallTimeout);
            }
        };
    }, []);



    const handleModeChange = (mode: Mode) => {
        updateDevice({ mode });
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

    const updateDevice = (newData: Partial<DeviceDetail>) => {
        const updatedDevice = { ...device, ...newData };
        setDevice(updatedDevice);

        const updatedDevicesList = devices.map(d => d.id === device?.id ? updatedDevice : d);
        updateDevices(updatedDevicesList);

        if (apiCallTimeout) {
            clearTimeout(apiCallTimeout);
        }

        setApiCallTimeout(setTimeout(callApiToUpdateIoTDevice, 2000));
    };



    return (
        <Surface style={styles.container}>
            <Text style={styles.title}>AC Mate</Text>
            <Text style={styles.area}>{device.name}</Text>
            <View style={{ flex: 1, justifyContent: 'center' }}>

                <View>
                    <TemperatureInfo currentTemperature={device.currentTemperature} />
                </View>

                <View style={styles.tempWidget}>
                    <CircularSlider
                        step={1}
                        radius={120}
                        min={MIN_TEMP}
                        max={MAX_TEMP}
                        value={device.setTemperature}
                        onChange={(value: number) => updateDevice({ setTemperature: value })}
                        strokeWidth={10}
                        buttonBorderColor={modeColors[device.mode]}
                        buttonFillColor="#fff"
                        buttonStrokeWidth={2}
                        openingRadian={1}
                        buttonRadius={13}
                        linearGradient={[{ stop: '0%', color: modeColors[device.mode] }, { stop: '100%', color: modeColors[device.mode] }]}
                    />
                    <View style={styles.temperatureControl}>
                        <IconButton
                            icon="minus"
                            onPress={() => device.setTemperature > MIN_TEMP && updateDevice({ setTemperature: device.setTemperature - 1 })}
                        />
                        <Text style={styles.temperatureText}>{device.setTemperature}°C</Text>
                        <IconButton
                            icon="plus"
                            onPress={() => {
                                if (device.setTemperature < MAX_TEMP) {
                                    updateDevice({ setTemperature: device.setTemperature + 1 });
                                }
                            }}
                        />
                    </View>
                    <Text style={styles.minTemp}>12°</Text>
                    <Text style={styles.maxTemp}>36°</Text>
                </View>
                <View style={{
                    justifyContent: 'center',
                    alignItems: 'center',
                }}>
                    <DisplayInfo
                        display={device.display}
                        airflow={device.airflow}
                        updateDevice={updateDevice}
                    />
                </View>

            </View>

            <Text style={styles.area}>Mode</Text>
            <View style={styles.buttonContainer}>
                <ModeButton
                    mode="Heating"
                    iconName="weather-sunny"
                    activeMode={device.mode}
                    handleModeChange={handleModeChange}
                    widthAnimation={heatingWidth}
                />
                <ModeButton
                    mode="Cooling"
                    iconName="snowflake"
                    activeMode={device.mode}
                    handleModeChange={handleModeChange}
                    widthAnimation={coolingWidth}
                />
                <ModeButton
                    mode="Dry"
                    iconName="water"
                    activeMode={device.mode}
                    handleModeChange={handleModeChange}
                    widthAnimation={dryWidth}
                />
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
        height: 70,
        flexDirection: 'row',
        alignItems: 'center',
        justifyContent: 'center',
        backgroundColor: '#2c2c2c',
    },
    tempWidget: {
        backgroundColor: '#121212',
        alignItems: 'center',
        marginTop: 30
    },
    title: {
        fontSize: 24,
        color: '#e0e0e0',
    },
    area: {
        fontSize: 16,
        marginBottom: 5,
        color: '#bbb',
    },
    temperatureControl: {
        position: 'absolute',
        flexDirection: 'row',
        alignItems: 'center',
        marginTop: 100
    },
    temperatureText: {
        color: '#f5f5f5',
        fontSize: 40,
        fontWeight: 'bold'
    },
    buttonContainer: {
        flexDirection: 'row',
        justifyContent: 'space-between',
        width: '100%',
    },
    buttonWrapperLeft: {
        flex: 1,
        borderRadius: 10,
        elevation: 5,
        height: 70,
        flexDirection: 'row',
        alignItems: 'center',
        justifyContent: 'center',
        backgroundColor: '#2c2c2c',
        marginRight: 5,
    },

    buttonWrapperRight: {
        flex: 1,
        borderRadius: 10,
        elevation: 5,
        height: 70,
        flexDirection: 'row',
        alignItems: 'center',
        justifyContent: 'center',
        backgroundColor: '#2c2c2c',
        marginLeft: 5,
    },
    minTemp: {
        position: 'absolute',
        marginTop: 200,
        left: 75,
        fontSize: 12
    },
    maxTemp: {
        position: 'absolute',
        marginTop: 200,
        right: 75,
        fontSize: 12
    }


});
