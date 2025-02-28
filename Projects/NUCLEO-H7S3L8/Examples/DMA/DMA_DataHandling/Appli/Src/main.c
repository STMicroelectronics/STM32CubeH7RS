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
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUFFER_SIZE 8U

/* DMA flags declaration */
__IO uint32_t TransferCompleteDetected = 0U, TransferErrorDetected = 0U;

/* Ensure 64-bit alignment for double word transfer */
/* Source buffer */
static const uint8_t aSRC_Buffer[BUFFER_SIZE] __attribute__((aligned (8))) = {0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7};

/* Destination buffer */
static uint8_t aDEST_Buffer[BUFFER_SIZE * 2U] __attribute__((aligned (8),section("noncacheable_buffer")));

/* Right aligned zero padding data expected result buffer */
static const uint8_t aRightAlign_ZeroPadding_ExpectedResult_Buffer[BUFFER_SIZE * 2U] = {0xB0, 0x00, 0xB1, 0x00, 0xB2, 0x00, 0xB3, 0x00, 0xB4, 0x00, 0xB5, 0x00, 0xB6, 0x00, 0xB7, 0x00};

/* Right aligned sign extend data expected result buffer */
static const uint8_t aRightAlign_SignExtend_ExpectedResult_Buffer[BUFFER_SIZE * 2U]  = {0xB0, 0xFF, 0xB1, 0xFF, 0xB2, 0xFF, 0xB3, 0xFF, 0xB4, 0xFF, 0xB5, 0xFF, 0xB6, 0xFF, 0xB7, 0xFF};

/* Right aligned left truncated data expected result buffer */
static const uint8_t aRightAlign_LeftTrunc_ExpectedResult_Buffer[BUFFER_SIZE]   = {0xB0, 0xB2, 0xB4, 0xB6, 0x00, 0x00, 0x00, 0x00};

/* Left aligned right truncated data expected result buffer */
static const uint8_t aLeftAlign_RightTrunc_ExpectedResult_Buffer[BUFFER_SIZE]   = {0xB1, 0xB3, 0xB5, 0xB7, 0x00, 0x00, 0x00, 0x00};

/* Source byte exchange data expected result buffer */
static const uint8_t aSrcByte_Exchange_ExpectedResult_Buffer[BUFFER_SIZE]       = {0xB0, 0xB2, 0xB1, 0xB3, 0xB4, 0xB6, 0xB5, 0xB7};

/* Destination byte exchange data expected result buffer */
static const uint8_t aDestByte_Exchange_ExpectedResult_Buffer[BUFFER_SIZE]      = {0xB1, 0xB0, 0xB3, 0xB2, 0xB5, 0xB4, 0xB7, 0xB6};

/* Destination half word exchange data expected result buffer */
static const uint8_t aDestHalfWord_Exchange_ExpectedResult_Buffer[BUFFER_SIZE]  = {0xB2, 0xB3, 0xB0, 0xB1, 0xB6, 0xB7, 0xB4, 0xB5};

/* Destination word exchange data expected result buffer */
static const uint8_t aDestWord_Exchange_ExpectedResult_Buffer[BUFFER_SIZE]  = {0xB4, 0xB5, 0xB6, 0xB7, 0xB0, 0xB1, 0xB2, 0xB3};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

DMA_HandleTypeDef handle_HPDMA1_Channel12;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_HPDMA1_Init(void);
/* USER CODE BEGIN PFP */
static void RightAlign_SignExtend_DMATransmit(void);
static void RightAlign_LeftTrunc_DMATransmit(void);
static void LeftAlign_RightTrunc_DMATransmit(void);
static void SrcByte_Exchange_DMATransmit(void);
static void DestByte_Exchange_DMATransmit(void);
static void DestHalfWord_Exchange_DMATransmit(void);
static void DestWord_Exchange_DMATransmit(void);

