import React from 'react';
import { View, Text, Animated, StyleSheet } from 'react-native';
import { IconButton } from 'react-native-paper';
import { modeColors } from '../types/types';

const ModeButton = ({ mode, iconName, activeMode, handleModeChange, widthAnimation }) => {
    return (
        <Animated.View style={[styles.modeButton, { width: widthAnimation.interpolate({ inputRange: [1, 1.1], outputRange: ['23%', '49%'] }) }]} onTouchStart={() => handleModeChange(mode)}>
            <View style={{  alignItems: 'center', justifyContent: 'center' }}>
                <IconButton icon={iconName} iconColor={activeMode === mode ? modeColors[mode] : 'white'} />
            </View>
            {activeMode === mode && 
                <View style={{  alignItems: 'center', justifyContent: 'center', paddingRight: 20, }}>
                    <Text>{mode}</Text>
                </View>
            }
        </Animated.View>
    );
}

const styles = StyleSheet.create({
    modeButton: {
        borderRadius: 10,
        elevation: 5,
        height: 70,
        flexDirection: 'row',
        alignItems: 'center',
        justifyContent: 'center',
        backgroundColor: '#2c2c2c',
    }
});

export default ModeButton;
