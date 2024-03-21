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
#define MEMORY_BASE              SRAM1_AXI_BASE        /* AXI-SRAM base address */
#define MEMORY_SIZE              SRAM1_AXI_SIZE        /* AXI-SRAM size         */
#define RAMECC_INSTANCE          RAMECC1_Monitor0

#define DATA_VALUE_64            ((uint64_t) 0x123456789ABCDEFF)

//#define ACTIVATE_ECC_ERROR       1

#if defined(ACTIVATE_ECC_ERROR)
#define MASK64_CHANGE_1BIT       ((uint64_t) 0xFFFFFFFFFFFFEFFF)
#define EXPECTED_64_CODE_MASK_1  ((uint32_t) 0x41)
#define ADDRESS_FAULT_ECC        (uint32_t)(MEMORY_BASE + (0x40))
#endif /* defined(ACTIVATE_ECC_ERROR)*/
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void RAMECC_DetectErrorCallback(RAMECC_HandleTypeDef *hramecc);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
RAMECC_HandleTypeDef hramecc;
__IO uint32_t RAMECC_Error_Detected = 0;
__IO uint32_t CurrentData =0 ;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  __IO uint32_t*  pcurrent_address_32bit;
  __IO uint64_t* pcurrent_address_64bit;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Update SystemCoreClock variable according to RCC registers values. */
  SystemCoreClockUpdate();

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */
  /* Configure LD1, LD2, LD3 */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD2);
  BSP_LED_Init(LD3);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

  /* Fill ITCM */
  pcurrent_address_32bit = (uint32_t*) ITCM_BASE;
  do
  {
    *pcurrent_address_32bit = 0U;
    pcurrent_address_32bit++;
  }while( ((uint32_t) pcurrent_address_32bit) < (ITCM_BASE + ITCM_SIZE));

  /* Initialize the all DTCM memory with 0 */
  pcurrent_address_32bit = (uint32_t*)  DTCM_BASE;
  do
  {
    *pcurrent_address_32bit = 0U;
    pcurrent_address_32bit++;
  }while( ((uint32_t)pcurrent_address_32bit) < (DTCM_BASE + DTCM_SIZE));

  /* Clear ITCM SR flags */
  *(uint32_t*) 0x52009044 = 0x0U;

  /* Clear DTCM SR flags */
  *(uint32_t*) 0x52009064 = 0x0U;
  *(uint32_t*) 0x52009084 = 0x0U;

  /* Initialize the memory with 0 */
  pcurrent_address_64bit = (uint64_t*) MEMORY_BASE;
  do
  {
    *pcurrent_address_64bit = (uint64_t) 0;
    if (*pcurrent_address_64bit != (uint64_t) 0)
    {
      Error_Handler();
    }
    pcurrent_address_64bit++;
  }while( ((uint32_t)pcurrent_address_64bit) < (MEMORY_BASE + MEMORY_SIZE));

  /* Write DATA to the memory */
  pcurrent_address_64bit = (uint64_t*) MEMORY_BASE;
  do
  {
    *pcurrent_address_64bit = DATA_VALUE_64;

    /* Read DATA from the memory */
    if (*pcurrent_address_64bit != DATA_VALUE_64)
    {
      return 2; /* KO */
    }
    pcurrent_address_64bit++;
  }while( ((uint32_t)pcurrent_address_64bit) < (MEMORY_BASE + MEMORY_SIZE));

  /* Select the monitor for SRAM ECC analysis */
  hramecc.Instance = RAMECC_INSTANCE;

#if defined(ACTIVATE_ECC_ERROR)
/* block ECC memory access for ECC fault injection test */
  SET_BIT(hramecc.Instance->CR, (0x2 << 16));

  /* Change 1 bit of DATA */
  pcurrent_address_64bit = (uint64_t*) ADDRESS_FAULT_ECC;
  *pcurrent_address_64bit = (uint64_t)(DATA_VALUE_64 & MASK64_CHANGE_1BIT);

  HAL_Delay(500);
  /* Return to normal mode */
  CLEAR_BIT(hramecc.Instance->CR, (0x3 << 16));
#endif /* defined(ACTIVATE_ECC_ERROR)*/

  /* Initialize the RAMECC peripheral */
  if (HAL_RAMECC_Init(&hramecc) != HAL_OK)
  {
    Error_Handler();
  }

  /* Select Callbacks functions called after single or double error detection */
  if (HAL_RAMECC_RegisterCallback(&hramecc, HAL_RAMECC_SE_DETECT_CB_ID, RAMECC_DetectErrorCallback) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable monitor notifications */
  /* ECC single error notification and ECC double error notification */
  if (HAL_RAMECC_EnableNotification(&hramecc, (RAMECC_IT_MONITOR_SINGLEERR_R))!= HAL_OK)
  {
    Error_Handler();
  }

  /* Start Monitor : Enable latching failing information
     Failing information : * Failing address
                            * Failing Data Low
                            * Failing Data High
                            * Hamming bits injected
  */
  if (HAL_RAMECC_StartMonitor(&hramecc) != HAL_OK)
  {
    Error_Handler();
  }

  /* NVIC configuration for RAMECC interrupt */
  /* Priority: high-priority */
  HAL_NVIC_SetPriority(RAMECC_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(RAMECC_IRQn);

  /* Analyse all memory */
  pcurrent_address_64bit = (uint64_t*) MEMORY_BASE;
  do
  {
    if (*pcurrent_address_64bit != (uint64_t) DATA_VALUE_64)
    {
      Error_Handler();
    }
    pcurrent_address_64bit++;
  }while( ((uint32_t)pcurrent_address_64bit) < (MEMORY_BASE + MEMORY_SIZE));


#if defined(ACTIVATE_ECC_ERROR)

 /* Check that SRAM word generate a single error interrupt */
  if(RAMECC_Error_Detected != 1)
  {
    Error_Handler();
  }

  /* Check RAMECC data low */
  if(HAL_RAMECC_GetFailingDataLow(&hramecc) !=  (uint32_t)(DATA_VALUE_64 & MASK64_CHANGE_1BIT))
  {
    Error_Handler();
  }

  /* Check RAMECC data high */
  if(HAL_RAMECC_GetFailingDataHigh(&hramecc) != ( (uint32_t)(DATA_VALUE_64 >> 32) & (uint32_t)(MASK64_CHANGE_1BIT >> 32)))
  {
    Error_Handler();
  }

  /* Check latched single error data address */
  uint32_t address_tets = HAL_RAMECC_GetFailingAddress(&hramecc);
  if (address_tets  != ((ADDRESS_FAULT_ECC- MEMORY_BASE) /8))
  {
    Error_Handler();
  }

  if(HAL_RAMECC_GetHammingErrorCode(&hramecc) != EXPECTED_64_CODE_MASK_1)
  {
    Error_Handler();
  }
#endif /* defined(ACTIVATE_ECC_ERROR)*/

  /* Turn LD1 On */
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
  * @brief  Single or double ECC error detected callback.
  * @param  hramecc : RAMECC handle
  * @retval None
  */
static void RAMECC_DetectErrorCallback(RAMECC_HandleTypeDef *hramecc)
{
  RAMECC_Error_Detected++;
  BSP_LED_On(LD2);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* Turn on the LD3 */
  BSP_LED_On(LD3);
 /* User can add his own implementation to report the HAL error return state */
  while(1)
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
  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
