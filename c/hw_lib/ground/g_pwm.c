#include "g_pwm.h"
#include "apm32f0xx_tmr.h"
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_rcm.h"

static TMR_T *GetTimer(ENUM_PWNCH ch);

//-------------------------------------------------------------------------------------------------------------------
//  @brief      PWM初始化
//  @param      ch			PWM通道
//  @param		freq
//  @param		duty
//  @return     void
//  @note
//  Sample usage:			InitPwm(TMR1_CH1_A8,1000,50);
//-------------------------------------------------------------------------------------------------------------------
void InitPwm(ENUM_PWNCH ch, uint32 freq, uint8 duty)
{
    if(ch==PWM_NOT)
    {
        return;
    }
    TMR_TimeBase_T timeBaseConfig;
    TMR_OCConfig_T occonfig;
    GPIO_Config_T gpioconfig;
    TMR_T *tmr;
    GPIO_T *port;

    switch (ch % 1000 % 100 / 32)
    {
    case 0:
        port = GPIOA;
        RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA);
        break;
    case 1:
        port = GPIOB;
        RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOB);
        break;
    case 2:
        port = GPIOC;
        RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOC);
        break;
    }

    switch (ch % 10000 / 1000)
    {
    case 1:
        tmr = TMR1;
        RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);
        RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR1);
        break;

    case 2:
        tmr = TMR3;
        RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR3);
        break;
    case 3:
        tmr = TMR14;
        RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR14);
        break;
    case 4:
        tmr = TMR15;
        RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);
        RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR15);
        break;
    case 5:
        tmr = TMR16;
        RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);
        RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR16);
        break;
    case 6:
        tmr = TMR17;
        RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);
        RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR17);
        break;

    default:
        break;
    }

    GPIO_ConfigPinAF(port, (GPIO_PIN_SOURCE_T)(ch % 1000 % 100 % 32), (GPIO_AF_T)(ch / 10000));

    gpioconfig.pin = 1 << (ch % 1000 % 100 % 32);
    gpioconfig.mode = GPIO_MODE_AF;
    gpioconfig.outtype = GPIO_OUT_TYPE_PP;
    gpioconfig.pupd = GPIO_PUPD_NO;
    gpioconfig.speed = GPIO_SPEED_10MHz;
    GPIO_Config(port, &gpioconfig);

    timeBaseConfig.clockDivision = TMR_CKD_DIV1;
    timeBaseConfig.counterMode = TMR_COUNTER_MODE_UP;
    timeBaseConfig.div = 0;
    timeBaseConfig.period = 16000 - 1;
    timeBaseConfig.repetitionCounter = 0;
    TMR_ConfigTimeBase(tmr, &timeBaseConfig);

    occonfig.OC_Mode = TMR_OC_MODE_PWM1;
    occonfig.OC_Idlestate = TMR_OCIDLESTATE_RESET;
    occonfig.OC_NIdlestate = TMR_OCNIDLESTATE_RESET;
    occonfig.OC_OutputNState = TMR_OUTPUT_NSTATE_DISABLE;
    occonfig.OC_OutputState = TMR_OUTPUT_STATE_ENABLE;
    occonfig.OC_Polarity = TMR_OC_POLARITY_HIGH;
    occonfig.OC_NPolarity = TMR_OC_NPOLARITY_HIGH;
    occonfig.Pulse = 0;

    switch (ch % 1000 / 100)
    {
    case 1:
        TMR_OC1Config(tmr, &occonfig);
        break;
    case 2:
        TMR_OC2Config(tmr, &occonfig);
        break;
    case 3:
        TMR_OC3Config(tmr, &occonfig);
        break;
    case 4:
        TMR_OC4Config(tmr, &occonfig);
        break;
    default:
        break;
    }

    TMR_EnablePWMOutputs(tmr);
    TMR_Enable(tmr);

    SetPwmFreq(ch, freq);
    SetPwmDuty(ch, duty);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      设置PWM频率
//  @param      ch			PWM通道
//  @param		freq		频率
//  @return     void
//  @note		同一定时器同一频率
//  Sample usage:			SetPwmFreq(TMR1_CH1_A8,1000);
//-------------------------------------------------------------------------------------------------------------------
void SetPwmFreq(ENUM_PWNCH ch, uint32 freq)
{
    if(ch==PWM_NOT)
    {
        return;
    }
    TMR_T *tmr = GetTimer(ch);
    if (1000 < freq)
    {
        tmr->PSC = 0;
        tmr->AUTORLD = 16000000 / freq - 1;
    }
    else if (100 < freq && freq <= 1000)
    {
        tmr->PSC = 10;
        tmr->AUTORLD = 1600000 / freq - 1;
    }
    else if (10 < freq && freq <= 100)
    {
        tmr->PSC = 100;
        tmr->AUTORLD = 160000 / freq - 1;
    }
    else if (1 <= freq && freq <= 10)
    {
        tmr->PSC = 1000;
        tmr->AUTORLD = 16000 / freq - 1;
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      设置PWM占空比
//  @param      ch			PWM通道
//  @param		duty		占空比
//  @return     void
//  @note
//  Sample usage:			SetPwmDuty(TMR1_CH1_A8,10);
//-------------------------------------------------------------------------------------------------------------------
void SetPwmDuty(ENUM_PWNCH ch, uint8 duty)
{
    if(ch==PWM_NOT)
    {
        return;
    }
    TMR_T *tmr = GetTimer(ch);
    if (duty > 100)
    {
        duty = 100;
    }
    if (duty == 0)
    {
        TMR_DisablePWMOutputs(tmr);
    }

    uint16 new_cc = duty * tmr->AUTORLD / 100;
    switch (ch % 1000 / 100)
    {
    case 1:
        tmr->CC1 = new_cc;
        break;
    case 2:
        tmr->CC2 = new_cc;
        break;
    case 3:
        tmr->CC3 = new_cc;
        break;
    case 4:
        tmr->CC4 = new_cc;
        break;
    default:
        break;
    }
}

// static-------------------------------------------------------------------------------------------------------------------
static TMR_T *GetTimer(ENUM_PWNCH ch)
{
    switch (ch % 10000 / 1000)
    {
    case 1:
        return TMR1;
    case 2:
        return TMR3;
    case 3:
        return TMR14;
    case 4:
        return TMR15;
    case 5:
        return TMR16;
    case 6:
        return TMR17;
    default:
        return 0;
    }
}
