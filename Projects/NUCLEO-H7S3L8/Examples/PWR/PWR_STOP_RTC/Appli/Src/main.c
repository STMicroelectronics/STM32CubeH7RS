/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    PWR/PWR_STOP_RTC/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32H7RSxx PWR HAL API to enter
  *          and exit the STOP mode using RTC.
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
#define LED_TOGGLE_DELAY         100U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN PV */
static __IO uint32_t TimingDelay;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
static void SYSCLKConfig_STOP(void);
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

  /* Configure LD1 */
  BSP_LED_Init(LD1);
  /* Configure LD3 for error handler */
  BSP_LED_Init(LD3);

  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  /* Reduce power consumption in Stop mode by configuring the Stop Mode Voltage Scaling to low */
  if (HAL_PWREx_ControlStopModeVoltageScaling(PWR_REGULATOR_STOP_VOLTAGE_SCALE5) != HAL_OK)
  {
    Error_Handler();
  }

  /* Internal Flash low-power mode for Stop mode usecase */
  HAL_PWREx_EnableFlashPowerDown();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* Turn LD1 on */
    BSP_LED_On(LD1);

    /* Disable all used wakeup source */
    __HAL_RTC_CLEAR_FLAG(&hrtc, RTC_CLEAR_WUTF);
    HAL_NVIC_ClearPendingIRQ(RTC_IRQn);

    /* Insert 5 second delay */
    HAL_Delay(5000);

    /* Turn LD1 off  */
    BSP_LED_Off(LD1);

    /* Enter Stop Mode */
    HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);

    /* ... STOP mode ... */

    /* Configures system clock after wake-up from Stop: enable PLLs and select
       PLL1 as system clock source (PLLs are disabled in stop mode) */
    SYSCLKConfig_STOP();

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */
  /*  RTC Wakeup Interrupt Generation:
      Wakeup Time Base = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSE or LSI))
      Wakeup Time = Wakeup Time Base * WakeUpCounter
      = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSE or LSI)) * WakeUpCounter
        ==> WakeUpCounter = Wakeup Time / Wakeup Time Base

      To configure the wake up timer to 25s the WakeUpCounter is set to 51200:
      RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16
      Wakeup Time Base = 16 /(32.768 KHz = 0,00048828 s
      Wakeup Time = 25s = 0,00048828 s  * WakeUpCounter
        ==> WakeUpCounter = 25s / 0,00048828 = 51200
  */

  /* USER CODE END RTC_Init 0 */

  RTC_PrivilegeStateTypeDef privilegeState = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  hrtc.Init.BinMode = RTC_BINARY_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  privilegeState.rtcPrivilegeFull = RTC_PRIVILEGE_FULL_NO;
  privilegeState.backupRegisterPrivZone = RTC_PRIVILEGE_BKUP_ZONE_NONE;
  privilegeState.backupRegisterStartZone2 = RTC_BKP_DR0;
  privilegeState.backupRegisterStartZone3 = RTC_BKP_DR0;
  if (HAL_RTCEx_PrivilegeModeSet(&hrtc, &privilegeState) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the WakeUp
  */
  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 51200, RTC_WAKEUPCLOCK_RTCCLK_DIV16, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /*Set RTC in low power mode  */
  HAL_RTCEx_SetLowPowerCalib(&hrtc, RTC_LPCAL_SET);

  /* USER CODE END RTC_Init 2 */

}

/* USER CODE BEGIN 4 */
#define PLL_TIMEOUT  50U  /* 50 ms */

/**
  * @brief  Configure system clock after wake-up from stop: re-enable PLLs
  *         and select PLL1 as system clock source and PLL2 for external memory.
  * @param  None
  * @retval None
  */
static void SYSCLKConfig_STOP(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  uint32_t tickstart;

  /* Reconfigure the main internal regulator output voltage to highest level */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0) != HAL_OK)
  {
    Error_Handler();
  }

  /* Re-enable PLL1 used for System clock */
  __HAL_RCC_PLL1_ENABLE();

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Wait till PLL1 is ready */
  while (__HAL_RCC_GET_FLAG(RCC_FLAG_PLL1RDY) == 0U)
  {
    if ((HAL_GetTick() - tickstart) > PLL_TIMEOUT)
    {
      Error_Handler();
    }
  }

  /* Select PLL1 as SYSCLK source */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
  {
    Error_Handler();
  }

  /* Disable clock protection to re-enable PLL2 */
  HAL_RCCEx_DisableClockProtection(RCC_CLOCKPROTECT_XSPI);

  /* Re-enable PLL2 used for XSPI kernel clock */
  __HAL_RCC_PLL2_ENABLE();

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Wait till PLL2 is ready */
  while ((__HAL_RCC_GET_FLAG(RCC_FLAG_PLL2RDY) == 0U))
  {
    if ((HAL_GetTick() - tickstart) > PLL_TIMEOUT)
    {
      Error_Handler();
    }
  }

  /* Re-enable clock protection */
  HAL_RCCEx_EnableClockProtection(RCC_CLOCKPROTECT_XSPI);
}

/**
  * @brief SYSTICK callback
  * @retval None
  */
void HAL_SYSTICK_Callback(void)
{
  if (TimingDelay != 0)
  {
    TimingDelay--;
  }
  else
  {
    /* Toggle LD1 */
    BSP_LED_Toggle(LD1);
    TimingDelay = LED_TOGGLE_DELAY;
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
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();

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
  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
