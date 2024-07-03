/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples_LL/TIM/TIM_DMA_Init/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to use DMA with TIM1 Update request to
  *          transfer Data from memory to TIM1 Capture Compare Register 3 (CCR3)
  *          using the STM32H7RSxx TIM LL API.
  *          Peripheral initialization done using LL unitary services functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics. 
  * All rights reserved.</center></h2>
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
/* Definition of buffer size */
#define CC_VALUE_NB       3
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Place the DMA Buffer in a non-cacheable RAM area */
static uint32_t aCCValue[CC_VALUE_NB] __attribute__((section("noncacheable_buffer")));

/* DMA node declaration in a non-cacheable RAM area */
LL_DMA_LinkNodeTypeDef pNode __attribute__((section("noncacheable_buffer")));

/* TIM1 Clock */
static uint32_t tim1_ker_ck = 0;
static uint32_t tim_autoreload = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_GPDMA1_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */
uint32_t GetTimerCLKFreq(void);
__STATIC_INLINE void     LED_Blinking(uint32_t Period);
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
  /***************************/
  /* Time base configuration */
  /***************************/
  /* Set the TIM1 auto-reload register to get a PWM frequency at 17.57 KHz  */
  /* Note that the timer pre-scaler isn't used, therefore the timer counter */
  /* clock frequency is equal to the timer frequency.                       */
  /* In this example TIM1 kernel clock (tim_ker_ck) frequency is set to     */
  /* timer group2 clock (timg2_clk) .                                       */
  tim1_ker_ck = GetTimerCLKFreq();

  /* Compute the period value (ARR) to have a PWM frequency equal to 17.57 KHz */
  tim_autoreload = __LL_TIM_CALC_ARR(tim1_ker_ck, LL_TIM_COUNTERMODE_UP, 17570);

  /* Compute compare value to generate a duty cycle at 75% */
  aCCValue[0] = (uint32_t)(0.75 * (tim_autoreload - 1));
  /* Compute compare value to generate a duty cycle at 50% */
  aCCValue[1] = (uint32_t)(0.50 * (tim_autoreload - 1));
  /* Compute compare value to generate a duty cycle at 25% */
  aCCValue[2] = (uint32_t)(0.25 * (tim_autoreload - 1));

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_GPDMA1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  /***************************/
  /* Enable the DMA channel  */
  /***************************/
  LL_DMA_EnableChannel(GPDMA1, LL_DMA_CHANNEL_0);

  /**********************************/
  /* Start output signal generation */
  /**********************************/
  /* Enable TIM1 channel 3 */
  LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3);

  /* Enable TIM1 outputs */
  LL_TIM_EnableAllOutputs(TIM1);

  /* Enable counter */
  LL_TIM_EnableCounter(TIM1);
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
  * @brief GPDMA1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPDMA1_Init(void)
{

  /* USER CODE BEGIN GPDMA1_Init 0 */
  /* DMA node configuration declaration */
  LL_DMA_InitNodeTypeDef       DMA_InitNodeStruct = {0};
  /* USER CODE END GPDMA1_Init 0 */

  LL_DMA_InitLinkedListTypeDef DMA_InitLinkedListStruct = {0};

  /* Peripheral clock enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPDMA1);

  /* GPDMA1 interrupt Init */
  NVIC_SetPriority(GPDMA1_Channel0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(GPDMA1_Channel0_IRQn);

  /* USER CODE BEGIN GPDMA1_Init 1 */
  /* Set node configuration ################################################*/
  /* Set node type */
  DMA_InitNodeStruct.NodeType                 = LL_DMA_GPDMA_LINEAR_NODE;
  /* Set node configuration */
  DMA_InitNodeStruct.BlkHWRequest             = LL_DMA_HWREQUEST_SINGLEBURST;
  DMA_InitNodeStruct.Direction                = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
  DMA_InitNodeStruct.Request                  = LL_GPDMA1_REQUEST_TIM1_UP;
  DMA_InitNodeStruct.Mode                     = LL_DMA_NORMAL;
  DMA_InitNodeStruct.DestAllocatedPort        = LL_DMA_DEST_ALLOCATED_PORT0;
  DMA_InitNodeStruct.DestBurstLength          = 1U;
  DMA_InitNodeStruct.DestIncMode              = LL_DMA_DEST_FIXED;
  DMA_InitNodeStruct.DestDataWidth            = LL_DMA_DEST_DATAWIDTH_WORD;
  DMA_InitNodeStruct.SrcAllocatedPort         = LL_DMA_SRC_ALLOCATED_PORT0;
  DMA_InitNodeStruct.SrcBurstLength           = 1U;
  DMA_InitNodeStruct.SrcIncMode               = LL_DMA_SRC_INCREMENT;
  DMA_InitNodeStruct.SrcDataWidth             = LL_DMA_SRC_DATAWIDTH_WORD;
  DMA_InitNodeStruct.TransferEventMode        = LL_DMA_TCEM_BLK_TRANSFER;

  /* Set node data handling parameters */
  DMA_InitNodeStruct.DataAlignment            = LL_DMA_DATA_ALIGN_ZEROPADD;
  DMA_InitNodeStruct.DestHWordExchange        = LL_DMA_DEST_HALFWORD_PRESERVE;
  DMA_InitNodeStruct.DestByteExchange         = LL_DMA_DEST_BYTE_PRESERVE;
  DMA_InitNodeStruct.SrcByteExchange          = LL_DMA_SRC_BYTE_PRESERVE;

  /* Set node trigger parameters */
  DMA_InitNodeStruct.TriggerPolarity          = LL_DMA_TRIG_POLARITY_MASKED;

  /* Set registers to be updated */
  DMA_InitNodeStruct.UpdateRegisters          = DEFAULT_REGISTER_UPDATE;

  /* Set transfer parameters */
  DMA_InitNodeStruct.SrcAddress               = (uint32_t)aCCValue;
  DMA_InitNodeStruct.DestAddress              = (uint32_t)&TIM1->CCR3;
  DMA_InitNodeStruct.BlkDataLength            = CC_VALUE_NB*4;

  /* Initializes DMA linked list node */
  LL_DMA_CreateLinkNode(&DMA_InitNodeStruct, &pNode);

  /* Build linked-list Queue #################################################*/
  /* Connect node to himself (repeat the same node) */
  LL_DMA_ConnectLinkNode(&pNode, LL_DMA_CLLR_OFFSET5, &pNode, LL_DMA_CLLR_OFFSET5);

  /* USER CODE END GPDMA1_Init 1 */
  DMA_InitLinkedListStruct.Priority = LL_DMA_HIGH_PRIORITY;
  DMA_InitLinkedListStruct.LinkStepMode = LL_DMA_LSM_FULL_EXECUTION;
  DMA_InitLinkedListStruct.LinkAllocatedPort = LL_DMA_LINK_ALLOCATED_PORT0;
  DMA_InitLinkedListStruct.TransferEventMode = LL_DMA_TCEM_LAST_LLITEM_TRANSFER;
  LL_DMA_List_Init(GPDMA1, LL_DMA_CHANNEL_0, &DMA_InitLinkedListStruct);
  /* USER CODE BEGIN GPDMA1_Init 2 */

  /* Link created queue to DMA channel #######################################*/
  LL_DMA_SetLinkedListBaseAddr(GPDMA1, LL_DMA_CHANNEL_0, (uint32_t)&pNode);
  LL_DMA_ConfigLinkUpdate(GPDMA1, LL_DMA_CHANNEL_0, DEFAULT_REGISTER_UPDATE, (uint32_t)&pNode);

  /* Enable DMA interrupts */
  LL_DMA_EnableIT_TC(GPDMA1, LL_DMA_CHANNEL_0);
  LL_DMA_EnableIT_USE(GPDMA1, LL_DMA_CHANNEL_0);
  LL_DMA_EnableIT_ULE(GPDMA1, LL_DMA_CHANNEL_0);
  LL_DMA_EnableIT_DTE(GPDMA1, LL_DMA_CHANNEL_0);
  /* USER CODE END GPDMA1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};
  LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
  LL_TIM_BDTR_InitTypeDef TIM_BDTRInitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

  /* TIM1 interrupt Init */
  NVIC_SetPriority(TIM1_UP_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(TIM1_UP_IRQn);

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  TIM_InitStruct.Prescaler = 0;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = tim_autoreload;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  TIM_InitStruct.RepetitionCounter = 4-1;
  LL_TIM_Init(TIM1, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM1);
  LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH3);
  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = aCCValue[0];
  TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
  TIM_OC_InitStruct.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
  TIM_OC_InitStruct.OCIdleState = LL_TIM_OCIDLESTATE_HIGH;
  TIM_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
  LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH3, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM1, LL_TIM_CHANNEL_CH3);
  LL_TIM_SetTriggerOutput(TIM1, LL_TIM_TRGO_RESET);
  LL_TIM_SetTriggerOutput2(TIM1, LL_TIM_TRGO2_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM1);
  TIM_BDTRInitStruct.OSSRState = LL_TIM_OSSR_DISABLE;
  TIM_BDTRInitStruct.OSSIState = LL_TIM_OSSI_DISABLE;
  TIM_BDTRInitStruct.LockLevel = LL_TIM_LOCKLEVEL_OFF;
  TIM_BDTRInitStruct.DeadTime = 0;
  TIM_BDTRInitStruct.BreakState = LL_TIM_BREAK_DISABLE;
  TIM_BDTRInitStruct.BreakPolarity = LL_TIM_BREAK_POLARITY_HIGH;
  TIM_BDTRInitStruct.BreakFilter = LL_TIM_BREAK_FILTER_FDIV1;
  TIM_BDTRInitStruct.BreakAFMode = LL_TIM_BREAK_AFMODE_INPUT;
  TIM_BDTRInitStruct.Break2State = LL_TIM_BREAK2_DISABLE;
  TIM_BDTRInitStruct.Break2Polarity = LL_TIM_BREAK2_POLARITY_HIGH;
  TIM_BDTRInitStruct.Break2Filter = LL_TIM_BREAK2_FILTER_FDIV1;
  TIM_BDTRInitStruct.Break2AFMode = LL_TIM_BREAK_AFMODE_INPUT;
  TIM_BDTRInitStruct.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_DISABLE;
  LL_TIM_BDTR_Init(TIM1, &TIM_BDTRInitStruct);
  /* USER CODE BEGIN TIM1_Init 2 */

  /**************************/
  /* TIM1 interrupts set-up */
  /**************************/
  /* Clear the update flag */
  LL_TIM_ClearFlag_UPDATE(TIM1);

  /* Enable the update interrupt */
  LL_TIM_EnableIT_UPDATE(TIM1);

  /****************************/
  /* TIM1 DMA requests set-up */
  /****************************/
  /* Enable DMA request on update event */
  LL_TIM_EnableDMAReq_UPDATE(TIM1);

  /* USER CODE END TIM1_Init 2 */
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOE);
  /**TIM1 GPIO Configuration
  PE13   ------> TIM1_CH3
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

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
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOE);
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
  * @brief Return the timer 1 kernel clock frequency (tim_ker_ck).
  * @param None
  * @retval Timer kernel clock frequency
  */
