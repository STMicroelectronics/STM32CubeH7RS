/**
  ******************************************************************************
  * @file    stm32h7rsxx_hal_msp_.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
  *          This file template is located in the HAL folder and should be copied
  *          to the user folder.
  *
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
#include "main.h"
#include "interfaces_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief This function configures FDCANx used hardware resources.
  * @param hfdcan FDCANx handle pointer.
  * @retval None.
  */
void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef *hfdcan)
{
  UNUSED(hfdcan);
  LL_GPIO_InitTypeDef gpio_initstruct;

  /* Enable all resources clocks ---------------------------------------------*/
  /* Enable used GPIOx clocks */
  FDCANx_GPIO_CLK_TX_ENABLE();
  FDCANx_GPIO_CLK_RX_ENABLE();

  /* Enable FDCANx clock */
  FDCANx_CLK_ENABLE();

  /* Common configuration for FDCANx PINs:
      Mode        : Alternate function.
      Output type : Push-Pull (No pull).
      Speed       : High speed.
  */
  gpio_initstruct.Mode       = LL_GPIO_MODE_ALTERNATE;
  gpio_initstruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  gpio_initstruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
  gpio_initstruct.Pull       = LL_GPIO_PULL_UP;
  gpio_initstruct.Alternate  = FDCANx_TX_AF;

  /* Configure FDCANx Tx PIN */
  gpio_initstruct.Pin = FDCANx_TX_PIN;

  if (LL_GPIO_Init(FDCANx_TX_GPIO_PORT, &gpio_initstruct) != SUCCESS)
  {
    Error_Handler();
  }

  /* Configure FDCANx Rx PIN */
  gpio_initstruct.Pin = FDCANx_RX_PIN;

  if (LL_GPIO_Init(FDCANx_RX_GPIO_PORT, &gpio_initstruct) != SUCCESS)
  {
    Error_Handler();
  }
}

/**
  * @brief  DeInitializes the FDCANx MSP.
  * @param  pfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval None.
  */
void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef *pfdcan)
{
  UNUSED(pfdcan);

  /* 1 - Reset peripherals */
  FDCANx_FORCE_RESET();
  FDCANx_RELEASE_RESET();

  /* 2 - De-initialize the GPIOx peripheral registers to their default reset values */
  HAL_GPIO_DeInit(FDCANx_TX_GPIO_PORT, FDCANx_TX_PIN);
  HAL_GPIO_DeInit(FDCANx_RX_GPIO_PORT, FDCANx_RX_PIN);
}
