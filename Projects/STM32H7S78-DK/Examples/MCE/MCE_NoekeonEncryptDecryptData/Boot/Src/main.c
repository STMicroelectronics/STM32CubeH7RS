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
#include "string.h"

#include <stdio.h>

MCE_AESConfigTypeDef ContextAESConfig;
MCE_RegionConfigTypeDef  RegionConfig ;
MCE_RegionConfigTypeDef  RegionConfig_npriv;
MCE_NoekeonConfigTypeDef NoekeonConfig ;

MCE_RegionConfigTypeDef  config;

XSPI_HandleTypeDef XSPIHandle[2] = {0};


uint32_t  MCE_INSTANCE_IDX, MCE_REGION_IDX, MCE_CONTEXT_IDX, MCE_ALGO_IDX, MCE_CONTEXTMODE_IDX;



uint32_t  Attributes_read = 0xFFFFFFFF;

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

#define BUFFER_SIZE 256
#define MCE_NB_INSTANCES              3


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

MCE_HandleTypeDef hmce1;
MCE_HandleTypeDef hmce2;

XSPI_HandleTypeDef hxspi1;
XSPI_HandleTypeDef hxspi2;

/* USER CODE BEGIN PV */
/* Buffer used for transmission */
uint8_t aTxBuffer[BUFFERSIZE];

uint8_t CmdCplt, TxCplt , StatusMatch , RxCplt;
XSPI_MemoryMappedTypeDef sMemMappedCfg;

MCE_AESConfigTypeDef ContextAESConfig;
MCE_RegionConfigTypeDef  RegionConfig ;

uint32_t RX_Buffer[256] __ALIGN_END ;

uint32_t Nonce[2][2]   =    { { 0xA5A5A5A5, 0xC3C3C3C3 },
                              { 0x11111111, 0x55555555 }
};

uint32_t Key[4][4]     = { { 0x71234567, 0x89ABCDEF, 0x71234567, 0x89ABCDEF },
                           { 0xEDCBA987, 0x6543210F, 0xEDCBA987, 0x6543210F },
                           { 0x23456789, 0xABCDEF01, 0x23456789, 0xABCDEF01 },
                           { 0xCBA98765, 0x43210FED, 0xCBA98765, 0x43210FED }
};





