import axios from 'axios';
import Config from "react-native-config";

export type DevicePayload = {
  deviceId: string,
  setTemperature: number,
  currentTemperature: number,
  currentMode: number,
  currentMotorState: number,
  displayPowerState: number
}

export async function fetchIotDevicesDataByIds(deviceIds: string[]): Promise<DevicePayload[]> {
  const url = `https://zonecontrollerdev.azurewebsites.net/api/GetIotDevicesDataFunction`;
  const params = {
    code: Config.API_KEY,
    deviceIds: deviceIds.join(',')
  };

  try {
    const response = await axios.get(url, { params });
    console.log(response.data);
    return response.data;
  } catch (error) {
    console.error('Error fetching IoT devices data:', error);
    throw error;
  }
}

export async function updateIotDeviceData(deviceDetails: DevicePayload) {
  const url = `https://zonecontrollerdev.azurewebsites.net/api/PostIotDeviceDataFunction?deviceId=${deviceDetails.deviceId}`;
  const headers = {
    'Content-Type': 'application/json',
    'x-functions-key': Config.API_KEY
  };

  try {
    const response = await axios.post(url, deviceDetails, {
      headers: headers
    });

    console.log(response.data);
    return response.data;
  } catch (error) {
    console.error('Error updating IoT device data:', error);
  }
}