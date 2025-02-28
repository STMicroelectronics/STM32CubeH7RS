/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    I3C/I3C_Target_Direct_Command_DMA/Appli/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32H7RSxx I3C HAL API as Target
  *          to manage a Direct Command procedure from a Controller
  *          with a communication process based on DMA transfer.
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
I3C_CCCInfoTypeDef CCCInfo;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

I3C_HandleTypeDef hi3c1;

/* USER CODE BEGIN PV */
/* Context buffer related to Frame context, contain different buffer value for a communication */
I3C_XferTypeDef aContextBuffers[1] __attribute__((section("noncacheable_buffer")));

/* Buffer used for reception during DEFTGTS or DEFGRPA procedure */
uint8_t aRxBuffer[0x1F] __attribute__((section("noncacheable_buffer")));

__IO uint8_t ubSetMRLReceived = 0;
__IO uint8_t ubSetMWLReceived = 0;

uint32_t uwSetMRLExpected = 0x04;
uint32_t uwSetMWLExpected = 0x04;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
static void MX_FLASH_Init(void);
static void MX_I3C1_Init(void);
/* USER CODE BEGIN PFP */
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
  MPU_Config();

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
  MX_GPIO_Init();
  MX_FLASH_Init();
  MX_I3C1_Init();
  /* USER CODE BEGIN 2 */
  /* Configure LD1 */
  BSP_LED_Init(LD1);

  /*##- Start the listen mode process #####################################*/
  /* Activate notifications for specially for this example
     - Dynamic address association
     - Get Common Command Code requested by the controller
     - Set Common Command Code requested by the controller
     And all other possible notification if any. */
  aContextBuffers->RxBuf.pBuffer = aRxBuffer;
  aContextBuffers->RxBuf.Size = sizeof(aRxBuffer);
  if (HAL_I3C_ActivateNotification(&hi3c1, &aContextBuffers[0], HAL_I3C_ALL_TGT_ITS) != HAL_OK)
  {
      /* Error_Handler() function is called when error occurs. */
      Error_Handler();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /*##- Catch SETMRL event #####################################*/
    /* To help example understanding, the SETMRL event is caught
       Mean If a SETMRL event is received, verify the updated data
       If the update is not inline with the expected data, Error_Handler is called
       User can modify this part of code to monitor other part of the event */
    if (ubSetMRLReceived == 1)
    {
      /* Retrieve updated data */
      HAL_I3C_GetCCCInfo(&hi3c1, EVENT_ID_SETMRL, &CCCInfo);

      /* Compare data updated with expected data */
      if (CCCInfo.MaxReadLength != uwSetMRLExpected)
      {
        /* Error_Handler() function is called when error occurs. */
        Error_Handler();
      }

      /* Reset Global variable */
      ubSetMRLReceived = 0;
    }

    /*##- Catch SETMWL event #####################################*/
    /* To help example understanding, the SETMWL event is caught
       Mean If a SETMWL event is received, verify the updated data
       If the update is not inline with the expected data, Error_Handler is called
       User can modify this part of code to monitor other part of the event */
    if (ubSetMWLReceived == 1)
    {
      /* Retrieve updated data */
      HAL_I3C_GetCCCInfo(&hi3c1, EVENT_ID_SETMWL, &CCCInfo);

      /* Compare data updated with expected data */
      if (CCCInfo.MaxWriteLength != uwSetMWLExpected)
      {
        /* Error_Handler() function is called when error occurs. */
        Error_Handler();
      }

      /* Reset Global variable */
      ubSetMWLReceived = 0;
    }
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
  * @brief I3C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I3C1_Init(void)
{

  /* USER CODE BEGIN I3C1_Init 0 */

  /* USER CODE END I3C1_Init 0 */

  I3C_FifoConfTypeDef sFifoConfig = {0};
  I3C_TgtConfTypeDef sTgtConfig = {0};

  /* USER CODE BEGIN I3C1_Init 1 */

  /* USER CODE END I3C1_Init 1 */
  hi3c1.Instance = I3C1;
  hi3c1.Mode = HAL_I3C_MODE_TARGET;
  hi3c1.Init.TgtBusCharacteristic.BusAvailableDuration = 0x94;
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

  /** Configure Target
  */
  sTgtConfig.Identifier = 0xC6;
  sTgtConfig.MIPIIdentifier = DEVICE_ID;
  sTgtConfig.CtrlRoleRequest = DISABLE;
  sTgtConfig.HotJoinRequest = DISABLE;
  sTgtConfig.IBIRequest = DISABLE;
  sTgtConfig.IBIPayload = DISABLE;
  sTgtConfig.IBIPayloadSize = HAL_I3C_PAYLOAD_EMPTY;
  sTgtConfig.MaxReadDataSize = 0xFF;
  sTgtConfig.MaxWriteDataSize = 0xFF;
  sTgtConfig.CtrlCapability = DISABLE;
  sTgtConfig.GroupAddrCapability = DISABLE;
  sTgtConfig.DataTurnAroundDuration = HAL_I3C_TURNAROUND_TIME_TSCO_LESS_12NS;
  sTgtConfig.MaxReadTurnAround = 0;
  sTgtConfig.MaxDataSpeed = HAL_I3C_GETMXDS_FORMAT_1;
  sTgtConfig.MaxSpeedLimitation = DISABLE;
  sTgtConfig.HandOffActivityState = HAL_I3C_HANDOFF_ACTIVITY_STATE_0;
  sTgtConfig.HandOffDelay = DISABLE;
  sTgtConfig.PendingReadMDB = DISABLE;
  if (HAL_I3C_Tgt_Config(&hi3c1, &sTgtConfig) != HAL_OK)
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
  * @brief I3C notify callback after receiving a notification.
  *        The main objective of this user function is to check on the notification ID and assign 1 to the global
  *        variable used to indicate that the event is well finished.
  * @par Called functions
  * - HAL_I3C_NotifyCallback()
  * @retval None
  */
void HAL_I3C_NotifyCallback(I3C_HandleTypeDef *hi3c, uint32_t eventId)
{
  if ((eventId & EVENT_ID_SETMRL) == EVENT_ID_SETMRL)
  {
    ubSetMRLReceived = 1;
  }

  if ((eventId & EVENT_ID_SETMWL) == EVENT_ID_SETMWL)
  {
    ubSetMWLReceived = 1;
  }

  /* Toggle LD1: Each time a Notification is detected */
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
