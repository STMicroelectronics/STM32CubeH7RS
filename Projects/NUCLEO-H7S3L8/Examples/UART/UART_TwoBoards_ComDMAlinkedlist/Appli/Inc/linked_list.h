/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : linked_list.h
  * Description        : This file provides code for the configuration
  *                      of the LinkedList.
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
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
HAL_StatusTypeDef MX_UART_Tx_Queue_Config(void);
HAL_StatusTypeDef MX_UART_Rx_Queue_Config(void);

#ifdef __cplusplus
}
#endif

#endif /* LINKED_LIST_H */