uint32_t GetTimerCLKFreq(void)
{
  uint32_t timer_clock_freq = 0; /* Timer clock frequency result */
  uint32_t APB2_prescaler = 0;   /* APB2 prescaler */
  LL_RCC_ClocksTypeDef rcc_clocks = {0};

  /* Retrieve RCC clocks frequencies */
  LL_RCC_GetSystemClocksFreq(&rcc_clocks);

  /* Retrieve APB2 prescaler */
  APB2_prescaler = LL_RCC_GetAPB2Prescaler();

  if (APB2_prescaler < LL_RCC_APB2_DIV_2)
  {
    /* If APBx prescaler (PPREx) is lower than LL_RCC_APB2_DIV_2, then TIMclock is equal to APBx clock */
    timer_clock_freq = rcc_clocks.PCLK2_Frequency;
  }
  else if ((APB2_prescaler >= LL_RCC_APB2_DIV_2) && (LL_RCC_GetTIMPrescaler() == LL_RCC_TIM_PRESCALER_DISABLE))
  {
    /* If APBx prescaler (PPREx) is higher or equal than LL_RCC_APB2_DIV_2 and Timers clocks prescaler selection (TIMPRE)
       is equal to 0, then TIMclock is twice APBx clock */
    timer_clock_freq = rcc_clocks.PCLK2_Frequency * 2;
  }
  else
  {
    /* If APBx prescaler (PPREx) is higher or equal than LL_RCC_APB2_DIV_2 and Timers clocks prescaler selection (TIMPRE)
       is equal to 1, then TIMclock is 4 times the APBx clock */
    timer_clock_freq = rcc_clocks.PCLK2_Frequency * 4;
  }

  return timer_clock_freq;
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
__STATIC_INLINE void LED_Blinking(uint32_t Period)
{
  /* Toggle IO in an infinite loop */
  while (1)
  {
    LL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);  
    LL_mDelay(Period);
  }
}


