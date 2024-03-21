/**
 ******************************************************************************
 * @file    stm32_device_info.c
 * @author  MCD Application Team
 * @brief   This file defines the structure containing information about the
 *          external flash memory used by STM32CubeProgramer in
 *          programming/erasing the device.
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
#if defined(STM32_EXTMEMLOADER_STM32CUBETARGET)
#include "stm32_device_info.h"


/** @addtogroup STM32_LOADER_Exported_definiton
  * @{
  */

/* Exported variables --------------------------------------------------------*/
/** @defgroup STM32_LOADER_Exported_Variables Variables
  * @{
  */

/**
 * @brief this variable is used by the loading program to get memory information
 */
#if defined(__ICCARM__)
__root sStorageInfo const StorageInfo =
#else
__attribute__((used)) sStorageInfo const StorageInfo =
#endif                                     /* __ICCARM__*/
{
    STM32EXTLOADER_DEVICE_NAME,            // Device Name
    STM32EXTLOADER_DEVICE_TYPE,            // Device Type
    STM32EXTLOADER_DEVICE_ADDR,            // Device Start Address
    STM32EXTLOADER_DEVICE_SIZE,            // Device Size in Bytes
    STM32EXTLOADER_DEVICE_PAGE_SIZE,       // Programming Page Size in Bytes
    STM32EXTLOADER_DEVICE_INITIAL_CONTENT, // Initial Content of Erased Memory
    {
      {STM32EXTLOADER_DEVICE_SECTOR_NUMBERS, STM32EXTLOADER_DEVICE_SECTOR_SIZE}, // Specify Number of sectors and size of each Sector
      {0x00000000, 0x00000000}
    },
};

/**
  * @}
  */

/**
  * @}
  */

#endif /* STM32_EXTMEMLOADER_CUBEPROGRAMMERTARGET */
