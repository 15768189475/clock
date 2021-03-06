/*********************************************************************************************************************
// @company     鸿威技术
// @group       BMS
// @project
// @MCU         apm32f030C8
// @file        g_gpio
// @author      xieyongqiang(1365168884@qq.com)
// @date        2021-11-22
// @version     V1.0	初始创建
*********************************************************************************************************************/

#ifndef _g_gpio_h
#define _g_gpio_h
#include "common.h"

typedef enum
{
    A0,  A1,  A2,  A3,  A4,  A5,  A6,  A7,
    A8,  A9,  A10, A11, A12, A13, A14, A15,
    A16, A17, A18, A19, A20, A21, A22, A23,
    A24, A25, A26, A27, A28, A29, A30, A31,

    B0,  B1,  B2,  B3,  B4,  B5,  B6,  B7,
    B8,  B9,  B10, B11, B12, B13, B14, B15,
    B16, B17, B18, B19, B20, B21, B22, B23,
    B24, B25, B26, B27, B28, B29, B30, B31,

    C0,  C1,  C2,  C3,  C4,  C5,  C6,  C7,
    C8,  C9,  C10, C11, C12, C13, C14, C15,
    C16, C17, C18, C19, C20, C21, C22, C23,
    C24, C25, C26, C27, C28, C29, C30, C31,

    D0,  D1,  D2,  D3,  D4,  D5,  D6,  D7,
    D8,  D9,  D10, D11, D12, D13, D14, D15,
    D16, D17, D18, D19, D20, D21, D22, D23,
    D24, D25, D26, D27, D28, D29, D30, D31,

    E0,  E1,  E2,  E3,  E4,  E5,  E6,  E7,
    E8,  E9,  E10, E11, E12, E13, E14, E15,
    E16, E17, E18, E19, E20, E21, E22, E23,
    E24, E25, E26, E27, E28, E29, E30, E31,

    F0,  F1,  F2,  F3,  F4,  F5,  F6,  F7,
    F8,  F9,  F10, F11, F12, F13, F14, F15,
    F16, F17, F18, F19, F20, F21, F22, F23,
    F24, F25, F26, F27, F28, F29, F30, F31,

    GPIO_NULL

} ENUM_PIN;

typedef enum
{
    Input = 0,
    Output = 1,
} ENUM_GPIODIR;

typedef enum
{
    Rising = 0,
    Falling = 1,
    Both = 2,
    Low,
    High
} ENUM_TRIGGER;

//-------------------------------------------------------------------------------------------------------------------
//  @brief      GPIO初始化
//  @param      pin             
//  @param      dir           
//  @param      level           
//  @return     void
//  @note
//  Sample usage:               InitGpio(A0,Output,1);
//-------------------------------------------------------------------------------------------------------------------
void InitGpio(ENUM_PIN pin, ENUM_GPIODIR dir, uint8 level);

//-------------------------------------------------------------------------------------------------------------------
//  @brief      GPIO输出电平设置
//  @param      pin             
//  @param      level           
//  @return     void
//  @note
//  Sample usage:               SetGpio(A0,1);
//-------------------------------------------------------------------------------------------------------------------
void SetGpio(ENUM_PIN pin, uint8 level);

//-------------------------------------------------------------------------------------------------------------------
//  @brief      GPIO电平获取
//  @param      pin             
//  @return     uint8           
//  @note
//  Sample usage:               GetGpio(A0);
//-------------------------------------------------------------------------------------------------------------------
uint8 GetGpio(ENUM_PIN pin);

//-------------------------------------------------------------------------------------------------------------------
//  @brief      GPIO中断初始化
//  @param      pin             
//  @param      trigger         触发方式
//  @return     void
//  @note
//  Sample usage:               InitGpioInterrupt(A0，Low);
//-------------------------------------------------------------------------------------------------------------------
void InitGpioInterrupt(ENUM_PIN pin, ENUM_TRIGGER trigger);



#endif
