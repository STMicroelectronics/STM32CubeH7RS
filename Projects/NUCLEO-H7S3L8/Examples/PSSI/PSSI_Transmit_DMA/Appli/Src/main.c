/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    PSSI/PSSI_Transmit_DMA/Appli/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32H7RSxx PSSI HAL API to
             manage a Transmission procedure in DMA mode.
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

PSSI_HandleTypeDef hpssi;
DMA_HandleTypeDef handle_GPDMA1_Channel3;

/* USER CODE BEGIN PV */
/* Buffer data to transmit to the Slave */
uint16_t pData16[]={0x0201,0x0804,0x2010,0x8040,0x0201,0x0804,0x2010,0x8040,
                    0x0201,0x0804,0x2010,0x8040,0x0201,0x0804,0x2010,0x8040};

uint16_t pData16_16lines[]={0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000,
                            0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000,
                            0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000,
                            0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000};

__IO  uint32_t PSSI_TransmitComplete_count;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
static void MX_GPDMA1_Init(void);
static void MX_PSSI_Init(void);
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
  MX_GPDMA1_Init();
  MX_PSSI_Init();
  /* USER CODE BEGIN 2 */
  /* Configure LEDs */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* Modify bus width to 8 lines */
    hpssi.Init.BusWidth = HAL_PSSI_8LINES;

    if (HAL_PSSI_Init(&hpssi) != HAL_OK)
    {
      Error_Handler();
    }

    PSSI_TransmitComplete_count = 0;
    /* Send a buffer to Slave until end of transmission */
    if (HAL_PSSI_Transmit_DMA(&hpssi, (uint32_t*)pData16, sizeof(pData16)) != HAL_OK)
    {
      Error_Handler();
    }

    while (PSSI_TransmitComplete_count != 1)
    {
      /* Wait until end of transmission */
    }

    HAL_Delay(100);

    /* Modify bus width to 16 lines */
    hpssi.Init.BusWidth = HAL_PSSI_16LINES;

    if (HAL_PSSI_Init(&hpssi) != HAL_OK)
    {
      Error_Handler();
    }

    /* Send a buffer to Slave until end of transmission */
    if (HAL_PSSI_Transmit_DMA(&hpssi, (uint32_t*)pData16_16lines, sizeof(pData16_16lines)) != HAL_OK)
    {
      Error_Handler();
    }

    while (PSSI_TransmitComplete_count != 2)
    {
      /* Wait until end of transmission */
    }

    HAL_Delay(100);
    BSP_LED_Toggle(LD1);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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
    HAL_NVIC_SetPriority(GPDMA1_Channel3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel3_IRQn);

  /* USER CODE BEGIN GPDMA1_Init 1 */

  /* USER CODE END GPDMA1_Init 1 */
  /* USER CODE BEGIN GPDMA1_Init 2 */

  /* USER CODE END GPDMA1_Init 2 */

}

/**
  * @brief PSSI Initialization Function
  * @param None
  * @retval None
  */
static void MX_PSSI_Init(void)
{

  /* USER CODE BEGIN PSSI_Init 0 */

  /* USER CODE END PSSI_Init 0 */

  /* USER CODE BEGIN PSSI_Init 1 */

  /* USER CODE END PSSI_Init 1 */
  hpssi.Instance = PSSI;
  hpssi.Init.DataWidth = HAL_PSSI_16BITS;
  hpssi.Init.BusWidth = HAL_PSSI_16LINES;
  hpssi.Init.ControlSignal = HAL_PSSI_DE_RDY_DISABLE;
  hpssi.Init.ClockPolarity = HAL_PSSI_FALLING_EDGE;
  hpssi.Init.DataEnablePolarity = HAL_PSSI_DEPOL_ACTIVE_LOW;
  hpssi.Init.ReadyPolarity = HAL_PSSI_RDYPOL_ACTIVE_LOW;
  if (HAL_PSSI_Init(&hpssi) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_PSSI_ClockConfig(&hpssi, HAL_PSSI_CLOCK_EXT) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN PSSI_Init 2 */

  /* USER CODE END PSSI_Init 2 */

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
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();

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
  * @brief PSSI transmit complete callback
  * @par hppsi: PSSI HAL handle
  * @retval None
  */
 void HAL_PSSI_TxCpltCallback(PSSI_HandleTypeDef *hpssi)
{
  PSSI_TransmitComplete_count++;
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
    HAL_Delay(1000);
    BSP_LED_Toggle(LD2);
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
