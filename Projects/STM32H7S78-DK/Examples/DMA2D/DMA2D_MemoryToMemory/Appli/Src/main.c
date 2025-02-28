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
#include "stm32h7s78_discovery.h"

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

DMA2D_HandleTypeDef hdma2d;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint32_t aBufferResult[(LAYER_SIZE_X * LAYER_SIZE_Y * LAYER_NB_BYTES_PER_PIXEL) / 4]__attribute__((aligned(32),section("noncacheable_buffer")));

/* ARGB4444 input buffer 16bpp */
uint32_t aBufferInput[(LAYER_SIZE_X * LAYER_SIZE_Y * LAYER_NB_BYTES_PER_PIXEL) / 4]__attribute__((aligned(32)))=
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
static void MX_DMA2D_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
static void TransferError(DMA2D_HandleTypeDef* dma2dHandle);
static void TransferComplete(DMA2D_HandleTypeDef* dma2dHandle);
ErrorStatus Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint32_t BufferLength);
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
  MX_DMA2D_Init();
  /* USER CODE BEGIN 2 */
  /* Configure LD1 and LD3 */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD3);

  /*## DMA2D Callbacks Configuration ######################################*/
  hdma2d.XferCpltCallback  = TransferComplete;
  hdma2d.XferErrorCallback = TransferError;

  /*## Start DMA2D transfer ###############################################*/
  if(HAL_DMA2D_Start_IT(&hdma2d,
                        (uint32_t)&aBufferInput,  /* Source memory buffer      */
                        (uint32_t)&aBufferResult, /* Destination memory buffer */
                        LAYER_SIZE_X,             /* Width of the 2D memory transfer in pixels */
                        LAYER_SIZE_Y)             /* Height of the 2D memory transfer in lines */
     != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*## Wait for the end of the transfer ###################################*/
  /*  Before starting a new transfer, you need to check the current state of
  the peripheral; if it is busy you need to wait for the end of current
  transfer before starting a new one.
  For simplicity reasons, this example is just waiting till the end of the
  transfer, but application may perform other tasks while transfer operation
  is ongoing. */
  while (HAL_DMA2D_GetState(&hdma2d) != HAL_DMA2D_STATE_READY)
  {
  }

  /*## Check transfer correctness #########################################*/
  if(Buffercmp(aBufferInput,
               aBufferResult,
               ((LAYER_SIZE_X * LAYER_SIZE_Y * LAYER_NB_BYTES_PER_PIXEL) / 4)) == ERROR)
  {
    /* KO */
    /* Turn LD3 On */
    BSP_LED_On(LD3);
  }
  else
  {
    /* OK */
    /* Turn on LD1 */
    BSP_LED_On(LD1);
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
  * @brief DMA2D Initialization Function
  * @param None
  * @retval None
  */
static void MX_DMA2D_Init(void)
{

  /* USER CODE BEGIN DMA2D_Init 0 */

  /* USER CODE END DMA2D_Init 0 */

  /* USER CODE BEGIN DMA2D_Init 1 */

  /* USER CODE END DMA2D_Init 1 */
  hdma2d.Instance = DMA2D;
  hdma2d.Init.Mode = DMA2D_M2M;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB4444;
  hdma2d.Init.OutputOffset = 0;
  hdma2d.LayerCfg[1].InputOffset = 0;
  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB4444;
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0xFF;
  hdma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;
  hdma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR;
  hdma2d.LayerCfg[1].ChromaSubSampling = DMA2D_NO_CSS;
  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DMA2D_Init 2 */

  /* USER CODE END DMA2D_Init 2 */

}

/* USER CODE BEGIN 4 */
/**
* @brief  DMA2D Transfer completed callback
* @param  hdma2d: DMA2D handle.
* @note   This example shows a simple way to report end of DMA2D transfer, and
*         you can add your own implementation.
* @retval None
*/
static void TransferComplete(DMA2D_HandleTypeDef *hdma2d)
{
  /* Turn LD1 On */
  BSP_LED_On(LD1);
}

/**
* @brief  DMA2D error callbacks
* @param  hdma2d: DMA2D handle
* @note   This example shows a simple way to report DMA2D transfer error, and you can
*         add your own implementation.
* @retval None
*/
static void TransferError(DMA2D_HandleTypeDef *hdma2d)
{
  /* Turn LD3 On */
  BSP_LED_On(LD3);
}

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

  /* Initialize the region corresponding to external RAM */
#if defined ( __ICCARM__ )
  extern uint32_t __region_EXTRAM_start__;
  extern uint32_t __region_EXTRAM_end__;

  address = (uint32_t)&__region_EXTRAM_start__;
  size = (uint32_t)&__region_EXTRAM_end__ - (uint32_t)&__region_EXTRAM_start__ + 1;

#elif defined (__CC_ARM) || defined(__ARMCC_VERSION)
  extern uint32_t Image$$RW_EXTRAM$$Base;
  extern uint32_t Image$$RW_EXTRAM$$ZI$$Length;
  extern uint32_t Image$$RW_EXTRAM$$Length;

  address = (uint32_t)&Image$$RW_EXTRAM$$Base;
  size  = (uint32_t)&Image$$RW_EXTRAM$$Length + (uint32_t)&Image$$RW_EXTRAM$$ZI$$Length;
#elif defined ( __GNUC__ )
  extern uint32_t __EXTRAM_BEGIN;
  extern uint32_t __EXTRAM_SIZE;
  address = (uint32_t)&__EXTRAM_BEGIN;
  size  = (uint32_t)&__EXTRAM_SIZE;
#else
#error "Compiler toolchain is unsupported"
#endif

  if (size != 0)
  {
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = index;
    MPU_InitStruct.SubRegionDisable = 0u;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
    MPU_AdjustRegionAddressSize(address, size, &MPU_InitStruct);
    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    index++;
  }

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
  __disable_irq();
  while (1)
  {
     BSP_LED_On(LD2);
  /* Blocking on error */
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
