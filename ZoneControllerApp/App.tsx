import React from 'react';
import { NavigationContainer, DarkTheme as NavigationDarkTheme } from '@react-navigation/native';
import { createStackNavigator } from '@react-navigation/stack';
import { enableScreens } from 'react-native-screens';
import { Provider as PaperProvider, MD3DarkTheme as PaperDarkTheme, IconButton } from 'react-native-paper';
import { DeviceProvider } from './contexts/DeviceContext'
import HomeScreen from './Screens/HomeScreen';
import AddDeviceScreen from './Screens/AddDeviceScreen';
import { DeviceScreen } from './Screens/DeviceScreen';
import { DeviceDetail } from './types/types';

export type RootStackParamList = {
  Home: undefined;
  Device: { deviceDetail: DeviceDetail };
  AddNewDevice: undefined;
};

const CombinedDarkTheme = {
  ...PaperDarkTheme,
  ...NavigationDarkTheme,
  colors: {
    ...PaperDarkTheme.colors,
    ...NavigationDarkTheme.colors,
    primary: '#3498db', 
    accent: '#f1c40f',
  },
};


const Stack = createStackNavigator<RootStackParamList>();

const App: React.FC = () => {
  enableScreens();
  return (
    <PaperProvider theme={CombinedDarkTheme}>
      <DeviceProvider>
        <NavigationContainer theme={CombinedDarkTheme}>
          <Stack.Navigator initialRouteName="Home">
            <Stack.Screen
              name="Home"
              component={HomeScreen}
              options={{
                title: 'My Devices',
                headerTitleAlign: 'center'
              }}
            />
            <Stack.Screen
              name="AddNewDevice"
              component={AddDeviceScreen}
              options={{
                title: 'Add Device',
                headerTitleAlign: 'center'
              }}
            />
            <Stack.Screen
              name="Device"
              component={DeviceScreen}
              options={({ navigation }) => ({
                title: 'Comfort Control',
                headerTitleAlign: 'center',
                headerRight: () => (
                  <IconButton
                    icon="cog-outline" 
                    size={24} 
                    onPress={() => {

                      navigation.navigate('Settings');
                    }}
                    style={{ marginRight: 10 }}
                  />
                )
              })}
            />


          </Stack.Navigator>
        </NavigationContainer>
      </DeviceProvider>
    </PaperProvider>
  );
};

export default App;

