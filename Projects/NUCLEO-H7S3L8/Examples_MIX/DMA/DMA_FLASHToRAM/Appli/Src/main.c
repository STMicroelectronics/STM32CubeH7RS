/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples_MIX/DMA/DMA_FLASHToRAM/Src/main.c
  * @author  MCD Application Team
  * @brief   This example provides a description of how to use a DMA channel
  *          to transfer a word data buffer from FLASH memory to embedded
  *          SRAM memory through the STM32H7RSxx DMA HAL and LL API.
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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

DMA_HandleTypeDef handle_HPDMA1_Channel12;

/* USER CODE BEGIN PV */

/* DMA Instance and Channel declaration */
DMA_TypeDef*  DmaInstance;
uint32_t      DmaChannel;

/* Source buffer aligned on 64bits */
static const uint32_t aSRC_Const_Buffer[BUFFER_SIZE] __attribute__((aligned(8))) =
{
  0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10,
  0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20,
  0x21222324, 0x25262728, 0x292A2B2C, 0x2D2E2F30,
  0x31323334, 0x35363738, 0x393A3B3C, 0x3D3E3F40,
  0x41424344, 0x45464748, 0x494A4B4C, 0x4D4E4F50,
  0x51525354, 0x55565758, 0x595A5B5C, 0x5D5E5F60,
  0x61626364, 0x65666768, 0x696A6B6C, 0x6D6E6F70,
  0x71727374, 0x75767778, 0x797A7B7C, 0x7D7E7F80
};

/* Destination buffer located inside a non cacheable area to avoid cache operation management */
static uint32_t aDST_Buffer[BUFFER_SIZE] __attribute__((aligned (8),section("noncacheable_buffer")));

static __IO uint32_t configErrorDetected;      /* Set to 1 if a configuration error is detected */
static __IO uint32_t transferErrorDetected;    /* Set to 1 if an error transfer is detected */
static __IO uint32_t transferCompleteDetected; /* Set to 1 if transfer is correctly completed */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_HPDMA1_Init(void);
/* USER CODE BEGIN PFP */
static uint32_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
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

  /* Initialize LEDs */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD3);

  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */

  /* Set to 1 if an transfer error is detected */
  configErrorDetected = 0;
  transferErrorDetected = 0;
  transferCompleteDetected = 0;

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_HPDMA1_Init();
  /* USER CODE BEGIN 2 */

  /* Clean destination buffer */
  memset((void *)aDST_Buffer, 0U, sizeof(aDST_Buffer) );

  /* Use Data Memory Barrier (DMB) to ensure all data memory transfers */
  /* are completed before the DMA operation                            */
  __DMB();

  /* Configure and start the DMA transfer using the interrupt mode */
  /* Enable All the DMA interrupts */

  /* Using HAL interface, use :                                   */
  /* - HAL_DMA_Start_IT() to Configure and start the DMA transfer */
  /*                      using the interrupt mode.               */
  /* Using LL interface, use :                                    */
  /* - LL_DMA_GET_INSTANCE() to convert LL_DMA_CHANNEL_12 into DMA1           */
  /* - LL_DMA_GET_CHANNEL() to convert LL_DMA_CHANNEL_12 into LL_DMA_CHANNEL_ */
  /* - LL_DMA_ConfigAddresses() to configure addresses source, destination   */
  /* - LL_DMA_SetBlkDataLength() to configure data length to transfer        */
  /* - LL_DMA_EnableIT_DTC() to enable Transfer Complete Interrupt           */
  /* - LL_DMA_EnableIT_DTE() to enable Transfer Error Interrupt              */
  /* - LL_DMA_EnableChannel() to enable DMA Transfer                         */

  /* ########## Starting from this point HAL API must not be used ########## */
  DmaInstance        = LL_DMA_GET_INSTANCE(handle_HPDMA1_Channel12.Instance);
  DmaChannel         = LL_DMA_GET_CHANNEL(handle_HPDMA1_Channel12.Instance);
  LL_DMA_ConfigAddresses(DmaInstance, DmaChannel,
                               (uint32_t)&aSRC_Const_Buffer,
                               (uint32_t)&aDST_Buffer);

  LL_DMA_SetBlkDataLength(DmaInstance, DmaChannel, (BUFFER_SIZE * 4U));
  LL_DMA_EnableIT_TC(DmaInstance, DmaChannel);
  LL_DMA_EnableIT_DTE(DmaInstance, DmaChannel);
  LL_DMA_EnableIT_USE(DmaInstance, DmaChannel);
  LL_DMA_EnableChannel(DmaInstance, DmaChannel);

  /* Wait for end of transmission or an error occurred */
  while ((transferCompleteDetected == 0) && (transferErrorDetected == 0U) && (configErrorDetected == 0U));

  /* Check DMA error */
  if ((transferErrorDetected == 1U) || (configErrorDetected == 1U))
  {
    Error_Handler();
  }

  /* Check destination aDST_Buffer */
  if (Buffercmp((uint8_t *)aSRC_Const_Buffer, (uint8_t *)aDST_Buffer, (BUFFER_SIZE * 4U)) != 0U)
  {
    /* Transfer Error */
    Error_Handler();
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* Turn LD1 on*/
    BSP_LED_On(LD1);
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
  handle_HPDMA1_Channel12.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_DOUBLEWORD;
  handle_HPDMA1_Channel12.Init.DestDataWidth = DMA_DEST_DATAWIDTH_DOUBLEWORD;
  handle_HPDMA1_Channel12.Init.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
  handle_HPDMA1_Channel12.Init.SrcBurstLength = 4;
  handle_HPDMA1_Channel12.Init.DestBurstLength = 4;
  handle_HPDMA1_Channel12.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT0;
  handle_HPDMA1_Channel12.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
  handle_HPDMA1_Channel12.Init.Mode = DMA_NORMAL;
  if (HAL_DMA_Init(&handle_HPDMA1_Channel12) != HAL_OK)
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
  * @brief  DMA conversion complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
void TransferComplete(void)
{
  transferCompleteDetected = 1;
}

/**
  * @brief  DMA conversion error callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
void TransferError(void)
{
  transferErrorDetected = 1;
}

/**
  * @brief  DMA configuration error callback
  * @note   This function is executed when a configuration error interrupt
  *         is generated on the DMA enable
  * @retval None
  */
void ConfigError(void)
{
  configErrorDetected = 1;
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0   : pBuffer1 identical to pBuffer2
  *         > 0 : pBuffer1 differs from pBuffer2
  */
static uint32_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
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
  /* Turn LD3 on: Transfer Error */
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
