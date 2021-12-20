#include "m_led.h"
#include "g_gpio.h"
#include "g_clock.h"
#include "g_pwm.h"


const uint8 led_number[10] = {0x7e, 0x0c, 0xe6, 0xce, 0x9c, 0xda, 0xfa, 0x4c, 0xfe, 0xde}; //LED码段 0 ~ 9
static uint8 led_data[3];                                                                  //显示缓存
static uint8 breath_pwm;                                                                   //呼吸灯PWM

static void LedReflesh(void const *argument);
static void PwmReflesh(void const *argument);
static void LedShowNumber(uint8 num);
static void LedShowGear(uint8 dat);
static void LedShowPlug(uint8 dat);
static void LedBreathe(uint16 breath_period);
static void LedClearNumber(void);

osTimerId led_reflesh;
osTimerId pwm_reflesh;
osTimerDef(led_reflesh, LedReflesh);
osTimerDef(pwm_reflesh, PwmReflesh);

//-------------------------------------------------------------------------------------------------------------------
//  @brief      LED初始化
//  @param      void
//  @return     void
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void LedInit(void)
{
    uint8 i;
    for (i = 0; i < 3; i++)
    {
        InitGpio(seg[i], Output, 0);
    }
    for (i = 2; i < 8; i++)
    {
        InitGpio(grid[i], Output, 0);
    }

    InitGpio(RED, Output, 0);
    InitGpio(YELLOW, Output, 0);
    InitPwm(L1_PWM, 10000, 0); 

    led_reflesh = osTimerCreate(osTimer(led_reflesh), osTimerPeriodic, 0);
    pwm_reflesh = osTimerCreate(osTimer(pwm_reflesh), osTimerPeriodic, 0);

    osTimerStart(led_reflesh, 1000 / LED_Hz / 3);
}

void LedShow(ENUM_LED led, uint16 dat)
{
    switch (led)
    {
    case SocLed:
        LedShowNumber(dat);
        break;
    case GearLed:
        LedShowGear(dat);
        break;
    case BreatheLed:
        LedBreathe(dat);
        break;
    case PlugLed:
        LedShowPlug(dat);
        break;
    default:
        break;
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      SOC显示
//  @param      num
//  @return     void
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void LedShowNumber(uint8 num)
{
    LedClearNumber();

    if (num >= 100) //百位
    {
        led_data[0] |= (1 << 2);
        led_data[0] |= (1 << 3);
    }

    uint8 x = num / 10 % 10; //十位
    led_data[0] |= (led_number[x] & 0xf0);
    led_data[1] |= (led_number[x] & 0x0f);

    x = num % 10; //个位
    led_data[1] |= (led_number[x] & 0xf0);
    led_data[2] |= (led_number[x] & 0x0f);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      档位指示灯显示
//  @param
//  @return     void
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void LedShowGear(uint8 dat)
{
    if (dat == 3)
    {
        SetGpio(RED, 1);
        SetGpio(YELLOW, 0);
        led_data[2] &= ~(1 << 6 | 1 << 7);
    }
    else if (dat == 2)
    {
        SetGpio(RED, 0);
        SetGpio(YELLOW, 1);
        led_data[2] &= ~(1 << 6 | 1 << 7);
    }
    else if (dat == 1)
    {
        SetGpio(RED, 0);
        SetGpio(YELLOW, 0);
        led_data[2] |= (1 << 6 | 1 << 7);
    }
    else if (dat == 0)
    {
        SetGpio(RED, 0);
        SetGpio(YELLOW, 0);
        led_data[2] &= ~(1 << 6 | 1 << 7);
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      堵转指示灯
//  @param      dat
//  @return     void
//  @note　　　　PCB上未焊接
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void LedShowPlug(uint8 dat)
{
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      呼吸灯
//  @param      呼吸的周期(ms)
//  @return     void
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void LedBreathe(uint16 breath_period)
{
    if (breath_period)
    {
        osTimerStart(pwm_reflesh, breath_period / 200);
        led_data[0] |= 0x02;
    }
    else //period为零，关闭呼吸灯
    {
        osTimerStop(pwm_reflesh);
        led_data[0] &= ~0x02;
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      清空SOC
//  @param      void
//  @return     void
//  @note       可用于关闭SOC显示
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
static void LedClearNumber(void)
{
    led_data[0] &= ~0xfc;
    led_data[1] &= ~0xff;
    led_data[2] &= ~0x0e;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      LED刷新函数
//  @param      void
//  @return     void
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
static void LedReflesh(void const *argument)
{
    static uint8 flag = 0;
	flag++;
    flag = flag % 3;

    //关闭显示
    SetGpio(seg[0], 0);
    SetGpio(seg[1], 0);
    SetGpio(seg[2], 0);

    //位
    if (led_data[flag] & 0x02)
    {
        if (flag == 0)
        {
            SetPwmDuty(L1_PWM, breath_pwm);
        }
        else
        {
            SetPwmDuty(L1_PWM, 0);
        }
    }
    else
    {
        SetPwmDuty(L1_PWM, 100);
    }

    for (uint8 i = 2; i < 8; i++)
    {
        if (led_data[flag] & (1 << i))
        {
            SetGpio(grid[i], 0);
        }
        else
        {
            SetGpio(grid[i], 1);
        }
    }

    //段
    SetGpio(seg[flag], 1);
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      呼吸灯的PWM刷新函数
//  @param      void
//  @return     void
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
static void PwmReflesh(void const *argument)
{
    static uint8 breath_pwm_add_flag;
    breath_pwm_add_flag++;
    if (0 < breath_pwm_add_flag && breath_pwm_add_flag < 100)
    {
        breath_pwm = breath_pwm_add_flag; //0->->100
    }
    else if (100 <= breath_pwm_add_flag && breath_pwm_add_flag < 200)
    {
        breath_pwm = 200 - breath_pwm_add_flag; //100->->0
    }
    else if (breath_pwm_add_flag == 200)
    {
        breath_pwm_add_flag = 0;
        breath_pwm = breath_pwm_add_flag;
    }
}
