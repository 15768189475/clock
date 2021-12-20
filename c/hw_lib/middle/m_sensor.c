#include "m_sensor.h"
#include "g_adc.h"
#include "g_gpio.h"

#define NTC_SIZE ((uint8)130)

const uint16 ntc_10k[NTC_SIZE] =
    {
        43000, 41062, 39224, 37479, 35822, 34247, 32751, 31329, 29977, 28691, //-10~-1
        27513, 26303, 25195, 24193, 23134, 22176, 21263, 20392, 19562, 18771, // 0-9
        18016, 17295, 16607, 15950, 15323, 14724, 14151, 13604, 13081, 12581, // 10-19
        12103, 11646, 11208, 10789, 10398, 10000, 9637, 9285, 8948, 8624,     // 20-29
        8315, 8018, 7733, 7459, 7197, 6946, 6704, 6473, 6250, 6036,           // 30-39
        5831, 5634, 5444, 5262, 5087, 4918, 4756, 4601, 4415, 4306,           // 40-49
        4168, 4034, 3905, 3781, 3662, 3545, 3436, 3329, 3226, 3126,           // 50-59
        3030, 2938, 2849, 2763, 2680, 2600, 2522, 2448, 2376, 2306,           // 60-69
        2239, 2174, 2111, 2050, 1992, 1935, 1880, 1827, 1776, 1726,           // 70-79
        1678, 1632, 1587, 1543, 1501, 1461, 1421, 1383, 1346, 1310,           // 80-89
        1275, 1242, 1209, 1178, 1147, 1118, 1089, 1061, 1034, 1008,           // 90-99
        983, 958, 934, 911, 888, 867, 846, 825, 805, 786,                     // 100-109
        767, 749, 731, 714, 698, 681, 666, 651, 636, 621                      // 110-119
};

//-------------------------------------------------------------------------------------------------------------------
//  @brief      初始化
//  @param      void
//  @return     void
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void InitSensor(ENUM_SENSOR sensor)
{
    switch (sensor)
    {
#if TEMPERATURE_SENSOR
    case TemperatureSensor:
        InitAdcDma(TEMPERATURE_ADC);
        break;
#endif
#if PRESSURE_SENSOR
    case PressureSensor:
        InitAdcDma(PRESSURE_ADC);
        break;

#endif

    default:
        break;
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      温度获取
//  @param      void
//  @return     T
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------

static int32 GetTemperature(void)
{
    uint32 x;
    uint16 ntc_r;

    uint16 adc_value = GetAdc(TEMPERATURE_ADC);
    x = (10000 * adc_value) / (4096 - adc_value);

    if (x > 53420)
    {
        return -10;
    }

    ntc_r = (uint16)x;

    for (uint16 i = 0; i < NTC_SIZE; i++)
    {
        if (ntc_r > ntc_10k[i])
        {
            return i - 10 - 3;
        }
    }

    return 120;
}

static int32 GetPressure(void)
{
    return 0;
}
int32 GetSensor(ENUM_SENSOR sensor)
{
    switch (sensor)
    {
#if TEMPERATURE_SENSOR
    case TemperatureSensor:
        return GetTemperature();
#endif

#if PRESSURE_SENSOR
    case PressureSensor:
        return GetPressure();
#endif
    default:
        return 0;
    }
}
