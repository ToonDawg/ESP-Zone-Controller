import React from 'react';
import { NavigationContainer, DefaultTheme as NavigationDefaultTheme, DarkTheme as NavigationDarkTheme } from '@react-navigation/native';
import { createStackNavigator } from '@react-navigation/stack';
import { enableScreens } from 'react-native-screens';
import { Provider as PaperProvider, MD3LightTheme as PaperDefaultTheme, MD3DarkTheme as PaperDarkTheme } from 'react-native-paper';
import { DeviceDetail, DeviceProvider } from './contexts/DeviceContext'


import HomeScreen from './Screens/HomeScreen';
import AddDeviceScreen from './Screens/AddDeviceScreen';
import { DeviceScreen } from './Screens/DeviceScreen';

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
              options={{
                title: 'Device Control',
                headerTitleAlign: 'center'
              }}
            />

          </Stack.Navigator>
        </NavigationContainer>
      </DeviceProvider>
    </PaperProvider>
  );
};

export default App;

