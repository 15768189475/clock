#include "g_adc.h"
#include "apm32f0xx_adc.h"
#include "apm32f0xx_rcm.h"
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_dma.h"
#include "apm32f0xx_misc.h"

#define ADC_TIMES (32)
#define WAIT_CH_NUM 20
ENUM_ADCCH wait_ch[WAIT_CH_NUM]; //等待转换的队列
uint8 wait_queue_front;          //转换队列头
uint8 wait_queue_rear;           //转换队列尾
uint16 buffer[ADC_TIMES];        //DMA缓存
uint32 convert_data_sum[15];     //ADC转换数据和
uint8 convert_updata_flag[15];   //ADC转换数据更新标志
uint8 convert_work_flag = false;
ENUM_ADCCH convert_ch_now;

static void AdcConvertStart(void);
static void AdcAddConvert(ENUM_ADCCH ch);

uint32 GetAdc(ENUM_ADCCH ch)
{
    if (ch == ADC_CH_NOT)
    {
        return 0;
    }
    if (convert_updata_flag[ch / 100] == true)
    {
        convert_updata_flag[ch / 100] = false;
        AdcAddConvert(ch);
    }
    return convert_data_sum[ch / 100] / ADC_TIMES;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ADC+DMA初始化
//  @param      ch
//  @param      loop_flag
//  @return     void
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void InitAdcDma(ENUM_ADCCH ch)
{
    if (ch == ADC_CH_NOT)
    {
        return;
    }
    static uint8 first_init;
    ADC_Config_T adcConfig;
    GPIO_Config_T gpioConfig;
    DMA_Config_T dmaConfig;

    gpioConfig.mode = GPIO_MODE_AN;
    gpioConfig.pupd = GPIO_PUPD_NO;
    gpioConfig.pin = 1 << (ch % 32);
    switch (ch % 100 / 32)
    {
    case 0:
        RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA);
        GPIO_Config(GPIOA, &gpioConfig);

        break;
    case 1:
        RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOB);
        GPIO_Config(GPIOB, &gpioConfig);

        break;
    case 2:
        RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOC);
        GPIO_Config(GPIOC, &gpioConfig);

        break;
    default:
        break;
    }

    if (first_init == 0)
    {
        ADC_Reset();
        RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_ADC1);
        RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);
        RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_DMA1);
        adcConfig.dataAlign = ADC_DATA_ALIGN_RIGHT;
        adcConfig.scanDir = ADC_SCAN_DIR_UPWARD;
        adcConfig.convMode = ADC_CONVERSION_CONTINUOUS;
        adcConfig.extTrigConv = ADC_EXT_TRIG_CONV_TRG0;
        adcConfig.extTrigEdge = ADC_EXT_TRIG_EDGE_NONE;
        adcConfig.resolution = ADC_RESOLUTION_12B;
        ADC_Config(&adcConfig);
        ADC->SMPTIM = (uint8)ADC_SAMPLE_TIME_239_5;
        ADC->CTRL_B.ADCEN = BIT_SET;

        ADC_EnableDMA();
        dmaConfig.bufferSize = ADC_TIMES;
        dmaConfig.memoryDataSize = DMA_MEMORY_DATASIZE_HALFWORD;
        dmaConfig.peripheralDataSize = DMA_PERIPHERAL_DATASIZE_HALFWORD;
        dmaConfig.memoryInc = DMA_MEMORY_INC_ENABLE;
        dmaConfig.peripheralInc = DMA_PERIPHERAL_INC_DISABLE;
        dmaConfig.circular = DMA_CIRCULAR_ENABLE;
        dmaConfig.memoryTomemory = DMA_M2M_DISABLE;
        dmaConfig.priority = DMA_PRIORITY_LEVEL_HIGHT;
        dmaConfig.direction = DMA_DIR_PERIPHERAL;
        dmaConfig.memoryAddress = (uint32)buffer;
        dmaConfig.peripheralAddress = (uint32)&ADC->DATA;
        DMA_Config(DMA1_CHANNEL_1, &dmaConfig);
        DMA_ClearIntFlag(DMA1_INT_FLAG_TF1);
        DMA_EnableInterrupt(DMA1_CHANNEL_1, DMA_INT_TFIE);
        NVIC_EnableIRQRequest(DMA1_CH1_IRQn, 3);
        DMA_Enable(DMA1_CHANNEL_1);

        first_init = 1;
    }
    AdcAddConvert(ch);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      添加一个转化通道到等待转换的队列中
//  @param      ch
//  @return     bool
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
static void AdcAddConvert(ENUM_ADCCH ch)
{
    wait_ch[wait_queue_rear] = ch;
    wait_queue_rear++;
    if (wait_queue_rear == WAIT_CH_NUM)
    {
        wait_queue_rear = 0;
    }

    if (convert_work_flag == false)
    {
        convert_work_flag = true;
        AdcConvertStart();
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ADC启动一次转换
//  @param      void
//  @return     void
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
static void AdcConvertStart(void)
{
    if (wait_queue_front == wait_queue_rear) //队列空了
    {
        convert_work_flag = false;
        return;
    }
    ADC->CHSEL = 1 << (wait_ch[wait_queue_front] / 100);
    ADC->CTRL_B.STARTCEN = BIT_SET;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ADCDMA中断
//  @param      none
//  @return     void
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void AdcDmaIsr(void)
{
    uint32 sum = 0;
    for (uint8 i = 0; i < ADC_TIMES; i++)
    {
        sum += buffer[i];
    }
    uint8 ch = wait_ch[wait_queue_front] / 100;
    convert_data_sum[ch] = sum;
    convert_updata_flag[ch] = true;

    convert_ch_now = wait_ch[wait_queue_front];
    wait_queue_front++;
    if (wait_queue_front == WAIT_CH_NUM)
    {
        wait_queue_front = 0;
    }

    AdcConvertStart();
}
