
export const modeColors: Record<Mode, string> = {
    'Heating': '#FF5733',
    'Cooling': '#33A6FF',
    'Dry': '#FFD733'
};

export const modeIcons: Record<Mode, string> = {
    'Heating': 'weather-sunny',
    'Cooling': 'snowflake',
    'Dry': 'water'
};

export type Mode = 'Heating' | 'Cooling' | 'Dry';

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