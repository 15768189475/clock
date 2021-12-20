#include "t_capacity.h"
#include "m_charger.h"
#include "m_motor.h"
#include "m_battery.h"

// #define BOARD_STATIC_CURRENT (20)
// #define INTEGRAL_TIME (10)

static void CapacityThread(void const *argument);
osThreadId capacity_thread;
osThreadDef(CapacityThread, osPriorityHigh, 1, 0);

static void CapacityThread(void const *argument)
{
    osEvent evt;
    while (1)
    {
        evt=osSignalWait(0x01,10);
        if(evt.status==osEventSignal)
        {

        }
        

        
    }
}

void CapacityThreadStart(void)
{
    capacity_thread = osThreadCreate(osThread(CapacityThread), NULL);
}
void CapacityThreadStop(void)
{
    osThreadTerminate(capacity_thread);
}
