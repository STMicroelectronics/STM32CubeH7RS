/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    RCC/RCC_CRS_Synchronization_IT/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to use the RCC HAL API to configure the
  *          Clock Recovery System (CRS) in Interrupt mode.
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
#define CRS_STATUS_INIT       0
#define CRS_STATUS_SYNCOK     1
#define CRS_STATUS_SYNCWARN   2
#define CRS_STATUS_SYNCMISS   3
#define CRS_STATUS_SYNCERR    4

#define CRS_MAX_FELIM_VALUE   0xFFU
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
RCC_CRSInitTypeDef CRSInitStruct = {0};
/* Variable to save SYNC status*/
__IO uint32_t CRSSyncStatus = CRS_STATUS_INIT;
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
  RCC_CRSSynchroInfoTypeDef syncinfo = {0};
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
  /* Configure LD1,  LD2 and LD3 */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD2);
  BSP_LED_Init(LD3);

  /* CRS initialization (enable HSI48 and LSE oscillators and then enable CRS clock */
  /* Enable CRS interrupt */
  CRS_Init();

  /* HSI48 Synchronization without synchronization frequency (source set to GPIO */
  /* which is not configured)*/
  CRSInitStruct.Prescaler             = RCC_CRS_SYNC_DIV1;
  CRSInitStruct.Source                = RCC_CRS_SYNC_SOURCE_GPIO;
  CRSInitStruct.Polarity              = RCC_CRS_SYNC_POLARITY_FALLING;
  /* F(LSE)=32.768kHz then Reload=(f(Target)/f(LSE))-1= 0x5B7*/
  CRSInitStruct.ReloadValue           = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(HSI48_VALUE, LSE_VALUE);
  /* Felim value calculated like this FELIM = (fTARGET / fSYNC) * STEP[%] / 100% / 2 with STEP=0.14% then FELIM = 2 */
  CRSInitStruct.ErrorLimitValue       = 2;
  /* Change the HSI trimming value to see the automatic calibration performed by CRS */
  CRSInitStruct.HSI48CalibrationValue = 0x00;

  /* Start automatic synchronization*/
  HAL_RCCEx_CRSConfig(&CRSInitStruct);

  /* Enable CRS interrupts */
  __HAL_RCC_CRS_ENABLE_IT(RCC_CRS_IT_SYNCOK | RCC_CRS_IT_SYNCWARN | RCC_CRS_IT_ERR | RCC_CRS_IT_ESYNC);

  /* Simulate a SYNC EVENT which will raise a SYNC_MISS EVENT */
  HAL_RCCEx_CRSSoftwareSynchronizationGenerate();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    switch (CRSSyncStatus)
    {
      case CRS_STATUS_SYNCOK:
        /* Power on LD1 */
        BSP_LED_On(LD1);
        /* Power off LD2*/
        BSP_LED_Off(LD2);
        /* Power off LD3*/
        BSP_LED_Off(LD3);
        /* For visual comfort on LD1 apply delay */
        HAL_Delay(1000);
        /* Enable CRS SYNCWARN interrupt (in case it goes out of sync) */
        __HAL_RCC_CRS_ENABLE_IT(RCC_CRS_IT_SYNCWARN);
        break;

      case CRS_STATUS_SYNCMISS:
        /* Power off LD1 */
        BSP_LED_Off(LD1);
        /* Power off LD2*/
        BSP_LED_Off(LD2);
        /* Power on LD3*/
        BSP_LED_On(LD3);
        /* Perform a new configuration to LSE when SYNC MISS is detected. */
        /* Delay to see the LED power on switch from LD3 to LD1 */
        HAL_Delay(1000);

        /* Retrieve synchronization information */
        HAL_RCCEx_CRSGetSynchronizationInfo(&syncinfo);

        /* Change source to LSE and restart synchronization if necessary */
        if (CRSInitStruct.Source != RCC_CRS_SYNC_SOURCE_LSE)
        {
          CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_LSE;
          HAL_RCCEx_CRSConfig(&CRSInitStruct);

          /* Enable CRS interrupts after new CRS configuration */
          __HAL_RCC_CRS_ENABLE_IT(RCC_CRS_IT_SYNCOK | RCC_CRS_IT_SYNCWARN | RCC_CRS_IT_ERR | RCC_CRS_IT_ESYNC);
        }
        else
        {
          /* Enable CRS SYNCOK interrupt */
          __HAL_RCC_CRS_ENABLE_IT(RCC_CRS_IT_SYNCOK);
        }
        break;

      case CRS_STATUS_SYNCWARN:
        /* Power off LD1 */
        BSP_LED_Off(LD1);
        /* Power on LD2*/
        BSP_LED_On(LD2);
        /* Power off LD3*/
        BSP_LED_Off(LD3);
        HAL_Delay(100);
        /* Increase tolerance (FELIM value) until no more warning */
        CRSInitStruct.ErrorLimitValue += 1;
        /* Max value for FELIM has been reached? */
        if (CRSInitStruct.ErrorLimitValue > CRS_MAX_FELIM_VALUE)
        {
          Error_Handler();
        }
        HAL_RCCEx_CRSConfig(&CRSInitStruct);
        /* Enable CRS interrupts after new CRS configuration */
        __HAL_RCC_CRS_ENABLE_IT(RCC_CRS_IT_SYNCOK | RCC_CRS_IT_SYNCWARN | RCC_CRS_IT_ERR | RCC_CRS_IT_ESYNC);
        break;

      case CRS_STATUS_SYNCERR:
        /* Power off LD1 */
        BSP_LED_Off(LD1);
        /* Power off LD2*/
        BSP_LED_Off(LD2);
        /* Power on LD3*/
        BSP_LED_On(LD3);
        break;

      default:
        break;
    }
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

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

  /* Enable and set CRS interrupt*/
  HAL_NVIC_SetPriority(CRS_IRQn, 0x03, 0x00);
  HAL_NVIC_EnableIRQ(CRS_IRQn);

  /* Output HSI48 MCO pin(PA8) */
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI48, RCC_MCODIV_1);
}


