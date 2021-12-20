#include "m_charger.h"
#include "g_adc.h"
#include "g_gpio.h"



//-------------------------------------------------------------------------------------------------------------------
//  @brief      充电初始化
//  @param      void
//  @return     void
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void InitCharge(void)
{
    InitGpio(PRECHARGE_MOS, Output, 0);
    InitGpio(CHARGE_MOS, Output, 0);
    InitAdcDma(CHARGE_VOLTAGE_CH);
    InitAdcDma(CHARGE_CURRENT_CH);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      充电电流获取(mA)
//  @param      void
//  @return     mA
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint32 GetChargeCurrent(void)
{
    static uint8 init_flag;
    if (init_flag == 0) //设置偏置？
    {

        init_flag = 1;
    }

    uint32 mA;
    mA = 10 * GetAdc(CHARGE_CURRENT_CH) * 806 / calib.ResISmR;
    return mA;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief
//  @param      void
//  @return     mV
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint32 GetChargeVoltage(void)
{
    uint32 mV;
    mV = GetAdc(CHARGE_VOLTAGE_CH) * 11 * 3300 / 4096;
    return mV;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief
//  @param      flag
//  @return     void
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void SetChargeMos(uint8 flag)
{
    SetGpio(CHARGE_MOS, flag);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      充电电压获取
//  @param      flag
//  @return     void
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void SetPrechargeMos(uint8 flag)
{
    SetGpio(PRECHARGE_MOS, flag);
}
