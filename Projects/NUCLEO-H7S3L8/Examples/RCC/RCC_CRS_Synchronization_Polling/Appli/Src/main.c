/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    RCC/RCC_CRS_Synchronization_Polling/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to use the RCC HAL API to configure the
  *          Clock Recovery System (CRS) in polling mode.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void CRS_Init(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  uint32_t status = RCC_CRS_TIMEOUT;
  RCC_CRSSynchroInfoTypeDef syncinfo = {0};
  RCC_CRSInitTypeDef crsinitstruct = {0};

  /* USER CODE END 1 */
/* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Update SystemCoreClock variable according to RCC registers values. */
  SystemCoreClockUpdate();

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  /* Configure LD1 and LD3 */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD3);

  /* CRS initialization (enable HSI48 and LSE oscillators and then enable CRS clock */
  CRS_Init();

  /* HSI48 Synchronization with LSE frequency */
  crsinitstruct.Prescaler             = RCC_CRS_SYNC_DIV1;
  crsinitstruct.Source                = RCC_CRS_SYNC_SOURCE_LSE;
  crsinitstruct.Polarity              = RCC_CRS_SYNC_POLARITY_FALLING;
  /* F(LSE)=32.768kHz then Reload=(f(Target)/f(LSE))-1= 0x5B7*/
  crsinitstruct.ReloadValue           = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(HSI48_VALUE, LSE_VALUE);
  /* Felim value calculated like this FELIM = (fTARGET / fSYNC) * STEP[%] / 100% / 2 with STEP=0.14% then FELIM = 2 */
  crsinitstruct.ErrorLimitValue       = 2;
  /* Change the HSI trimming value to see the automatic calibration performed by CRS */
  crsinitstruct.HSI48CalibrationValue = 0x00;

  /* Start automatic synchronization using polling mode */
  HAL_RCCEx_CRSConfig(&crsinitstruct);

  /* Wait for synchronization OK event */
  do
  {
    /* Check status of CRS synchronization */
    /* Timeout is based on F(LSE) then less than 1 ms*/
    status = HAL_RCCEx_CRSWaitSynchronization(1);

    if ((status & RCC_CRS_TIMEOUT) == RCC_CRS_TIMEOUT)
    {
      /* Timeout issue. May have a problem with synchronization frequency */
      Error_Handler();
    }

    /* Adapt synchronization input parameters in case of SYNC error or SYNC miss event */
    if (((status & RCC_CRS_SYNCERR) == RCC_CRS_SYNCERR) || ((status & RCC_CRS_SYNCMISS) == RCC_CRS_SYNCMISS))
    {
      HAL_RCCEx_CRSGetSynchronizationInfo(&syncinfo);

      /* User can check different parameters returned in synchronization info structure*/
      /* and restart a new synchronization in changing input parameters */

      HAL_RCCEx_CRSConfig(&crsinitstruct);
    }
  }
  while ((status & RCC_CRS_SYNCOK) != RCC_CRS_SYNCOK);

  /* Power on LD1 */
  BSP_LED_On(LD1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief  This function initializes the clock configuration for CRS.
  * @param  None
  * @retval None
  */
static void CRS_Init(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct =  {0};

  HAL_RCC_GetOscConfig(&RCC_OscInitStruct);

  if (RCC_OscInitStruct.HSI48State != RCC_HSI48_ON)
  {
    /* Enable HSI48 and LSE Oscillator*/
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.HSI48State     = RCC_HSI48_ON;
    RCC_OscInitStruct.LSEState       = RCC_LSE_ON;
    RCC_OscInitStruct.PLL1.PLLState  = RCC_PLL_NONE; /*!< PLL1 configuration unchanged */
    RCC_OscInitStruct.PLL2.PLLState  = RCC_PLL_NONE; /*!< PLL2 configuration unchanged */
    RCC_OscInitStruct.PLL3.PLLState  = RCC_PLL_NONE; /*!< PLL3 configuration unchanged */
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
      Error_Handler();
    }
  }

  /* Enable CRS clock*/
  __HAL_RCC_CRS_CLK_ENABLE();

  /* Output HSI48 MCO pin(PA8) */
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI48, RCC_MCODIV_1);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* Turn LD3 on */
  BSP_LED_On(LD3);

  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