static void TransferComplete(DMA_HandleTypeDef *hdma);
static void TransferError(DMA_HandleTypeDef *hdma);
static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
static void MPU_AdjustRegionAddressSize(uint32_t Address, uint32_t Size, MPU_Region_InitTypeDef* pInit);
static void MPU_Config(void);
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
  MPU_Config();
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
  /* Initialize LD1 and LD3 : GREEN and RED leds */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD3);

  /* Clean destination buffer */
  memset((void *)aDEST_Buffer, 0U, BUFFER_SIZE * 2U);
  TransferCompleteDetected = 0U;
  TransferErrorDetected    = 0U;
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_HPDMA1_Init();
  /* USER CODE BEGIN 2 */

  /* Select Callbacks functions called after Half Transfer Complete, Transfer Complete and Transfer Error */
  HAL_DMA_RegisterCallback(&handle_HPDMA1_Channel12, HAL_DMA_XFER_CPLT_CB_ID, TransferComplete);
  HAL_DMA_RegisterCallback(&handle_HPDMA1_Channel12, HAL_DMA_XFER_ERROR_CB_ID, TransferError);

  /* Configure the source, destination and buffer size DMA fields and Start DMA Channel/Stream transfer */
  /* Enable All the DMA interrupts */
  if (HAL_DMA_Start_IT(&handle_HPDMA1_Channel12, (uint32_t)&aSRC_Buffer, (uint32_t)&aDEST_Buffer, BUFFER_SIZE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Wait for end of transmission or an error occurred */
  while ((TransferCompleteDetected == 0) && (TransferErrorDetected == 0U));

  /* Check DMA error */
  if (TransferErrorDetected == 1U)
  {
    Error_Handler();
  }

  /* Check destination buffer */
  if (Buffercmp((uint8_t*)aDEST_Buffer, (uint8_t*)aRightAlign_ZeroPadding_ExpectedResult_Buffer, BUFFER_SIZE * 2U) != 0U)
  {
    Error_Handler();
  }

  /* Clean destination buffer */
  memset((void *)aDEST_Buffer, 0U, BUFFER_SIZE * 2U);
  TransferCompleteDetected = 0U;
  TransferErrorDetected    = 0U;


  /*##-2- Right aligned / Sign extended data handling ########################*/
  RightAlign_SignExtend_DMATransmit();

  /* Wait for end of transmission or an error occurred */
  while ((TransferCompleteDetected == 0) && (TransferErrorDetected == 0U));

  /* Check DMA error */
  if (TransferErrorDetected == 1U)
  {
    Error_Handler();
  }

  /* Check destination buffer */
  if (Buffercmp((uint8_t*)aDEST_Buffer, (uint8_t*)aRightAlign_SignExtend_ExpectedResult_Buffer, BUFFER_SIZE * 2U) != 0U)
  {
    Error_Handler();
  }

  /* Clean destination buffer */
  memset((void *)aDEST_Buffer, 0U, BUFFER_SIZE * 2U);
  TransferCompleteDetected = 0U;
  TransferErrorDetected    = 0U;


  /*##-3- Right aligned / Left truncated data handling #######################*/
  RightAlign_LeftTrunc_DMATransmit();

  /* Wait for end of transmission or an error occurred */
  while ((TransferCompleteDetected == 0) && (TransferErrorDetected == 0U));

  /* Check DMA error */
  if (TransferErrorDetected == 1U)
  {
    Error_Handler();
  }

  /* Check destination buffer */
  if (Buffercmp((uint8_t*)aDEST_Buffer, (uint8_t*)aRightAlign_LeftTrunc_ExpectedResult_Buffer, BUFFER_SIZE) != 0U)
  {
    Error_Handler();
  }

  /* Clean destination buffer */
  memset((void *)aDEST_Buffer, 0U, BUFFER_SIZE);
  TransferCompleteDetected = 0U;
  TransferErrorDetected    = 0U;


  /*##-4- Left aligned / Right truncated data handling #######################*/
  LeftAlign_RightTrunc_DMATransmit();

  /* Wait for end of transmission or an error occurred */
  while ((TransferCompleteDetected == 0) && (TransferErrorDetected == 0U));

  /* Check DMA error */
  if (TransferErrorDetected == 1U)
  {
    Error_Handler();
  }

  /* Check destination buffer */
  if (Buffercmp((uint8_t*)aDEST_Buffer, (uint8_t*)aLeftAlign_RightTrunc_ExpectedResult_Buffer, BUFFER_SIZE) != 0U)
  {
    Error_Handler();
  }

  /* Clean destination buffer */
  memset((void *)aDEST_Buffer, 0U, BUFFER_SIZE);
  TransferCompleteDetected = 0U;
  TransferErrorDetected    = 0U;


  /*##-5- Source byte exchange data handling #################################*/
  SrcByte_Exchange_DMATransmit();

  /* Wait for end of transmission or an error occurred */
  while ((TransferCompleteDetected == 0) && (TransferErrorDetected == 0U));

  /* Check DMA error */
  if (TransferErrorDetected == 1U)
  {
    Error_Handler();
  }

  /* Check destination buffer */
  if (Buffercmp((uint8_t*)aDEST_Buffer, (uint8_t*)aSrcByte_Exchange_ExpectedResult_Buffer, BUFFER_SIZE) != 0U)
  {
    Error_Handler();
  }

  /* Clean destination buffer */
  memset((void *)aDEST_Buffer, 0U, BUFFER_SIZE);
  TransferCompleteDetected = 0U;
  TransferErrorDetected    = 0U;


  /*##-6- Destination byte exchange data handling #################################*/
  DestByte_Exchange_DMATransmit();

  /* Wait for end of transmission or an error occurred */
  while ((TransferCompleteDetected == 0) && (TransferErrorDetected == 0U));

  /* Check DMA error */
  if (TransferErrorDetected == 1U)
  {
    Error_Handler();
  }

  /* Check destination buffer */
  if (Buffercmp((uint8_t*)aDEST_Buffer, (uint8_t*)aDestByte_Exchange_ExpectedResult_Buffer, BUFFER_SIZE) != 0U)
  {
    Error_Handler();
  }

  /* Clean destination buffer */
  memset((void *)aDEST_Buffer, 0U, BUFFER_SIZE);
  TransferCompleteDetected = 0U;
  TransferErrorDetected    = 0U;

  /*##-7- Destination Half-Word exchange data handling #################################*/
  DestHalfWord_Exchange_DMATransmit();

  /* Wait for end of transmission or an error occurred */
  while ((TransferCompleteDetected == 0) && (TransferErrorDetected == 0U));

  /* Check DMA error */
  if (TransferErrorDetected == 1U)
  {
    Error_Handler();
  }

  /* Check destination buffer */
  if (Buffercmp((uint8_t*)aDEST_Buffer, (uint8_t*)aDestHalfWord_Exchange_ExpectedResult_Buffer, BUFFER_SIZE) != 0U)
  {
    Error_Handler();
  }

  /* Clean destination buffer */
  memset((void *)aDEST_Buffer, 0U, BUFFER_SIZE);
  TransferCompleteDetected = 0U;
  TransferErrorDetected    = 0U;

  /*##-8- Destination word exchange data handling #################################*/
  DestWord_Exchange_DMATransmit();

  /* Wait for end of transmission or an error occurred */
  while ((TransferCompleteDetected == 0) && (TransferErrorDetected == 0U));

  /* Check DMA error */
  if (TransferErrorDetected == 1U)
  {
    Error_Handler();
  }

  /* Check destination buffer */
  if (Buffercmp((uint8_t*)aDEST_Buffer, (uint8_t*)aDestWord_Exchange_ExpectedResult_Buffer, BUFFER_SIZE) != 0U)
  {
    Error_Handler();
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_Delay(250);
    BSP_LED_Toggle(LD1);
  }
  /* USER CODE END 3 */
}

/**
  * @brief HPDMA1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_HPDMA1_Init(void)
{

  /* USER CODE BEGIN HPDMA1_Init 0 */

  /* USER CODE END HPDMA1_Init 0 */

  DMA_DataHandlingConfTypeDef DataHandlingConfig = {0};

  /* Peripheral clock enable */
  __HAL_RCC_HPDMA1_CLK_ENABLE();

  /* HPDMA1 interrupt Init */
    HAL_NVIC_SetPriority(HPDMA1_Channel12_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(HPDMA1_Channel12_IRQn);

  /* USER CODE BEGIN HPDMA1_Init 1 */

  /* USER CODE END HPDMA1_Init 1 */
  handle_HPDMA1_Channel12.Instance = HPDMA1_Channel12;
  handle_HPDMA1_Channel12.Init.Request = DMA_REQUEST_SW;
  handle_HPDMA1_Channel12.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
  handle_HPDMA1_Channel12.Init.Direction = DMA_MEMORY_TO_MEMORY;
  handle_HPDMA1_Channel12.Init.SrcInc = DMA_SINC_INCREMENTED;
  handle_HPDMA1_Channel12.Init.DestInc = DMA_DINC_INCREMENTED;
  handle_HPDMA1_Channel12.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
  handle_HPDMA1_Channel12.Init.DestDataWidth = DMA_DEST_DATAWIDTH_HALFWORD;
  handle_HPDMA1_Channel12.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
  handle_HPDMA1_Channel12.Init.SrcBurstLength = 1;
  handle_HPDMA1_Channel12.Init.DestBurstLength = 1;
  handle_HPDMA1_Channel12.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT0;
  handle_HPDMA1_Channel12.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
  handle_HPDMA1_Channel12.Init.Mode = DMA_NORMAL;
  if (HAL_DMA_Init(&handle_HPDMA1_Channel12) != HAL_OK)
  {
    Error_Handler();
  }
  DataHandlingConfig.DataExchange = DMA_EXCHANGE_NONE;
  DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_ZEROPADDED;
  if (HAL_DMAEx_ConfigDataHandling(&handle_HPDMA1_Channel12, &DataHandlingConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA_ConfigChannelAttributes(&handle_HPDMA1_Channel12, DMA_CHANNEL_NPRIV) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN HPDMA1_Init 2 */

  /* USER CODE END HPDMA1_Init 2 */

}

/* USER CODE BEGIN 4 */
/**
  * @brief  Right aligned and sign extended data DMA transmit
  * @param  None
  * @retval None
  */
static void RightAlign_SignExtend_DMATransmit(void)
{
  DMA_DataHandlingConfTypeDef DataHandlingConfig = {0};

  /* Set DMA Instance */
  handle_HPDMA1_Channel12.Instance                    = HPDMA1_Channel12;

  /* Set parameter to be configured */
  handle_HPDMA1_Channel12.Init.Request                = DMA_REQUEST_SW;
  handle_HPDMA1_Channel12.Init.Direction              = DMA_MEMORY_TO_MEMORY;
  handle_HPDMA1_Channel12.Init.SrcInc                 = DMA_SINC_INCREMENTED;
  handle_HPDMA1_Channel12.Init.DestInc                = DMA_DINC_INCREMENTED;
  handle_HPDMA1_Channel12.Init.SrcDataWidth           = DMA_SRC_DATAWIDTH_BYTE;
  handle_HPDMA1_Channel12.Init.DestDataWidth          = DMA_DEST_DATAWIDTH_HALFWORD;
  handle_HPDMA1_Channel12.Init.SrcBurstLength         = 1;
  handle_HPDMA1_Channel12.Init.DestBurstLength        = 1;
  handle_HPDMA1_Channel12.Init.Priority               = DMA_LOW_PRIORITY_HIGH_WEIGHT;
  handle_HPDMA1_Channel12.Init.TransferEventMode      = DMA_TCEM_BLOCK_TRANSFER;
  handle_HPDMA1_Channel12.Init.TransferAllocatedPort  = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
  handle_HPDMA1_Channel12.Init.Mode                   = DMA_NORMAL;

  /* Initialize the DMA channel */
  if (HAL_DMA_Init(&handle_HPDMA1_Channel12) != HAL_OK)
  {
    Error_Handler();
  }

  /* Set data handling block configuration */
  DataHandlingConfig.DataExchange  = DMA_EXCHANGE_NONE;
  DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_SIGNEXT;

  /* Data handling configuration */
  if (HAL_DMAEx_ConfigDataHandling(&handle_HPDMA1_Channel12, &DataHandlingConfig) != HAL_OK)
  {
    while (1); /* KO */
  }

  /* Select Callbacks functions called after Half Transfer Complete, Transfer Complete and Transfer Error */
  HAL_DMA_RegisterCallback(&handle_HPDMA1_Channel12, HAL_DMA_XFER_CPLT_CB_ID, TransferComplete);
  HAL_DMA_RegisterCallback(&handle_HPDMA1_Channel12, HAL_DMA_XFER_ERROR_CB_ID, TransferError);

  /* Configure the source, destination and buffer size DMA fields and Start DMA Channel/Stream transfer */
  /* Enable All the DMA interrupts */
  if (HAL_DMA_Start_IT(&handle_HPDMA1_Channel12, (uint32_t)&aSRC_Buffer, (uint32_t)&aDEST_Buffer, BUFFER_SIZE) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Right aligned and left truncated data DMA transmit
  * @param  None
  * @retval None
  */
static void RightAlign_LeftTrunc_DMATransmit(void)
{
  DMA_DataHandlingConfTypeDef DataHandlingConfig = {0};

  /* Set DMA Instance */
  handle_HPDMA1_Channel12.Instance                    = HPDMA1_Channel12;

  /* Set parameter to be configured */
  handle_HPDMA1_Channel12.Init.Request                = DMA_REQUEST_SW;
  handle_HPDMA1_Channel12.Init.Direction              = DMA_MEMORY_TO_MEMORY;
  handle_HPDMA1_Channel12.Init.SrcInc                 = DMA_SINC_INCREMENTED;
  handle_HPDMA1_Channel12.Init.DestInc                = DMA_DINC_INCREMENTED;
  handle_HPDMA1_Channel12.Init.SrcDataWidth           = DMA_SRC_DATAWIDTH_HALFWORD;
  handle_HPDMA1_Channel12.Init.DestDataWidth          = DMA_DEST_DATAWIDTH_BYTE;
  handle_HPDMA1_Channel12.Init.SrcBurstLength         = 1;
  handle_HPDMA1_Channel12.Init.DestBurstLength        = 1;
  handle_HPDMA1_Channel12.Init.Priority               = DMA_LOW_PRIORITY_HIGH_WEIGHT;
  handle_HPDMA1_Channel12.Init.TransferEventMode      = DMA_TCEM_BLOCK_TRANSFER;
  handle_HPDMA1_Channel12.Init.TransferAllocatedPort  = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
  handle_HPDMA1_Channel12.Init.Mode                   = DMA_NORMAL;

  /* Initialize the DMA channel */
  if (HAL_DMA_Init(&handle_HPDMA1_Channel12) != HAL_OK)
  {
    Error_Handler();
  }

  /* Set data handling block configuration */
  DataHandlingConfig.DataExchange  = DMA_EXCHANGE_NONE;
  DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_LEFTTRUNC;

  /* Data handling configuration */
  if (HAL_DMAEx_ConfigDataHandling(&handle_HPDMA1_Channel12, &DataHandlingConfig) != HAL_OK)
  {
    while (1); /* KO */
  }

  /* Select Callbacks functions called after Half Transfer Complete, Transfer Complete and Transfer Error */
  HAL_DMA_RegisterCallback(&handle_HPDMA1_Channel12, HAL_DMA_XFER_CPLT_CB_ID, TransferComplete);
  HAL_DMA_RegisterCallback(&handle_HPDMA1_Channel12, HAL_DMA_XFER_ERROR_CB_ID, TransferError);

  /* Configure the source, destination and buffer size DMA fields and Start DMA Channel/Stream transfer */
  /* Enable All the DMA interrupts */
  if (HAL_DMA_Start_IT(&handle_HPDMA1_Channel12, (uint32_t)&aSRC_Buffer, (uint32_t)&aDEST_Buffer, BUFFER_SIZE) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Left aligned and right truncated data DMA transmit
  * @param  None
  * @retval None
  */
static void LeftAlign_RightTrunc_DMATransmit(void)
{
  DMA_DataHandlingConfTypeDef DataHandlingConfig = {0};

  /* Set DMA Instance */
  handle_HPDMA1_Channel12.Instance                    = HPDMA1_Channel12;

  /* Set parameter to be configured */
  handle_HPDMA1_Channel12.Init.Request                = DMA_REQUEST_SW;
  handle_HPDMA1_Channel12.Init.Direction              = DMA_MEMORY_TO_MEMORY;
  handle_HPDMA1_Channel12.Init.SrcInc                 = DMA_SINC_INCREMENTED;
  handle_HPDMA1_Channel12.Init.DestInc                = DMA_DINC_INCREMENTED;
  handle_HPDMA1_Channel12.Init.SrcDataWidth           = DMA_SRC_DATAWIDTH_HALFWORD;
  handle_HPDMA1_Channel12.Init.DestDataWidth          = DMA_DEST_DATAWIDTH_BYTE;
  handle_HPDMA1_Channel12.Init.SrcBurstLength         = 1;
  handle_HPDMA1_Channel12.Init.DestBurstLength        = 1;
  handle_HPDMA1_Channel12.Init.Priority               = DMA_LOW_PRIORITY_HIGH_WEIGHT;
  handle_HPDMA1_Channel12.Init.TransferEventMode      = DMA_TCEM_BLOCK_TRANSFER;
  handle_HPDMA1_Channel12.Init.TransferAllocatedPort  = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
  handle_HPDMA1_Channel12.Init.Mode                   = DMA_NORMAL;

  /* Initialize the DMA channel */
  if (HAL_DMA_Init(&handle_HPDMA1_Channel12) != HAL_OK)
  {
    Error_Handler();
  }

  /* Set data handling block configuration */
  DataHandlingConfig.DataExchange  = DMA_EXCHANGE_NONE;
  DataHandlingConfig.DataAlignment = DMA_DATA_LEFTALIGN_RIGHTTRUNC;

  /* Data handling configuration */
  if (HAL_DMAEx_ConfigDataHandling(&handle_HPDMA1_Channel12, &DataHandlingConfig) != HAL_OK)
  {
    while (1); /* KO */
  }


  /* Select Callbacks functions called after Half Transfer Complete, Transfer Complete and Transfer Error */
  HAL_DMA_RegisterCallback(&handle_HPDMA1_Channel12, HAL_DMA_XFER_CPLT_CB_ID, TransferComplete);
  HAL_DMA_RegisterCallback(&handle_HPDMA1_Channel12, HAL_DMA_XFER_ERROR_CB_ID, TransferError);

  /* Configure the source, destination and buffer size DMA fields and Start DMA Channel/Stream transfer */
  /* Enable All the DMA interrupts */
  if (HAL_DMA_Start_IT(&handle_HPDMA1_Channel12, (uint32_t)&aSRC_Buffer, (uint32_t)&aDEST_Buffer, BUFFER_SIZE) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Source byte exchange data DMA transmit
  * @param  None
  * @retval None
  */
static void SrcByte_Exchange_DMATransmit(void)
{
  DMA_DataHandlingConfTypeDef DataHandlingConfig = {0};

  /* Set DMA Instance */
  handle_HPDMA1_Channel12.Instance                    = HPDMA1_Channel12;

  /* Set parameter to be configured */
  handle_HPDMA1_Channel12.Init.Request                = DMA_REQUEST_SW;
  handle_HPDMA1_Channel12.Init.Direction              = DMA_MEMORY_TO_MEMORY;
  handle_HPDMA1_Channel12.Init.SrcInc                 = DMA_SINC_INCREMENTED;
  handle_HPDMA1_Channel12.Init.DestInc                = DMA_DINC_INCREMENTED;
  handle_HPDMA1_Channel12.Init.SrcDataWidth           = DMA_SRC_DATAWIDTH_WORD;
  handle_HPDMA1_Channel12.Init.DestDataWidth          = DMA_DEST_DATAWIDTH_WORD;
  handle_HPDMA1_Channel12.Init.SrcBurstLength         = 1;
  handle_HPDMA1_Channel12.Init.DestBurstLength        = 1;
  handle_HPDMA1_Channel12.Init.Priority               = DMA_LOW_PRIORITY_HIGH_WEIGHT;
  handle_HPDMA1_Channel12.Init.TransferEventMode      = DMA_TCEM_BLOCK_TRANSFER;
  handle_HPDMA1_Channel12.Init.TransferAllocatedPort  = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
  handle_HPDMA1_Channel12.Init.Mode                   = DMA_NORMAL;

  /* Initialize the DMA channel */
  if (HAL_DMA_Init(&handle_HPDMA1_Channel12) != HAL_OK)
  {
    Error_Handler();
  }

  /* Set data handling block configuration */
  DataHandlingConfig.DataExchange  = DMA_EXCHANGE_SRC_BYTE;
  DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_ZEROPADDED;

  /* Data handling configuration */
  if (HAL_DMAEx_ConfigDataHandling(&handle_HPDMA1_Channel12, &DataHandlingConfig) != HAL_OK)
  {
    while (1); /* KO */
  }

  /* Select Callbacks functions called after Half Transfer Complete, Transfer Complete and Transfer Error */
  HAL_DMA_RegisterCallback(&handle_HPDMA1_Channel12, HAL_DMA_XFER_CPLT_CB_ID, TransferComplete);
  HAL_DMA_RegisterCallback(&handle_HPDMA1_Channel12, HAL_DMA_XFER_ERROR_CB_ID, TransferError);

  /* Configure the source, destination and buffer size DMA fields and Start DMA Channel/Stream transfer */
  /* Enable All the DMA interrupts */
  if (HAL_DMA_Start_IT(&handle_HPDMA1_Channel12, (uint32_t)&aSRC_Buffer, (uint32_t)&aDEST_Buffer, BUFFER_SIZE) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Destination byte exchange data DMA transmit
  * @param  None
  * @retval None
  */
static void DestByte_Exchange_DMATransmit(void)
{
  DMA_DataHandlingConfTypeDef DataHandlingConfig = {0};

  /* Set DMA Instance */
  handle_HPDMA1_Channel12.Instance                    = HPDMA1_Channel12;

  /* Set parameter to be configured */
  handle_HPDMA1_Channel12.Init.Request                = DMA_REQUEST_SW;
  handle_HPDMA1_Channel12.Init.Direction              = DMA_MEMORY_TO_MEMORY;
  handle_HPDMA1_Channel12.Init.SrcInc                 = DMA_SINC_INCREMENTED;
  handle_HPDMA1_Channel12.Init.DestInc                = DMA_DINC_INCREMENTED;
  handle_HPDMA1_Channel12.Init.SrcDataWidth           = DMA_SRC_DATAWIDTH_WORD;
  handle_HPDMA1_Channel12.Init.DestDataWidth          = DMA_DEST_DATAWIDTH_WORD;
  handle_HPDMA1_Channel12.Init.SrcBurstLength         = 1;
  handle_HPDMA1_Channel12.Init.DestBurstLength        = 1;
  handle_HPDMA1_Channel12.Init.Priority               = DMA_LOW_PRIORITY_HIGH_WEIGHT;
  handle_HPDMA1_Channel12.Init.TransferEventMode      = DMA_TCEM_BLOCK_TRANSFER;
  handle_HPDMA1_Channel12.Init.Mode                   = DMA_NORMAL;

  /* Initialize the DMA channel */
  if (HAL_DMA_Init(&handle_HPDMA1_Channel12) != HAL_OK)
  {
    Error_Handler();
  }

  /* Set data handling block configuration */
  DataHandlingConfig.DataExchange  = DMA_EXCHANGE_DEST_BYTE;
  DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_ZEROPADDED;

  /* Data handling configuration */
  if (HAL_DMAEx_ConfigDataHandling(&handle_HPDMA1_Channel12, &DataHandlingConfig) != HAL_OK)
  {
    while (1); /* KO */
  }

  /* Select Callbacks functions called after Half Transfer Complete, Transfer Complete and Transfer Error */
  HAL_DMA_RegisterCallback(&handle_HPDMA1_Channel12, HAL_DMA_XFER_CPLT_CB_ID, TransferComplete);
  HAL_DMA_RegisterCallback(&handle_HPDMA1_Channel12, HAL_DMA_XFER_ERROR_CB_ID, TransferError);

  /* Configure the source, destination and buffer size DMA fields and Start DMA Channel/Stream transfer */
  /* Enable All the DMA interrupts */
  if (HAL_DMA_Start_IT(&handle_HPDMA1_Channel12, (uint32_t)&aSRC_Buffer, (uint32_t)&aDEST_Buffer, BUFFER_SIZE) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Destination half word exchange data DMA transmit
  * @param  None
  * @retval None
  */
static void DestHalfWord_Exchange_DMATransmit(void)
{
  DMA_DataHandlingConfTypeDef DataHandlingConfig = {0};

  /* Set DMA Instance */
  handle_HPDMA1_Channel12.Instance                    = HPDMA1_Channel12;

  /* Set parameter to be configured */
  handle_HPDMA1_Channel12.Init.Request                = DMA_REQUEST_SW;
  handle_HPDMA1_Channel12.Init.Direction              = DMA_MEMORY_TO_MEMORY;
  handle_HPDMA1_Channel12.Init.SrcInc                 = DMA_SINC_INCREMENTED;
  handle_HPDMA1_Channel12.Init.DestInc                = DMA_DINC_INCREMENTED;
  handle_HPDMA1_Channel12.Init.SrcDataWidth           = DMA_SRC_DATAWIDTH_WORD;
  handle_HPDMA1_Channel12.Init.DestDataWidth          = DMA_DEST_DATAWIDTH_WORD;
  handle_HPDMA1_Channel12.Init.SrcBurstLength         = 1;
  handle_HPDMA1_Channel12.Init.DestBurstLength        = 1;
  handle_HPDMA1_Channel12.Init.Priority               = DMA_LOW_PRIORITY_HIGH_WEIGHT;
  handle_HPDMA1_Channel12.Init.TransferEventMode      = DMA_TCEM_BLOCK_TRANSFER;
  handle_HPDMA1_Channel12.Init.Mode                   = DMA_NORMAL;

  /* Initialize the DMA channel */
  if (HAL_DMA_Init(&handle_HPDMA1_Channel12) != HAL_OK)
  {
    Error_Handler();
  }

  /* Set data handling block configuration */
  DataHandlingConfig.DataExchange  = DMA_EXCHANGE_DEST_HALFWORD;
  DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_ZEROPADDED;

  /* Data handling configuration */
  if (HAL_DMAEx_ConfigDataHandling(&handle_HPDMA1_Channel12, &DataHandlingConfig) != HAL_OK)
  {
    while (1); /* KO */
  }

  /* Select Callbacks functions called after Half Transfer Complete, Transfer Complete and Transfer Error */
  HAL_DMA_RegisterCallback(&handle_HPDMA1_Channel12, HAL_DMA_XFER_CPLT_CB_ID, TransferComplete);
  HAL_DMA_RegisterCallback(&handle_HPDMA1_Channel12, HAL_DMA_XFER_ERROR_CB_ID, TransferError);

  /* Configure the source, destination and buffer size DMA fields and Start DMA Channel/Stream transfer */
  /* Enable All the DMA interrupts */
  if (HAL_DMA_Start_IT(&handle_HPDMA1_Channel12, (uint32_t)&aSRC_Buffer, (uint32_t)&aDEST_Buffer, BUFFER_SIZE) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Destination word exchange data DMA transmit
  * @param  None
  * @retval None
  */
static void DestWord_Exchange_DMATransmit(void)
{
  DMA_DataHandlingConfTypeDef DataHandlingConfig = {0};

  /* Set DMA Instance */
  handle_HPDMA1_Channel12.Instance                    = HPDMA1_Channel12;

  /* Set parameter to be configured */
  handle_HPDMA1_Channel12.Init.Request                = DMA_REQUEST_SW;
  handle_HPDMA1_Channel12.Init.Direction              = DMA_MEMORY_TO_MEMORY;
  handle_HPDMA1_Channel12.Init.SrcInc                 = DMA_SINC_INCREMENTED;
  handle_HPDMA1_Channel12.Init.DestInc                = DMA_DINC_INCREMENTED;
  handle_HPDMA1_Channel12.Init.SrcDataWidth           = DMA_SRC_DATAWIDTH_DOUBLEWORD;
  handle_HPDMA1_Channel12.Init.DestDataWidth          = DMA_DEST_DATAWIDTH_DOUBLEWORD;
  handle_HPDMA1_Channel12.Init.SrcBurstLength         = 4;
  handle_HPDMA1_Channel12.Init.DestBurstLength        = 4;
  handle_HPDMA1_Channel12.Init.Priority               = DMA_LOW_PRIORITY_HIGH_WEIGHT;
  handle_HPDMA1_Channel12.Init.TransferEventMode      = DMA_TCEM_BLOCK_TRANSFER;
  handle_HPDMA1_Channel12.Init.TransferAllocatedPort  = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
  handle_HPDMA1_Channel12.Init.Mode                   = DMA_NORMAL;

  /* Initialize the DMA channel */
  if (HAL_DMA_Init(&handle_HPDMA1_Channel12) != HAL_OK)
  {
    Error_Handler();
  }

  /* Set data handling block configuration */
  DataHandlingConfig.DataExchange  = DMA_EXCHANGE_DEST_WORD;
  DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_ZEROPADDED;

  /* Data handling configuration */
  if (HAL_DMAEx_ConfigDataHandling(&handle_HPDMA1_Channel12, &DataHandlingConfig) != HAL_OK)
  {
    while (1); /* KO */
  }

  /* Select Callbacks functions called after Half Transfer Complete, Transfer Complete and Transfer Error */
  HAL_DMA_RegisterCallback(&handle_HPDMA1_Channel12, HAL_DMA_XFER_CPLT_CB_ID, TransferComplete);
  HAL_DMA_RegisterCallback(&handle_HPDMA1_Channel12, HAL_DMA_XFER_ERROR_CB_ID, TransferError);

  /* Configure the source, destination and buffer size DMA fields and Start DMA Channel/Stream transfer */
  /* Enable All the DMA interrupts */
  if (HAL_DMA_Start_IT(&handle_HPDMA1_Channel12, (uint32_t)&aSRC_Buffer, (uint32_t)&aDEST_Buffer, BUFFER_SIZE) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  DMA transfer complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
static void TransferComplete(DMA_HandleTypeDef *hdma)
{
  TransferCompleteDetected = 1U;
}

/**
  * @brief  DMA transfer error callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
static void TransferError(DMA_HandleTypeDef *hdma)
{
  TransferErrorDetected = 1U;
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0   : pBuffer1 identical to pBuffer2
  *         > 0 : pBuffer1 differs from pBuffer2
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

/**
  * @brief  This function configures the MPU context of the application.
  * @retval None
  */
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};
  uint32_t index = MPU_REGION_NUMBER0;
  uint32_t address;
  uint32_t size;

  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Initialize the background region */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = index;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  index++;

  /* Initialize the non cacheable region */
#if defined ( __ICCARM__ )
  /* get the region attribute form the icf file */
  extern uint32_t NONCACHEABLEBUFFER_start;
  extern uint32_t NONCACHEABLEBUFFER_size;

  address = (uint32_t)&NONCACHEABLEBUFFER_start;
  size = (uint32_t)&NONCACHEABLEBUFFER_size;

#elif defined (__CC_ARM) || defined(__ARMCC_VERSION)
  extern uint32_t Image$$RW_NONCACHEABLEBUFFER$$Base;
  extern uint32_t Image$$RW_NONCACHEABLEBUFFER$$Length;
  extern uint32_t Image$$RW_NONCACHEABLEBUFFER$$ZI$$Length;

  address = (uint32_t)&Image$$RW_NONCACHEABLEBUFFER$$Base;
  size  = (uint32_t)&Image$$RW_NONCACHEABLEBUFFER$$Length + (uint32_t)&Image$$RW_NONCACHEABLEBUFFER$$ZI$$Length;
#elif defined ( __GNUC__ )
  extern int __NONCACHEABLEBUFFER_BEGIN;
  extern int __NONCACHEABLEBUFFER_END;

  address = (uint32_t)&__NONCACHEABLEBUFFER_BEGIN;
  size  = (uint32_t)&__NONCACHEABLEBUFFER_END - (uint32_t)&__NONCACHEABLEBUFFER_BEGIN;
#else
#error "Compiler toolchain is unsupported"
#endif

  if (size != 0)
  {
    /* Configure the MPU attributes as Normal Non Cacheable */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number = index;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_AdjustRegionAddressSize(address, size, &MPU_InitStruct);
    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    index++;
  }

  /* Initialize the region corresponding to the execution area
     (external or internal flash or external or internal RAM
     depending on scatter file definition) */
#if defined ( __ICCARM__ )
  extern uint32_t __ICFEDIT_region_ROM_start__;
  extern uint32_t __ICFEDIT_region_ROM_end__;
  address = (uint32_t)&__ICFEDIT_region_ROM_start__;
  size = (uint32_t)&__ICFEDIT_region_ROM_end__ - (uint32_t)&__ICFEDIT_region_ROM_start__ + 1;
#elif defined (__CC_ARM) || defined(__ARMCC_VERSION)
  extern uint32_t Image$$ER_ROM$$Base;
  extern uint32_t Image$$ER_ROM$$Limit;
  address = (uint32_t)&Image$$ER_ROM$$Base;
  size    = (uint32_t)&Image$$ER_ROM$$Limit-(uint32_t)&Image$$ER_ROM$$Base;
#elif defined ( __GNUC__ )
  extern uint32_t __FLASH_BEGIN;
  extern uint32_t __FLASH_SIZE;
  address = (uint32_t)&__FLASH_BEGIN;
  size  = (uint32_t)&__FLASH_SIZE;
#else
#error "Compiler toolchain is unsupported"
#endif

  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = index;
  MPU_InitStruct.SubRegionDisable = 0u;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
  MPU_AdjustRegionAddressSize(address, size, &MPU_InitStruct);
  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  index++;

  /* Reset unused MPU regions */
  for(; index < __MPU_REGIONCOUNT ; index++)
  {
    /* All unused regions disabled */
    MPU_InitStruct.Enable = MPU_REGION_DISABLE;
    MPU_InitStruct.Number = index;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);
  }

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
  * @brief This function adjusts the MPU region Address and Size within an MPU configuration.
  * @param Address memory address
  * @param Size memory size
  * @param pInit pointer to an MPU initialization structure
  * @retval None
  */
static void MPU_AdjustRegionAddressSize(uint32_t Address, uint32_t Size, MPU_Region_InitTypeDef* pInit)
{
  static uint8_t loopcontrol = 0;
  uint32_t Modulo;

  /* control the loop increment */
  loopcontrol++;

  if (loopcontrol > 3)
  {
    /* the scatter file input is too complex to determine the MPU configuration */
    Error_Handler();
  }

  /* Compute the MPU region size */
  pInit->Size = ((31 - __CLZ(Size)) - 1);
  if (Size > (1 << (pInit->Size + 1)))
  {
    pInit->Size++;
  }
  Modulo = Address % (1 << (pInit->Size + 1));
  if (0 != Modulo)
  {
    /* Align address with MPU region size considering there is no need to increase the size */
    MPU_AdjustRegionAddressSize(Address - Modulo, Size + Modulo, pInit);
  }
  else
  {
    pInit->BaseAddress = Address;
  }

  /* control the loop decrement */
  loopcontrol--;
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
  /* Turn LD3 on */
  BSP_LED_On(LD3);
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
