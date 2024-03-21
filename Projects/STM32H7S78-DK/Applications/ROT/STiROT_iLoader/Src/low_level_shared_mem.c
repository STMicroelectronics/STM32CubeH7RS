/**
  ******************************************************************************
  * @file    low_level_shared_mem.c
  * @author  MCD Application Team
  * @brief   This file contains device definition for low_level_shared_mem
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
#include "low_level_shared_mem.h"
#include "stm32h7rsxx_hal.h"
#include <string.h>

/* Private variables ---------------------------------------------------------*/
/* shared_mem area */
#if defined(__ICCARM__)
#pragma location=".shared_mem"
#elif defined ( __CC_ARM ) || defined(__ARMCC_VERSION)
__attribute__((section(".bss.shared_mem")))
#elif defined(__GNUC__)
__attribute__((section(".shared_mem")))
#endif /* __ICCARM__ */
static LL_SHARED_MEM_Config_t SharedRam;

LL_SHARED_MEM_Config_t  *const pSharedRam = (LL_SHARED_MEM_Config_t *) &SharedRam;

/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Read slot configuration in the shared_mem memory
  * @param  pSlotCfg: pointer on slot configurations
  * @retval None
  */
void LL_SHARED_MEM_ReadConfig(LL_SHARED_MEM_Config_t *pCfg)
{
  assert_param(pSlotCfg != NULL);
  memcpy((void *) pCfg, (void *) pSharedRam, sizeof(LL_SHARED_MEM_Config_t));
}
