#ifndef _param_h
#define _param_h
#include "common.h"


typedef struct
{
    uint16 BatteryVoltageRatio[7];
    uint16 ResISmR;
    uint16 ResIMTuR;
    uint16 ResCHGmR;

    uint32 DataValid;
    uint32 Crc32;
} TYPE_CALIB;

typedef struct
{
    uint16 ChargeVoltage;
    uint16 ChargeCurrent;

    uint16 ChargeOverHeatTemperature;
    uint16 ChargeOverHeatRelease;
    uint16 ChargeCoolWaitTime;
    uint16 FirstOverChargeVoltage;

    uint16 PrechargeOverVoltage;
    uint32 PrechargeTimeout;
    uint32 ChargeTimeout;
    uint16 FastChargeOverVoltage;
    uint16 FullChargeOverVoltage;

    uint16 DischargeOverHeatTemperature;
    uint16 DischargeOverHeatRelease;
    uint32 DischargeCoolWaitTime;
    uint32 DischargeOverVoltage;
    uint8 BigMotorPwmHigh;
    uint8 BigMotorPwmMiddle;
    uint8 BigMotorPwmLow;
    uint32 BigMotorLockCurrent;
    uint32 BigMotorShortCurrent;
    uint32 BigMotorLowCurrent;
    uint32 SmallMotorVoltage;

    uint32 SmallMotorLockCurrent;

    uint16 BatteryCapacity;
    uint8 CellNumber;

    uint32 DataValid;
    uint32 Crc32;
} TYPE_PARAM;

typedef struct
{
    uint16 ChargePluginCnt;
    uint16 ChargeOverTemperatureCnt;
    uint16 ChargeTimeoutCnt;
    uint16 RemainCapacity;
    uint16 FullCapacity;

    uint32 DataValid;
    uint32 Crc32;
} TYPE_STATISTICS;

extern TYPE_PARAM param;
extern TYPE_CALIB calib;
extern TYPE_STATISTICS statistics;

#ifdef _g_flash_h

const TYPE_PARAM default_param =
    {
        .ChargeVoltage = 33000,
        .ChargeCurrent = 1000,

        .ChargeOverHeatTemperature = 55,
        .ChargeOverHeatRelease = 50,
        .ChargeCoolWaitTime = 15000,
        .FirstOverChargeVoltage = 4200,

        .PrechargeOverVoltage = 3000,
        .PrechargeTimeout = 3600000,
        .ChargeTimeout = 3600000 * 5,
        .FastChargeOverVoltage = 4100,
        .FullChargeOverVoltage = 4250,

        .DischargeOverHeatTemperature = 50,
        .DischargeOverHeatRelease = 45,
        .DischargeCoolWaitTime = 180000,
        .BigMotorPwmHigh = 90,
        .BigMotorPwmMiddle = 50,
        .BigMotorPwmLow = 25,
        .BigMotorLockCurrent = 18000,
        .BigMotorShortCurrent = 25000,
        .BigMotorLowCurrent = 1000,
        .SmallMotorVoltage = 24000,
        .SmallMotorLockCurrent = 2000,

        .BatteryCapacity = 2000,
        .CellNumber = 7,
};

const TYPE_STATISTICS default_statistics =
    {
        .ChargeOverTemperatureCnt = 0,
        .ChargePluginCnt = 0,
        .ChargeTimeoutCnt = 0,
        .RemainCapacity = 50,
        .FullCapacity = 2000,
};
const TYPE_CALIB default_calib =
    {
        .BatteryVoltageRatio[0] = 1500,
        .BatteryVoltageRatio[1] = 3000,
        .BatteryVoltageRatio[2] = 4400,
        .BatteryVoltageRatio[3] = 6000,
        .BatteryVoltageRatio[4] = 7353,
        .BatteryVoltageRatio[5] = 9000,
        .BatteryVoltageRatio[6] = 10333,

        .ResISmR = 505,
        .ResIMTuR = 8400,
        .ResCHGmR = 650,
};

#endif

#endif
