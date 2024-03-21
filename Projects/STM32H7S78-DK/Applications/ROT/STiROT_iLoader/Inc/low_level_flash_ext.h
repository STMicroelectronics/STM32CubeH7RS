/**
  ******************************************************************************
  * @file    low_level_flash_ext.h
  * @author  MCD Application Team
  * @brief   Header for low_level_flash_ext.c module
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
#ifndef LOW_LEVEL_FLASH_EXT_H
#define LOW_LEVEL_FLASH_EXT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define ERASE_BLOC_SIZE           0x1000U           /*!< 4 Kbytes */
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
STIROT_ILOADER_ErrorStatus FLASH_EXT_Init(uint32_t Address);
STIROT_ILOADER_ErrorStatus FLASH_EXT_Erase_Size(uint32_t Address, uint32_t Length);
STIROT_ILOADER_ErrorStatus FLASH_EXT_Write(uint32_t WriteAddress, const uint8_t *pData, uint32_t Length);
STIROT_ILOADER_ErrorStatus FLASH_EXT_DeInit(void);

#ifdef __cplusplus
}
#endif

#endif /* LOW_LEVEL_FLASH_EXT_H */