/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT                                               */
/******************************************************************************/
/**
  * @brief  Timer update interrupt processing
  * @param  None
  * @retval None
  */
void TimerUpdate_Callback(void)
{
  static uint32_t UpdateEventCnt = 0;

  /* At every update event the CCR3 register is updated with a new value */
  /* which is DMA transferred from aCCValue[].                           */
  /* Note that the update event (UEV) is generated after upcounting is   */
  /* repeated for the number of times programmed in the repetition       */
  /* counter register (TIM1_RCR) + 1                                     */
  if (LL_TIM_OC_GetCompareCH3(TIM1) != aCCValue[UpdateEventCnt])
  {
    LED_Blinking(LED_BLINK_ERROR);
  }
  else
  {
    UpdateEventCnt = (UpdateEventCnt+1) % CC_VALUE_NB;
  }
}

/**
  * @brief  DMA transfer complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated after DMA transfer
  * @retval None
  */
void TransferComplete_Callback()
{
  /* Once the DMA transfer is completed the CCR3 value must match */
  /* the value of the last element of aCCValue[].                 */
  if (LL_TIM_OC_GetCompareCH3(TIM1) != aCCValue[CC_VALUE_NB-1])
  {
    LED_Blinking(LED_BLINK_ERROR);
  }
}

/**
  * @brief  DMA transfer error callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
void TransferError_Callback()
{
  LED_Blinking(LED_BLINK_ERROR);
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {
  }

  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
