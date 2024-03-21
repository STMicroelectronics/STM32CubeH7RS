/**
  ******************************************************************************
  * @file    low_level_shared_mem.h
  * @author  MCD Application Team
  * @brief   Header for low_level_shared_mem.c module
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
#ifndef LOW_LEVEL_SHARED_MEM_H
#define LOW_LEVEL_SHARED_MEM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/

typedef struct
{
  uint32_t Operation;             /*!< Operation ID */
  uint32_t SubOperations;         /*!< Suboperations used with the operation "Load" only (bit field) */
  uint32_t CodeVmStart;           /*!< Start address of the code slot in SRAM */
  uint32_t CodeSize;              /*!< Size of the code slot in SRAM */
  uint32_t Reserved1;             /*!< Reserved */
  uint32_t Reserved2;             /*!< Reserved */
  uint32_t CodePrimaryNvmStart;   /*!< Start address of the code primary slot in external flash */
  uint32_t CodeSecondaryNvmStart; /*!< Start address of the code secondary slot in external flash */
  uint32_t Reserved3;             /*!< Reserved */
} LL_SHARED_MEM_Config_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void LL_SHARED_MEM_ReadConfig(LL_SHARED_MEM_Config_t *pSlotCfg);

#ifdef __cplusplus
}
#endif

#endif /*  LOW_LEVEL_SHARED_MEM_H */

