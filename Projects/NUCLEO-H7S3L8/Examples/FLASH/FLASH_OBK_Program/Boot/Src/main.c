/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#define KEY_128BIT_INDEX 28U
#define KEY_256BIT_INDEX 29U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

RNG_HandleTypeDef hrng;

/* USER CODE BEGIN PV */
FLASH_KeyConfigTypeDef KeyConfig;
uint32_t ProgKey128[4];
uint32_t ProgKey256[8];
uint32_t ReadKey128[4];
uint32_t ReadKey256[8];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_RNG_Init(void);
/* USER CODE BEGIN PFP */

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
  uint8_t index;
  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();
  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  if (BSP_LED_Init(LED_GREEN) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  if (BSP_LED_Init(LED_RED) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_RNG_Init();
  /* USER CODE BEGIN 2 */
  /* Initialize key table with random values */
  for (index = 0U; index < 4U; index++)
  {
    if (HAL_RNG_GenerateRandomNumber(&hrng, &(ProgKey128[index])) != HAL_OK)
    {
      Error_Handler();
    }
  }

  for (index = 0U; index < 8U; index++)
  {
    if (HAL_RNG_GenerateRandomNumber(&hrng, &(ProgKey256[index])) != HAL_OK)
    {
      Error_Handler();
    }
  }

  /* Unlock Flash interface */
  if (HAL_FLASH_Unlock() != HAL_OK)
  {
    Error_Handler();
  }

  /* Unlock Option Bytes access */
  if (HAL_FLASH_OB_Unlock() != HAL_OK)
  {
    Error_Handler();
  }

  /* ----------- Programming keys with several size and HDP level ----------- */
  /* Program a 128-bit key */
  KeyConfig.Size = FLASH_KEY_128_BITS;
  KeyConfig.Index = KEY_128BIT_INDEX;
  KeyConfig.HDPLLevel = FLASH_KEY_LEVEL_CURRENT;
  if (HAL_FLASHEx_KeyConfig(&KeyConfig, ProgKey128) != HAL_OK)
  {
    Error_Handler();
  }

  /* Program a 256-bit key */
  KeyConfig.Size = FLASH_KEY_256_BITS;
  KeyConfig.Index = KEY_256BIT_INDEX;
  KeyConfig.HDPLLevel = FLASH_KEY_LEVEL_NEXT;
  if (HAL_FLASHEx_KeyConfig(&KeyConfig, ProgKey256) != HAL_OK)
  {
    Error_Handler();
  }
  /* ----------------------- End of keys programming ------------------------ */

  /* --------------- Check available keys according HDP level --------------- */
  /* --> Current HDL level */
  KeyConfig.HDPLLevel = FLASH_KEY_LEVEL_CURRENT;

  /* Read back 128-bit key value and check consistency */
  KeyConfig.Size = FLASH_KEY_128_BITS;
  KeyConfig.Index = KEY_128BIT_INDEX;
  if (HAL_FLASHEx_GetKey(&KeyConfig, ReadKey128) != HAL_OK)
  {
    Error_Handler();
  }

  /* 128-bit key has been programmed for current level => no error expected */
  for (index = 0U; index < 4U; index++)
  {
    if (ProgKey128[index] != ReadKey128[index])
    {
      Error_Handler();
    }
  }

  /* Read back 256-bit key value and check consistency => no key expected  */
  KeyConfig.Size = FLASH_KEY_256_BITS;
  KeyConfig.Index = KEY_256BIT_INDEX;
  if (HAL_FLASHEx_GetKey(&KeyConfig, ReadKey256) == HAL_OK)
  {
    Error_Handler();
  }

  /* 256-bit key has been programmed for next level => no key expected */
  if ((HAL_FLASH_GetError() & HAL_FLASH_ERROR_KV) == 0U)
  {
    Error_Handler();
  }

  /* --> Next HDP level */
  KeyConfig.HDPLLevel = FLASH_KEY_LEVEL_NEXT;

  /* Read back 128-bit key value and check consistency => no key expected  */
  KeyConfig.Size = FLASH_KEY_128_BITS;
  KeyConfig.Index = KEY_128BIT_INDEX;
  if (HAL_FLASHEx_GetKey(&KeyConfig, ReadKey128) == HAL_OK)
  {
    Error_Handler();
  }

  /* 128-bit key has been programmed for current level => no key expected */
  if ((HAL_FLASH_GetError() & HAL_FLASH_ERROR_KV) == 0U)
  {
    Error_Handler();
  }

  /* Read back 256-bit key value and check consistency */
  KeyConfig.Size = FLASH_KEY_256_BITS;
  KeyConfig.Index = KEY_256BIT_INDEX;
  if (HAL_FLASHEx_GetKey(&KeyConfig, ReadKey256) != HAL_OK)
  {
    Error_Handler();
  }

  /* 256-bit key has been programmed for next level => no error expected */
  for (index = 0U; index < 8U; index++)
  {
    if (ProgKey256[index] != ReadKey256[index])
    {
      Error_Handler();
    }
  }
  /* ------------------ End of check for current HDP level ------------------ */

  /* ---------------------------- Increment HDPL ---------------------------- */
  HAL_SBS_IncrementHDPLValue();

  /* --------------- Check available keys according HDP level --------------- */
  /* --> Current HDL level */
  KeyConfig.HDPLLevel = FLASH_KEY_LEVEL_CURRENT;

  /* Read back 128-bit key value and check consistency */
  KeyConfig.Size = FLASH_KEY_128_BITS;
  KeyConfig.Index = KEY_128BIT_INDEX;
  if (HAL_FLASHEx_GetKey(&KeyConfig, ReadKey128) == HAL_OK)
  {
    Error_Handler();
  }

  /* 128-bit key has been programmed at previous level => no key expected */
  if ((HAL_FLASH_GetError() & HAL_FLASH_ERROR_KV) == 0U)
  {
    Error_Handler();
  }

  /* Read back 256-bit key value and check consistency */
  KeyConfig.Size = FLASH_KEY_256_BITS;
  KeyConfig.Index = KEY_256BIT_INDEX;
  if (HAL_FLASHEx_GetKey(&KeyConfig, ReadKey256) != HAL_OK)
  {
    Error_Handler();
  }

  /* 256-bit key has been programmed at this level => no error expected */
  for (index = 0U; index < 8U; index++)
  {
    if (ProgKey256[index] != ReadKey256[index])
    {
      Error_Handler();
    }
  }
  /* ------------------ End of check for next HDP level ------------------ */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    (void)BSP_LED_Toggle(LED_GREEN);
    HAL_Delay(200U);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL1.PLLM = 32;
  RCC_OscInitStruct.PLL1.PLLN = 300;
  RCC_OscInitStruct.PLL1.PLLP = 1;
  RCC_OscInitStruct.PLL1.PLLQ = 2;
  RCC_OscInitStruct.PLL1.PLLR = 2;
  RCC_OscInitStruct.PLL1.PLLS = 2;
  RCC_OscInitStruct.PLL1.PLLT = 2;
  RCC_OscInitStruct.PLL1.PLLFractional = 0;
  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK4|RCC_CLOCKTYPE_PCLK5;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
  RCC_ClkInitStruct.APB5CLKDivider = RCC_APB5_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  hrng.Init.ClockErrorDetection = RNG_CED_DISABLE;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /* MPU Configuration */

static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while (1)
  {
    (void)BSP_LED_Toggle(LED_RED);
    HAL_Delay(200U);
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
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
