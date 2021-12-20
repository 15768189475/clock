#include "t_charge.h"
#include "m_battery.h"
#include "m_sensor.h"
#include "m_led.h"
#include "m_charger.h"

static void TemperatureCheck(void);
static void PrechargeCheck(void);
static void FullChargeCheck(void);
static void ChargerVoltageCheck(void);
static void ChargerCurrentCheck(void);
static void TimeoutCheck(void);

static void ChargeThread(void const *argument);
osThreadId charge_thread;
osThreadDef(ChargeThread, osPriorityHigh, 1, 0);

static uint32 charge_start_time;
void ChargeThread(void const *argument)
{
    while (1)
    {
        TemperatureCheck();
        PrechargeCheck();
        FullChargeCheck();
        ChargerVoltageCheck();
        ChargerCurrentCheck();
        TimeoutCheck();
        osDelay(5);
    }
}
void ChargeThreadStart(void)
{
    SetPrechargeMos(0);
    SetChargeMos(1);
    LedShow(BreatheLed, 3000);
    charge_start_time = osKernelSysTick();
    charge_thread = osThreadCreate(osThread(ChargeThread), NULL);
}

void ChargeThreadStop(void)
{
    osThreadTerminate(charge_thread);
}

static void PrechargeCheck(void)
{
    static uint8 precharge_flag = true;
    if (precharge_flag)
    {
        if (GetVoltageMin() > param.PrechargeOverVoltage)
        {
            precharge_flag = false;
            SetPrechargeMos(1);
        }
        if (osKernelSysTick() - charge_start_time > param.PrechargeTimeout)
        {
            precharge_flag = false;
            SetPrechargeMos(1);
        }
    }
}
static void FullChargeCheck(void)
{
    static uint8 fast_charge_flag = true;
    static uint8 cnt;
    uint32 cell_voltage_max = GetVoltageMax();

    if (fast_charge_flag) //快充阶段
    {
        if (cell_voltage_max > param.FastChargeOverVoltage)
        {
            cnt++;
            if (cnt > 10)
            {
                cnt = 0;
                fast_charge_flag = false;
            }
        }
        else
        {
            if (cnt > 0)
            {
                cnt--;
            }
        }
    }
    else //涓流阶段
    {
        static uint8 full_charge_flag = false;
        static uint32 full_charge_start_time;
        if (full_charge_flag == false)
        {
            if (osKernelSysTick() % 1000 < 100)
            {
                SetChargeMos(1);
            }
            else
            {
                SetChargeMos(0);
            }
            if (cell_voltage_max > param.FullChargeOverVoltage)
            {
                cnt++;
                if (cnt > 10)
                {
                    full_charge_start_time = osKernelSysTick();
                    SetChargeMos(0);
                    full_charge_flag = true;
                }
            }
            else
            {
                if (cnt > 0)
                {
                    cnt--;
                    if (cnt == 0)
                    {
                        full_charge_flag = false;
                    }
                }
            }
        }
        else
        {
            if (osKernelSysTick() - full_charge_start_time > 1000 * 60 * 10)
            {
                ShutDown();
            }
        }
    }

   
}
static void ChargerVoltageCheck(void)
{
    static uint8 error_cnt;
    uint32 charger_voltage = GetChargeVoltage();
    if (charger_voltage < param.ChargeVoltage - 3000 || param.ChargeVoltage + 3000 < charger_voltage)
    {
        if (error_cnt < 5)
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
    if (error_cnt == 5)
    {
        SetChargeMos(0);
        ShutDown();
    }
}
static void ChargerCurrentCheck(void)
{
    static uint8 error_cnt;
    if (GetChargeCurrent() > param.ChargeCurrent)
    {
        if (error_cnt < 5)
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
    if (error_cnt == 5)
    {
        SetChargeMos(0);
        ShutDown();
    }
}
static void TimeoutCheck(void)
{
    if (osKernelSysTick() - charge_start_time > param.ChargeTimeout)
    {
        SetChargeMos(0);
        ChargeThreadStop();
        ShutDown();
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
        compare_temperature = param.ChargeOverHeatRelease;
    }
    else
    {
        compare_temperature = param.ChargeOverHeatTemperature;
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
        if (osKernelSysTick() - over_hot_start_time > param.ChargeCoolWaitTime)
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
