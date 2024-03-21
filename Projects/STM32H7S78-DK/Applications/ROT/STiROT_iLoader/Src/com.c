/**
  ******************************************************************************
  * @file    com.c
  * @author  MCD Application Team
  * @brief   COM module.
  *          This file provides set of firmware functions to manage Com
  *          functionalities.
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
#include "com.h"

/** @addtogroup STM32H7xx_ROT_Applications STM32H7xx ROT Applications
  * @{
  */

/** @addtogroup STM32H7xx_ROT_Common Common
  * @{
  */

/** @defgroup  COM_Private_Defines Private Defines
  * @{
  */
#define UART_BAUDRATE (115200U)

/**
  * @}
  */

/** @defgroup  COM_Private_Variables Exported Variables
  * @{
  */

static UART_HandleTypeDef   UartHandle; /*!< Uart Handler*/

/**
  * @}
  */

/** @defgroup  COM_Exported_Functions Exported Functions
  * @{
  */

/** @defgroup  COM_Initialization_Functions Initialization Functions
  * @{
  */

/**
  * @brief  Initialize COM module.
  * @param  None.
  * @retval HAL Status.
  */
HAL_StatusTypeDef  COM_Init(void)
{
#if defined(__GNUC__)
  setvbuf(stdout, NULL, _IONBF, 0);
#endif /* __GNUC__ */

  /* USART resources configuration (Clock, GPIO pins and USART registers) ----*/
  /* USART configured as follow:
  - BaudRate = 115200 baud
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */
  UartHandle.Instance = COM_UART;
  UartHandle.Init.BaudRate = UART_BAUDRATE;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits = UART_STOPBITS_1;
  UartHandle.Init.Parity = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode = UART_MODE_TX_RX;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
  UartHandle.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  UartHandle.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT;
  UartHandle.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;
  UartHandle.FifoMode = UART_FIFOMODE_ENABLE;
  return HAL_UART_Init(&UartHandle);
}

/**
  * @brief  DeInitialize COM module.
  * @retval HAL Status.
  */
HAL_StatusTypeDef  COM_DeInit(void)
{
  /*
  * ADD SRC CODE HERE
  * ...
  */
  return HAL_OK;
}


/**
  * @}
  */

/** @defgroup  COM_Control_Functions Control Functions
  * @{
  */

/**
  * @brief Transmit Data.
  * @param uDataLength: Data pointer to the Data to transmit.
  * @param uTimeout: Timeout duration.
  * @retval Status of the Transmit operation.
  */
HAL_StatusTypeDef COM_Transmit(uint8_t *Data, uint16_t uDataLength, uint32_t uTimeout)
{
  return HAL_UART_Transmit(&UartHandle, (uint8_t *)Data, uDataLength, uTimeout);
}

/**
  * @brief Receive Data.
  * @param uDataLength: Data pointer to the Data to receive.
  * @param uTimeout: Timeout duration.
  * @retval Status of the Receive operation.
  */
HAL_StatusTypeDef COM_Receive(uint8_t *Data, uint16_t uDataLength, uint32_t uTimeout)
{
  return HAL_UART_Receive(&UartHandle, (uint8_t *)Data, uDataLength, uTimeout);
}

/**
  * @brief  Flush COM Input.
  * @param None.
  * @retval HAL_Status.
  */
HAL_StatusTypeDef COM_Flush(void)
{
  /* Clean the input path */
  __HAL_UART_FLUSH_DRREGISTER(&UartHandle);
  return HAL_OK;
}

/**
  * @}
  */

/** @addtogroup  COM_Private_Functions
  * @{
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
