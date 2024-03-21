/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples_LL/CRS/CRS_Synchronization_IT/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to configure Clock Recovery System in
  *          IT mode through the STM32H7RSxx CRS LL API.
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

/* Oscillator time-out values */
#define LSE_TIMEOUT_VALUE     ((uint32_t)5000)  /* 5 s */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
__IO uint8_t ubButtonPress = 0;

/* Variable to save SYNC status*/
__IO uint32_t CRSSyncStatus = CRS_STATUS_INIT;

/* Variable used for Timeout management */
#if (USE_TIMEOUT == 1)
uint32_t Timeout = 0;
#endif /* USE_TIMEOUT */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */


void     Enable_Sync_Reference_Clock(void);
void     Enable_CRS_Clocks(void);
void     Configure_CRS(uint32_t SyncSource);
void     MCO_ConfigGPIO(void);
void     LED_On(uint32_t LED_Number);
void     LED_Off(uint32_t LED_Number);
void     LED_Blinking(uint32_t LED_Number, uint32_t Period);
void     WaitForUserButtonPress(void);

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

  /* Configure the system Power Supply */

  LL_PWR_ConfigSupply(LL_PWR_LDO_SUPPLY);
  while (LL_PWR_IsActiveFlag_ACTVOSRDY() == 0)
  {
  }

  /* USER CODE BEGIN 1 */

  uint32_t felim = 0;

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

  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),15, 0));

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  /* Enable the reference clock used for synchronization (LSE) */
  Enable_Sync_Reference_Clock();

  /* Enable HSI48 and CRS */
  Enable_CRS_Clocks();

  /* For monitoring output HSI48 MCO pin(PA8) */
  MCO_ConfigGPIO();

  /* Wait for USER push-button press to start transfer */
  WaitForUserButtonPress();

  /* Configure CRS with LSE parameters                                */
  /* but set the source to external via GPIO (which is not connected) */
  Configure_CRS(LL_CRS_SYNC_SOURCE_GPIO);

  /* Simulate a SYNC EVENT which will raise a SYNC_MISS EVENT */
  /* because no external sync clock is connected to GPIO      */
  LL_CRS_GenerateEvent_SWSYNC();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    switch (CRSSyncStatus)
    {
      case CRS_STATUS_SYNCOK:
        /* Power on LD1 */
        LED_On(LD1);
        /* Power off LD2*/
        LED_Off(LD2);
        /* Power off LD3*/
        LED_Off(LD3);
        /* For visual comfort on LED2 apply delay */
        LL_mDelay(500);
        /* Enable CRS SYNCWARN interrupt (in case it goes out of sync) */
        LL_CRS_EnableIT_SYNCWARN();
        break;

      case CRS_STATUS_SYNCMISS:
        /* Power off LD1 */
        LED_Off(LD1);
        /* Power off LD2*/
        LED_Off(LD2);
        /* Power on LD3*/
        LED_On(LD3);
        /* Perform a new configuration to LSE when SYNC MISS is detected. */
        /* Delay to see the LED power on switch from LD3 to LD1 */
        LL_mDelay(1000);

        /* Change source to LSE and restart synchronization if necessary */
        if (LL_CRS_GetSyncSignalSource() != LL_CRS_SYNC_SOURCE_LSE)
        {
          /* CRS configuration can be changed only if frequency error is disabled */
          LL_CRS_DisableFreqErrorCounter();

          /* Change CRS SYNC signal source to LSE */
          LL_CRS_SetSyncSignalSource(LL_CRS_SYNC_SOURCE_LSE);

          /* Enable Frequency error counter */
          LL_CRS_EnableFreqErrorCounter();
        }
        else
        {
          /* Enable CRS SYNCOK interrupt */
          LL_CRS_EnableIT_SYNCOK();
        }
        break;

      case CRS_STATUS_SYNCWARN:
        /* Power off LD1 */
        LED_Off(LD1);
        /* Power on LD2*/
        LED_On(LD2);
        /* Power off LD3*/
        LED_Off(LD3);
        LL_mDelay(100);
        /* Increase tolerance (FELIM value) until no more warning */
        LL_CRS_DisableFreqErrorCounter();
        felim = LL_CRS_GetFreqErrorLimit();
        LL_CRS_SetFreqErrorLimit(felim + 1);
        LL_CRS_EnableFreqErrorCounter();
        /* Enable CRS SYNCWARN interrupt (to allow resync) */
        LL_CRS_EnableIT_SYNCWARN();
        /* Enable CRS SYNCOK interrupt (if it was in SYNCOK but then in WARN again) */
        LL_CRS_EnableIT_SYNCOK();
        break;

      case CRS_STATUS_SYNCERR:
        /* Power off LD1 */
        LED_Off(LD1);
        /* Power off LD2*/
        LED_Off(LD2);
        /* Power on LD3*/
        LED_On(LD3);
        break;

      default:
        break;
    }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_6);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_6)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE0);
  while (LL_PWR_IsActiveFlag_VOSRDY() == 0)
  {
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_HSI_SetCalibTrimming(64);
  LL_RCC_HSI_SetDivider(LL_RCC_HSI_DIV_1);
  LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_2_4);
  LL_RCC_PLL1_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
  LL_RCC_PLL1_SetM(32);
  LL_RCC_PLL1_SetN(300);
  LL_RCC_PLL1_SetP(1);
  LL_RCC_PLL1_SetQ(2);
  LL_RCC_PLL1_SetR(2);
  LL_RCC_PLL1_SetS(2);
  LL_RCC_PLL1_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL1_IsReady() != 1)
  {
  }

  LL_RCC_PLL1P_Enable();
  LL_RCC_SetSysPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  LL_RCC_SetAPB4Prescaler(LL_RCC_APB4_DIV_2);
  LL_RCC_SetAPB5Prescaler(LL_RCC_APB5_DIV_2);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1)
  {

  }

  LL_Init1msTick(600000000);

  LL_SetSystemCoreClock(600000000);
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOB);
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOC);
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD);

  /**/
  LL_GPIO_ResetOutputPin(LD3_GPIO_Port, LD3_Pin);

  /**/
  LL_GPIO_ResetOutputPin(GPIOD, LD2_Pin|LD1_Pin);

  /**/
  GPIO_InitStruct.Pin = LD3_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LD2_Pin|LD1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*SBS Clock Enable */
  LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_SBS);

  /**/
  LL_SBS_SetEXTISource(LL_SBS_EXTI_PORTC, LL_SBS_EXTI_LINE13);

  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_13;
  EXTI_InitStruct.Line_32_63 = LL_EXTI_LINE_NONE;
  EXTI_InitStruct.Line_64_95 = LL_EXTI_LINE_NONE;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
  LL_EXTI_Init(&EXTI_InitStruct);

  /**/
  LL_GPIO_SetPinPull(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin, LL_GPIO_PULL_UP);

  /**/
  LL_GPIO_SetPinMode(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin, LL_GPIO_MODE_INPUT);

  /* EXTI interrupt init*/
  NVIC_SetPriority(EXTI13_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(EXTI13_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief  This function enables the reference clock used for sync (LSE)
  * @param  None
  * @retval None
  */
void Enable_Sync_Reference_Clock(void)
{
  /* To enable LSE, before it is necessary to:
     - Reset the Back up Domain */
  LL_PWR_EnableBkUpAccess();

  /* Enable LSE only if disabled.*/
  if (LL_RCC_LSE_IsReady() == 0)
  {
    LL_RCC_ForceBackupDomainReset();
    LL_RCC_ReleaseBackupDomainReset();
    LL_RCC_LSE_Enable();
#if (USE_TIMEOUT == 1)
    Timeout = LSE_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */
    while (LL_RCC_LSE_IsReady() != 1)
    {
#if (USE_TIMEOUT == 1)
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        Timeout --;
      }
      if (Timeout == 0)
      {
        /* LSE activation error */
        LED_Blinking(LED_BLINK_ERROR);
      }
#endif /* USE_TIMEOUT */
    }
  }
}

/**
  * @brief  This function enables CRS and HSI48 clocks
  * @param  None
  * @retval None
  */
void Enable_CRS_Clocks(void)
{
  /* CRS initialization (enable HSI48 oscillator and enable CRS clock) */

  /* Enable CRS clock*/
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_CRS);


  /* Enable HSI48 only if disabled.*/
  if (LL_RCC_HSI48_IsReady() == 0)
  {
    LL_RCC_HSI48_Enable();
#if (USE_TIMEOUT == 1)
    Timeout = LSE_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */
    while (LL_RCC_HSI48_IsReady() != 1)
    {
#if (USE_TIMEOUT == 1)
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        Timeout --;
      }
      if (Timeout == 0)
      {
        /* HSI48 activation error */
        LED_Blinking(LED_BLINK_ERROR);
      }
#endif /* USE_TIMEOUT */
    }
  }
}

