#include "g_clock.h"
#include "apm32f0xx_rcm.h"
#include "apm32f0xx_fmc.h"
#include "apm32f0xx.h"

uint32 tick;
osTimerId tick_timer;
static void TickTimer(void const* argument){tick+=1;}
osTimerDef(tick_timer, TickTimer);

//-------------------------------------------------------------------------------------------------------------------
//  @brief      系统时钟初始化
//  @param      void            
//  @return     void
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void InitSystemClock(void)
{
    FMC_SetLatency(FMC_LATENCY_0);
    RCM_EnableHSI14();
    RCM_ConfigPLL(RCM_PLL_SEL_HSI, RCM_PLLMF_9);
    RCM_EnablePLL();

    while(RCM->CTRL1_B.PLLRDYFLG == SET);

    RCM_ConfigSYSCLK(RCM_SYSCLK_SEL_PLL);
    RCM_ConfigAHB(RCM_SYSCLK_DIV_1);
    RCM_ConfigAPB(RCM_HCLK_DIV_1);

    for(uint16 i = 0; i < 1000; i++);

    SystemCoreClockUpdate();
	
	tick_timer = osTimerCreate(osTimer(tick_timer),osTimerPeriodic,0);
	osTimerStart(tick_timer,1);
}




