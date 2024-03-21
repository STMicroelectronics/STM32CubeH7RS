/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : cdc_send.h
  * @brief          : Header for cdc_send.c file.
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
#ifndef __CDC_SEND_H
#define __CDC_SEND_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/


/* Private includes ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void CDC_Send_Data(void);

#ifdef __cplusplus
}
#endif

#endif /* __CDC_SEND_H */
