import React from 'react';
import { NavigationContainer, DefaultTheme as NavigationDefaultTheme, DarkTheme as NavigationDarkTheme } from '@react-navigation/native';
import { createStackNavigator } from '@react-navigation/stack';
import { enableScreens } from 'react-native-screens';
import { Provider as PaperProvider, MD3LightTheme as PaperDefaultTheme, MD3DarkTheme as PaperDarkTheme, IconButton } from 'react-native-paper';
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
              options={({ navigation }) => ({
                title: 'Comfort Control', // Temperature Adjust, Comfort Settings
                headerTitleAlign: 'center',
                headerRight: () => (
                  <IconButton
                    icon="cog-outline" // or any other icon you want
                    size={24} // adjust as needed
                    onPress={() => {
                      // handle what happens when the icon is pressed
                      navigation.navigate('Settings'); // example: navigate to settings screen
                    }}
                    style={{ marginRight: 10 }} // to give it some space from the right edge
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

