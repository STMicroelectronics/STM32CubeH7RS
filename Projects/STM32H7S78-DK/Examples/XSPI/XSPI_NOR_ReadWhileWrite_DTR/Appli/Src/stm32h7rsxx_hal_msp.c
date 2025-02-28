/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32h7rsxx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
extern DMA_HandleTypeDef handle_HPDMA1_Channel1;
extern DMA_HandleTypeDef handle_HPDMA1_Channel0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{

  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  /* System interrupt init*/

  /* Enable the XSPIM_P2 interface */
  HAL_PWREx_EnableXSPIM2();

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/* USER CODE BEGIN 1 */
/**
* @brief XSPI MSP Initialization
* This function configures the hardware resources used in this example
* @param hxspi: XSPI handle pointer
* @retval None
*/
void HAL_XSPI_MspInit(XSPI_HandleTypeDef* hxspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(hxspi->Instance==XSPI1)
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

    /* Peripheral clock enable */
    __HAL_RCC_XSPIM_CLK_ENABLE();
    __HAL_RCC_XSPI1_CLK_ENABLE();

    HAL_NVIC_SetPriority(XSPI1_IRQn, 0x01, 1);
    HAL_NVIC_EnableIRQ(XSPI1_IRQn);

    __HAL_RCC_GPION_CLK_ENABLE();
    /**XSPI1 GPIO Configuration
    PN3     ------> XSPIM_P2_IO1
    PN0     ------> XSPIM_P2_DQS0
    PN11     ------> XSPIM_P2_IO7
    PN10     ------> XSPIM_P2_IO6
    PN9     ------> XSPIM_P2_IO5
    PN2     ------> XSPIM_P2_IO0
    PN6     ------> XSPIM_P2_CLK
    PN8     ------> XSPIM_P2_IO4
    PN4     ------> XSPIM_P2_IO2
    PN5     ------> XSPIM_P2_IO3
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_0|GPIO_PIN_11|GPIO_PIN_10
                          |GPIO_PIN_9|GPIO_PIN_2|GPIO_PIN_6|GPIO_PIN_8
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_XSPIM_P2;
    HAL_GPIO_Init(GPION, &GPIO_InitStruct);

    /* XSPI1 DMA Init */
    /* HPDMA1_REQUEST_XSPI1 Init */
    handle_HPDMA1_Channel1.Instance = HPDMA1_Channel1;
    handle_HPDMA1_Channel1.Init.Request = HPDMA1_REQUEST_XSPI1;
    handle_HPDMA1_Channel1.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    handle_HPDMA1_Channel1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    handle_HPDMA1_Channel1.Init.SrcInc = DMA_SINC_INCREMENTED;
    handle_HPDMA1_Channel1.Init.DestInc = DMA_DINC_FIXED;
    handle_HPDMA1_Channel1.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
    handle_HPDMA1_Channel1.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
    handle_HPDMA1_Channel1.Init.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
    handle_HPDMA1_Channel1.Init.SrcBurstLength = 4;
    handle_HPDMA1_Channel1.Init.DestBurstLength = 4;
    handle_HPDMA1_Channel1.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT1;
    handle_HPDMA1_Channel1.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    handle_HPDMA1_Channel1.Init.Mode = DMA_NORMAL;
    if (HAL_DMA_Init(&handle_HPDMA1_Channel1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hxspi, hdmatx, handle_HPDMA1_Channel1);

    if (HAL_DMA_ConfigChannelAttributes(&handle_HPDMA1_Channel1, DMA_CHANNEL_NPRIV) != HAL_OK)
    {
      Error_Handler();
    }

    /* HPDMA1_REQUEST_XSPI1 Init */
    handle_HPDMA1_Channel0.Instance = HPDMA1_Channel0;
    handle_HPDMA1_Channel0.Init.Request = HPDMA1_REQUEST_XSPI1;
    handle_HPDMA1_Channel0.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    handle_HPDMA1_Channel0.Init.Direction = DMA_PERIPH_TO_MEMORY;
    handle_HPDMA1_Channel0.Init.SrcInc = DMA_SINC_FIXED;
    handle_HPDMA1_Channel0.Init.DestInc = DMA_DINC_INCREMENTED;
    handle_HPDMA1_Channel0.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
    handle_HPDMA1_Channel0.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
    handle_HPDMA1_Channel0.Init.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
    handle_HPDMA1_Channel0.Init.SrcBurstLength = 4;
    handle_HPDMA1_Channel0.Init.DestBurstLength = 4;
    handle_HPDMA1_Channel0.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT1|DMA_DEST_ALLOCATED_PORT0;
    handle_HPDMA1_Channel0.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    handle_HPDMA1_Channel0.Init.Mode = DMA_NORMAL;
    if (HAL_DMA_Init(&handle_HPDMA1_Channel0) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hxspi, hdmarx, handle_HPDMA1_Channel0);

    if (HAL_DMA_ConfigChannelAttributes(&handle_HPDMA1_Channel0, DMA_CHANNEL_NPRIV) != HAL_OK)
    {
      Error_Handler();
    }

  /* USER CODE BEGIN XSPI1_MspInit 1 */

  /* USER CODE END XSPI1_MspInit 1 */

  }

}

/**
* @brief XSPI MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hxspi: XSPI handle pointer
* @retval None
*/
void HAL_XSPI_MspDeInit(XSPI_HandleTypeDef* hxspi)
{
  if(hxspi->Instance==XSPI1)
  {
  /* USER CODE BEGIN XSPI1_MspDeInit 0 */

  /* USER CODE END XSPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_XSPIM_CLK_DISABLE();
    __HAL_RCC_XSPI1_CLK_DISABLE();

    /**XSPI1 GPIO Configuration
    PN3     ------> XSPIM_P2_IO1
    PN0     ------> XSPIM_P2_DQS0
    PN11     ------> XSPIM_P2_IO7
    PN10     ------> XSPIM_P2_IO6
    PN9     ------> XSPIM_P2_IO5
    PN2     ------> XSPIM_P2_IO0
    PN6     ------> XSPIM_P2_CLK
    PN8     ------> XSPIM_P2_IO4
    PN4     ------> XSPIM_P2_IO2
    PN5     ------> XSPIM_P2_IO3
    */
    HAL_GPIO_DeInit(GPION, GPIO_PIN_3|GPIO_PIN_0|GPIO_PIN_11|GPIO_PIN_10
                          |GPIO_PIN_9|GPIO_PIN_2|GPIO_PIN_6|GPIO_PIN_8
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_1);

    /* XSPI1 DMA DeInit */
    HAL_DMA_DeInit(hxspi->hdmatx);
    HAL_DMA_DeInit(hxspi->hdmarx);
  /* USER CODE BEGIN XSPI1_MspDeInit 1 */

  /* USER CODE END XSPI1_MspDeInit 1 */
  }

}

/* USER CODE END 1 */
