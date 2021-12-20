#include "m_key.h"
#include "g_adc.h"
#include "g_gpio.h"

void (*PowerKeyCallbackPtr)(void); //回调函数指针
void (*GearKeyCallbackPtr)(void);  //回调函数指针

static ENUM_KEY which_key;
static void KeyCallback(void const *argument);

osTimerId key_timer; //消抖定时器
osTimerDef(key_timer, KeyCallback);

void InitKey(ENUM_KEY key, void (*Callback)(void))
{
    switch (key)
    {
    case PowerKey:
        InitGpioInterrupt(KEY_POWER, Falling);
        PowerKeyCallbackPtr = Callback;
        break;
    case GearKey:
        InitGpioInterrupt(KEY_GEAR, Rising);
        GearKeyCallbackPtr = Callback;
        break;
    default:
        break;
    }
    static uint8 init_flag;
    if (init_flag == 0)
    {
        init_flag = 1;
        key_timer = osTimerCreate(osTimer(key_timer), osTimerOnce, (void *)0);
    }
}
void KeyIsr(ENUM_KEY key)
{
    //30ms消抖
    which_key=key;
    osTimerStart(key_timer, 30);
}

static void KeyCallback(void const *argument)
{

    switch (which_key)
    {
    case PowerKey:
        if (!GetGpio(KEY_POWER))
        {
            (*PowerKeyCallbackPtr)();
        }
        break;
    case GearKey:
        if (GetGpio(KEY_GEAR))
        {
            (*GearKeyCallbackPtr)();
        }
        break;
    default:
        break;
    }
}