/**
  * @brief  This function configures CRS
  * @param  None
  * @retval None
  */
void Configure_CRS(uint32_t SyncSource)
{
  /* Enable and set CRS interrupt*/
  NVIC_SetPriority(CRS_IRQn, 0x03);
  NVIC_EnableIRQ(CRS_IRQn);

  /* HSI48 Synchronization:
     - HSI trimming value set to 0x00 to see the automatic calibration performed by CRS
     - Reload value calculated like this F(LSE)=32.768kHz then Reload=(f(Target)/f(LSE))-1= 0x5B7
     - Felim value calculated like this FELIM = (fTARGET / fSYNC) * STEP[%] / 100% / 2 with STEP=0.14% then FELIM = 2
  */
  LL_CRS_ConfigSynchronization(0x00,
                               2,
                               __LL_CRS_CALC_CALCULATE_RELOADVALUE(HSI48_VALUE, LSE_VALUE),
                               LL_CRS_SYNC_DIV_1 | SyncSource | LL_CRS_SYNC_POLARITY_FALLING
                              );

  /* Enable Automatic trimming */
  LL_CRS_EnableAutoTrimming();

  /* Enable Frequency error counter */
  LL_CRS_EnableFreqErrorCounter();

  /* Enable CRS interrupts */
  /* Each interruption can be enabled independently in using following functions:
  - LL_CRS_EnableIT_SYNCOK();
  - LL_CRS_EnableIT_SYNCWARN();
  - LL_CRS_EnableIT_ERR();
  - LL_CRS_EnableIT_ESYNC();
  */
  LL_CRS_WriteReg(CRS, CR, LL_CRS_ReadReg(CRS, CR) | LL_CRS_CR_SYNCOKIE | LL_CRS_CR_SYNCWARNIE | LL_CRS_CR_ERRIE | LL_CRS_CR_ESYNCIE);
}

