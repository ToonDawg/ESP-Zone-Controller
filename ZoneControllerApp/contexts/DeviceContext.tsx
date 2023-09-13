import React, { createContext, useContext, useState } from 'react';
import { Mode } from '../Screens/DeviceScreen';

export type DeviceDetail = {
  id: string;
  name: string;
  mode: Mode;
  display: boolean;
  setTemperature: number;
  currentTemperature: number;
  airflow: "Open" | "Closed";
  isEditing: boolean;
};

const defaultDevice: DeviceDetail = {
  id: 'default-001',
  name: 'Default Device',
  mode: 'Heating',
  display: true,
  setTemperature: 24,
  currentTemperature: 24,
  airflow: 'Open',
  isEditing: false,
};


type DeviceContextType = {
    devices: DeviceDetail[];
    addDevice: (device: DeviceDetail) => void;
    updateDevices: (newDevices: DeviceDetail[]) => void;
  };

const DeviceContext = createContext<DeviceContextType | undefined>(undefined);

type DeviceProviderProps = {
  children: React.ReactNode;
};

export const DeviceProvider: React.FC<DeviceProviderProps> = ({ children }) => {
  const [devices, setDevices] = useState<DeviceDetail[]>([defaultDevice]);

  const addDevice = (device: DeviceDetail) => {
    if (!devices.some(d => d.id === device.id)) {
      setDevices(prevDevices => [...prevDevices, device]);
    }
  };
  const updateDevices = (newDevices: DeviceDetail[]) => {
    setDevices(newDevices);
  };

  return (
    <DeviceContext.Provider value={{ devices, addDevice, updateDevices }}>
      {children}
    </DeviceContext.Provider>
  );
};

export const useDevices = (): DeviceContextType => {
  const context = useContext(DeviceContext);
  if (!context) {
    throw new Error('useDevices must be used within a DeviceProvider');
  }
  return context;
};
