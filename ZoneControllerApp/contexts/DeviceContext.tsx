import React, { createContext, useContext, useState } from 'react';

export type DeviceDetail = {
  id: string;
  name: string;
  mode: "Heat" | "Cool";
  display: boolean;
  setTemperature: number;
  currentTemperature: number;
  airflow: "Open" | "Closed";
  isEditing: boolean;
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
  const [devices, setDevices] = useState<DeviceDetail[]>([]);

  const addDevice = (device: DeviceDetail) => {
    // Avoid adding duplicates
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
