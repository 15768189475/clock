#include "t_isr.h"
#include "t_charge.h"
#include "t_discharge.h"
#include "m_motor.h"
#include "m_battery.h"
#include "m_key.h"
#include "g_clock.h"
#include "g_adc.h"
#include "g_gpio.h"
#include "apm32f0xx_eint.h"
#include "apm32f0xx_dma.h"

extern ENUM_ADCCH convert_ch_now;

osThreadId isr_thread;
static void isrThread(void const *argument);
osThreadDef(isrThread, osPriorityRealtime, 1, 0);

static void isrThread(void const *argument)
{
   osEvent evt;
   while (1)
   {
      evt = osSignalWait(0, osWaitForever);
      switch (evt.value.signals)
      {
      case 0x01:
         AdcDmaIsr();
//         switch (convert_ch_now)
//         {
//         case BIGMOTOR_ADC_CH:
//            BigMotorCurrentCheck();
//            break;
//         case SMALLMOTOR_ADC_CH:
//            SmallMotorCurrentCheck();
//            break;
//         default:
//            break;
//         }

         break;

      case 0x02:
         KeyIsr(PowerKey);
         break;

      case 0x03:
         KeyIsr(GearKey);
         break;

      case 0x04:;
         SmallMotorShortIsr();
         break;

      case 0x05:

         break;
      default:
         break;
      }
   }
}

void IsrThreadStart(void)
{
   isr_thread = osThreadCreate(osThread(isrThread), NULL);
}
void IsrThreadStop(void)
{
   osThreadTerminate(isr_thread);
}

//-------------------------------------------------------------------------------------------------------------------
void DMA1_CH1_IRQHandler(void)
{
   if (DMA_ReadStatusFlag(DMA1_FLAG_TF1))
   {
      DMA_ClearStatusFlag(DMA1_FLAG_TF1);
      osSignalSet(isr_thread, 0x01);
   }
}
void EINT0_1_IRQHandler(void)
{
   if (EINT_ReadStatusFlag(EINT_LINE0) == SET)
   {
      EINT_ClearStatusFlag(EINT_LINE0);
      osSignalSet(isr_thread, 0x02); //PowerKey
   }
}
void EINT4_15_IRQHandler(void)
{
   if (EINT_ReadStatusFlag(EINT_LINE15) == SET)
   {
      EINT_ClearStatusFlag(EINT_LINE15);
      osSignalSet(isr_thread, 0x03); //GearKey
   }
}
void EINT2_3_IRQHandler(void)
{
   if (EINT_ReadStatusFlag(EINT_LINE2) == SET)
   {
      EINT_ClearStatusFlag(EINT_LINE2); //SmallMotorShort
      osSignalSet(isr_thread, 0x04);
   }
}

void USART1_IRQHandler(void)
{
}

void HardFault_Handler(void)
{
   while (1)
   {
   }
}
