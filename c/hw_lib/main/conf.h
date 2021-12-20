#ifndef _conf_h
#define _conf_h
#include "common.h"
#include "param.h"
#include "cmsis_os.h"

#define SEL1 0x00
#define SEL2 0x10
#define SEL3 0x20
#define BAT1 0x00
#define BAT2 0x01
#define BAT3 0x02
#define BAT4 0x03

#define SEL1_NUM 4
#define SEL2_NUM 3
#define SEL3_NUM 0
#define CELL_NUM (SEL1_NUM + SEL2_NUM + SEL3_NUM)

#ifdef _m_battery_h
#include "g_gpio.h"
#include "g_adc.h"
const uint8 bat_sel[CELL_NUM] = {
    SEL2 + BAT1,
    SEL1 + BAT1,
    SEL1 + BAT2,
    SEL2 + BAT2,
    SEL1 + BAT3,
    SEL2 + BAT3,
    SEL1 + BAT4};
const ENUM_PIN sel_pin[3] = {A12, B5, GPIO_NULL};
const ENUM_ADCCH bat_adc_ch[3][4] = {{ADC_CH3_A3, ADC_CH2_A2, ADC_CH1_A1, ADC_CH0_A0},
                                     {ADC_CH3_A3, ADC_CH2_A2, ADC_CH1_A1, ADC_CH_NOT},
                                     {ADC_CH_NOT, ADC_CH_NOT, ADC_CH_NOT, ADC_CH_NOT}};
#endif
#define BATTERY_MOS B7

#define PRECHARGE_MOS A15
#define CHARGE_MOS A11
#define CHARGE_VOLTAGE_CH ADC_CH4_A4
#define CHARGE_CURRENT_CH ADC_CH6_A6

#define COMIO A9

typedef enum
{
    PowerKey,
    GearKey
} ENUM_KEY;
#define KEY_POWER B0
#define KEY_GEAR C15

typedef enum
{
    SocLed,
    GearLed,
    BreatheLed,
    PlugLed
} ENUM_LED;
#define LED_Hz (50)
#define RED C13
#define YELLOW C14
#define L1_PWM TMR17_CH1_B9

#ifdef _m_led_h
#include "g_gpio.h"
const ENUM_PIN seg[3] = {B10, B15, B11};                      /*LED1,LED2,LED3*/
const ENUM_PIN grid[8] = {A0, A0, B14, B13, B12, F6, F7, B8}; /*A0,A0,L2,L3,L4,L5,L6,L7*/
#endif


#define SHORT_IO B2
#define DISABLE_MOTOR 0xff
#define MOTOR_NUMBER 2
typedef enum
{
    BigMotor,
    SmallMotor,

} ENUM_MOTOR;
#ifdef _m_motor_h
#include "g_adc.h"
#include "g_pwm.h"
ENUM_PWNCH motor_pwm[MOTOR_NUMBER]={TMR1_CH3_A10,TMR1_CH1_A8};
ENUM_ADCCH motor_adc[MOTOR_NUMBER]={ADC_CH7_A7,ADC_CH6_A6};
uint32 motor_freq[MOTOR_NUMBER]={1250,1250};
uint16 motor_filter[MOTOR_NUMBER]={30,30};

#endif



#define TEMPERATURE_SENSOR 1
#define TEMPERATURE_ADC ADC_CH5_A5

#define PRESSURE_SENSOR 0
#define PRESSURE_ADC ADC_CH_NOT
typedef enum
{
#if TEMPERATURE_SENSOR
    TemperatureSensor,
#endif

#if PRESSURE_SENSOR
    PressureSensor,
#endif

} ENUM_SENSOR;

void ShutDown(void);
extern osThreadId capacity_thread;
#endif
