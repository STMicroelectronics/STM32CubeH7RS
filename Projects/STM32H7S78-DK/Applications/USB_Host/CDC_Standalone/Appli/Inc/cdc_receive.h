/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : cdc_receive.h
  * @brief          : Header for cdc_receive.c file.
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
#ifndef __CDC_RECEIVE_H
#define __CDC_RECEIVE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/


/* Private includes ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define RX_BUFF_SIZE   0x400  /* Max Received data 1KB */
/* Exported variables --------------------------------------------------------*/
extern uint8_t CDC_RX_Buffer[RX_BUFF_SIZE];
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif /* __CDC_RECEIVE_H */