uint32_t Plain1[BUFFER_SIZE] = { 0x0F, 0x11, 0x25, 0x3F, 0x4D, 0x60, 0x60, 0x77, 0x84, 0x97, 0xCC, 0xAE, 0x65, 0xEF, 0xEE, 0xF0,
                                 0xF1, 0xD1, 0xA1, 0x10, 0x41, 0x56, 0x51, 0x71, 0xE0, 0x11, 0x21, 0xA1, 0x56, 0xD1, 0x77, 0xF1,
                                 0xF2, 0xD2, 0xA2, 0x20, 0x42, 0x56, 0x52, 0x72, 0xE0, 0x12, 0x22, 0xA2, 0x56, 0xD2, 0x78, 0xF2,
                                 0xF3, 0xD3, 0xA3, 0x30, 0x43, 0x56, 0x53, 0x73, 0xE0, 0x13, 0x23, 0xA3, 0x56, 0xD3, 0x7A, 0xF3,
                                 0xF4, 0xD4, 0xA4, 0x40, 0x44, 0x56, 0x54, 0x74, 0xE0, 0x14, 0x24, 0xA4, 0x56, 0xD4, 0x7B, 0xF4,
                                 0xF5, 0xC5, 0xA5, 0x50, 0x45, 0x56, 0x55, 0x75, 0xE0, 0x15, 0x25, 0xA5, 0x56, 0xD5, 0x7C, 0xF5,
                                 0xF6, 0xC6, 0xA6, 0x60, 0x46, 0x56, 0x56, 0x76, 0xE0, 0x16, 0x26, 0xA6, 0x56, 0xD6, 0x7D, 0xF6,
                                 0xE7, 0xC7, 0xA7, 0x70, 0x47, 0x56, 0x57, 0x77, 0xE0, 0x17, 0x27, 0xA7, 0x56, 0xD7, 0x7E, 0xF7,
                                 0xE8, 0xC8, 0x28, 0x80, 0x48, 0x56, 0x58, 0x78, 0xE0, 0x18, 0x28, 0xA8, 0xC8, 0xD8, 0xE8, 0xF8,
                                 0xEA, 0xCA, 0x2A, 0xA0, 0x4A, 0x56, 0x5A, 0x7A, 0xFA, 0x1A, 0x2A, 0xAA, 0xCA, 0xDA, 0xEA, 0xFA,
                                 0xEB, 0xCB, 0xEB, 0xB0, 0x4B, 0x56, 0x5B, 0x7B, 0xFB, 0x1B, 0x2B, 0xAB, 0x56, 0xDB, 0xEB, 0x77,
                                 0xC0, 0x1C, 0xEC, 0x4C, 0x4C, 0x56, 0x5C, 0x7C, 0xFC, 0x1C, 0xAC, 0xAC, 0x56, 0xDC, 0xEC, 0x78,
                                 0xD0, 0x9D, 0xED, 0x3D, 0x4D, 0x56, 0x5D, 0x7D, 0xFD, 0x1D, 0xAD, 0xAD, 0x56, 0xDD, 0xED, 0x7A,
                                 0xE0, 0x9E, 0xEE, 0x5E, 0x4E, 0x56, 0x5E, 0x7E, 0xFE, 0x1E, 0xEE, 0xBE, 0x56, 0xDE, 0xEE, 0x7B,
                                 0xF0, 0x9F, 0xEF, 0x1F, 0x4F, 0x56, 0x6F, 0x7F, 0xFF, 0x1F, 0xAF, 0xBF, 0x56, 0xDF, 0xEF, 0x7C,
                                 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0xF0, 0x10, 0xA0, 0xB0, 0x56, 0xD0, 0xE0, 0xF0,
};       
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_MCE1_Init(void);
static void MX_MCE2_Init(void);
static void MX_XSPI1_Init(void);
static void MX_XSPI2_Init(void);
/* USER CODE BEGIN PFP */
uint32_t XSPI_Memory_WriteReg(XSPI_HandleTypeDef *Ctx, uint32_t Address, uint8_t *Value);
static void Configure_APMemory(void);
void DMA_Read_WORD(uint32_t * SrcBuffer, uint32_t * DstBuffer, uint32_t Size);

