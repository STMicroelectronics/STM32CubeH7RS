/**
  ******************************************************************************
  * @file    stm32h7rsxx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
  *          This file should be copied to the application folder and renamed
  *          to stm32h7rsxx_hal_msp.c
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
#include "com.h"

/** @addtogroup STM32H7RSxx_HAL_Driver
  * @{
  */

/** @defgroup HAL_MSP HAL MSP module driver
  * @brief HAL MSP module.
  * @{
  */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief  Initialize the Global MSP.
  * @retval None
  */
void HAL_MspInit(void)
{
  /* NOTE : This function is generated automatically by STM32CubeMX and eventually
            modified by the user
   */

  /* Configure the system Power Supply */
  __HAL_RCC_SBS_CLK_ENABLE();

  /* System interrupt init*/

  /* Enable the XSPIM_P1 interface */
  HAL_PWREx_EnableXSPIM1();

  /* Enable the XSPIM_P2 interface */
  HAL_PWREx_EnableXSPIM2();

  /* high speed low voltage config */
  HAL_SBS_EnableIOSpeedOptimize(SBS_IO_XSPI1_HSLV);
  HAL_SBS_EnableIOSpeedOptimize(SBS_IO_XSPI2_HSLV);

  __HAL_RCC_SBS_CLK_ENABLE();
}

/**
  * @brief  DeInitialize the Global MSP.
  * @retval None
  */
void HAL_MspDeInit(void)
{
  /* NOTE : This function is generated automatically by STM32CubeMX and eventually
            modified by the user
   */
}

/**
  * @brief  Initialize the UART MSP.
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if (huart->Instance == COM_UART)
  {
    /* Peripheral Clock Enable */
    COM_UART_CLK_ENABLE();

    /* GPIO Ports Clock Enable */
    COM_UART_TX_GPIO_CLK_ENABLE();
    COM_UART_RX_GPIO_CLK_ENABLE();

    /*Configure GPIO pins : COM_UART_TX_Pin  */
    GPIO_InitStruct.Pin = COM_UART_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = COM_UART_TX_AF;
    HAL_GPIO_Init(COM_UART_TX_GPIO_PORT, &GPIO_InitStruct);

    /*Configure GPIO pins : COM_UART_RX_Pin  */
    GPIO_InitStruct.Pin = COM_UART_RX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = COM_UART_RX_AF;
    HAL_GPIO_Init(COM_UART_RX_GPIO_PORT, &GPIO_InitStruct);
  }
}

/**
  * @brief  DeInitialize the UART MSP.
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  if (huart->Instance == COM_UART)
  {
    /* Peripheral clock disable */
    COM_UART_CLK_DISABLE();

    HAL_GPIO_DeInit(COM_UART_TX_GPIO_PORT, COM_UART_TX_PIN);
    HAL_GPIO_DeInit(COM_UART_RX_GPIO_PORT, COM_UART_RX_PIN);
  }
}

/**
  * @brief XSPI MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hxspi: XSPI handle pointer
  * @retval None
  */
void HAL_XSPI_MspInit(XSPI_HandleTypeDef *hxspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  __HAL_RCC_XSPIM_CLK_ENABLE();

  /* Enable only GPIO pin of XSPI port 2 for the external flash */
  __HAL_RCC_GPION_CLK_ENABLE();

  GPIO_InitStruct.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | \
                              GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_8 | \
                              GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Alternate = GPIO_AF9_XSPIM_P2;
  HAL_GPIO_Init(GPION, &GPIO_InitStruct);

  if (hxspi->Instance == XSPI1)
  {
    /* USER CODE BEGIN XSPI1_MspInit 0 */

    /* USER CODE END XSPI1_MspInit 0 */

    /** Initializes the peripherals clock
      */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_XSPI1;
    PeriphClkInit.Xspi1ClockSelection = RCC_XSPI1CLKSOURCE_PLL2S;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_RCC_XSPI1_CLK_ENABLE();

    /* NVIC configuration for XSPI interrupt */
    HAL_NVIC_SetPriority(XSPI1_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ(XSPI1_IRQn);

    /* USER CODE BEGIN XSPI1_MspInit 1 */

    /* USER CODE END XSPI1_MspInit 1 */
  }
  else if (hxspi->Instance == XSPI2)
  {
    /* Initializes the peripherals clock */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_XSPI2;
    PeriphClkInit.Xspi2ClockSelection = RCC_XSPI2CLKSOURCE_PLL2S;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* Enable the XSPI memory interface clock */
    __HAL_RCC_XSPI2_CLK_ENABLE();

    /* NVIC configuration for XSPI interrupt */
    HAL_NVIC_SetPriority(XSPI2_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ(XSPI2_IRQn);
  }
  else
  {
    Error_Handler();
  }
}

/**
  * @brief XSPI MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @retval None
  */
void HAL_XSPI_MspDeInit(XSPI_HandleTypeDef *hxspi)
{
  /* Disable and reset the HPDMA clock */
  __HAL_RCC_HPDMA1_FORCE_RESET();
  __HAL_RCC_HPDMA1_RELEASE_RESET();
  __HAL_RCC_HPDMA1_CLK_DISABLE();

  /* Disable XSPI port 2 pins */
  HAL_GPIO_DeInit(GPION, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | \
                  GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_8 | \
                  GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11);

  if (hxspi->Instance == XSPI1)
  {
    /* Disable xSPI interrupts */
    HAL_NVIC_DisableIRQ(XSPI1_IRQn);
    /* Reset the xSPI memory interface */
    __HAL_RCC_XSPI1_FORCE_RESET();
    __HAL_RCC_XSPI1_RELEASE_RESET();

    /* Disable the xSPI memory interface clock */
    __HAL_RCC_XSPI1_CLK_DISABLE();
  }

  if (hxspi->Instance == XSPI2)
  {
    /* Disable xSPI interrupts */
    HAL_NVIC_DisableIRQ(XSPI2_IRQn);
    /* Reset the xSPI memory interface */
    __HAL_RCC_XSPI2_FORCE_RESET();
    __HAL_RCC_XSPI2_RELEASE_RESET();

    /* Disable the xSPI memory interface clock */
    __HAL_RCC_XSPI2_CLK_DISABLE();
  }
}

/**
  * @brief  Initialize the PPP MSP.
  * @retval None
  */
/*
void HAL_PPP_MspInit(void)
{
}
*/

/**
  * @brief  DeInitialize the PPP MSP.
  * @retval None
  */
/*
void HAL_PPP_MspDeInit(void)
{
}
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
