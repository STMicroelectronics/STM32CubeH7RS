/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32h7s78_discovery.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/* Flash commands */
#define OCTAL_IO_READ_CMD           0xEC13
#define OCTAL_IO_DTR_READ_CMD       0xEE11
#define OCTAL_PAGE_PROG_CMD         0x12ED

#define OCTAL_WRBI_CMD              0x22DD
#define OCTAL_RDBUF_CMD             0x25DA
#define OCTAL_WRCT_CMD              0x24DB
#define OCTAL_WRCF_CMD              0x31CE

#define OCTAL_READ_STATUS_REG_CMD   0x05FA
#define OCTAL_SECTOR_ERASE_CMD      0x21DE
#define OCTAL_BLOCK_ERASE_CMD       0xDC23
#define OCTAL_WRITE_ENABLE_CMD      0x06F9

/* Dummy clocks cycles */
#define DUMMY_CLOCK_CYCLES_WRITE         0
#define DUMMY_CLOCK_CYCLES_READ          6
#define DUMMY_CLOCK_CYCLES_READ_REG      4
#define DUMMY_CLOCK_CYCLES_READ_OCTAL    6

/* Auto-polling values */
#define WRITE_ENABLE_MATCH_VALUE    0x02
#define WRITE_ENABLE_MASK_VALUE     0x02

#define MEMORY_READY_MATCH_VALUE    0x00
#define MEMORY_READY_MASK_VALUE     0x01

#define AUTO_POLLING_INTERVAL       0x10

/* OSPI NOR Banks address*/
#define BANK0_ADDRESS               0x00000000
#define BANK1_ADDRESS               0x02000000
#define BANK2_ADDRESS               0x04000000
#define BANK3_ADDRESS               0x06000000

/* Size of buffers */
#define BUFFERSIZE                  256
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void MX_XSPI1_Init(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
