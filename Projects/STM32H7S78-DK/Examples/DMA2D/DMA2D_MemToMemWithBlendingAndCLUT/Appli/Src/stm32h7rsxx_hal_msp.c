/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32h7rsxx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
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

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
* @brief DMA2D MSP Initialization
* This function configures the hardware resources used in this example
* @param hdma2d: DMA2D handle pointer
* @retval None
*/
void HAL_DMA2D_MspInit(DMA2D_HandleTypeDef* hdma2d)
{
  if(hdma2d->Instance==DMA2D)
  {
  /* USER CODE BEGIN DMA2D_MspInit 0 */

  /* USER CODE END DMA2D_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_DMA2D_CLK_ENABLE();
    /* DMA2D interrupt Init */
    HAL_NVIC_SetPriority(DMA2D_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2D_IRQn);
  /* USER CODE BEGIN DMA2D_MspInit 1 */

  /* USER CODE END DMA2D_MspInit 1 */
  }

}

/**
* @brief DMA2D MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hdma2d: DMA2D handle pointer
* @retval None
*/
void HAL_DMA2D_MspDeInit(DMA2D_HandleTypeDef* hdma2d)
{
  if(hdma2d->Instance==DMA2D)
  {
  /* USER CODE BEGIN DMA2D_MspDeInit 0 */

  /* USER CODE END DMA2D_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DMA2D_CLK_DISABLE();

    /* DMA2D interrupt DeInit */
    HAL_NVIC_DisableIRQ(DMA2D_IRQn);
  /* USER CODE BEGIN DMA2D_MspDeInit 1 */

  /* USER CODE END DMA2D_MspDeInit 1 */
  }

}

/**
* @brief LTDC MSP Initialization
* This function configures the hardware resources used in this example
* @param hltdc: LTDC handle pointer
* @retval None
*/
void HAL_LTDC_MspInit(LTDC_HandleTypeDef* hltdc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(hltdc->Instance==LTDC)
  {
  /* USER CODE BEGIN LTDC_MspInit 0 */
  RCC_OscInitTypeDef        RCC_OscInitStruct={0};
  /* USER CODE END LTDC_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInit.LtdcClockSelection = RCC_LTDCCLKSOURCE_PLL3R;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_LTDC_CLK_ENABLE();

    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**LTDC GPIO Configuration
    PF0     ------> LTDC_R2
    PG0     ------> LTDC_R7
    PB3(JTDO/TRACESWO)     ------> LTDC_R4
    PG2     ------> LTDC_HSYNC
    PE11     ------> LTDC_VSYNC
    PA12     ------> LTDC_B2
    PA11     ------> LTDC_B3
    PB4(NJTRST)     ------> LTDC_R3
    PA9     ------> LTDC_B5
    PG1     ------> LTDC_R6
    PA10     ------> LTDC_B4
    PG14     ------> LTDC_B1
    PA8     ------> LTDC_B6
    PG13     ------> LTDC_CLK
    PA15(JTDI)     ------> LTDC_R5
    PF7     ------> LTDC_G0
    PF9     ------> LTDC_R0
    PF10     ------> LTDC_R1
    PB12     ------> LTDC_G5
    PA0     ------> LTDC_G3
    PA1     ------> LTDC_G2
    PF11     ------> LTDC_B0
    PB14     ------> LTDC_DE
    PB15     ------> LTDC_G7
    PF15     ------> LTDC_G1
    PB11     ------> LTDC_G6
    PB13     ------> LTDC_G4
    PA6     ------> LTDC_B7
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_LTDC;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_14
                          |GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_LTDC;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_12|GPIO_PIN_14
                          |GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_LTDC;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_LTDC;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_11|GPIO_PIN_8|GPIO_PIN_15
                          |GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_LTDC;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_9|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_LTDC;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF10_LTDC;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF12_LTDC;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN LTDC_MspInit 1 */
  /* LCD clock configuration */
  /* Typical PCLK is 25 MHz so the PLL3R is configured to provide this clock */
  /* LCD clock configuration */

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL3.PLLSource = RCC_PLLSOURCE_HSI;

  RCC_OscInitStruct.PLL3.PLLM = 4;
  RCC_OscInitStruct.PLL3.PLLN = 25;
  RCC_OscInitStruct.PLL3.PLLP = 2;
  RCC_OscInitStruct.PLL3.PLLQ = 20;
  RCC_OscInitStruct.PLL3.PLLR = 16;
  RCC_OscInitStruct.PLL3.PLLS = 2;
  RCC_OscInitStruct.PLL3.PLLT = 2;
  RCC_OscInitStruct.PLL3.PLLFractional = 0;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
   /* Initialization error */
   while(1);
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInit.LtdcClockSelection   = RCC_LTDCCLKSOURCE_PLL3R;

  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    while(1);
  }
  /* USER CODE END LTDC_MspInit 1 */
  }

}

/**
* @brief LTDC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hltdc: LTDC handle pointer
* @retval None
*/
void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef* hltdc)
{
  if(hltdc->Instance==LTDC)
  {
  /* USER CODE BEGIN LTDC_MspDeInit 0 */

  /* USER CODE END LTDC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_LTDC_CLK_DISABLE();

    /**LTDC GPIO Configuration
    PF0     ------> LTDC_R2
    PG0     ------> LTDC_R7
    PB3(JTDO/TRACESWO)     ------> LTDC_R4
    PG2     ------> LTDC_HSYNC
    PE11     ------> LTDC_VSYNC
    PA12     ------> LTDC_B2
    PA11     ------> LTDC_B3
    PB4(NJTRST)     ------> LTDC_R3
    PA9     ------> LTDC_B5
    PG1     ------> LTDC_R6
    PA10     ------> LTDC_B4
    PG14     ------> LTDC_B1
    PA8     ------> LTDC_B6
    PG13     ------> LTDC_CLK
    PA15(JTDI)     ------> LTDC_R5
    PF7     ------> LTDC_G0
    PF9     ------> LTDC_R0
    PF10     ------> LTDC_R1
    PB12     ------> LTDC_G5
    PA0     ------> LTDC_G3
    PA1     ------> LTDC_G2
    PF11     ------> LTDC_B0
    PB14     ------> LTDC_DE
    PB15     ------> LTDC_G7
    PF15     ------> LTDC_G1
    PB11     ------> LTDC_G6
    PB13     ------> LTDC_G4
    PA6     ------> LTDC_B7
    */
    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_0|GPIO_PIN_7|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_15);

    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_14
                          |GPIO_PIN_13);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_12|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_11|GPIO_PIN_13);

    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_11);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_12|GPIO_PIN_11|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_8|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1
                          |GPIO_PIN_6);

  /* USER CODE BEGIN LTDC_MspDeInit 1 */

  /* USER CODE END LTDC_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
