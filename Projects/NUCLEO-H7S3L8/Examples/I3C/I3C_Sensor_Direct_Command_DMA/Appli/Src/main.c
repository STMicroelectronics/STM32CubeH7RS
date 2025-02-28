/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    I3C/I3C_Sensor_Direct_Command_DMA/Appli/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32H7RSxx I3C HAL API as Controller
  *          to manage a Direct Command procedure between a Controller and a Target
  *          with a communication process based on DMA transfer on Controller Side.
  *          The communication is done using STM32H7RSxx Nucleo and
  *          X-NUCLEO-IKS01A3.
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
#include "desc_target1.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define I3C_IDX_FRAME_1         0U  /* Index of Frame 1, GET Direct CCC */
#define I3C_IDX_FRAME_2         1U  /* Index of Frame 2, SET Direct CCC */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

I3C_HandleTypeDef hi3c1;
DMA_HandleTypeDef handle_GPDMA1_Channel2;
DMA_HandleTypeDef handle_GPDMA1_Channel1;
DMA_HandleTypeDef handle_GPDMA1_Channel0;

/* USER CODE BEGIN PV */
__IO uint8_t LoopCount = 1;

/* Context buffer related to Frame context, contain different buffer value for a communication */
I3C_XferTypeDef aContextBuffers[2] __attribute__((section("noncacheable_buffer")));

/* Number of Targets detected during DAA procedure */
__IO uint32_t uwTargetCount = 0;

/* Buffer used for reception during Get CCC procedure */
uint8_t aRxBuffer[0x1F] __attribute__((section("noncacheable_buffer")));

/* Buffer used for transmission during Set CCC procedure */
uint8_t aTxBuffer[0x0F] __attribute__((section("noncacheable_buffer")));

/* Buffer used by HAL to compute control data for the Direct Communication */
uint32_t aControlBuffer[0x1F] __attribute__((section("noncacheable_buffer")));

/* Array contain targets descriptor */
TargetDesc_TypeDef *aTargetDesc[1] =
                          {
                            &TargetDesc1       /* DEVICE_ID1 */
                          };

/* Array contain SET CCC associated data */
uint8_t aDISEC_data[1]   = {0x08};
uint8_t aSETDASA_LSM6DSO_data[1]   = {(TARGET1_DYN_ADDR << 1)};
uint8_t aSETMWL_data[2]   = {0x0, 0x4};
uint8_t aSETMRL_data[2]   = {0x0, 0x4};

/* Descriptor for broadcast CCC */
I3C_CCCTypeDef aBroadcast_CCC[] =
{
    /*   Target Addr              CCC Value    CCC data + defbyte pointer  CCC size + defbyte         Direction        */
    {0,                Broadcast_DISEC,           {aDISEC_data,           1},              LL_I3C_DIRECTION_WRITE},
    {0,                Broadcast_RSTDAA,          {NULL,                  0},              LL_I3C_DIRECTION_WRITE},
};

/* Descriptor for direct CCC */
I3C_CCCTypeDef aDirect_CCC[] =
{
    /*   Target Addr              CCC Value    CCC data + defbyte pointer  CCC size + defbyte         Direction        */
    {LSM6DSO_STA,         Direct_SETDASA,            {aSETDASA_LSM6DSO_data, 1},              LL_I3C_DIRECTION_WRITE},
};

/* Descriptor for direct CCC */
I3C_CCCTypeDef aCCCList[] =
{
    /*   Target Addr              CCC Value    CCC data + defbyte pointer  CCC size + defbyte         Direction        */
    {TARGET1_DYN_ADDR,          Direct_GETPID,          {NULL,                  6},              LL_I3C_DIRECTION_READ },
    {TARGET1_DYN_ADDR,          Direct_GETBCR,          {NULL,                  1},              LL_I3C_DIRECTION_READ },
    {TARGET1_DYN_ADDR,          Direct_GETDCR,          {NULL,                  1},              LL_I3C_DIRECTION_READ },
    {TARGET1_DYN_ADDR,          Direct_SETMWL,          {aSETMWL_data,          2},              LL_I3C_DIRECTION_WRITE},
    {TARGET1_DYN_ADDR,          Direct_GETMWL,          {NULL,                  2},              LL_I3C_DIRECTION_READ },
    {TARGET1_DYN_ADDR,          Direct_SETMRL,          {aSETMRL_data,          2},              LL_I3C_DIRECTION_WRITE},
    {TARGET1_DYN_ADDR,          Direct_GETMRL,          {NULL,                  2},              LL_I3C_DIRECTION_READ },
    {TARGET1_DYN_ADDR,          Direct_GETSTATUS,       {NULL,                  2},              LL_I3C_DIRECTION_READ },
};