void configureMemoryMappedMode(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
XSPI_HandleTypeDef XSPIHandle[2];

XSPI_TypeDef * HSPI_INSTANCE[2] = {XSPI1, XSPI2};
static void XSPI_SwitchOctalToHexa(uint32_t InstanceNumber);


uint32_t XSPI_HyperRAM_Init(uint32_t InstanceNumber);
uint32_t XSPI_HyperRAM_EnableMemoryMapped(uint32_t InstanceNumber);

XSPI_RegularCmdTypeDef sCommand = {0};

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  __IO uint32_t *mem_addr;
  uint16_t index;
  uint32_t Identic;

  /* USER CODE END 1 */

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_MCE1_Init();
  MX_MCE2_Init();
  MX_XSPI1_Init();
  MX_XSPI2_Init();
  /* USER CODE BEGIN 2 */

  /* Initialization of the LED_GREEN and LED_ORANGE ------------------------- */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);

  BSP_LED_Off(LED_GREEN);
  BSP_LED_Off(LED_RED);

  configureMemoryMappedMode();

  MCE_INSTANCE_IDX=1;
  if (XSPI_HyperRAM_Init(1) != 0)
    {
      printf( "\n Error XSPI HyperRAM Init \n");
    }

    /* Enable memory mapped mode*/
  if (XSPI_HyperRAM_EnableMemoryMapped(1) != 0)
    {
      printf( "\n Error XSPI HyperRAM Enable Memory Mapped \n");
    }
    /* Init MCE */
    memset(&hmce2, 0, sizeof(hmce2) );
    hmce2.Instance = MCE2;
    if (HAL_MCE_Init(&hmce2) != HAL_OK)
    {
      Error_Handler();
    }

    NoekeonConfig.KeyType = MCE_USE_MASTERKEYS;
    NoekeonConfig.pKey = Key[2];

    if (HAL_MCE_ConfigNoekeon(&hmce2, &NoekeonConfig) != HAL_OK)
    {
      Error_Handler();
    }

    /* Set the MCE Region configuration*/
    RegionConfig.Mode             = MCE_BLOCK_CIPHER;
    RegionConfig.ContextID        = MCE_NO_CONTEXT;
    RegionConfig.StartAddress     = 0x70000000;
    RegionConfig.EndAddress       = 0x70000FFF;
    RegionConfig.PrivilegedAccess = MCE_REGION_NPRIV;
    RegionConfig.AccessMode       = MCE_REGION_READWRITE;

    if (HAL_MCE_ConfigRegion(&hmce2, 0, &RegionConfig) != HAL_OK)
    {
      Error_Handler();
    }

    mem_addr= (uint32_t *)0x70000000 ;
    for (index = 0; index <BUFFER_SIZE; index++)
    {
      *mem_addr = Plain1[index];
      mem_addr++;
    }

    SCB_CleanDCache_by_Addr((void*)0x70000000, BUFFER_SIZE*4);
    /* disable the region */
    if (HAL_MCE_DisableRegion(&hmce2, 0) != HAL_OK)
    {
      Error_Handler();
    }

    /* Reset Rx Buffer */
    memset(RX_Buffer, 0, sizeof(BUFFER_SIZE));

    SCB_InvalidateDCache_by_Addr((void*)0x70000000, BUFFER_SIZE*4);

    /* Read DATA when Cipher Context disabled */
    mem_addr= (uint32_t *)0x70000000;
    for (index = 0; index < BUFFER_SIZE; index++)
    {
      RX_Buffer[index] = *mem_addr;
      mem_addr++;
    }

    /* Check that DATA is encrypted */
    Identic = 0;
    for (index = 0; index < BUFFER_SIZE; index++)
    {
      if (RX_Buffer[index] == Plain1[index])
      {
        Identic++;
      }
    }

    if (Identic != (0U))
    {
      /* DeInit the external HyperRAM memory */
      Error_Handler();
    }

    /* Re-Enable region config */
    if (HAL_MCE_EnableRegion(&hmce2, 0) != HAL_OK)
    {
      Error_Handler();
    }
    SCB_InvalidateDCache_by_Addr((void*)0x70000000, BUFFER_SIZE*4);

    /* Read DATA when Cipher Context Enabled */
    mem_addr= (uint32_t *)0x70000000;
    for (index = 0; index < BUFFER_SIZE; index++)
    {
      RX_Buffer[index] = *mem_addr;
      mem_addr++;
    }

    /* Check that DATA is plain */
    for (index = 0; index < BUFFER_SIZE; index++)
    {
      if (RX_Buffer[index] != Plain1[index])
      {
         Identic++;
      }
    }

    if (Identic != (0U))
    {
      /* DeInit the external HyperRAM memory */
      Error_Handler();
    }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      BSP_LED_Toggle(LED_GREEN);
      HAL_Delay(200);

    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL1.PLLM = 32;
  RCC_OscInitStruct.PLL1.PLLN = 300;
  RCC_OscInitStruct.PLL1.PLLP = 1;
  RCC_OscInitStruct.PLL1.PLLQ = 2;
  RCC_OscInitStruct.PLL1.PLLR = 2;
  RCC_OscInitStruct.PLL1.PLLS = 2;
  RCC_OscInitStruct.PLL1.PLLT = 2;
  RCC_OscInitStruct.PLL1.PLLFractional = 0;
  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL2.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL2.PLLM = 4;
  RCC_OscInitStruct.PLL2.PLLN = 25;
  RCC_OscInitStruct.PLL2.PLLP = 2;
  RCC_OscInitStruct.PLL2.PLLQ = 2;
  RCC_OscInitStruct.PLL2.PLLR = 2;
  RCC_OscInitStruct.PLL2.PLLS = 2;
  RCC_OscInitStruct.PLL2.PLLT = 2;
  RCC_OscInitStruct.PLL2.PLLFractional = 0;
  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK4|RCC_CLOCKTYPE_PCLK5;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
  RCC_ClkInitStruct.APB5CLKDivider = RCC_APB5_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief MCE1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_MCE1_Init(void)
{

  /* USER CODE BEGIN MCE1_Init 0 */

  /* USER CODE END MCE1_Init 0 */

  /* USER CODE BEGIN MCE1_Init 1 */

  /* USER CODE END MCE1_Init 1 */
  hmce1.Instance = MCE1;
  if (HAL_MCE_Init(&hmce1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MCE1_Init 2 */

  /* USER CODE END MCE1_Init 2 */

}

/**
  * @brief MCE2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_MCE2_Init(void)
{

  /* USER CODE BEGIN MCE2_Init 0 */

  /* USER CODE END MCE2_Init 0 */

  /* USER CODE BEGIN MCE2_Init 1 */

  /* USER CODE END MCE2_Init 1 */
  hmce2.Instance = MCE2;
  if (HAL_MCE_Init(&hmce2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MCE2_Init 2 */

  /* USER CODE END MCE2_Init 2 */

}

/**
  * @brief XSPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_XSPI1_Init(void)
{

  /* USER CODE BEGIN XSPI1_Init 0 */

  /* USER CODE END XSPI1_Init 0 */

  XSPIM_CfgTypeDef sXspiManagerCfg = {0};

  /* USER CODE BEGIN XSPI1_Init 1 */

  /* USER CODE END XSPI1_Init 1 */
  /* XSPI1 parameter configuration*/
  hxspi1.Instance = XSPI1;
  hxspi1.Init.FifoThresholdByte = 4;
  hxspi1.Init.MemoryMode = HAL_XSPI_SINGLE_MEM;
  hxspi1.Init.MemoryType = HAL_XSPI_MEMTYPE_APMEM_16BITS;
  hxspi1.Init.MemorySize = HAL_XSPI_SIZE_64MB;
  hxspi1.Init.ChipSelectHighTimeCycle = 5;
  hxspi1.Init.FreeRunningClock = HAL_XSPI_FREERUNCLK_DISABLE;
  hxspi1.Init.ClockMode = HAL_XSPI_CLOCK_MODE_0;
  hxspi1.Init.WrapSize = HAL_XSPI_WRAP_NOT_SUPPORTED;
  hxspi1.Init.ClockPrescaler = 1;
  hxspi1.Init.SampleShifting = HAL_XSPI_SAMPLE_SHIFT_NONE;
  hxspi1.Init.DelayHoldQuarterCycle = HAL_XSPI_DHQC_DISABLE;
  hxspi1.Init.ChipSelectBoundary = HAL_XSPI_BONDARYOF_NONE;
  hxspi1.Init.MaxTran = 0;
  hxspi1.Init.Refresh = 0;
  hxspi1.Init.MemorySelect = HAL_XSPI_CSSEL_NCS1;
  if (HAL_XSPI_Init(&hxspi1) != HAL_OK)
  {
    Error_Handler();
  }
  sXspiManagerCfg.nCSOverride = HAL_XSPI_CSSEL_OVR_NCS1;
  sXspiManagerCfg.IOPort = HAL_XSPIM_IOPORT_1;
  if (HAL_XSPIM_Config(&hxspi1, &sXspiManagerCfg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN XSPI1_Init 2 */

  /* USER CODE END XSPI1_Init 2 */

}

/**
  * @brief XSPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_XSPI2_Init(void)
{

  /* USER CODE BEGIN XSPI2_Init 0 */

  /* USER CODE END XSPI2_Init 0 */

  XSPIM_CfgTypeDef sXspiManagerCfg = {0};

  /* USER CODE BEGIN XSPI2_Init 1 */

  /* USER CODE END XSPI2_Init 1 */
  /* XSPI2 parameter configuration*/
  hxspi2.Instance = XSPI2;
  hxspi2.Init.FifoThresholdByte = 4;
  hxspi2.Init.MemoryMode = HAL_XSPI_SINGLE_MEM;
  hxspi2.Init.MemoryType = HAL_XSPI_MEMTYPE_APMEM_16BITS;
  hxspi2.Init.MemorySize = HAL_XSPI_SIZE_64MB;
  hxspi2.Init.ChipSelectHighTimeCycle = 1;
  hxspi2.Init.FreeRunningClock = HAL_XSPI_FREERUNCLK_DISABLE;
  hxspi2.Init.ClockMode = HAL_XSPI_CLOCK_MODE_0;
  hxspi2.Init.WrapSize = HAL_XSPI_WRAP_NOT_SUPPORTED;
  hxspi2.Init.ClockPrescaler = 1;
  hxspi2.Init.SampleShifting = HAL_XSPI_SAMPLE_SHIFT_NONE;
  hxspi2.Init.DelayHoldQuarterCycle = HAL_XSPI_DHQC_DISABLE;
  hxspi2.Init.ChipSelectBoundary = HAL_XSPI_BONDARYOF_NONE;
  hxspi2.Init.MaxTran = 0;
  hxspi2.Init.Refresh = 0;
  hxspi2.Init.MemorySelect = HAL_XSPI_CSSEL_NCS1;
  if (HAL_XSPI_Init(&hxspi2) != HAL_OK)
  {
    Error_Handler();
  }
  sXspiManagerCfg.nCSOverride = HAL_XSPI_CSSEL_OVR_DISABLED;
  sXspiManagerCfg.IOPort = HAL_XSPIM_IOPORT_1;
  if (HAL_XSPIM_Config(&hxspi2, &sXspiManagerCfg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN XSPI2_Init 2 */

  /* USER CODE END XSPI2_Init 2 */

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
  __HAL_RCC_GPIOO_CLK_ENABLE();
  __HAL_RCC_GPIOP_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
* @brief  Write mode register
* @param  Ctx Component object pointer
* @param  Address Register address
* @param  Value Register value pointer
* @retval error status
*/
uint32_t XSPI_Memory_WriteReg(XSPI_HandleTypeDef *Ctx, uint32_t Address, uint8_t *Value)
{
  XSPI_RegularCmdTypeDef sCommand1={0};

  /* Initialize the write register command */
  sCommand1.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand1.InstructionMode    = HAL_XSPI_INSTRUCTION_8_LINES;
  sCommand1.InstructionWidth    = HAL_XSPI_INSTRUCTION_8_BITS;
  sCommand1.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand1.Instruction        = WRITE_REG_CMD;
  sCommand1.AddressMode        = HAL_XSPI_ADDRESS_8_LINES;
  sCommand1.AddressWidth        = HAL_XSPI_ADDRESS_32_BITS;
  sCommand1.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_ENABLE;
  sCommand1.Address            = Address;
  sCommand1.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand1.DataMode           = HAL_XSPI_DATA_8_LINES;
  sCommand1.DataDTRMode        = HAL_XSPI_DATA_DTR_ENABLE;
  sCommand1.DataLength         = 2;
  sCommand1.DummyCycles        = 0;
  sCommand1.DQSMode            = HAL_XSPI_DQS_DISABLE;

  /* Configure the command */
  if (HAL_XSPI_Command(Ctx, &sCommand1, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Transmission of the data */
  if (HAL_XSPI_Transmit(Ctx, (uint8_t *)(Value), HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

void configureMemoryMappedMode()
{
  Configure_APMemory();

  /* Configure Memory Mapped mode */

  sCommand.OperationType      = HAL_XSPI_OPTYPE_WRITE_CFG;
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_8_LINES;
  sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.Instruction        = WRITE_CMD;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_8_LINES;
  sCommand.AddressWidth       = HAL_XSPI_ADDRESS_32_BITS;
  sCommand.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_ENABLE;
  sCommand.Address            = 0x0;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_16_LINES;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_ENABLE;
  sCommand.DataLength         = BUFFERSIZE;
  sCommand.DummyCycles        = DUMMY_CLOCK_CYCLES_WRITE;
  sCommand.DQSMode            = HAL_XSPI_DQS_ENABLE;
  if (HAL_XSPI_Command(&hxspi2, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  sCommand.OperationType = HAL_XSPI_OPTYPE_READ_CFG;
  sCommand.Instruction = READ_CMD;
  sCommand.DummyCycles = DUMMY_CLOCK_CYCLES_READ;
  sCommand.DQSMode     = HAL_XSPI_DQS_ENABLE;

}

uint32_t XSPI_HyperRAM_Init(uint32_t InstanceNumber)
{
  XSPI_RegularCmdTypeDef  sCommand;

  memset(&sCommand, 0, sizeof(sCommand) );
  HAL_XSPI_DeInit(&XSPIHandle[InstanceNumber]);

  XSPIHandle[InstanceNumber].Instance                     = HSPI_INSTANCE[InstanceNumber];
  XSPIHandle[InstanceNumber].Init.ChipSelectBoundary      = HAL_XSPI_BONDARYOF_16KB;
  XSPIHandle[InstanceNumber].Init.ChipSelectHighTimeCycle = 1;
  XSPIHandle[InstanceNumber].Init.ClockMode               = HAL_XSPI_CLOCK_MODE_0;
  XSPIHandle[InstanceNumber].Init.ClockPrescaler          = 0x03;
  XSPIHandle[InstanceNumber].Init.DelayHoldQuarterCycle   = HAL_XSPI_DHQC_ENABLE;
  XSPIHandle[InstanceNumber].Init.FifoThresholdByte       = 8;
  XSPIHandle[InstanceNumber].Init.FreeRunningClock        = HAL_XSPI_FREERUNCLK_DISABLE;
  XSPIHandle[InstanceNumber].Init.MemoryMode              = HAL_XSPI_SINGLE_MEM;
  XSPIHandle[InstanceNumber].Init.MemorySize              = HAL_XSPI_SIZE_256MB;
  XSPIHandle[InstanceNumber].Init.MemoryType              = HAL_XSPI_MEMTYPE_APMEM_16BITS;
  XSPIHandle[InstanceNumber].Init.SampleShifting          = HAL_XSPI_SAMPLE_SHIFT_NONE;
  XSPIHandle[InstanceNumber].Init.WrapSize                = HAL_XSPI_WRAP_NOT_SUPPORTED;
  if (HAL_XSPI_Init(&XSPIHandle[InstanceNumber]) != HAL_OK)
  {
     Error_Handler();
  }

  XSPI_SwitchOctalToHexa(InstanceNumber);

  sCommand.OperationType      = HAL_XSPI_OPTYPE_WRITE_CFG;
  sCommand.Instruction        = 0x80;
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_8_LINES;
  sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_8_LINES;
  sCommand.AddressWidth       = HAL_XSPI_ADDRESS_32_BITS;
  sCommand.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_ENABLE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_16_LINES;
  sCommand.DataLength         = 0x400;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_ENABLE;
  sCommand.DummyCycles        = 4;/*DUMMY_CLOCK_CYCLES_WRITE;*/
  sCommand.DQSMode            = HAL_XSPI_DQS_ENABLE;

  if (HAL_XSPI_Command(&XSPIHandle[InstanceNumber], &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  sCommand.OperationType = HAL_XSPI_OPTYPE_READ_CFG;
  sCommand.Instruction   = 0x00 ;
  sCommand.DummyCycles   = 4; /*DUMMY_CLOCK_CYCLES_READ;*/

  if (HAL_XSPI_Command(&XSPIHandle[InstanceNumber], &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* if oK */
  return 0;
}

uint32_t XSPI_HyperRAM_EnableMemoryMapped(uint32_t InstanceNumber)
{
  XSPI_MemoryMappedTypeDef sMemMappedCfg = {0};
    sMemMappedCfg.TimeOutActivation    = HAL_XSPI_TIMEOUT_COUNTER_DISABLE;
  sMemMappedCfg.TimeoutPeriodClock   = 0x50;
  if (HAL_XSPI_MemoryMapped(&XSPIHandle[InstanceNumber], &sMemMappedCfg) != HAL_OK)
  {
     return 4;
  }

  /* if oK */
  return 0;
}

static void XSPI_SwitchOctalToHexa(uint32_t InstanceNumber)
{
  XSPI_RegularCmdTypeDef sCommand;
  uint8_t reg[2];

  sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.IOSelect           = HAL_XSPI_SELECT_IO_7_0;
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_8_LINES;
  sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.Instruction        = READ_REG_CMD;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_8_LINES;
  sCommand.AddressWidth       = HAL_XSPI_ADDRESS_32_BITS;
  sCommand.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_ENABLE;
  sCommand.Address            = 8;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_8_LINES;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_ENABLE;
  sCommand.DataLength         = 2;
  sCommand.DummyCycles        = DUMMY_CLOCK_CYCLES_READ;
  sCommand.DQSMode            = HAL_XSPI_DQS_ENABLE;

  if (HAL_XSPI_Command(&XSPIHandle[InstanceNumber], &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_XSPI_Receive(&XSPIHandle[InstanceNumber], reg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  reg[0] |= 0x43;
  sCommand.Instruction = WRITE_REG_CMD;
  sCommand.DummyCycles = 0;

  if (HAL_XSPI_Command(&XSPIHandle[InstanceNumber], &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_XSPI_Transmit(&XSPIHandle[InstanceNumber], reg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
* @brief  Switch from Octal Mode to Hexa Mode on the memory
* @param  None
* @retval None
*/
static void Configure_APMemory(void)
{
  /* MR0 register for read and write */
  uint8_t regW_MR0[2]={0x24,0x8D}; /* To configure AP memory Latency Type and drive Strength */

  /* MR8 register for read and write */
  uint8_t regW_MR8[2]={0x4B,0x08}; /* To configure AP memory Burst Type */

  /* Configure Read Latency and drive Strength */
  if (XSPI_Memory_WriteReg(&hxspi2, MR0, regW_MR0) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure Burst Length */
  if (XSPI_Memory_WriteReg(&hxspi2, MR8, regW_MR8) != HAL_OK)
  {
    Error_Handler();
  }

}

/**
  * @brief  This function is used to read data with DMA in word.
  * @param  SrcBuffer
  * @param  DstBuffer
  * @param  Size
  * @retval status
  */
void DMA_Read_WORD(uint32_t * SrcBuffer, uint32_t * DstBuffer, uint32_t Size)
{
   static DMA_HandleTypeDef hdmarx;

   /* Enable DMAs clock */
   __HAL_RCC_HPDMA1_CLK_ENABLE();

   /* Clear the handler of DMA */
   memset ((void *)&hdmarx, 0, sizeof(DMA_HandleTypeDef));

   HAL_NVIC_SetPriority(HPDMA1_Channel5_IRQn, 0x0F, 0);
   HAL_NVIC_EnableIRQ(HPDMA1_Channel5_IRQn);

   /* Configure the XSPI DMA transmit */
   hdmarx.Init.Request             = DMA_REQUEST_SW;
   hdmarx.Init.Direction           = DMA_MEMORY_TO_MEMORY;
   hdmarx.Init.SrcInc              = DMA_SINC_INCREMENTED;
   hdmarx.Init.DestInc             = DMA_DINC_INCREMENTED;
   hdmarx.Init.Priority            = DMA_HIGH_PRIORITY;
   hdmarx.Init.SrcBurstLength      = 1;
   hdmarx.Init.DestBurstLength     = 1;
   hdmarx.Init.TransferEventMode   = DMA_TCEM_BLOCK_TRANSFER;
   hdmarx.Init.SrcDataWidth        = DMA_SRC_DATAWIDTH_WORD ;
   hdmarx.Init.DestDataWidth       = DMA_DEST_DATAWIDTH_WORD  ;
   hdmarx.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;

   hdmarx.Instance                 = HPDMA1_Channel5;

   /* Enable the GPDMA clock */
   __HAL_RCC_HPDMA1_CLK_ENABLE();

   /* Initialize the DMA channel */
   HAL_DMA_Init(&hdmarx);

   /* DMA attributes configuration in secure mode  */
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
   HAL_DMA_ConfigChannelAttributes(&hdmarx, (DMA_CHANNEL_PRIV | DMA_CHANNEL_SEC | DMA_CHANNEL_DEST_SEC | DMA_CHANNEL_SRC_SEC));
#endif /* __ARM_FEATURE_CMSE */

    /* Clear the destination buffer */
    memset( DstBuffer, 0, Size );

    /* Start DMA StreamIdx transfer */
    HAL_DMA_Start(&hdmarx, (uint32_t)SrcBuffer, (uint32_t)DstBuffer, Size);
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
 
  while (1)
  {
      BSP_LED_Toggle(LED_RED);
      HAL_Delay(100);
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
