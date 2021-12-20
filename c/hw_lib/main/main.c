#include "g_clock.h"
#include "g_gpio.h"
#include "g_adc.h"
#include "g_pwm.h"
#include "g_flash.h"

osThreadId draw_thread;
void DrawThread(void const *argument);
osThreadDef(DrawThread, osPriorityHigh, 1, 0);


int main(void)
{
    InitSystemClock();
	
	
	
	draw_thread=osThreadCreate(osThread(DrawThread),0);
	while(1)
	{
		
	}
	
	
	
	
}

