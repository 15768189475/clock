#include "g_flash.h"
#include "apm32f0xx_rcm.h"
#include "apm32f0xx_fmc.h"


#define DATA_VALID 0x55aa34cd
#define FLASH_PAGE_SIZE 0x400

#define FLASH_ADDR_PARAM 0x08007C00
#define FLASH_ADDR_PARAM_BAK 0x08007800
#define FLASH_ADDR_CALIB 0x08007E00
#define FLASH_ADDR_CALIB_BAK 0x08007A00
#define FLASH_ADDR_STATISTICS 0x08007400
#define FLASH_ADDR_STATISTICS_BAK 0x08007000





//-------------------------------------------------------------------------------------------------------------------
//  @brief
//  @param      void
//  @return     void
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void LoadFlashParam(void)
{
    memcpy((void *)&param, (const void *)FLASH_ADDR_PARAM, sizeof(param)); //

    if (param.DataValid == DATA_VALID && param.Crc32 == Crc32((uint8 *)&param, sizeof(param) - 4))
    {
        return; //
    }

    memcpy((void *)&param, (const void *)FLASH_ADDR_PARAM_BAK, sizeof(param)); //

    if (param.DataValid == DATA_VALID && param.Crc32 == Crc32((uint8 *)&param, sizeof(param) - 4))
    {
        return;
    }

    param = default_param;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief
//  @param      void
//  @return     bool
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 SaveFlashParam(void)
{
    param.DataValid = DATA_VALID;
    param.Crc32 = Crc32((uint8 *)&param, sizeof(param) - 4);
    FMC_Unlock();
    FMC_ErasePage(FLASH_ADDR_PARAM);
    WriteFlash(FLASH_ADDR_PARAM, (uint8 *)&param, sizeof(param));
    FMC_ErasePage(FLASH_ADDR_PARAM_BAK);
    WriteFlash(FLASH_ADDR_PARAM_BAK, (uint8 *)&param, sizeof(param));
    FMC_Lock();

    if (!memcmp((uint8 *)&param, (const uint8 *)FLASH_ADDR_PARAM, sizeof(param)) && !memcmp((uint8 *)&param, (const uint8 *)FLASH_ADDR_PARAM_BAK, sizeof(param)))
        return true;
    else
        return false;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief
//  @param      void
//  @return     void
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void LoadFlashStatistics(void)
{
    memcpy((void *)&statistics, (const void *)FLASH_ADDR_STATISTICS, sizeof(statistics)); //

    if (statistics.DataValid == DATA_VALID && statistics.Crc32 == Crc32((uint8 *)&statistics, sizeof(statistics) - 4))
    {
        return; //
    }

    memcpy((void *)&statistics, (const void *)FLASH_ADDR_STATISTICS_BAK, sizeof(statistics)); //

    if (statistics.DataValid == DATA_VALID && statistics.Crc32 == Crc32((uint8 *)&statistics, sizeof(statistics) - 4))
    {
        return;
    }

    statistics = default_statistics; //
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief
//  @param      void
//  @return     bool
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 SaveFlashStatistics(void)
{
    statistics.DataValid = DATA_VALID;
    statistics.Crc32 = Crc32((uint8 *)&statistics, sizeof(statistics) - 4);
    FMC_Unlock();
    FMC_ErasePage(FLASH_ADDR_STATISTICS);
    WriteFlash(FLASH_ADDR_STATISTICS, (uint8 *)&statistics, sizeof(statistics));
    FMC_ErasePage(FLASH_ADDR_STATISTICS_BAK);
    WriteFlash(FLASH_ADDR_STATISTICS_BAK, (uint8 *)&statistics, sizeof(statistics));
    FMC_Lock();

    if (!memcmp((uint8 *)&statistics, (const uint8 *)FLASH_ADDR_STATISTICS, sizeof(statistics)) &&
        !memcmp((uint8 *)&statistics, (const uint8 *)FLASH_ADDR_STATISTICS_BAK, sizeof(statistics)))
        return true;
    else
        return false;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief
//  @param      void
//  @return     void
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void LoadFlashCalib(void)
{
    memcpy((void *)&calib, (const void *)FLASH_ADDR_CALIB, sizeof(calib)); //

    if (calib.DataValid == DATA_VALID && calib.Crc32 == Crc32((uint8 *)&calib, sizeof(calib) - 4))
    {
        return; //
    }

    memcpy((void *)&calib, (const void *)FLASH_ADDR_CALIB_BAK, sizeof(calib)); //

    if (calib.DataValid == DATA_VALID && calib.Crc32 == Crc32((uint8 *)&calib, sizeof(calib) - 4))
    {
        return;
    }

    calib = default_calib; //
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief
//  @param      void
//  @return     bool
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 SaveFlashCalib(void)
{
    calib.DataValid = DATA_VALID;
    calib.Crc32 = Crc32((uint8 *)&calib, sizeof(calib) - 4);
    FMC_Unlock();
    FMC_ErasePage(FLASH_ADDR_CALIB);
    WriteFlash(FLASH_ADDR_CALIB, (uint8 *)&calib, sizeof(calib));
    FMC_ErasePage(FLASH_ADDR_CALIB_BAK);
    WriteFlash(FLASH_ADDR_CALIB_BAK, (uint8 *)&calib, sizeof(calib));
    FMC_Lock();

    if (!memcmp((uint8 *)&calib, (const uint8 *)FLASH_ADDR_CALIB, sizeof(calib)) &&
        !memcmp((uint8 *)&calib, (const uint8 *)FLASH_ADDR_CALIB_BAK, sizeof(calib)))
        return true;
    else
        return false;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      Flash写数据
//  @param      addr
//  @param      buffer
//  @return     bool
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 WriteFlash(uint32 addr, uint8 *buffer, uint16 len)
{
    uint32 data;
    if ((addr & 0x03) != 0)
    {
        return 0;
    }

    while (len > 0)
    {
        data = ((uint32)buffer[3] << 24) | ((uint32)buffer[2] << 16) | ((uint32)buffer[1] << 8) | buffer[0];
        FMC_ProgramWord(addr, data);
        buffer += 4;
        addr += 4;
        len -= 4;
    }
    return 1;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      CRC校验
//  @param      buf
//  @param		len
//  @return     crc值
//  @note
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint32 Crc32(void *buf, uint32 len)
{
    uint8 *data = (uint8 *)buf;
    uint32 tmp;

    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_CRC);
    CRC->CTRL_B.RST = BIT_SET;

    if (((uint32)buf & 3) == 0)
    {
        while (len >= 4)
        {
            CRC->DATA = *(uint32 *)data;
            data += 4;
            len -= 4;
        }
    }
    else
    {
        while (len >= 4)
        {
            tmp = ((uint32)data[3] << 24) + ((uint32)data[2] << 16) + ((uint32)data[1] << 8) + data[0];
            CRC->DATA = tmp;
            data += 4;
            len -= 4;
        }
    }

    if (len == 3)
    {
        tmp = ((uint32)data[2] << 16) + ((uint32)data[1] << 8) + data[0];
        CRC->DATA = tmp;
    }
    else if (len == 2)
    {
        tmp = ((uint32)data[1] << 8) + data[0];
        CRC->DATA = tmp;
    }
    else if (len == 1)
    {
        tmp = data[0];
        CRC->DATA = tmp;
    }

    return CRC->DATA;
}
