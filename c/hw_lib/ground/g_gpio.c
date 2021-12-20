#include "g_gpio.h"
#include "apm32f0xx_rcm.h"
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_eint.h"
#include "apm32f0xx_syscfg.h"
#include "apm32f0xx_misc.h"
static GPIO_T *GetPort(ENUM_PIN pin);

//-------------------------------------------------------------------------------------------------------------------
//  @brief      GPIO初始化
//  @param      pin
//  @param      dir
//  @param      level
//  @return     void
//  @note
//  Sample usage:               InitGpio(A0,Output,1);
//-------------------------------------------------------------------------------------------------------------------
void InitGpio(ENUM_PIN pin, ENUM_GPIODIR dir, uint8 level)
{
    if (GPIO_NULL == pin)
    {
        return;
    }
    GPIO_Config_T gpioConfig;
    uint16 flag_pin;
    GPIO_T *port;
    uint32 ahb_clock;

    flag_pin = 1 << (pin % 32);
    port = GetPort(pin);
    ahb_clock = 1 << (pin / 32 + 17);

    RCM_EnableAHBPeriphClock(ahb_clock);
    gpioConfig.pin = flag_pin;

    if (Output == dir)
    {
        gpioConfig.mode = GPIO_MODE_OUT;
        gpioConfig.speed = GPIO_SPEED_50MHz;
        gpioConfig.outtype = GPIO_OUT_TYPE_PP;

        if (1 == level)
            gpioConfig.pupd = GPIO_PUPD_PU;
        else if (0 == level)
            gpioConfig.pupd = GPIO_PUPD_PD;

        GPIO_Config(port, &gpioConfig);

        if (1 == level)
            SetGpio(pin, 1);
        else if (0 == level)
            SetGpio(pin, 0);
    }
    else if (Input == dir)
    {
        gpioConfig.mode = GPIO_MODE_IN;

        if (1 == level)
            gpioConfig.pupd = GPIO_PUPD_PU;
        else if (0 == level)
            gpioConfig.pupd = GPIO_PUPD_PD;

        GPIO_Config(port, &gpioConfig);
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      GPIO输出电平设置
//  @param      pin
//  @param      level
//  @return     void
//  @note
//  Sample usage:               SetGpio(A0,1);
//-------------------------------------------------------------------------------------------------------------------
void SetGpio(ENUM_PIN pin, uint8 level)
{
    if (pin == GPIO_NULL)
    {
        return;
    }
    if (level == 1)
    {
        GetPort(pin)->BSC = (uint32)1 << (pin % 32);
    }
    else
    {
        GetPort(pin)->BR = (uint32)1 << (pin % 32);
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      GPIO电平获取
//  @param      pin
//  @return     uint8
//  @note
//  Sample usage:               GetGpio(A0);
//-------------------------------------------------------------------------------------------------------------------
uint8 GetGpio(ENUM_PIN pin)
{
    return GPIO_ReadInputBit(GetPort(pin), 1 << (pin % 32));
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      GPIO中断初始化
//  @param      pin
//  @param      trigger
//  @return     void
//  @note
//  Sample usage:               InitGpioInterrupt(A0，Low);
//-------------------------------------------------------------------------------------------------------------------
void InitGpioInterrupt(ENUM_PIN pin, ENUM_TRIGGER trigger)
{
    EINT_Config_T eintConfig;
    GPIO_Config_T gpioConfig;
    uint16 flag_pin;
    GPIO_T *port;
    uint32 ahb_clock;

    flag_pin = 1 << (pin % 32);
    port = GetPort(pin);
    ahb_clock = 1 << (pin / 32 + 17);
    RCM_EnableAHBPeriphClock(ahb_clock);
    gpioConfig.pin = flag_pin;
    gpioConfig.mode = GPIO_MODE_IN;

    if (trigger == Rising || trigger == High)
    {
        gpioConfig.pupd = GPIO_PUPD_PD;
    }
    else if (trigger == Falling || trigger == Low)
    {
        gpioConfig.pupd = GPIO_PUPD_PU;
    }
    else
    {
        gpioConfig.pupd = GPIO_PUPD_NO;
    }

    GPIO_Config(port, &gpioConfig);

    SYSCFG_EINTLine((SYSCFG_PORT_T)(pin / 32), (SYSCFG_PIN_T)(pin % 32));
    eintConfig.line = 1 << (pin % 32);
    eintConfig.lineCmd = ENABLE;
    eintConfig.mode = EINT_MODE_INTERRUPT;
    eintConfig.trigger = (EINT_TRIGGER_T)(trigger);
    EINT_Config(&eintConfig);

    switch (pin % 32)
    {
    case 0:
    case 1:
        NVIC_EnableIRQRequest(EINT0_1_IRQn, 0x0f);
        break;
    case 2:
    case 3:
        NVIC_EnableIRQRequest(EINT2_3_IRQn, 0x0f);
        break;
    default:
        NVIC_EnableIRQRequest(EINT4_15_IRQn, 0x0f);
        break;
    }
}

//-------------------------------------------------------------------------------------------------------------------
static GPIO_T *GetPort(ENUM_PIN pin)
{
    switch (pin / 32)
    {
    case 0:
        return GPIOA;

    case 1:
        return GPIOB;

    case 2:
        return GPIOC;

    case 3:
        return GPIOD;

    case 4:
        return GPIOE;

    case 5:
        return GPIOF;
    default:
        return NULL;
    }
}
