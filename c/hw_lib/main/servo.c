#include "g_pwm.h"

#define ServoLeftPwm TMR1_CH1_A8
#define ServoRightPwm TMR1_CH2_A9
#define ServoBottomPwm TMR1_CH2_A9

void ServoInit(void)
{

    InitPwm(ServoLeftPwm, 50, 0);
    InitPwm(ServoRightPwm, 50, 0);
    InitPwm(ServoBottomPwm, 50, 0);
}

void Write(uint8 flag)
{
    if (flag)//Ð´
    {
        SetPwmDuty(ServoBottomPwm, 0);
        osDelay(10);
    }
    else//²»Ð´
    {
        SetPwmDuty(ServoBottomPwm, 0);
        osDelay(10);
    }
}

void SetLeftServo(uint8 angle)
{
    uint8 duty;

    duty = angle;

    SetPwmDuty(ServoLeftPwm, duty);
}

void SetRightServo(uint8 angle)
{
    uint8 duty;

    duty = angle;

    SetPwmDuty(ServoRightPwm, duty);
}