import React from 'react';
import { View, Text, StyleSheet } from 'react-native';

type DescriptorValueDisplayProps = {
    data: {
        desc: string;
        value: string;
    }[];
};



const DescriptorValueDisplay = ({ data }: DescriptorValueDisplayProps) => {
    return (
        <View style={styles.container}>
            {data.map((item, index) => (
                <React.Fragment key={index}>
                    <View style={styles.section}>
                        <Text style={styles.description}>{item.desc}</Text>
                        <Text style={styles.value}>{item.value}</Text>
                    </View>
                    {index !== data.length - 1 && <View style={styles.divider} />}
                </React.Fragment>
            ))}
        </View>
    );
};

const styles = StyleSheet.create({
    container: {
        flexDirection: 'row',
        alignItems: 'center',
        marginTop: 50,
        width: '100%',
    },
    section: {
        flex: 1,
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

export default DescriptorValueDisplay;