/* Array contain Get CCC name in char, this array is use to print the value receive during Get CCC procedure */
char* aCommandCode[] = {
  "GETPID",
  "GETBCR",
  "GETDCR",
  "GETMWL",
  "GETMRL",
  "GETSTATUS"};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_GPDMA1_Init(void);
static void MX_GPIO_Init(void);
static void MX_FLASH_Init(void);
static void MX_I3C1_Init(void);
/* USER CODE BEGIN PFP */
static void MPU_AdjustRegionAddressSize(uint32_t Address, uint32_t Size, MPU_Region_InitTypeDef* pInit);
static void MPU_Config(void);
static void DisplayCCCValue(I3C_CCCTypeDef *pGetCCCList, char **pCCCCharList, uint8_t *pCCCBuffer, uint8_t nbCCC);
#if (defined (__GNUC__) && !defined(__ARMCC_VERSION))
extern void initialise_monitor_handles(void);
#endif
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
  LL_I3C_CtrlBusConfTypeDef CtrlBusConf;
  I3C_CtrlTimingTypeDef CtrlTiming;

  MPU_Config();

#if (defined (__GNUC__) && !defined(__ARMCC_VERSION))
  initialise_monitor_handles();
  printf("Semihosting Test...\n\r");
#endif
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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPDMA1_Init();
  MX_GPIO_Init();
  MX_FLASH_Init();
  MX_I3C1_Init();
  /* USER CODE BEGIN 2 */
  /* Configure LD1 */
  BSP_LED_Init(LD1);

  /* Configure USER push-button */
  BSP_PB_Init(BUTTON_USER,BUTTON_MODE_GPIO);

  /*#-STEP1-#- Disable IBI interrupt, Reset a previous Dynamic address ###############*/
  aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.pBuffer = aControlBuffer;
  aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.Size    = COUNTOF(aControlBuffer);
  aContextBuffers[I3C_IDX_FRAME_1].TxBuf.pBuffer   = aTxBuffer;
  aContextBuffers[I3C_IDX_FRAME_1].TxBuf.Size      = 1;

  /* Associate a new Dynamic address through Static address */
  aContextBuffers[I3C_IDX_FRAME_2].CtrlBuf.pBuffer = aControlBuffer;
  aContextBuffers[I3C_IDX_FRAME_2].CtrlBuf.Size    = COUNTOF(aControlBuffer);
  aContextBuffers[I3C_IDX_FRAME_2].TxBuf.pBuffer   = aTxBuffer;
  aContextBuffers[I3C_IDX_FRAME_2].TxBuf.Size      = 1;

  /*##- Add context buffer Set CCC frame in Frame context ###########################*/
  if (HAL_I3C_AddDescToFrame(&hi3c1,
                             aBroadcast_CCC,
                             NULL,
                             &aContextBuffers[I3C_IDX_FRAME_1],
                             COUNTOF(aBroadcast_CCC),
                             I3C_BROADCAST_WITHOUT_DEFBYTE_RESTART) != HAL_OK)
  {
    /* Error_Handler() function is called when error occurs. */
    Error_Handler();
  }

  if (HAL_I3C_Ctrl_TransmitCCC_IT(&hi3c1, &aContextBuffers[I3C_IDX_FRAME_1]) != HAL_OK)
  {
    /* Error_Handler() function is called when error occurs. */
    Error_Handler();
  }

  /*##- Wait for the end of the transfer #################################*/
  /*  Before starting a new communication transfer, you need to check the current
  state of the peripheral; if it is busy you need to wait for the end of current
  transfer before starting a new one.
  For simplicity reasons, this example is just waiting till the end of the
  transfer, but application may perform other tasks while transfer operation
  is ongoing. */
  while (HAL_I3C_GetState(&hi3c1) != HAL_I3C_STATE_READY)
  {
  }

  /* Wait for USER push-button press before starting the Communication */
  while (BSP_PB_GetState(BUTTON_USER) != GPIO_PIN_RESET)
  {
  }

  /* Wait for USER push-button release before starting the Communication */
  while (BSP_PB_GetState(BUTTON_USER) != GPIO_PIN_SET)
  {
  }

  /*#-STEP2-#- Set Dynamic address using Static address #############################*/
  /*##- Add context buffer Set CCC frame in Frame context ###########################*/
  if (HAL_I3C_AddDescToFrame(&hi3c1,
                             aDirect_CCC,
                             NULL,
                             &aContextBuffers[I3C_IDX_FRAME_2],
                             COUNTOF(aDirect_CCC),
                             I3C_DIRECT_WITHOUT_DEFBYTE_STOP) != HAL_OK)
  {
    /* Error_Handler() function is called when error occurs. */
    Error_Handler();
  }

  if (HAL_I3C_Ctrl_TransmitCCC_IT(&hi3c1, &aContextBuffers[I3C_IDX_FRAME_2]) != HAL_OK)
  {
    /* Error_Handler() function is called when error occurs. */
    Error_Handler();
  }

  /*##- Wait for the end of the transfer #################################*/
  /*  Before starting a new communication transfer, you need to check the current
  state of the peripheral; if it is busy you need to wait for the end of current
  transfer before starting a new one.
  For simplicity reasons, this example is just waiting till the end of the
  transfer, but application may perform other tasks while transfer operation
  is ongoing. */
  while (HAL_I3C_GetState(&hi3c1) != HAL_I3C_STATE_READY)
  {
  }

  /* After a dynamic address has been assigned, the sensor is recognized as an I3C device */
  /*##- Check if the LSM6DSO sensor is ready to communicate in I3C #######################*/
  if (HAL_I3C_Ctrl_IsDeviceI3C_Ready(&hi3c1, TARGET1_DYN_ADDR, 300, 1000) != HAL_OK)
  {
    /* Error_Handler() function is called when error occurs. */
    Error_Handler();
  }

  /* Set bus speed to 5MHz as on IKS01A3 is the maximum allowed speed due to the level shifters */
  /* Calculate the new timing for Controller */
  CtrlTiming.clockSrcFreq = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_I2C1_I3C1);
  CtrlTiming.i3cPPFreq = 5000000;
  CtrlTiming.i2cODFreq = 0;
  CtrlTiming.dutyCycle = 50;
  CtrlTiming.busType = I3C_PURE_I3C_BUS;

  /* Calculate the new timing for Controller */
  I3C_CtrlTimingComputation(&CtrlTiming, &CtrlBusConf);

  /* Update Controller Bus characteristic */
  HAL_I3C_Ctrl_BusCharacteristicConfig(&hi3c1, &CtrlBusConf);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    LoopCount++;

    /*#-STEP3-#- Set and retrieve data from the Gyroscope #############################*/
    /*##- Prepare context buffers process #############################################*/
    /* Prepare multiple transfer context buffer with the different parameters */
    aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.pBuffer = aControlBuffer;
    aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.Size    = COUNTOF(aControlBuffer);
    aContextBuffers[I3C_IDX_FRAME_1].TxBuf.pBuffer   = aTxBuffer;
    aContextBuffers[I3C_IDX_FRAME_1].TxBuf.Size      = 4;
    aContextBuffers[I3C_IDX_FRAME_1].RxBuf.pBuffer   = aRxBuffer;
    aContextBuffers[I3C_IDX_FRAME_1].RxBuf.Size      = 14;

    /*##- Add context buffer multiple transfer CCC frame in Frame context ##############*/
    if (HAL_I3C_AddDescToFrame(&hi3c1,
                               aCCCList,
                               NULL,
                               &aContextBuffers[I3C_IDX_FRAME_1],
                               COUNTOF(aCCCList),
                               I3C_DIRECT_WITHOUT_DEFBYTE_RESTART) != HAL_OK)
    {
      /* Error_Handler() function is called when error occurs. */
      Error_Handler();
    }

    /* Wait for USER push-button press before starting the Communication */
    while (BSP_PB_GetState(BUTTON_USER) != GPIO_PIN_RESET)
    {
    }

    /* Wait for USER push-button release before starting the Communication */
    while (BSP_PB_GetState(BUTTON_USER) != GPIO_PIN_SET)
    {
    }

    /* The board sends the Set CCC message with its associated data and
       the Get CCC message and expects to transmit / receive associated data*/

    /*##- Start the multiple transfer process ##################################################*/
    if (HAL_I3C_Ctrl_MultipleTransfer_DMA(&hi3c1, &aContextBuffers[I3C_IDX_FRAME_1]) != HAL_OK)
    {
      /* Error_Handler() function is called when error occurs. */
      Error_Handler();
    }

    /*##- Wait for the end of the transfer #####################################################*/
    /*  Before starting a new communication transfer, you need to check the current
    state of the peripheral; if it is busy you need to wait for the end of current
    transfer before starting a new one.
    For simplicity reasons, this example is just waiting till the end of the
    transfer, but application may perform other tasks while transfer operation
    is ongoing. */
    while (HAL_I3C_GetState(&hi3c1) != HAL_I3C_STATE_READY)
    {
    }

    /*##- Monitor the different value retrieve from the sensor #################################*/
    /*  At the end, of transfer, the application have retrieve all the data of
    the frame in an unique buffer,
    which must be unfill to retrieve the associated value for each get CCC command. */
    /* Display through external Terminal IO the Get CCC associated value received */
    DisplayCCCValue(aCCCList, &aCommandCode[0], &aRxBuffer[0], COUNTOF(aCCCList));

    printf("\n\r-------------- SETMRL and SETMWL updated --------------\n\r");
    /*## Update SETMRL and SETMWL ###########################*/
    aSETMWL_data[1] = LoopCount;
    aSETMRL_data[1] = LoopCount;

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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

  FLASH_OBProgramInitTypeDef pOBInit = {0};

  /* USER CODE BEGIN FLASH_Init 1 */

  /* USER CODE END FLASH_Init 1 */
  HAL_FLASHEx_OBGetConfig(&pOBInit);
  if ((pOBInit.USERConfig2 & OB_I2C_NI3C_I2C) != OB_I2C_NI3C_I3C)
  {
    if (HAL_FLASH_Unlock() != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_FLASH_OB_Unlock() != HAL_OK)
    {
      Error_Handler();
    }
    pOBInit.OptionType = OPTIONBYTE_USER;
    pOBInit.USERType = OB_USER_I2C_NI3C;
    pOBInit.USERConfig2 = OB_I2C_NI3C_I3C;
    if (HAL_FLASHEx_OBProgram(&pOBInit) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_FLASH_OB_Lock() != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_FLASH_Lock() != HAL_OK)
    {
      Error_Handler();
    }
  }
  /* USER CODE BEGIN FLASH_Init 2 */

  /* USER CODE END FLASH_Init 2 */

}

