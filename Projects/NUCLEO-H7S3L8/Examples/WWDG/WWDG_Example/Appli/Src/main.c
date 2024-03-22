/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    WWDG/WWDG_Example/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use the WWDG HAL API
  *          to update at regular period the WWDG counter and how to generate
  *          a software fault generating an MCU WWDG reset on expiry of a
  *          programmed time period.
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
#define WWDG_WINDOW    0x50
#define WWDG_COUNTER   0x7F
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

WWDG_HandleTypeDef hwwdg;

/* USER CODE BEGIN PV */
uint32_t WwdgStatus = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_WWDG_Init(void);
/* USER CODE BEGIN PFP */
static uint32_t TimeoutCalculation(uint32_t timevalue);

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
 uint32_t delay;
  /* STM32H7RSxx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
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
  /* Configure LD1 and LD2 */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD2);

  /*##-1- Check if the system has resumed from WWDG reset ####################*/
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != 0x00u)
  {
    /* WWDGRST flag set: Turn LD1 on and set WWDGStatus */
    WwdgStatus = 1;
    BSP_LED_On(LD1);

    /* Insert 4s delay */
    HAL_Delay(4000);

    /* Prior to clear WWDGRST flag: Turn LD1 off */
    BSP_LED_Off(LD1);
  }

  /* Clear reset flags in any case */
  __HAL_RCC_CLEAR_RESET_FLAGS();
  WwdgStatus = 0;

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_WWDG_Init();
  /* USER CODE BEGIN 2 */
  /* Configure USER push-button */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* calculate delay to enter window. Add 1ms to secure round number to upper number  */
  delay = TimeoutCalculation((hwwdg.Init.Counter-hwwdg.Init.Window) + 1) + 1;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* Toggle LD1 */
    BSP_LED_Toggle(LD1);

    /* Insert calculated delay */
    HAL_Delay(delay);

    if (HAL_WWDG_Refresh(&hwwdg) != HAL_OK)
    {
      Error_Handler();
    }

  }
  /* USER CODE END 3 */
}

/**
  * @brief WWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_WWDG_Init(void)
{

  /* USER CODE BEGIN WWDG_Init 0 */
     /*  Default WWDG Configuration:
      1] Set WWDG counter to 0x7F  and window to 0x50
      2] Set Prescaler to WWDG_PRESCALER_64

      Timing calculation:
      a) WWDG clock counter period (in ms) = (4096 * WWDG_PRESCALER_64) / (PCLK1 / 1000)
                                           = 0,873 ms
      b) WWDG timeout (in ms) = (0x7F + 1) * 0,873
                              ~= 111,84 ms
      => After refresh, WWDG will expires after 111,84 ms and generate reset if
      counter is not reloaded.
      c) Time to enter inside window
      Window timeout (in ms) = (127 - 80 + 1) * 0,873
                             = 41,9 ms */
  /* USER CODE END WWDG_Init 0 */

  /* USER CODE BEGIN WWDG_Init 1 */

  /* USER CODE END WWDG_Init 1 */
  hwwdg.Instance = WWDG;
  hwwdg.Init.Prescaler = WWDG_PRESCALER_64;
  hwwdg.Init.Window = WWDG_WINDOW;
  hwwdg.Init.Counter = WWDG_COUNTER;
  hwwdg.Init.EWIMode = WWDG_EWI_DISABLE;
  if (HAL_WWDG_Init(&hwwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN WWDG_Init 2 */

  /* USER CODE END WWDG_Init 2 */

}

/* USER CODE BEGIN 4 */
/**
  * @brief  Timeout calculation function.
  *         This function calculates any timeout related to
  *         WWDG with given prescaler and system clock.
  * @param  timevalue: period in term of WWDG counter cycle.
  * @retval None
  */
static uint32_t TimeoutCalculation(uint32_t timevalue)
{
  uint32_t timeoutvalue = 0;
  uint32_t pclk1 = 0;
  uint32_t wdgtb = 0;

  /* considering APB divider is still 1, use HCLK value */
  pclk1 = HAL_RCC_GetPCLK1Freq();

  /* get prescaler */
  wdgtb = (1 << ((hwwdg.Init.Prescaler) >> WWDG_CFR_WDGTB_Pos)); /* 2^WDGTB[1:0] */

  /* calculate timeout */
  timeoutvalue = ((4096 * wdgtb * timevalue) / (pclk1 / 1000));

  return timeoutvalue;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* Turn LD2 on */
  BSP_LED_On(LD2);

  WwdgStatus = 0xE;
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
  while (1)
  {}
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
