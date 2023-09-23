import React, { useState } from 'react';
import { View, Text, StyleSheet, TouchableOpacity } from 'react-native';
import { IconButton } from 'react-native-paper';

interface DeviceComponentProps {
    display: boolean;
    airflow: string;
    updateDevice: (update: any) => void;
}

export const DisplayInfo = ({ display, airflow, updateDevice }: DeviceComponentProps) => {
    return (
        <View style={styles.container}>
            <View style={styles.section}>
                <Text style={styles.description}>Display</Text>
                <IconButton
                    icon={display ? "monitor" : "monitor-off"}
                    onPress={() => updateDevice({ display: !display })}
                />
            </View>
            <View style={styles.divider} />
            <View style={styles.section}>
                <Text style={styles.description}>Airflow</Text>
                <IconButton
                    icon={airflow === "Open" ? "fan" : "fan-off"}
                />
            </View>
        </View>
    );
};

const styles = StyleSheet.create({
    container: {
        flexDirection: 'row',
        width: '100%',
        height: 80
    },
    section: {
        flex: 1,
        alignItems: 'center',
    },
    description: {
        fontSize: 12,
        color: 'gray',
    },
    divider: {
        height: '100%',
        width: 1,
        backgroundColor: 'gray',
    },
});
