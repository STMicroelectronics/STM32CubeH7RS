/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples_LL/I2C/I2C_OneBoard_Communication_IT_Init/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to send/receive bytes over I2C IP using
  *          the STM32H7RSxx I2C LL API.
  *          Peripheral initialization done using LL initialization function.
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
LL_I2C_InitTypeDef i2c_initstruct;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/**
  * @brief I2C devices settings
  */

/**
  * @brief Define related to SlaveTransmit process
  */
#define SLAVE_BYTE_TO_SEND       (uint8_t)0xA5

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
__IO uint8_t ubButtonPress       = 0;

/**
  * @brief Variables related to MasterReceive process
  */
uint8_t      aReceiveBuffer[0xF] = {0};
__IO uint8_t ubReceiveIndex      = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_FLASH_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
/* USER CODE BEGIN PFP */
void     LED_On(void);
void     LED_Off(void);
void     LED_Blinking(uint32_t Period);
void     WaitForUserButtonPress(void);
void     Handle_I2C_Master(void);

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


  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_FLASH_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */
  /* Set LD1 Off */
  LED_Off();

  /* Wait for USER push-button press to start transfer */
  WaitForUserButtonPress();

  /* Handle I2C2 events (Master) */
  Handle_I2C_Master();
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
  /* USER CODE BEGIN FLASH_Init 2 */
 
  /* USER CODE END FLASH_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  LL_I2C_InitTypeDef I2C_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_PCLK1);

  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOB);
  /**I2C1 GPIO Configuration
  PB8   ------> I2C1_SCL
  PB9   ------> I2C1_SDA
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8|LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

  /* USER CODE BEGIN I2C1_Init 1 */
  /* Configure Event IT:
   *  - Set priority for I2C1_EV_IRQn
   *  - Enable I2C1_EV_IRQn
   */
  NVIC_SetPriority(I2C1_EV_IRQn, 0);
  NVIC_EnableIRQ(I2C1_EV_IRQn);

  /* Configure Error IT:
   *  - Set priority for I2C1_ER_IRQn
   *  - Enable I2C1_ER_IRQn
   */
  NVIC_SetPriority(I2C1_ER_IRQn, 0);
  NVIC_EnableIRQ(I2C1_ER_IRQn);
  /* Reset I2C1 data registers */
  if (LL_I2C_DeInit(I2C1) != SUCCESS)
  {
    /* Initialization Error */
    LED_Blinking(LED_BLINK_ERROR);
  }
  /* USER CODE END I2C1_Init 1 */

  /** I2C Initialization
  */
  LL_I2C_EnableAutoEndMode(I2C1);
  LL_I2C_SetOwnAddress2(I2C1, 0, LL_I2C_OWNADDRESS2_NOMASK);
  LL_I2C_DisableOwnAddress2(I2C1);
  LL_I2C_DisableGeneralCall(I2C1);
  LL_I2C_EnableClockStretching(I2C1);
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.Timing = 0x00B01D60;
  I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
  I2C_InitStruct.DigitalFilter = 0;
  I2C_InitStruct.OwnAddress1 = SLAVE_OWN_ADDRESS;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C1, &I2C_InitStruct);

  /** I2C Fast mode Plus enable
  */
  LL_I2C_EnableFastModePlus(I2C1);
  /* USER CODE BEGIN I2C1_Init 2 */
  /* Initialize I2C instance according to parameters defined in initialization structure. */
  if (LL_I2C_Init(I2C1, &I2C_InitStruct) != SUCCESS)
  {
    /* Initialization Error */
    LED_Blinking(LED_BLINK_ERROR);
  }

  /* (5) Enable I2C1 address match/error interrupts:
   *  - Enable Address Match Interrupt
   *  - Enable Not acknowledge received interrupt
   *  - Enable Error interrupts
   *  - Enable Stop interrupt
   */
  LL_I2C_EnableIT_ADDR(I2C1);
  LL_I2C_EnableIT_NACK(I2C1);
  LL_I2C_EnableIT_ERR(I2C1);
  LL_I2C_EnableIT_STOP(I2C1);
  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  LL_I2C_InitTypeDef I2C_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOF);
  /**I2C2 GPIO Configuration
  PF0   ------> I2C2_SDA
  PF1   ------> I2C2_SCL
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0|LL_GPIO_PIN_1;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);

  /* USER CODE BEGIN I2C2_Init 1 */
  /* Configure Event IT:
   *  - Set priority for I2C2_EV_IRQn
   *  - Enable I2C2_EV_IRQn
   */
  NVIC_SetPriority(I2C2_EV_IRQn, 0);
  NVIC_EnableIRQ(I2C2_EV_IRQn);

  /* Configure Error IT:
   *  - Set priority for I2C2_ER_IRQn
   *  - Enable I2C2_ER_IRQn
   */
  NVIC_SetPriority(I2C2_ER_IRQn, 0);
  NVIC_EnableIRQ(I2C2_ER_IRQn);
  /* Reset I2C2 data registers */
  if (LL_I2C_DeInit(I2C2) != SUCCESS)
  {
    /* Initialization Error */
    LED_Blinking(LED_BLINK_ERROR);
  }
  /* USER CODE END I2C2_Init 1 */

  /** I2C Initialization
  */
  LL_I2C_EnableAutoEndMode(I2C2);
  LL_I2C_SetOwnAddress2(I2C2, 0, LL_I2C_OWNADDRESS2_NOMASK);
  LL_I2C_DisableOwnAddress2(I2C2);
  LL_I2C_DisableGeneralCall(I2C2);
  LL_I2C_EnableClockStretching(I2C2);
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.Timing = 0x00B01D60;
  I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
  I2C_InitStruct.DigitalFilter = 0;
  I2C_InitStruct.OwnAddress1 = 0;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C2, &I2C_InitStruct);

  /** I2C Fast mode Plus enable
  */
  LL_I2C_EnableFastModePlus(I2C2);
  /* USER CODE BEGIN I2C2_Init 2 */
  /* Initialize I2C instance according to parameters defined in initialization structure. */
  if (LL_I2C_Init(I2C2, &I2C_InitStruct) != SUCCESS)
  {
    /* Initialization Error */
    LED_Blinking(LED_BLINK_ERROR);
  }

  /* (5) Enable I2C2 transfer complete/error interrupts:
   *  - Enable Receive Interrupt
   *  - Enable Not acknowledge received interrupt
   *  - Enable Error interrupts
   *  - Enable Stop interrupt
   */
  LL_I2C_EnableIT_RX(I2C2);
  LL_I2C_EnableIT_NACK(I2C2);
  LL_I2C_EnableIT_ERR(I2C2);
  LL_I2C_EnableIT_STOP(I2C2);
  /* USER CODE END I2C2_Init 2 */

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
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOF);
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOC);
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD);

  /**/
  LL_GPIO_ResetOutputPin(LD1_GPIO_Port, LD1_Pin);

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

  /**/
  GPIO_InitStruct.Pin = LD1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LD1_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  NVIC_SetPriority(EXTI13_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(EXTI13_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


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
  LL_GPIO_SetOutputPin(LD1_GPIO_Port, LD1_Pin);

  /* Toggle IO in an infinite loop */
  while (1)
  {
    LL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
    LL_mDelay(Period);
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
  /* Turn LD1 off */
  LL_GPIO_ResetOutputPin(LD1_GPIO_Port, LD1_Pin);
}

/**
  * @brief  This Function handle Master events to perform a reception process
  * @note  This function is composed in one step :
  *        -1- Initiate a Start condition to the Slave device.
  * @param  None
  * @retval None
  */
void Handle_I2C_Master(void)
{
  /* (1) Initiate a Start condition to the Slave device ***********************/

  /* Master Generate Start condition for a read request:
   *  - to the Slave with a 7-Bit SLAVE_OWN_ADDRESS
   *  - with a auto stop condition generation when receive 1 byte
   */
  LL_I2C_HandleTransfer(I2C2, SLAVE_OWN_ADDRESS, LL_I2C_ADDRSLAVE_7BIT, 1, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);
}


/******************************************************************************/
/*   IRQ HANDLER TREATMENT Functions                                          */
/******************************************************************************/
/**
  * @brief  Function to manage USER push-button
  * @param  None
  * @retval None
  */
void UserButton_Callback(void)
{
  /* Update USER push-button variable : to be checked in waiting loop in main program */
  ubButtonPress = 1;
}

/**
  * @brief  Function called from I2C IRQ Handler when TXIS flag is set
  *         Function is in charge of transmit a byte on I2C lines.
  * @param  None
  * @retval None
  */
void Slave_Ready_To_Transmit_Callback(void)
{
  /* Send the Byte requested by the Master */
  LL_I2C_TransmitData8(I2C1, SLAVE_BYTE_TO_SEND);
}

/**
  * @brief  Function called from I2C IRQ Handler when RXNE flag is set
  *         Function is in charge of reading byte received on I2C lines.
  * @param  None
  * @retval None
  */
void Master_Reception_Callback(void)
{
  /* Read character in Receive Data register.
  RXNE flag is cleared by reading data in RXDR register */
  aReceiveBuffer[ubReceiveIndex++] = LL_I2C_ReceiveData8(I2C2);
}

/**
  * @brief  Function called from I2C IRQ Handler when STOP flag is set
  *         Function is in charge of checking data received,
  *         LD1 is On if data are correct.
  * @param  None
  * @retval None
  */
void Master_Complete_Callback(void)
{
  /* Read Received character.
  RXNE flag is cleared by reading of RXDR register */
  if (aReceiveBuffer[ubReceiveIndex - 1] == SLAVE_BYTE_TO_SEND)
  {
    /* Turn LD1 On:
     * - Expected byte has been received
     * - Master Rx sequence completed successfully
     */
    LED_On();
  }
  else
  {
    /* Call Error function */
    Error_Callback();
  }
}

/**
  * @brief  Function called in case of error detected in I2C IT Handler
  * @param  None
  * @retval None
  */
void Error_Callback(void)
{
  /* Disable I2C1_EV_IRQn and I2C2_EV_IRQn */
  NVIC_DisableIRQ(I2C1_EV_IRQn);
  NVIC_DisableIRQ(I2C2_EV_IRQn);

  /* Disable I2C1_ER_IRQn and I2C2_ER_IRQn */
  NVIC_DisableIRQ(I2C1_ER_IRQn);
  NVIC_DisableIRQ(I2C2_ER_IRQn);

  /* Unexpected event : Set LD1 to Blinking mode to indicate error occurs */
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
     text: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite Loop */
  while(1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
