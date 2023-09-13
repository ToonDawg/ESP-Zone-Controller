import { Card, Text, IconButton } from 'react-native-paper';
import { DeviceDetail, modeColors, modeIcons } from '../types/types';
import { View } from 'react-native';



type DeviceCardProps = {
    device: DeviceDetail;
    onEdit: (device: DeviceDetail) => void;
    onPress: (device: DeviceDetail) => void;
    onLongPress: (device: DeviceDetail) => void;
};

export const DeviceCard: React.FC<DeviceCardProps> = ({ device, onEdit, onPress, onLongPress }) => {
    return (
        <Card onPress={() => onPress(device)} onLongPress={() => onLongPress(device)}>
            <Card.Content style={{ flexDirection: 'row', justifyContent: 'space-between' }}>
                <View style={{ flex: 3 }}>
                    <Text style={{ fontSize: 14 }}>{device.name}</Text>
                    <Text style={{ fontSize: 12 }}>
                        Set {device.setTemperature}°C | {device.currentTemperature}°C
                    </Text>
                </View>
                <View style={{ flex: 1 }}>
                    <IconButton
                        icon={modeIcons[device.mode]}
                        iconColor={modeColors[device.mode]}
                        size={24}
                        onPress={() => onEdit(device)}
                    />
                </View>
            </Card.Content>

        </Card>
    );
};


