#include "m_battery.h"
#include "g_gpio.h"
#include "g_adc.h"
#include "g_clock.h"

osTimerId add_convert_queue;
static void AddConvertQueue(void const *argument);
osTimerDef(add_convert_queue, AddConvertQueue);

static uint8 voltage_updata_flag;
static uint32 bat_adc_data[3][4];
static uint32 *GetCellVoltage(void);

//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取单体电池电压最小值
//  @param      void
//  @return     mV
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint32 GetVoltageMin(void)
{
    uint32 *voltage = GetCellVoltage();
    uint32 min = voltage[0];

    for (int i = 1; i < CELL_NUM; i++)
    {
        if (voltage[i] < min)
        {
            min = voltage[i];
        }
    }

    return min;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取单体电池电压最大值
//  @param      void
//  @return     mV
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint32 GetVoltageMax(void)
{
    uint32 *voltage = GetCellVoltage();
    uint32 max = voltage[0];

    for (int i = 1; i < CELL_NUM; i++)
    {
        if (voltage[i] > max)
        {
            max = voltage[i];
        }
    }

    return max;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      初始化
//  @param      void
//  @return     void
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void BatteryInit(void)
{
    InitGpio(BATTERY_MOS, Output, 1);

    uint8 i, j;
    for (i = 0; i < 3; i++)
    {
        InitGpio(sel_pin[i], Output, 0);
    }
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 4; j++)
        {
            InitAdcDma(bat_adc_ch[i][j]);
        }
    }

    add_convert_queue = osTimerCreate(osTimer(add_convert_queue), osTimerPeriodic, 0);
    osTimerStart(add_convert_queue, 500 / 6);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      电池供电MOS开关
//  @param      flag
//  @return     void
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void BatterySwitch(uint8 flag)
{
    SetGpio(BATTERY_MOS, flag);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取电池电压
//  @param      charge_flag
//  @return     battery_voltage
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint32 GetBatteryVoltage(void)
{
    uint32 *voltage = GetCellVoltage();
    uint32 bat_data_mv = 0;
    for (uint8 i = 0; i < CELL_NUM; i++)
    {
        bat_data_mv += voltage[i];
    }
    return bat_data_mv;
}

//static-------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------
//  @brief
//  @param      each_cell_voltage
//  @param      charge_flag
//  @return     battery_voltage
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
static uint32 *GetCellVoltage(void)
{
    static uint32 voltage[CELL_NUM];

    if (voltage_updata_flag)
    {
        voltage_updata_flag = false;

        uint32 bat_data_mv[CELL_NUM];
        for (uint8 i = 0; i < CELL_NUM; i++)
        {
            bat_data_mv[i] = bat_adc_data[bat_sel[i] >> 4][bat_sel[i] & 0x0f] * 3300 / 4096 * calib.BatteryVoltageRatio[i] / 1000;
        }

        uint32 mV = GetAdc(CHARGE_CURRENT_CH) * 806;

        for (int i = 0; i < CELL_NUM; i++)
        {
            bat_data_mv[i] -= mV;
        }

        voltage[0] = bat_data_mv[0];
        for (uint8 i = 1; i < CELL_NUM; i++)
        {
            voltage[i] = bat_data_mv[i] - bat_data_mv[i - 1];
        }
    }
    return voltage;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      添加adc转换
//  @param      void
//  @return     null
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
static void AddConvertQueue(void const *argument)
{
    static uint8 flag;
    switch (flag)
    {
    case 0:
        bat_adc_data[2][0] = GetAdc(bat_adc_ch[2][0]);
        bat_adc_data[2][1] = GetAdc(bat_adc_ch[2][1]);
        bat_adc_data[2][2] = GetAdc(bat_adc_ch[2][2]);
        bat_adc_data[2][3] = GetAdc(bat_adc_ch[2][3]);
        SetGpio(sel_pin[0], 1);
        SetGpio(sel_pin[1], 0);
        SetGpio(sel_pin[2], 0);
        flag++;
        break;
    case 1:
        bat_adc_data[0][0] = GetAdc(bat_adc_ch[0][0]);
        bat_adc_data[0][1] = GetAdc(bat_adc_ch[0][1]);
        bat_adc_data[0][2] = GetAdc(bat_adc_ch[0][2]);
        bat_adc_data[0][3] = GetAdc(bat_adc_ch[0][3]);
        flag++;
        break;
    case 2:
        bat_adc_data[0][0] = GetAdc(bat_adc_ch[0][0]);
        bat_adc_data[0][1] = GetAdc(bat_adc_ch[0][1]);
        bat_adc_data[0][2] = GetAdc(bat_adc_ch[0][2]);
        bat_adc_data[0][3] = GetAdc(bat_adc_ch[0][3]);
        SetGpio(sel_pin[0], 0);
        SetGpio(sel_pin[1], 1);
        SetGpio(sel_pin[2], 0);
        flag++;
        break;
    case 3:
        bat_adc_data[1][0] = GetAdc(bat_adc_ch[1][0]);
        bat_adc_data[1][1] = GetAdc(bat_adc_ch[1][1]);
        bat_adc_data[1][2] = GetAdc(bat_adc_ch[1][2]);
        bat_adc_data[1][3] = GetAdc(bat_adc_ch[1][3]);
        flag++;
        break;
    case 4:
        bat_adc_data[1][0] = GetAdc(bat_adc_ch[1][0]);
        bat_adc_data[1][1] = GetAdc(bat_adc_ch[1][1]);
        bat_adc_data[1][2] = GetAdc(bat_adc_ch[1][2]);
        bat_adc_data[1][3] = GetAdc(bat_adc_ch[1][3]);
        SetGpio(sel_pin[0], 0);
        SetGpio(sel_pin[1], 0);
        SetGpio(sel_pin[2], 1);
        flag++;
        break;
    case 5:
        bat_adc_data[2][0] = GetAdc(bat_adc_ch[2][0]);
        bat_adc_data[2][1] = GetAdc(bat_adc_ch[2][1]);
        bat_adc_data[2][2] = GetAdc(bat_adc_ch[2][2]);
        bat_adc_data[2][3] = GetAdc(bat_adc_ch[2][3]);
        voltage_updata_flag = true;
        flag = 0;
        break;

    default:
        break;
    }
}
