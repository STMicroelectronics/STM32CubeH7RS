/**
 ******************************************************************************
 * @file    stm32_loader_api.h
 * @author  MCD Application Team
 * @brief   Header file of stm32_loader_api.c
 *
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32_LOADER_API_H
#define STM32_LOADER_API_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32_extmemloader_conf.h"

/** @addtogroup STM32_LOADER 
  * @ingroup EXTMEM_LOADER
  * @{
  */

/** @defgroup STM32_LOADER_Exported_definiton Exported definition
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup STM32_LOADER_Exported_Constants constants
  * @{
  */

/**
 * @brief macro to force the compiler to keep the code
 */
#if defined(__ICCARM__)
#define KeepInCompilation __root
#else
#define KeepInCompilation __attribute__((used))
#endif /* __ICCARM__ */

/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/** @addtogroup STM32_LOADER_Exported_Functions
  * @{
  */
KeepInCompilation uint32_t Init (void);
KeepInCompilation uint32_t Write (uint32_t Address, uint32_t Size, uint8_t* buffer);
KeepInCompilation uint32_t SectorErase (uint32_t EraseStartAddress ,uint32_t EraseEndAddress);
KeepInCompilation uint64_t Verify (uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size, uint32_t missalignement);
KeepInCompilation uint32_t MassErase (uint32_t Parallelism );

/**
  * @}
  */

/** @addtogroup STM32_LOADER_Exported_Debug_Functions 
  * @{
  */
int main(void);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32_LOADER_API_H */