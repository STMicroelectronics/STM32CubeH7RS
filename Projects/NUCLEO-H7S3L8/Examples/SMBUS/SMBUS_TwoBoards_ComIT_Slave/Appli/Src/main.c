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
#define SMBUS_ADDRESS          0x42
#define SMBUS_PEC_SIZE         0x1

/* Address match */
__IO uint32_t SlaveAddrMatchCode = 0;

/* Buffer used for transmission */
uint8_t aTxBuffer[] = " ****SMBUS_TwoBoards communication using Interrupts****  ****SMBUS_TwoBoards communication using Interrupts****  ****SMBUS_TwoBoards communication using Interrupts**** ";

/* Buffer used for data and PEC reception */
uint8_t aRxBuffer[RXBUFFERSIZE + SMBUS_PEC_SIZE];
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

SMBUS_HandleTypeDef hsmbus2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
static void MX_I2C2_SMBUS_Init(void);
/* USER CODE BEGIN PFP */
static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
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
  /* Configure LD1,LD2 and LD3 */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD2);
  BSP_LED_Init(LD3);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C2_SMBUS_Init();
  /* USER CODE BEGIN 2 */
  /* Enable the Analog SMBUS Filter */
  HAL_SMBUS_ConfigAnalogFilter(&hsmbus2,SMBUS_ANALOGFILTER_ENABLE);

  /* Enable address Match detect with interrupt */
  HAL_SMBUS_EnableListen_IT(&hsmbus2);
  /* Wait until address matched */
  while(SlaveAddrMatchCode != SMBUS_ADDRESS);
  SlaveAddrMatchCode = 0;
  BSP_LED_Off(LD2);

  /* The board receives the message and sends it back */

  /*##-2- Put SMBUS peripheral in reception process ############################*/
  if(HAL_SMBUS_Slave_Receive_IT(&hsmbus2, (uint8_t *)aRxBuffer, RXBUFFERSIZE + SMBUS_PEC_SIZE, SMBUS_FIRST_AND_LAST_FRAME_WITH_PEC) != HAL_OK)
  {
    /* Transfer error in reception process */
    Error_Handler();
  }
  while (hsmbus2.State != HAL_SMBUS_STATE_READY);

  /* Compare PEC register and sent PEC #######################################*/
  if(aRxBuffer[RXBUFFERSIZE + SMBUS_PEC_SIZE - 1] != hsmbus2.Instance->PECR)
  {
    /* Processing Error */
    Error_Handler();
  }

  /* Turn LD1 on: Transfer in reception process is correct */
  BSP_LED_On(LD1);

  /*##-3- Start the transmission process #####################################*/
  /* While the SMBUS in reception process, user can transmit data through
     "aTxBuffer" buffer */

  /* Enable address Match detect with interrupt */
  HAL_SMBUS_EnableListen_IT(&hsmbus2);
  /* Wait until address matched */
  while(SlaveAddrMatchCode != SMBUS_ADDRESS);
  SlaveAddrMatchCode = 0;

  /*##-2- Put SMBUS peripheral in transmission process ############################*/
  if(HAL_SMBUS_Slave_Transmit_IT(&hsmbus2, (uint8_t*)aTxBuffer, TXBUFFERSIZE + SMBUS_PEC_SIZE, SMBUS_FIRST_AND_LAST_FRAME_WITH_PEC)!= HAL_OK)
  {
    /* Transfer error in transmission process */
    Error_Handler();
  }
  while (hsmbus2.State != HAL_SMBUS_STATE_READY);
  /* Turn LD1 off: Transfer in transmission process is correct */
  BSP_LED_Off(LD1);

    /*##-4- Compare the sent and received buffers ##############################*/
  if(Buffercmp((uint8_t*)aTxBuffer,(uint8_t*)aRxBuffer,RXBUFFERSIZE))
  {
    /* Processing Error */
    Error_Handler();
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
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_SMBUS_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hsmbus2.Instance = I2C2;
  hsmbus2.Init.Timing = 0x20C0EDFF;
  hsmbus2.Init.AnalogFilter = SMBUS_ANALOGFILTER_ENABLE;
  hsmbus2.Init.OwnAddress1 = 0x42;
  hsmbus2.Init.AddressingMode = SMBUS_ADDRESSINGMODE_7BIT;
  hsmbus2.Init.DualAddressMode = SMBUS_DUALADDRESS_ENABLE;
  hsmbus2.Init.OwnAddress2 = 0xFF;
  hsmbus2.Init.OwnAddress2Masks = SMBUS_OA2_NOMASK;
  hsmbus2.Init.GeneralCallMode = SMBUS_GENERALCALL_DISABLE;
  hsmbus2.Init.NoStretchMode = SMBUS_NOSTRETCH_DISABLE;
  hsmbus2.Init.PacketErrorCheckMode = SMBUS_PEC_ENABLE;
  hsmbus2.Init.PeripheralMode = SMBUS_PERIPHERAL_MODE_SMBUS_SLAVE;
  hsmbus2.Init.SMBusTimeout = 0x00008727;
  if (HAL_SMBUS_Init(&hsmbus2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_SMBUS_ConfigDigitalFilter(&hsmbus2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief  SMBUS error callbacks.
  * @param  SMBUSHandle: SMBUS handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_SMBUS_ErrorCallback(SMBUS_HandleTypeDef *SMBUSHandle)
{
  /** Error_Handler() function is called when error occurs.
    * 1- When Slave doesn't acknowledge its address, Master restarts communication.
    * 2- When Master doesn't acknowledge the last data transferred, Slave doesn't care in this example.
    */
  if (HAL_SMBUS_GetError(SMBUSHandle) != HAL_SMBUS_ERROR_ACKF)
  {

    /* Turn Off LD1 */
    BSP_LED_Off(LD1);

    /* Turn On LD3 */
    BSP_LED_On(LD3);
  }
}

/**
  * @brief  Slave Address Match callbacks.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval None
  */
void HAL_SMBUS_AddrCallback(SMBUS_HandleTypeDef *hsmbus,uint8_t TransferDirection, uint16_t AddrMatchCode)
{
    SlaveAddrMatchCode = (AddrMatchCode << 1);
    UNUSED(TransferDirection);
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0  : pBuffer1 identical to pBuffer2
  *         >0 : pBuffer1 differs from pBuffer2
  */
static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
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
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */

  /* Turn LD1 off */
  BSP_LED_Off(LD1);
  /* Turn LD3 on */
  BSP_LED_On(LD3);
  while(1)
  {
  }
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
  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
