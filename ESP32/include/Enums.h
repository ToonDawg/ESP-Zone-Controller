#pragma once

enum class MotorState
{
    Open,
    Closed
};

enum class Mode
{
    Heat,
    Cool
};

enum class MotorDirection
{
    Normal,
    Reversed
};

struct TemperatureStatus
{
    float setTemperature;
    float currentTemperature;
    Mode mode;
    MotorState motorState;
};