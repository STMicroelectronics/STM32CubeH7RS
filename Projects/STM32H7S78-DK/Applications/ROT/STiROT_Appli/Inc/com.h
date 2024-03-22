/**
  ******************************************************************************
  * @file    com.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for COM module functionalities.
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
#ifndef COM_H
#define COM_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USER_APP User App Example
  * @{
  */

/** @addtogroup USER_APP_COMMON Common
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdio.h"
#include "stm32h7rsxx.h"

/** @defgroup  COM_Exported_Constants Exported Constants
  * @{
  */
/** @defgroup COM_UART  UART Configuration
  * @{
  */

#define COM_UART                                UART4
#define COM_UART_CLK_ENABLE()                   __HAL_RCC_UART4_CLK_ENABLE()
#define COM_UART_CLK_DISABLE()                  __HAL_RCC_UART4_CLK_DISABLE()

#define COM_UART_TX_AF                          GPIO_AF8_UART4
#define COM_UART_TX_GPIO_PORT                   GPIOD
#define COM_UART_TX_PIN                         GPIO_PIN_1
#define COM_UART_TX_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOD_CLK_ENABLE()
#define COM_UART_TX_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOD_CLK_DISABLE()

#define COM_UART_RX_AF                          GPIO_AF8_UART4
#define COM_UART_RX_GPIO_PORT                   GPIOD
#define COM_UART_RX_PIN                         GPIO_PIN_0
#define COM_UART_RX_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOD_CLK_ENABLE()
#define COM_UART_RX_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOD_CLK_DISABLE()

/* Maximum Timeout values for flags waiting loops.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */
#define COM_UART_TIMEOUT_MAX                    1000U

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup  COM_Exported_Functions
  * @{
  */

HAL_StatusTypeDef  COM_Init(void);
HAL_StatusTypeDef  COM_DeInit(void);
HAL_StatusTypeDef  COM_Transmit(uint8_t *Data, uint16_t uDataLength, uint32_t uTimeout);

HAL_StatusTypeDef  COM_Receive(uint8_t *Data, uint16_t uDataLength, uint32_t uTimeout);
HAL_StatusTypeDef  COM_Flush(void);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* COM_H */

