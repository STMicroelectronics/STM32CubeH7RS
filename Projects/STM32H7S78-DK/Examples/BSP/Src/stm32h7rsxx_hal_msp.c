/**
  ******************************************************************************
  * @file    stm32h7rsxx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
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

/** @addtogroup STM32H7RSxx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief  Initializes the Global MSP.
  * @retval None
  */
void HAL_MspInit(void)
{
  /* NOTE : This function is generated automatically by STM32CubeMX and eventually
            modified by the user
   */
}

/**
  * @brief  DeInitializes the Global MSP.
  * @retval None
  */
void HAL_MspDeInit(void)
{
  /* NOTE : This function is generated automatically by STM32CubeMX and eventually
            modified by the user
   */
}

/**
  * @brief  I2S6 clock Config.
  * @param  hi2s I2S handle.
  * @param  SampleRate Audio sample rate used to play the audio stream.
  * @note   The PLL configuration done within this function assumes that
  *         the PLL input is HSI clock and that HSI is already enabled.
  * @retval HAL status.
  */
HAL_StatusTypeDef MX_I2S6_ClockConfig(I2S_HandleTypeDef *hi2s, uint32_t SampleRate)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2s);

  HAL_StatusTypeDef         status;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
  RCC_OscInitTypeDef        RCC_OscInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL2.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL2.PLLM = 4;  /* 16MHz */
  RCC_OscInitStruct.PLL2.PLLN = 25; /* 16*25 = 400MHz */
  RCC_OscInitStruct.PLL2.PLLQ = 8;  /* 400/8 = SPI6-> I2S 16KHz */
  RCC_OscInitStruct.PLL2.PLLP = 8;  /* 400/8 = MDF */
  RCC_OscInitStruct.PLL2.PLLR = 2;  /* NOT USED */
  RCC_OscInitStruct.PLL2.PLLS = 2;  /* SDMMC */
  RCC_OscInitStruct.PLL2.PLLT = 2;  /* NOT USED */
  RCC_OscInitStruct.PLL2.PLLFractional = 0;
  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_NONE;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    status = HAL_ERROR;
  }
  else
  {
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI6;
    PeriphClkInitStruct.Spi6ClockSelection   = RCC_SPI6CLKSOURCE_PLL2Q;
    status = HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADF1;
    PeriphClkInitStruct.Adf1ClockSelection   = RCC_ADF1CLKSOURCE_PLL2P;
    status |= HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
  }

  return status;
}

/**
  * @brief  ADF1 clock Config.
  * @param  hmdf MDF handle.
  * @param  SampleRate Audio sample rate used to record the audio stream.
  * @retval HAL status.
  */
HAL_StatusTypeDef MX_ADF1_ClockConfig(MDF_HandleTypeDef *hmdf, uint32_t SampleRate)
{
   /* Prevent unused argument(s) compilation warning */
  UNUSED(hmdf);

  HAL_StatusTypeDef         status;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
  RCC_OscInitTypeDef        RCC_OscInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL2.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL2.PLLM = 4;  /* 16MHz */
  RCC_OscInitStruct.PLL2.PLLN = 25; /* 16*25 = 400MHz */
  RCC_OscInitStruct.PLL2.PLLQ = 8;  /* 400/8 = SPI6-> I2S 16KHz */
  RCC_OscInitStruct.PLL2.PLLP = 8;  /* 400/8 = MDF */
  RCC_OscInitStruct.PLL2.PLLR = 2;  /* NOT USED */
  RCC_OscInitStruct.PLL2.PLLS = 2;  /* SDMMC */
  RCC_OscInitStruct.PLL2.PLLT = 2;  /* NOT USED */
  RCC_OscInitStruct.PLL2.PLLFractional = 0;
  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_NONE;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    status = HAL_ERROR;
  }
  else
  {
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI6;
    PeriphClkInitStruct.Spi6ClockSelection   = RCC_SPI6CLKSOURCE_PLL2Q;
    status = HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADF1;
    PeriphClkInitStruct.Adf1ClockSelection   = RCC_ADF1CLKSOURCE_PLL2P;
    status |= HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
  }

  return status;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

