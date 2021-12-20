#include "t_discharge.h"
#include "m_motor.h"
#include "m_led.h"
#include "m_sensor.h"
#include "m_key.h"
#include "m_battery.h"
#include "m_led.h"
#include "m_charger.h"

static uint32 discharge_start_time;
static uint32 high_gear_start_time;
static void ChangeGear(void);
static void MotorDutyChange(void);
static void TemperatureCheck(void);
static void BatteryVoltageCheck(void);
void DischargeThread(void const *argument);
osThreadId discharge_thread;
osThreadDef(DischargeThread, osPriorityHigh, 1, 0);

void DischargeThreadStart(void)
{
    ChangeGear();
    InitKey(GearKey, ChangeGear);
    discharge_thread = osThreadCreate(osThread(DischargeThread), NULL);
}

void DischargeThread(void const *argument)
{
    while (1)
    {
        MotorDutyChange();
        TemperatureCheck();
        BatteryVoltageCheck();

        osDelay(2);
    }
}
static void BatteryVoltageCheck(void)
{
    static uint8 cnt;
    if (GetVoltageMin() < param.DischargeOverVoltage)
    {
        if (cnt < 10)
        {
            cnt++;
        }
    }
    else
    {
        if (cnt > 0)
        {
            cnt--;
        }
    }

    if (cnt == 10)
    {
        osSignalSet(capacity_thread,0x01);
        ShutDown();
    }
}
static void MotorDutyChange(void)
{
    // Bigmotor
    if (high_gear_start_time != 0)
    {
        if (osKernelSysTick() - high_gear_start_time > 1000 * 60 * 5)
        {
            SetMotor(BigMotor, 60);
            high_gear_start_time = 0;
        }
    }
    // Smallmotor
    uint8 smallmotor_duty = 100 * GetBatteryVoltage() / param.SmallMotorVoltage;
    if (smallmotor_duty > 100)
    {
        smallmotor_duty = 100;
    }
    SetMotor(SmallMotor, smallmotor_duty);
}
static void ChangeGear(void)
{
    static uint8 gear = 1;
    switch (gear)
    {
    case 1:
        gear = 2;
        high_gear_start_time = 0;
        SetMotor(BigMotor, param.BigMotorPwmMiddle);
        LedShow(GearLed, 2);
        break;

    case 2:
        gear = 3;
        high_gear_start_time = osKernelSysTick();
        SetMotor(BigMotor, param.BigMotorPwmHigh);
        LedShow(GearLed, 3);
        break;
    case 3:
        gear = 1;
        high_gear_start_time = 0;
        SetMotor(BigMotor, param.BigMotorPwmLow);
        LedShow(GearLed, 1);
        break;
    default:
        break;
    }
}
static void TemperatureCheck(void)
{
    static uint8 error_cnt, over_hot_flag;
    static uint32 over_hot_start_time;
    //------------------------------------------
    int8 compare_temperature;
    if (over_hot_flag)
    {
        compare_temperature = param.DischargeOverHeatRelease;
    }
    else
    {
        compare_temperature = param.DischargeOverHeatTemperature;
    }
    //------------------------------------------

    //------------------------------------------
    if (GetSensor(TemperatureSensor) > compare_temperature)
    {
        if (error_cnt < 10)
        {
            error_cnt++;
        }
    }
    else
    {
        if (error_cnt > 0)
        {
            error_cnt--;
        }
    }
    //------------------------------------------

    //------------------------------------------
    if (error_cnt == 10 && over_hot_flag == false)
    {

        if (statistics.ChargeOverTemperatureCnt < 0xffff)
        {
            statistics.ChargeOverTemperatureCnt++;
        }
        over_hot_start_time = osKernelSysTick();
        over_hot_flag = true;
        SetChargeMos(0);
    }
    else if (error_cnt == 10 && over_hot_flag == true)
    {
        if (osKernelSysTick() - over_hot_start_time > param.DischargeCoolWaitTime)
        {
            ShutDown();
        }
    }
    else if (error_cnt == 0)
    {
        SetChargeMos(1);
        over_hot_flag = false;
    }
    //------------------------------------------
}
void DischargeThreadStop(void)
{
    osThreadTerminate(discharge_thread);
}

void SmallMotorShortIsr(void)
{
    SetMotor(SmallMotor, 0);
}
void BigMotorCurrentCheck(void)
{
    static uint8 short_cnt, lock_cnt;
    uint32 current = GetMotorCurrent(BigMotor);
    if (current > param.BigMotorShortCurrent)
    {
        if (short_cnt < 3)
        {
            short_cnt++;
        }
    }
    else
    {
        if (short_cnt > 0)
        {
            short_cnt--;
        }
    }
    if (short_cnt == 3)
    {
        SetMotor(BigMotor, 0);
    }

    if (current > param.BigMotorLockCurrent)
    {
        if (lock_cnt < 10)
        {
            lock_cnt++;
        }
    }
    else
    {
        if (lock_cnt > 0)
        {
            lock_cnt--;
        }
    }
    if (lock_cnt == 3)
    {
        SetMotor(BigMotor, 0);
    }
}
void SmallMotorCurrentCheck(void)
{
    static uint8  lock_cnt;
    uint32 current = GetMotorCurrent(SmallMotor);

    if (current > param.SmallMotorLockCurrent)
    {
        if (lock_cnt < 10)
        {
            lock_cnt++;
        }
    }
    else
    {
        if (lock_cnt > 0)
        {
            lock_cnt--;
        }
    }
    if (lock_cnt == 3)
    {
        SetMotor(SmallMotor, 0);
    }
    
}