/**
  * @brief  RCCEx Clock Recovery System SYNCOK interrupt callback.
  * @retval none
  */
void HAL_RCCEx_CRS_SyncOkCallback(void)
{
  CRSSyncStatus = CRS_STATUS_SYNCOK;

  /* Disable CRS SYNCOK interrupt since synchronization succeeded */
  __HAL_RCC_CRS_DISABLE_IT(RCC_CRS_IT_SYNCOK);
}

/**
  * @brief  RCCEx Clock Recovery System SYNCWARN interrupt callback.
  * @retval none
  */
void HAL_RCCEx_CRS_SyncWarnCallback(void)
{
  CRSSyncStatus = CRS_STATUS_SYNCWARN;
  /* Disable temporary CRS SYNCWARN interrupt to let the main loop to be execute */
  __HAL_RCC_CRS_DISABLE_IT(RCC_CRS_IT_SYNCWARN);
}

/**
  * @brief  RCCEx Clock Recovery System Expected SYNC interrupt callback.
  * @retval none
  */
void HAL_RCCEx_CRS_ExpectedSyncCallback(void)
{
}

/**
  * @brief  RCCEx Clock Recovery System Error interrupt callback.
  * @param  Combination of Error status.
  *         This parameter can be a combination of the following values:
  *           @arg @ref RCC_CRS_SYNCERR
  *           @arg @ref RCC_CRS_SYNCMISS
  *           @arg @ref RCC_CRS_TRIMOVF
  * @retval none
  */
void HAL_RCCEx_CRS_ErrorCallback(uint32_t Error)
{
  if ((Error & RCC_CRS_SYNCMISS) != RESET)
  {
    CRSSyncStatus = CRS_STATUS_SYNCMISS;
  }
  else if ((Error & RCC_CRS_SYNCERR) != RESET)
  {
    CRSSyncStatus = CRS_STATUS_SYNCERR;
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* LD3 is slowly blinking (1 sec. period) */
  while (1)
  {
    BSP_LED_Toggle(LD3);
    HAL_Delay(1000);
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

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
