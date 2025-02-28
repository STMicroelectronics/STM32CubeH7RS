/**
  ******************************************************************************
  * @file    sd_diskio_config.h
  * @brief   Template for the sd_diskio_config.h. This file should be copied and
             under project.
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

#ifndef __SD_DISKIO_CONFIG_H
#define __SD_DISKIO_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include "stm32h7rsxx_hal.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Maximum waiting time to do an operation in the HAL_SD_ReadBlocks() and HAL_SD_WriteBlocks() */
#define SD_TIMEOUT    30000

/*
 * Depending on the usecase, the SD card initialization could be done at the
 * application level, if it is the case define the flag below to enable
 * the MX_SD_Init().
 */
#define ENABLE_SD_INIT 0

/*
 * when using cacheable memory region, it may be needed to maintain the cache
 * validity. Enable the define below to activate a cache maintenance at each
 * read and write operation.
 * Notice: This is applicable only for cortex M7 based platform.
 */

#define ENABLE_SD_DMA_CACHE_MAINTENANCE  1

extern SD_HandleTypeDef hsd1;

/* Default handle used in sd_diskio.c file */
#define sdmmc_handle hsd1

/* Default Init function of sdmmc IP used in sd_diskio.c file */
#if (ENABLE_SD_INIT == 1)
extern void MX_SDMMC1_SD_Init(void);
#define sdmmc_sd_init MX_SDMMC1_SD_Init
#endif

#ifdef __cplusplus
}
#endif

#endif /* __SD_DISKIO_CONFIG_H */
