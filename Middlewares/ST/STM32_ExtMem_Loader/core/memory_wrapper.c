/**
  ******************************************************************************
  * @file    memory_wrapper.c
  * @author  MCD Application Team
  * @brief   This file defines the services used by the different types
  *          of the external loader available in the module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "extmemloader_init.h"
#include "stm32_extmemloader_conf.h"
#include "memory_wrapper.h"

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup EXTMEM_LOADER_CORE Core
  * @ingroup EXTMEM_LOADER
  * @{
  */

/** @defgroup EXTMEM_LOADER_MEMWRAPPER Memory Wrapper
  * @ingroup EXTMEM_LOADER_CORE
  * @{
  */

/** @defgroup EXTMEM_LOADER_MEMWRAPPER_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief   memory initialization.
  * @retval  Operation status @ref MEM_MAPSTAT
  */
__weak MEM_STATUS memory_init(void)
{
  return MEM_OK; /* No error returned */
}

/**
  * @brief   Program memory.
  * @param   Address: page address
  * @param   Size   : size of data
  * @param   buffer : pointer to data buffer
  * @retval  Operation status @ref MEM_MAPSTAT
  */
__weak MEM_STATUS memory_write(uint32_t Address, uint32_t Size, uint8_t* buffer)
{
  MEM_STATUS retr = MEM_OK; /* No error returned */
  uint32_t addr = Address & 0x0FFFFFFFUL;

  if (EXTMEM_Write(STM32EXTLOADER_DEVICE_MEMORY_ID, addr, buffer, Size) != EXTMEM_OK)
  {
    retr = MEM_FAIL;
  }

  return retr;
}

/**
  * @brief    Full erase of the device
  * @retval  Operation status @ref MEM_MAPSTAT
  */
__weak MEM_STATUS memory_masserase(void)
{
  MEM_STATUS retr = MEM_OK;
  if (EXTMEM_EraseAll(STM32EXTLOADER_DEVICE_MEMORY_ID) != EXTMEM_OK)
  {
    retr = MEM_FAIL;
  }

  return retr;
}

/**
  * @brief   Sector erase.
  * @param   EraseStartAddress :  erase start address
  * @param   EraseEndAddress   :  erase end address
  * @param   SectorSize        :  sector size
  * @retval  Operation status @ref MEM_MAPSTAT
  */
__weak MEM_STATUS memory_sectorerase(uint32_t EraseStartAddress, uint32_t EraseEndAddress, uint32_t SectorSize)
{
  uint32_t start_addr = EraseStartAddress & 0x0FFFFFFFUL;
  uint32_t end_addr = (EraseEndAddress & 0x0FFFFFFFUL)+SectorSize;
  MEM_STATUS retr = MEM_OK;

  do
  {
    if (EXTMEM_EraseSector(STM32EXTLOADER_DEVICE_MEMORY_ID, start_addr, SectorSize) != EXTMEM_OK)
    {
      retr = MEM_FAIL;
    }
    start_addr = start_addr + SectorSize;
  }while ((end_addr > start_addr) && (retr == MEM_OK));

  return retr;
}

/**
  * @brief   enable/disable memory mapped mode.
  * @param   State @ref MEM_MAPSTAT
  * @retval  Operation status @ref MEM_MAPSTAT
  */
__weak MEM_STATUS memory_mapmode(MEM_MAPSTAT State)
{
  MEM_STATUS retr = MEM_OK;
  if (EXTMEM_MemoryMappedMode(STM32EXTLOADER_DEVICE_MEMORY_ID, State == MEM_MAPENABLE ? EXTMEM_ENABLE: EXTMEM_DISABLE) != EXTMEM_OK)
  {
    retr = MEM_FAIL;
  }
  return retr;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */