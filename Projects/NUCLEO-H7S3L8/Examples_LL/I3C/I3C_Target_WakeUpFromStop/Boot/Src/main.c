/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    I3C/I3C_Target_WakeUpFromStop/Boot/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32H7RSxx I3C LL API as Target
  *          to transmit and receive a data buffer
  *          with a communication process based on Interrupt transfer
  *          after a low power mode.
  *          The communication is done using 2 Boards.
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
/* Buffer used for transmission */
uint8_t aTxBuffer[] = " ****I3C_TwoBoards communication based on IT****  ****I3C_TwoBoards communication based on IT****  ****I3C_TwoBoards communication based on IT**** ";

/* Buffer used for reception */
uint8_t aRxBuffer[RXBUFFERSIZE];

/* Pointer used for reception */
uint32_t *pBuffRXWordPtr = NULL;

/* Pointer used for transmission */
uint32_t *pBuffTXWordPtr = NULL;

/* Completion status */
__IO uint8_t ubDynamicAddressComplete = 0;
__IO uint8_t ubFrameComplete = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_FLASH_Init(void);
static void MX_I3C1_Init(void);
/* USER CODE BEGIN PFP */
void FlushBuffer8(uint8_t* pBuffer1, uint16_t BufferSize);
void LED_On(void);
void LED_Off(void);
void LED_Toggle(void);
void LED_Blinking(uint32_t Period);
void EnterLowPowerMode(void);
static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);
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
  /* Initialize transfer pointer */
  pBuffRXWordPtr    = ((uint32_t *)&aRxBuffer[0]);
  pBuffTXWordPtr    = ((uint32_t *)&aTxBuffer[0]);

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
  MX_FLASH_Init();
  MX_I3C1_Init();
  /* USER CODE BEGIN 2 */
  /* Set LD1 Off */
  LED_Off();

  /* Set Preload information */
  LL_I3C_ConfigTxPreload(I3C1, TXBUFFERSIZE);

  /* Wait completion of Dynamic address assignment procedure */
  while(ubDynamicAddressComplete == 0);

  /** Request to enter STOP mode:
  *  - Following procedure describe in STM32H7RSxx Reference Manual
  *  - See PWR part, section Low-power modes, STOP mode
  */
  /* UnMask wakeup interrupt just before go to low power */
  LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_25);

  /* Enter Stop mode */
  EnterLowPowerMode();

  /* Wait end of Private transfer procedure */
  while(ubFrameComplete != 2);

  /*##- Compare the sent and received buffers ############################*/
  if (Buffercmp((uint8_t *)aTxBuffer, (uint8_t *)aRxBuffer, RXBUFFERSIZE))
  {
    /* Processing Error */
    Error_Handler();
  }
  else
  {
    /* LD1 On */
    LED_On();
  }
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

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_7);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_7)
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
  * @brief FLASH Initialization Function
  * @param None
  * @retval None
  */
static void MX_FLASH_Init(void)
{

  /* USER CODE BEGIN FLASH_Init 0 */

  /* USER CODE END FLASH_Init 0 */

  /* USER CODE BEGIN FLASH_Init 1 */

  /* USER CODE END FLASH_Init 1 */
   if (LL_FLASH_OptionBytes_IsI3CEnabled() == 0U)
  {
    LL_FLASH_Unlock();
    LL_FLASH_OptionBytes_Unlock();
    LL_FLASH_OptionBytes_EnableProg();
    LL_FLASH_OptionBytes_EnableI3C();

    while (LL_FLASH_IsPendingOperation() != 0U) {}

    LL_FLASH_OptionBytes_DisableProg();
    LL_FLASH_OptionBytes_Lock();
    LL_FLASH_Lock();
  }
  /* USER CODE BEGIN FLASH_Init 2 */

  /* USER CODE END FLASH_Init 2 */

}

/**
  * @brief I3C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I3C1_Init(void)
{

  /* USER CODE BEGIN I3C1_Init 0 */

  /* USER CODE END I3C1_Init 0 */

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_RCC_SetI3CClockSource(LL_RCC_I3C1_CLKSOURCE_HSI);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I3C1);

  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOB);
  /**I3C1 GPIO Configuration
  PB8   ------> I3C1_SCL
  PB9   ------> I3C1_SDA
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8|LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* I3C1 interrupt Init */
  NVIC_SetPriority(I3C1_WKUP_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1, 0));
  NVIC_EnableIRQ(I3C1_WKUP_IRQn);
  NVIC_SetPriority(I3C1_EV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(I3C1_EV_IRQn);
  NVIC_SetPriority(I3C1_ER_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(I3C1_ER_IRQn);

  /* USER CODE BEGIN I3C1_Init 1 */
  /* EXTI line Configuration for wakeup interrupt */
  LL_EXTI_InitTypeDef EXTI_InitStruct = {0};

  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_25;
  EXTI_InitStruct.Line_32_63 = LL_EXTI_LINE_NONE;
  EXTI_InitStruct.Line_64_95 = LL_EXTI_LINE_NONE;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING;
  LL_EXTI_Init(&EXTI_InitStruct);
  /* USER CODE END I3C1_Init 1 */

  /** I3C Initialization
  */
  LL_I3C_SetMode(I3C1, LL_I3C_MODE_TARGET);
  LL_I3C_SetAvalTiming(I3C1, 0x0000003e);

  /** Configure FIFO
  */
  LL_I3C_SetRxFIFOThreshold(I3C1, LL_I3C_RXFIFO_THRESHOLD_4_4);
  LL_I3C_SetTxFIFOThreshold(I3C1, LL_I3C_TXFIFO_THRESHOLD_4_4);
  LL_I3C_DisableControlFIFO(I3C1);
  LL_I3C_DisableStatusFIFO(I3C1);

  /** Configure Target
  */
  LL_I3C_SetDeviceCharacteristics(I3C1, 0xC6);
  LL_I3C_SetMIPIInstanceID(I3C1, DEVICE_ID);
  LL_I3C_DisableControllerRoleReq(I3C1);
  LL_I3C_DisableHotJoin(I3C1);
  LL_I3C_DisableIBI(I3C1);
  LL_I3C_SetDeviceIBIPayload(I3C1, LL_I3C_IBI_NO_ADDITIONAL_DATA);
  LL_I3C_ConfigNbIBIAddData(I3C1, LL_I3C_PAYLOAD_1_BYTE);
  LL_I3C_SetMaxReadLength(I3C1, 0);
  LL_I3C_SetMaxWriteLength(I3C1, 0);
  LL_I3C_SetDeviceCapabilityOnBus(I3C1, LL_I3C_DEVICE_ROLE_AS_TARGET);
  LL_I3C_SetGrpAddrHandoffSupport(I3C1, LL_I3C_HANDOFF_GRP_ADDR_NOT_SUPPORTED);
  LL_I3C_SetDataTurnAroundTime(I3C1, LL_I3C_TURNAROUND_TIME_TSCO_LESS_12NS);
  LL_I3C_SetMiddleByteTurnAround(I3C1, 0);
  LL_I3C_SetDataSpeedLimitation(I3C1, LL_I3C_NO_DATA_SPEED_LIMITATION);
  LL_I3C_SetMaxDataSpeedFormat(I3C1, LL_I3C_GETMXDS_FORMAT_1);
  LL_I3C_SetHandoffActivityState(I3C1, LL_I3C_HANDOFF_ACTIVITY_STATE_0);
  LL_I3C_SetControllerHandoffDelayed(I3C1, LL_I3C_HANDOFF_NOT_DELAYED);
  LL_I3C_SetPendingReadMDB(I3C1, LL_I3C_MDB_NO_PENDING_READ_NOTIFICATION);

  /** Enable the selected I3C peripheral
  */
  LL_I3C_Enable(I3C1);
  /* USER CODE BEGIN I3C1_Init 2 */
  LL_I3C_EnableIT_DAUPD(I3C1);
  LL_I3C_EnableIT_FC(I3C1);
  LL_I3C_EnableIT_RXFNE(I3C1);
  LL_I3C_EnableIT_TXFNF(I3C1);
  LL_I3C_EnableIT_ERR(I3C1);
  LL_I3C_EnableIT_WKP(I3C1);
  /* USER CODE END I3C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOB);
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD);

  /**/
  LL_GPIO_ResetOutputPin(LD1_GPIO_Port, LD1_Pin);

  /**/
  GPIO_InitStruct.Pin = LD1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LD1_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Flush 8-bit buffer.
  * @param  pBuffer1: pointer to the buffer to be flushed.
  * @param  BufferSize: Size of Buffer.
  * @retval None
  */
void FlushBuffer8(uint8_t* pBuffer1, uint16_t BufferSize)
{
  uint8_t Index = 0;

  for (Index = 0; Index < BufferSize; Index++)
  {
    pBuffer1[Index] = 0;
  }
}

/**
  * @brief  Turn-on LD1.
  * @param  None
  * @retval None
  */
void LED_On(void)
{
  /* Turn LD1 on */
  LL_GPIO_SetOutputPin(LD1_GPIO_Port, LD1_Pin);
}

/**
  * @brief  Turn-off LD1.
  * @param  None
  * @retval None
  */
void LED_Off(void)
{
  /* Turn LD1 off */
  LL_GPIO_ResetOutputPin(LD1_GPIO_Port, LD1_Pin);
}

/**
  * @brief  Toggle LD1.
  * @param  None
  * @retval None
  */
void LED_Toggle(void)
{
  /* Toggle LD1 on */
  LL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
}

/**
  * @brief  Set LD1 to Blinking mode for an infinite loop (toggle period based on value provided as input parameter).
  * @param  Period : Period of time (in ms) between each toggling of LED
  *   This parameter can be user defined values. Pre-defined values used in that example are :
  *     @arg LED_BLINK_FAST : Fast Blinking
  *     @arg LED_BLINK_SLOW : Slow Blinking
  *     @arg LED_BLINK_ERROR : Error specific Blinking
  * @retval None
  */
void LED_Blinking(uint32_t Period)
{
  /* Turn LD1 on */
  LED_On();

  /* Toggle IO in an infinite loop */
  while (1)
  {
    LED_Toggle();
    LL_mDelay(Period);
  }
}

/**
  * @brief  Function to configure and enter in STOP Mode.
  * @param  None
  * @retval None
  */
void EnterLowPowerMode(void)
{
  /** Request to enter STOP mode:
  *  - Following procedure describe in STM32H7RSxx Reference Manual
  *  - See PWR part, section Low-power modes, STOP mode
  */
  /* Set the system wakeup clock source */
  LL_RCC_SetSysWakeUpClkSource(LL_RCC_SYSWAKEUP_CLKSOURCE_HSI);

  /* Set the kernel wakeup clock source */
  LL_RCC_SetKerWakeUpClkSource(LL_RCC_KERWAKEUP_CLKSOURCE_HSI);

  /* Set low-power mode */
  LL_PWR_SetPowerDownModeDS(LL_PWR_POWERDOWN_MODE_DS_STOP);

  /* Set SLEEPDEEP bit of Cortex System Control Register */
  LL_LPM_EnableDeepSleep(); /* SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; */

  /* This option is used to ensure that store operations are completed */
#if defined (__CC_ARM)
  __force_stores();
#endif

  /* Request Wait For Interrupt */
  __WFI();
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0  : pBuffer1 identical to pBuffer2
  *         >0 : pBuffer1 differs from pBuffer2
  */
static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

/******************************************************************************/
/*   IRQ HANDLER TREATMENT Functions                                          */
/******************************************************************************/
/**
  * @brief  Target Event treatment callback.
  * @param  targetPayload Parameter indicates the target payload.
  * @retval None
  */
void Target_Event_Treatment_Callback(void)
{
  /* Check Dynamic Address update flag in EVR register */
  if (LL_I3C_IsActiveFlag_DAUPD(I3C1))
  {
    /* Clear Dynamic Address update flag */
    LL_I3C_ClearFlag_DAUPD(I3C1);

    /* Update Completion status */
    ubDynamicAddressComplete++;

    LED_Toggle();
  }
}

/**
  * @brief  Target Complete callback.
  * @param  None
  * @retval None
  */
void Target_Complete_Callback(void)
{
  /* Update Completion status */
  ubFrameComplete++;

  /* Toggle LD1: Transfer in Transmission process is correct */
  LED_Toggle();
}

/**
  * @brief  Target Receive callback.
  * @param  None
  * @retval None
  */
void Target_Reception_Callback(void)
{
  do{
    *pBuffRXWordPtr = LL_I3C_ReceiveData32(I3C1);
    pBuffRXWordPtr++;
  }while (LL_I3C_IsActiveFlag_RXFNE(I3C1));
}

/**
  * @brief  Target Transmit callback.
  * @param  None
  * @retval None
  */
void Target_Transmit_Callback(void)
{
  do{
    /* Write Tx buffer data to transmit register */
    LL_I3C_TransmitData32(I3C1, *pBuffTXWordPtr);
    pBuffTXWordPtr++;
  }while (LL_I3C_IsActiveFlag_TXFNF(I3C1));
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
  while (1)
  {
    /* Unexpected event : Set LD1 to Blinking mode to indicate error occurs */
    LED_Blinking(LED_BLINK_ERROR);
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
  Error_Handler();
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
