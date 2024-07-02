/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   fatfs.h
  * @brief  Header for fatfs applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#ifndef __fatfs_H
#define __fatfs_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ff.h"
#include "sd_diskio_dma_rtos.h" /* defines SD_Driver as external */
#include "main.h"
/* USER CODE END Includes */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
#define FATFS_MKFS_ALLOWED 1
#define DISK_READY_EVENT   1
#define DISK_REMOVE_EVENT  2
/* USER CODE END Private defines */

/* Exported functions prototypes ---------------------------------------------*/
void MX_FATFS_Init(void);
/* USER CODE BEGIN Prototypes */
int32_t MX_FATFS_Process(void);
int32_t MX_FATFS_DeInit(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__fatfs_H */
