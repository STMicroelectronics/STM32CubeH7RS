/**
  ******************************************************************************
  * @file    low_level_ext_ram.h
  * @author  MCD Application Team
  * @brief   Header for low_level_ext_flash.c module
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
#ifndef LOW_LEVEL_EXT_RAM_H
#define LOW_LEVEL_EXT_RAM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_hal.h"
#include "flash_layout.h"
#include "boot_hal_cfg.h"

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
HAL_StatusTypeDef Ext_Ram_Initialize(void);
HAL_StatusTypeDef Ext_Ram_Uninitialize(void);

#ifdef __cplusplus
}
#endif

#endif /* LOW_LEVEL_EXT_RAM_H */