/**
  * @brief  Configure MCO pin (PA8).
  * @param  None
  * @retval None
  */
void MCO_ConfigGPIO(void)
{
  /* MCO Clock Enable */
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOA);

  /* Configure the MCO pin in alternate function mode */
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_8, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_8, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_8, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_8, LL_GPIO_PULL_NO);
  LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_8, LL_GPIO_AF_0);

  /* Select MCO clock source and prescaler */
  LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_HSI48, LL_RCC_MCO1_DIV_1);
}

/**
  * @brief  Turn-on LED provided as parameter
  * @param  LED_Number LED number (LD1, LD2 or LD3)
  * @retval None
  */
void LED_On(uint32_t LED_Number)
{
  switch (LED_Number)
  {
    case LD1:
      /* Turn LD1 on */
      LL_GPIO_SetOutputPin(LD1_GPIO_Port, LD1_Pin);
      break;
    case LD2:
      /* Turn LD2 on */
      LL_GPIO_SetOutputPin(LD2_GPIO_Port, LD2_Pin);
      break;
    case LD3:
      /* Turn LD3 on */
      LL_GPIO_SetOutputPin(LD3_GPIO_Port, LD3_Pin);
      break;
  }
}

/**
  * @brief  Turn-off LED provided as parameter
  * @param  LED_Number LED number (LD1, LD2 or LD3)
  * @retval None
  */
void LED_Off(uint32_t LED_Number)
{
  switch (LED_Number)
  {
    case LD1:
      /* Turn LD1 off */
      LL_GPIO_ResetOutputPin(LD1_GPIO_Port, LD1_Pin);
      break;
    case LD2:
      /* Turn LD2 off */
      LL_GPIO_ResetOutputPin(LD2_GPIO_Port, LD2_Pin);
      break;
    case LD3:
      /* Turn LD3 off */
      LL_GPIO_ResetOutputPin(LD3_GPIO_Port, LD3_Pin);
      break;
  }
}

/**
  * @brief  Set LED_Number to Blinking mode for an infinite loop (toggle period based on value provided as input parameter).
  * @param  LED_Number LED number (LD1, LD2 or LD3)
  * @param  Period : Period of time (in ms) between each toggling of LED
  *   This parameter can be user defined values. Pre-defined values used in that example are :
  *     @arg LED_BLINK_FAST : Fast Blinking
  *     @arg LED_BLINK_SLOW : Slow Blinking
  *     @arg LED_BLINK_ERROR : Error specific Blinking
  * @retval None
  */
