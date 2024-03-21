/**
  ******************************************************************************
  * @file    explorer.h
  * @author  MCD Application Team
  * @brief          : Header for explorer.c file.
  *                   This file contains the common defines of the application.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EXPLORER_H
#define __EXPLORER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_hal.h"

/* Private function prototypes ----------------------------------------------- */
void SDMMC1_SD_Init(void);
uint8_t SD_StorageInit(void);
FRESULT SD_StorageParse(void);
uint8_t Audio_ShowWavFiles(void);
/* Exported function prototypes ----------------------------------------------- */
extern void MX_SDMMC1_SD_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __EXPLORER_H */
