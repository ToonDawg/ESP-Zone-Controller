
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
export type Airflow = "Open" | "Closed";

export type DeviceDetail = {
    id: string;
    name: string;
    mode: Mode;
    display: boolean;
    setTemperature: number;
    currentTemperature: number;
    airflow: Airflow;
    isEditing: boolean;
};

export function modeToNumber(mode: Mode) {
    switch (mode) {
        case 'Cooling':
            return 0;
        case 'Heating':
            return 1;
        case 'Dry':
        default:
            return 0;
    }
}