void LED_Blinking(uint32_t LED_Number, uint32_t Period)
{
  switch (LED_Number)
  {
    case LD1:
      /* Toggle IO in an infinite loop */
      while (1)
      {
        LL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
        LL_mDelay(Period);
      }
    case LD2:
      /* Toggle IO in an infinite loop */
      while (1)
      {
        LL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        LL_mDelay(Period);
      }
    case LD3:
      /* Toggle IO in an infinite loop */
      while (1)
      {
        LL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
        LL_mDelay(Period);
      }
  }
}

/**
  * @brief  Wait for USER push-button press to start transfer.
  * @param  None
  * @retval None
  */
/*  */
void WaitForUserButtonPress(void)
{
  while (ubButtonPress == 0)
  {
    LL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
    LL_mDelay(LED_BLINK_FAST);
  }
  /* Ensure that LD1 is turned Off */
  LED_Off(LD1);
}


/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT                                               */
/******************************************************************************/
/**
  * @brief  Function to manage User button callback interruption
  * @param  None
  * @retval None
  */
void UserButton_Callback(void)
{
  /* Update USER push-button variable : to be checked in waiting loop in main program */
  ubButtonPress = 1;
}

/**
  * @brief  Handle CRS interruption
  * @param  none
  * @retval none
  */
void CRS_Handler_Callback(void)
{
  /* Check CRS SYNCOK flag  */
  if (LL_CRS_IsActiveFlag_SYNCOK() && LL_CRS_IsEnabledIT_SYNCOK())
  {
    CRSSyncStatus = CRS_STATUS_SYNCOK;

    /* Disable CRS SYNCOK interrupt since synchronization succeeded */
    LL_CRS_DisableIT_SYNCOK();

    /* Clear CRS SYNC event OK bit */
    LL_CRS_ClearFlag_SYNCOK();
  }

  /* Check CRS SYNCWARN flag  */
  if (LL_CRS_IsActiveFlag_SYNCWARN() && LL_CRS_IsEnabledIT_SYNCWARN())
  {
    CRSSyncStatus = CRS_STATUS_SYNCWARN;

    /* Disable temporary CRS SYNCWARN interrupt to let the main loop to be execute */
    LL_CRS_DisableIT_SYNCWARN();

    /* Clear CRS SYNCWARN bit */
    LL_CRS_ClearFlag_SYNCWARN();
  }

  /* Check CRS SYNCERR flag  */
  if (LL_CRS_IsActiveFlag_SYNCERR() && LL_CRS_IsEnabledIT_ERR())
  {
    CRSSyncStatus = CRS_STATUS_SYNCERR;

    /* Clear CRS Error bit */
    LL_CRS_ClearFlag_ERR();
  }

  /* Check CRS SYNC Missed flag  */
  if (LL_CRS_IsActiveFlag_SYNCMISS() && LL_CRS_IsEnabledIT_ERR())
  {
    CRSSyncStatus = CRS_STATUS_SYNCMISS;

    /* Clear CRS SYNC Missed bit */
    LL_CRS_ClearFlag_ERR();
  }

  /* Check CRS Expected SYNC flag  */
  if (LL_CRS_IsActiveFlag_ESYNC() && LL_CRS_IsEnabledIT_ESYNC())
  {
    /* frequency error counter reached a zero value */
    LL_CRS_ClearFlag_ESYNC();
  }
}


/* USER CODE END 4 */

 /* MPU Configuration */

static void MPU_Config(void)
{

  /* Disables the MPU */
  LL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  LL_MPU_ConfigRegion(LL_MPU_REGION_NUMBER0, 0x87, 0x0, LL_MPU_REGION_SIZE_4GB|LL_MPU_TEX_LEVEL0|LL_MPU_REGION_NO_ACCESS|LL_MPU_INSTRUCTION_ACCESS_DISABLE|LL_MPU_ACCESS_SHAREABLE|LL_MPU_ACCESS_NOT_CACHEABLE|LL_MPU_ACCESS_NOT_BUFFERABLE);

  /** Initializes and configures the Region and the memory to be protected
  */
  LL_MPU_ConfigRegion(LL_MPU_REGION_NUMBER1, 0x0, 0x24071C00, LL_MPU_REGION_SIZE_1KB|LL_MPU_TEX_LEVEL1|LL_MPU_REGION_FULL_ACCESS|LL_MPU_INSTRUCTION_ACCESS_DISABLE|LL_MPU_ACCESS_NOT_SHAREABLE|LL_MPU_ACCESS_NOT_CACHEABLE|LL_MPU_ACCESS_NOT_BUFFERABLE);
  /* Enables the MPU */
  LL_MPU_Enable(LL_MPU_CTRL_PRIVILEGED_DEFAULT);

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
     ex: printf("Wrong parameters value: file %s on line %d", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
