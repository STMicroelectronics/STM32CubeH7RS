/**
  ******************************************************************************
  * @file    memory_wrapper.h
  * @author  MCD Application Team
  * @brief   Header file of extmemloader.c
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
#ifndef MEMORY_WRAPPER_H_
#define MEMORY_WRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif


/* Public include ------------------------------------------------------------*/
#include "stm32_extmemloader_conf.h"

/** @addtogroup EXTMEM_LOADER_MEMWRAPPER
  * @ingroup EXTMEM_LOADER_CORE
  * @{
  */

/* Private typedefs ----------------------------------------------------------*/
/** @addtogroup EXTMEM_LOADER_MEMWRAPPER_TypeDefs Exported typedefs
  * @{
  */

/**
 * @brief map mode status
 */
typedef enum {
  MEM_MAPDISABLE = 0, /*!< map mode disabled      */
  MEM_MAPENABLE       /*!< map mode enabled       */
} MEM_MAPSTAT;

/**
 * @brief memory function status
 */
typedef enum {
  MEM_OK,         /*!< mem function return status OK      */
  MEM_FAIL        /*!< mem function return status FAIL    */
} MEM_STATUS;

/**
  * @}
  */

/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @addtogroup EXTMEM_LOADER_MEMWRAPPER_Exported_Functions Exported Functions
  * @{
  */

MEM_STATUS memory_init(void);
MEM_STATUS memory_write(uint32_t Address, uint32_t Size, uint8_t* buffer);
MEM_STATUS memory_masserase(void);
MEM_STATUS memory_sectorerase(uint32_t EraseStartAddress, uint32_t EraseEndAddress, uint32_t SectorSize);
MEM_STATUS memory_mapmode(MEM_MAPSTAT State);

/**
  * @}
  */

/**
  * @}
  */
#ifdef __cplusplus
}
#endif

#endif /* MEMORY_WRAPPER_H_ */
