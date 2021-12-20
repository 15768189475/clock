#include "m_motor.h"
#include "g_adc.h"
#include "g_pwm.h"
#include "g_gpio.h"

//-------------------------------------------------------------------------------------------------------------------
//  @brief      初始化
//  @param      void
//  @return     void
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void ChangeDuty(void const *arugment);
static osTimerId change_duty;
static osTimerDef(change_duty, ChangeDuty);
static uint32 motor_res[MOTOR_NUMBER];
void InitMotor(ENUM_MOTOR motor, uint32 res)
{
    InitAdcDma(motor_adc[(uint8)motor]);
    InitPwm(motor_pwm[(uint8)motor], motor_freq[(uint8)motor], 0);
    motor_res[(uint8)motor] = res;
    static uint8 first_init_flag;
    if (first_init_flag == 0)
    {
        change_duty = osTimerCreate(osTimer(change_duty), osTimerPeriodic, 0);
        first_init_flag = 1;
    }
}

static uint8 goal_duty[MOTOR_NUMBER];
static uint8 now_duty[MOTOR_NUMBER];
void ChangeDuty(void const *arugment)
{
    uint8 cnt = 0;
    for (uint8 i = 0; i < MOTOR_NUMBER; i++)
    {
        if (now_duty[i] < goal_duty[i])
        {
            now_duty[i]++;
            SetPwmDuty(motor_pwm[i], now_duty[i]);
        }
        else if (now_duty[i] > goal_duty[i])
        {
            now_duty[i]--;
            SetPwmDuty(motor_pwm[i], now_duty[i]);
        }
        else
        {
            cnt++;
        }
    }

    if (cnt == MOTOR_NUMBER) //所有的电机占空比都已经调节完毕
    {
        osTimerStop(change_duty);
    }
}
void SetMotor(ENUM_MOTOR motor, uint8 duty)
{
    if (duty == DISABLE_MOTOR)
    {
        SetPwmDuty(motor_pwm[(uint8)motor], 0);
    }
    goal_duty[(uint8)motor] = duty;
    osTimerStart(change_duty, 1);
}

uint32 GetMotorCurrent(ENUM_MOTOR motor)
{
    static int16 filter_data[MOTOR_NUMBER];
    uint32 mA, uV;
    uV = GetAdc(motor_adc[(uint8)motor]) * 3300000 / 4096;
    mA = 10 * uV / motor_res[(uint8)motor];
    filter_data[(uint8)motor]=filter_data[(uint8)motor]*(motor_filter[(uint8)motor]-1)/motor_filter[(uint8)motor]+mA;
    mA=filter_data[(uint8)motor]/motor_filter[(uint8)motor];
    return mA;
}
