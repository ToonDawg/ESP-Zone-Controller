import React, { useEffect, useState } from 'react';
import { View, Text, StyleSheet, TouchableOpacity } from 'react-native';
import Geolocation from '@react-native-community/geolocation';
import axios from 'axios';
import Config from "react-native-config";
import { ActivityIndicator } from 'react-native-paper';

interface WeatherComponentProps {
    currentTemperature: number;
}

export const TemperatureInfo = ({ currentTemperature }: WeatherComponentProps) => {
    const [outsideTemperature, setOutsideTemperature] = useState<number>();

    const fetchTemperature = async () => {
        Geolocation.getCurrentPosition(async (position) => {
            const { latitude, longitude } = position.coords;

            const endpoint = `https://api.weatherapi.com/v1/current.json?key=${Config.WEATHER_API_KEY}&q=${latitude},${longitude}`;

            try {
                const response = await axios.get(endpoint);
                setOutsideTemperature(response.data.current.temp_c);
            } catch (error) {
                console.error('Error fetching weather data:', error);
            }
        });
    };

    useEffect(() => {
        fetchTemperature();
    }, []);
    

    return (
        <View style={styles.container}>
            <View style={styles.section}>
                <Text style={styles.description}>Current Temp.</Text>
                <Text style={styles.value}>{currentTemperature.toFixed(1)}°C</Text>
            </View>
            <View style={styles.divider} />
            <TouchableOpacity onPress={fetchTemperature} style={styles.section}>
                <Text style={styles.description}>Outside Temp.</Text>
                {outsideTemperature
                    ? <Text style={styles.value}>{outsideTemperature.toFixed(1)}°C</Text>
                    : <ActivityIndicator animating={true}  />
                }
            </TouchableOpacity>
        </View>
    );
};

const styles = StyleSheet.create({
    container: {
        flexDirection: 'row',
        height: 80,
        justifyContent: 'center',
    },
    section: {
        flex: 1,
        justifyContent: 'center',
        alignItems: 'center',
    },
    description: {
        fontSize: 12,
        color: 'gray',
    },
    value: {
        fontSize: 16,
        fontWeight: 'bold',
    },
    divider: {
        height: '100%',
        width: 1,
        backgroundColor: 'gray',
    },
});
