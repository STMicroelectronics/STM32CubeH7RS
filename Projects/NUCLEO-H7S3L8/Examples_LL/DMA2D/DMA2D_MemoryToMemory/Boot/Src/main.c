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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t aBufferResult[(LAYER_SIZE_X * LAYER_SIZE_Y * LAYER_NB_BYTES_PER_PIXEL) / 4] __attribute__((aligned(32),section("noncacheable_buffer")));
/* ARGB4444 input buffer 16bpp */
uint32_t aBufferInput[(LAYER_SIZE_X * LAYER_SIZE_Y * LAYER_NB_BYTES_PER_PIXEL) / 4] __attribute__((aligned(32)))=
      {0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203,
       0xEFFFFFF2, 0xAB1FFFF0, 0x0504044E, 0x89ADCDEF, 0x15248722, 0x5201245E, 0x01025FDE, 0xFE010203};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA2D_Init(void);
/* USER CODE BEGIN PFP */
static void LED_On(void);
static void LED_Blinking(uint32_t Period);
ErrorStatus Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint32_t BufferLength);
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
  MX_DMA2D_Init();
  /* USER CODE BEGIN 2 */
  /* Set input (foreground) memory address */
  LL_DMA2D_FGND_SetMemAddr(DMA2D, (uint32_t)&aBufferInput);

  /* Set output memory address */
  LL_DMA2D_SetOutputMemAddr(DMA2D, (uint32_t)&aBufferResult);

  /* Set number of lines and number of pixels per line */
  LL_DMA2D_SetNbrOfLines(DMA2D, LAYER_SIZE_Y);
  LL_DMA2D_SetNbrOfPixelsPerLines(DMA2D, LAYER_SIZE_X);
  
  /* Start transfer */
  LL_DMA2D_Start(DMA2D);

  /* Wait until transfer is over */
  while (LL_DMA2D_IsTransferOngoing(DMA2D));

  /*## Check transfer correctness #########################################*/
  if(Buffercmp(aBufferInput,
               aBufferResult,
               ((LAYER_SIZE_X * LAYER_SIZE_Y * LAYER_NB_BYTES_PER_PIXEL) / 4)) != SUCCESS)
  {
    /* KO */
    /* Turn LD3 On */
    LED_Blinking(100);
  }
  else
  {
    /* OK */
    /* Turn on LD1 */
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
  * @brief DMA2D Initialization Function
  * @param None
  * @retval None
  */
static void MX_DMA2D_Init(void)
{

  /* USER CODE BEGIN DMA2D_Init 0 */

  /* USER CODE END DMA2D_Init 0 */

  /* Peripheral clock enable */
  LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_DMA2D);

  /* USER CODE BEGIN DMA2D_Init 1 */

  /* USER CODE END DMA2D_Init 1 */
  LL_DMA2D_SetMode(DMA2D, LL_DMA2D_MODE_M2M);
  LL_DMA2D_SetOutputColorMode(DMA2D, LL_DMA2D_OUTPUT_MODE_ARGB4444);
  LL_DMA2D_SetLineOffset(DMA2D, 0);
  LL_DMA2D_FGND_SetColorMode(DMA2D, LL_DMA2D_INPUT_MODE_ARGB4444);
  LL_DMA2D_FGND_SetAlphaMode(DMA2D, LL_DMA2D_ALPHA_MODE_NO_MODIF);
  LL_DMA2D_FGND_SetAlpha(DMA2D, 0);
  LL_DMA2D_FGND_SetLineOffset(DMA2D, 0);
  LL_DMA2D_FGND_SetRBSwapMode(DMA2D, LL_DMA2D_RB_MODE_REGULAR);
  LL_DMA2D_FGND_SetAlphaInvMode(DMA2D, LL_DMA2D_ALPHA_REGULAR);
  LL_DMA2D_FGND_SetChrSubSampling(DMA2D, LL_DMA2D_CSS_444);
  /* USER CODE BEGIN DMA2D_Init 2 */

  /* USER CODE END DMA2D_Init 2 */

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
  LL_GPIO_ResetOutputPin(LD3_GPIO_Port, LD3_Pin);

  /**/
  LL_GPIO_ResetOutputPin(LD1_GPIO_Port, LD1_Pin);

  /**/
  GPIO_InitStruct.Pin = LD3_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);

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
* @brief  Compares two buffers.
* @param  pBuffer1, pBuffer2: buffers to be compared.
* @param  BufferLength: buffer's length.
* @retval  0: pBuffer1 identical to pBuffer2
*          1: pBuffer1 differs from pBuffer2
*/
ErrorStatus Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint32_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return ERROR;
    }
    pBuffer1++;
    pBuffer2++;
  }
  return SUCCESS;
}

/**
  * @brief  Turn-on LED2 .
  * @param  None
  * @retval None
  */
void LED_On(void)
{
  /* Turn LED1 on */
  LL_GPIO_SetOutputPin(LD1_GPIO_Port, LD1_Pin);
}

/**
  * @brief  Set LED1 to Blinking mode for an infinite loop (toggle period based on value provided as input parameter).
  * @param  Period : Period of time (in ms) between each toggling of LED
  *   This parameter can be user defined values. Pre-defined values used in that example are :
  *     @arg LED_BLINK_FAST : Fast Blinking
  *     @arg LED_BLINK_SLOW : Slow Blinking
  *     @arg LED_BLINK_ERROR : Error specific Blinking
  * @retval None
  */
void LED_Blinking(uint32_t Period)
{
  /* Toggle IO in an infinite loop */
  while (1)
  {
    LL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
    LL_mDelay(Period);
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
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