/**
  * @brief GPDMA1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPDMA1_Init(void)
{

  /* USER CODE BEGIN GPDMA1_Init 0 */

  /* USER CODE END GPDMA1_Init 0 */

  /* Peripheral clock enable */
  __HAL_RCC_GPDMA1_CLK_ENABLE();

  /* GPDMA1 interrupt Init */
    HAL_NVIC_SetPriority(GPDMA1_Channel0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel0_IRQn);
    HAL_NVIC_SetPriority(GPDMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel1_IRQn);
    HAL_NVIC_SetPriority(GPDMA1_Channel2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel2_IRQn);

  /* USER CODE BEGIN GPDMA1_Init 1 */

  /* USER CODE END GPDMA1_Init 1 */
  /* USER CODE BEGIN GPDMA1_Init 2 */

  /* USER CODE END GPDMA1_Init 2 */

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

  I3C_FifoConfTypeDef sFifoConfig = {0};
  I3C_CtrlConfTypeDef sCtrlConfig = {0};

  /* USER CODE BEGIN I3C1_Init 1 */

  /* USER CODE END I3C1_Init 1 */
  hi3c1.Instance = I3C1;
  hi3c1.Mode = HAL_I3C_MODE_CONTROLLER;
  hi3c1.Init.CtrlBusCharacteristic.SDAHoldTime = HAL_I3C_SDA_HOLD_TIME_0_5;
  hi3c1.Init.CtrlBusCharacteristic.WaitTime = HAL_I3C_OWN_ACTIVITY_STATE_0;
  hi3c1.Init.CtrlBusCharacteristic.SCLPPLowDuration = 0x0e;
  hi3c1.Init.CtrlBusCharacteristic.SCLI3CHighDuration = 0x0e;
  hi3c1.Init.CtrlBusCharacteristic.SCLODLowDuration = 0x36;
  hi3c1.Init.CtrlBusCharacteristic.SCLI2CHighDuration = 0x00;
  hi3c1.Init.CtrlBusCharacteristic.BusFreeDuration = 0x1e;
  hi3c1.Init.CtrlBusCharacteristic.BusIdleDuration = 0x94;
  if (HAL_I3C_Init(&hi3c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure FIFO
  */
  sFifoConfig.RxFifoThreshold = HAL_I3C_RXFIFO_THRESHOLD_1_4;
  sFifoConfig.TxFifoThreshold = HAL_I3C_TXFIFO_THRESHOLD_1_4;
  sFifoConfig.ControlFifo = HAL_I3C_CONTROLFIFO_DISABLE;
  sFifoConfig.StatusFifo = HAL_I3C_STATUSFIFO_DISABLE;
  if (HAL_I3C_SetConfigFifo(&hi3c1, &sFifoConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure controller
  */
  sCtrlConfig.DynamicAddr = 0;
  sCtrlConfig.StallTime = 0x00;
  sCtrlConfig.HotJoinAllowed = DISABLE;
  sCtrlConfig.ACKStallState = DISABLE;
  sCtrlConfig.CCCStallState = DISABLE;
  sCtrlConfig.TxStallState = DISABLE;
  sCtrlConfig.RxStallState = DISABLE;
  sCtrlConfig.HighKeeperSDA = DISABLE;
  if (HAL_I3C_Ctrl_Config(&hi3c1, &sCtrlConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I3C1_Init 2 */

  /* USER CODE END I3C1_Init 2 */

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
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
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

/**
  * @brief  Controller multiple Direct CCC Command, I3C private or I2C transfer Complete callback.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                     for the specified I3C.
  * @retval None
  */
void HAL_I3C_CtrlMultipleXferCpltCallback(I3C_HandleTypeDef *hi3c)
{
  /* Toggle LD1: Transfer in transmission process is correct */
  BSP_LED_Toggle(LD1);
}

/**
  * @brief  Error callback.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                     for the specified I3C.
  * @retval None
  */
void HAL_I3C_ErrorCallback(I3C_HandleTypeDef *hi3c)
{
  /* Error_Handler() function is called when error occurs. */
  Error_Handler();
}

/**
  * @brief  Display the different value retrieve during GET CCC.
  * @param  pGetCCCList  Pointer to a CCC List descriptor.
  * @param  pCCCCharList Pointer to a buffer contain CCC name in char.
  * @param  pCCCBuffer   Pointer to a receive CCC buffer.
  * @param  nbCCC        Number of Get CCC to display.
  * @retval None.
  */
static void DisplayCCCValue(I3C_CCCTypeDef *pGetCCCList, char **pCCCCharList, uint8_t *pCCCBuffer, uint8_t nbCCC)
{
  uint8_t index_CCCCharList = 0;

  printf("\n\r---------------- GETCCC retrieve value ----------------\n\r");
  for (uint8_t j = 0; j < nbCCC; j++)
  {
    if (pGetCCCList[j].Direction == LL_I3C_DIRECTION_READ)
    {
      printf("%s: 0x", pCCCCharList[index_CCCCharList]);
      for (uint8_t i = 0; i < (pGetCCCList[j].CCCBuf.Size); i++)
      {
        /* Fill local buffer with different CCC value */
        printf("%x", *pCCCBuffer++);
      }
      printf("\n\r");

      index_CCCCharList++;
    }
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
  /* Error if LD1 is slowly blinking (1 sec. period) */
  while (1)
  {
    BSP_LED_Toggle(LD1);
    HAL_Delay(1000);
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
  /* Error_Handler() function is called when error occurs. */
  Error_Handler();
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
