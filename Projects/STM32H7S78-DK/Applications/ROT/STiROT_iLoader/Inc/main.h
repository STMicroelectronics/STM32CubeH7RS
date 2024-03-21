/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
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
#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_hal.h"
#include "region_defs.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/
/**
  * @brief Constants used by Serial Command Line Mode
  */
#define TX_TIMEOUT          ((uint32_t)1000U) /*!< UART TX timeout */
#define RX_TIMEOUT          ((uint32_t)2000U) /*!< UART RX timeout */

/**
  * @brief STiROT-iLoader debug configuration
  */
/* #define CONTROL_FLASH_WRITEOPERATION   */  /*!< Enable the control of each write operation */

/**
  * @brief STiROT-iLoader trace configuration
  */
#if defined(STIROT_ILOADER_DEBUG)
#define LOADER_PRINT (void) printf
#else /* not STIROT_ILOADER_DEBUG */
#define LOADER_PRINT(...)
#endif /* STIROT_ILOADER_DEBUG */
/**
  * @brief  STiROT-iLoader Error Typedef
  */
typedef enum
{
  STIROT_ILOADER_ERROR = 0x00001FE1U,
  STIROT_ILOADER_SUCCESS = 0x00122F11U
} STIROT_ILOADER_ErrorStatus;

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
