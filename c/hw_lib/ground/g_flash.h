/*********************************************************************************************************************
// @company     ��������
// @group       BMS
// @project
// @MCU         apm32f030C8
// @file        g_flash
// @author      xieyongqiang(1365168884@qq.com)
// @date        2021-11-22
// @version     V1.0	��ʼ����
*********************************************************************************************************************/

#ifndef _g_flash_h
#define _g_flash_h
#include "common.h"
#include "param.h"
//-------------------------------------------------------------------------------------------------------------------
//  @brief      Flashд����
//  @param      addr
//  @param      buffer
//  @return     bool
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 WriteFlash(uint32 u32Addr, uint8 *u8Buffer, uint16 len);

//-------------------------------------------------------------------------------------------------------------------
//  @brief      CRCУ��
//  @param      buf
//  @param		len
//  @return     crcֵ
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint32 Crc32(void *buf, uint32 len);

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ��������
//  @param		void
//  @return     bool
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 SaveFlashParam(void);
uint8 SaveFlashStatistics(void);
uint8 SaveFlashCalib(void);

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ��ȡ����
//  @param		void
//  @return     bool
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void LoadFlashParam(void);
void LoadFlashStatistics(void);
void LoadFlashCalib(void);

#endif