/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbh_platform.c

  * @brief          : This file implements the USB platform
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

/* Includes ------------------------------------------------------------------*/
#include "usbh_platform.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/* External variables ---------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;

/**
  * @brief  Drive VBUS.
  * @param  state : VBUS state
  *          This parameter can be one of the these values:
  *          - 1 : VBUS Active
  *          - 0 : VBUS Inactive
  */
void MX_DriverVbusHS(uint8_t state)
{
  /* USER CODE BEGIN PREPARE_I2C_REG_VBUS_HS */
  uint8_t Component_Reg  = 0;
  /* USER CODE END PREPARE_I2C_REG_VBUS_HS */
  HAL_StatusTypeDef status = HAL_OK;
   /* USER CODE BEGIN PREPARE_I2C_ADDR_VBUS_HS */
  uint8_t Component_Addr = 0 << 1;
  /* USER CODE END PREPARE_I2C_ADDR_VBUS_HS */
  /* USER CODE BEGIN PREPARE_DATA_VBUS_HS */
  uint8_t data = state;
  /* USER CODE END PREPARE_DATA_VBUS_HS */
  uint8_t data_tmp = 0;
  status = HAL_I2C_Mem_Read(&hi2c1, Component_Addr, (uint16_t)Component_Reg, I2C_MEMADD_SIZE_8BIT, &data_tmp, 1, 100);
  data |= data_tmp;
  status = HAL_I2C_Mem_Write(&hi2c1,Component_Addr,(uint16_t)Component_Reg, I2C_MEMADD_SIZE_8BIT,&data, 1, 100);
  /* USER CODE BEGIN CHECK_STATUS_VBUS_HS */
  /* Check the communication status */
  if(status != HAL_OK)
  {

  }
  /* USER CODE END CHECK_STATUS_VBUS_HS */
}

