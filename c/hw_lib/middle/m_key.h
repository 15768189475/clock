/*********************************************************************************************************************
// @company     鸿威技术
// @group       BMS
// @project     3968
// @MCU         apm32f030C8
// @file        m_key
// @author      xieyongqiang(1365168884@qq.com)
// @date        2021-11-22
// @version     V1.0	初始创建
*********************************************************************************************************************/

#ifndef _m_key_h
#define _m_key_h
#include "param.h"
#include "conf.h"

void InitKey(ENUM_KEY key,void (*Callback)(void));
void KeyIsr(ENUM_KEY key);

#endif
