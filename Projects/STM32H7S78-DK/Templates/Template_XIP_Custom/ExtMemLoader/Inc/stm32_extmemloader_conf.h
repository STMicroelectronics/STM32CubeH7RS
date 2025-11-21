
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32_extmemloader_conf.h
  * @author  MCD Application Team
  * @brief   This file contains the device information.
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

#ifndef __EXTMEMLOADER_CONF_H_
#define __EXTMEMLOADER_CONF_H_

#include "extmemloader_init.h"
#include "stm32_extmem_conf.h"

#define STM32EXTLOADER_DEVICE_MEMORY_ID       EXTMEMORY_1
#define	STM32EXTLOADER_DEVICE_NAME            "EXTMEMLOADER_STM32H7S78-DK"
#define STM32EXTLOADER_DEVICE_TYPE            NOR_FLASH
#define	STM32EXTLOADER_DEVICE_ADDR            0x70000000
#define	STM32EXTLOADER_DEVICE_SIZE            0x8000000
#define	STM32EXTLOADER_DEVICE_SECTOR_NUMBERS  2048
#define	STM32EXTLOADER_DEVICE_SECTOR_SIZE     0x10000
#define	STM32EXTLOADER_DEVICE_PAGE_SIZE       0x1000
#define	STM32EXTLOADER_DEVICE_INITIAL_CONTENT 0xFF
#define STM32EXTLOADER_DEVICE_PAGE_TIMEOUT    10000
#define STM32EXTLOADER_DEVICE_SECTOR_TIMEOUT  6000

#endif /* __EXTMEMLOADER_CONF_H_ */
