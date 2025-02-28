/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   Main program body
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
#include <arm_math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Pass/Fail Status */
#define PASS              0
#define FAIL              1

/* Size of data array */
#define ARRAY_SIZE        48U
#define NB_CALC           (ARRAY_SIZE / 2)

/* Reference values in float format */
#define DELTA_FLOAT  (float)(1.0f / pow(2.0f, 19))  /* Max residual error for Phase, 
                                                       with 6 cycles precision: 2^-19 max residual error */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CORDIC_HandleTypeDef hcordic;
DMA_HandleTypeDef handle_GPDMA1_Channel2;
DMA_HandleTypeDef handle_GPDMA1_Channel1;

/* USER CODE BEGIN PV */

/* CORDIC configuration structure */
CORDIC_ConfigTypeDef sCordicConfig;

/* Initial vector values from range [-1, 1] */
static const float aRefVector_float[ARRAY_SIZE] =
{
  /* Reference positions on the Trigonometric circle. For each vector(x,y), the modulus is 1.0f */
  1.0f, 0.0f,               /* (1, 0) => Phase = 0 deg */
  0.8660254f, 0.5f,         /* (sqrt(3)/2, 0.5f) => Phase = 30 deg */
  0.7071067f, 0.7071067f,   /* (sqrt(2)/2, sqrt(2)/2 => Phase = 45 deg */
  0.5f, 0.8660254f,         /* (0.5f, sqrt(3)/2) => Phase = 60 deg */
  0.0f, 1.0f,               /* (0, 1) => Phase = 90 deg */
  -0.5f, 0.8660254f,        /* (-0.5f, sqrt(3)/2) => Phase = 120 deg */
  -0.7071067f, 0.7071067f,  /* (-sqrt(2)/2, sqrt(2)/2) => Phase = 135 deg */
  -0.8660254f, 0.5f,        /* (-sqrt(3)/2, 0.5f) => Phase = 150 deg */
  -1.0f, 0.0f,              /* (-1, 0) => Phase = 180 deg */
  -0.8660254f, -0.5f,       /* (-sqrt(3)/2, -0.5f) => Phase = 210 deg */
  -0.7071067f, -0.7071067f, /* (-sqrt(2)/2, -sqrt(2)/2) => Phase = 225 deg */
  -0.5f, -0.8660254f,       /* (-0.5f, -sqrt(3)/2) => Phase = 240 deg */
  0.0f, -1.0f,              /* (0, -1) => Phase = 270 deg */
  0.5f, -0.8660254f,        /* (0.5, -sqrt(3)/2) => Phase = 300 deg */
  0.7071067f, -0.7071067f,  /* (sqrt(2)/2, -sqrt(2)/2) => Phase = 315 deg */
  0.8660254f, -0.5f,        /* (sqrt(3)/2, -0.5f => Phase = 330 deg */

  /* Positions inside the Trigonometric circle. For each present vector(x,y), the modulus is sqrt(2)/2 */
  0.5f, 0.5f,   /* Phase = 45 deg  */
  -0.5f, 0.5f,  /* Phase = 135 deg */
  -0.5f, -0.5f, /* Phase = 225 deg */
  0.5f, -0.5f,  /* Phase = 315 deg */

  /* Corner Positions of the square where the Trigonometric circle is inscribed. */
  /* For each vector(x, y), the modulus, is sqrt(2) */
  1.0f, 1.0f,   /* Phase = 45 deg  */
  -1.0f, 1.0f,  /* Phase = 135 deg */
  -1.0f, -1.0f, /* Phase = 225 deg */
  1.0f, -1.0f,  /* Phase = 315 deg */
};

/* Reference float values converted in q1.31 format */
static q31_t aRefValues_q1_31[ARRAY_SIZE]__attribute__((section("noncacheable_buffer")));

/* Phase & Modulus values calculated by CORDIC in q1.31 format */
static q31_t aCalculatedPhase_q1_31[ARRAY_SIZE]__attribute__((section("noncacheable_buffer")));

/* Array of calculated Phase & Modulus in float format */
static float aCalculatedPhase_float[ARRAY_SIZE];

/* State of CORDIC calculations: 0 - Not done, 1 - Done */
__IO uint32_t calculate_complete;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_GPDMA1_Init(void);
static void MX_CORDIC_Init(void);
/* USER CODE BEGIN PFP */
uint32_t Check_Residual_Error(float VarA, float VarB, float MaxError);
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
  /* Initialize LEDs on board */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD3);

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPDMA1_Init();
  MX_CORDIC_Init();
  /* USER CODE BEGIN 2 */

  /* Convert initial float values to q1.31 format */
  arm_float_to_q31(aRefVector_float, aRefValues_q1_31, ARRAY_SIZE);

  /*## Configure the CORDIC peripheral #######################################*/
  sCordicConfig.Function         = CORDIC_FUNCTION_PHASE;     /* Phase function */
  sCordicConfig.Precision        = CORDIC_PRECISION_6CYCLES;  /* max precision for q1.31 sqrt */
  sCordicConfig.Scale            = CORDIC_SCALE_0;            /* no scale */
  sCordicConfig.NbWrite          = CORDIC_NBWRITE_2;          /* Two input data: x & y values */
  sCordicConfig.NbRead           = CORDIC_NBREAD_2;           /* Two output data: modulus & phase */
  sCordicConfig.InSize           = CORDIC_INSIZE_32BITS;      /* q1.31 format for input data */
  sCordicConfig.OutSize          = CORDIC_OUTSIZE_32BITS;     /* q1.31 format for output data */

  if (HAL_CORDIC_Configure(&hcordic, &sCordicConfig) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler();
  }

  /*## Start calculation of Phase in DMA mode #################################*/
  calculate_complete = 0;
  if (HAL_CORDIC_Calculate_DMA(&hcordic, aRefValues_q1_31, aCalculatedPhase_q1_31,
                               NB_CALC, CORDIC_DMA_DIR_IN_OUT) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }

  /* Wait the end of CORDIC process */
  while (calculate_complete == 0)
  {
  }

  /* Convert q1.31 Phase & Modulus results to float */
  arm_q31_to_float(aCalculatedPhase_q1_31, aCalculatedPhase_float, ARRAY_SIZE);

  /*## Compare CORDIC results to the reference values ########################*/
  for (uint32_t i = 0; i < ARRAY_SIZE; i += 2)
  {
    /* Check Phase result */
    float c_phase_expected = atan2(aRefVector_float[i + 1], aRefVector_float[i]) / PI;

    if (Check_Residual_Error(aCalculatedPhase_float[i], c_phase_expected, DELTA_FLOAT) == FAIL)
    {
      Error_Handler();
    }

    /* Check Modulus result */
    float modulus_expected = sqrt((aRefVector_float[i] * aRefVector_float[i]) + (aRefVector_float[i + 1] * aRefVector_float[i + 1]));

    /* If modulus > 1, the CORDIC modulus result is saturated to 1 */
    if (modulus_expected > 1.0f) { modulus_expected = 1.0f; }

    if (Check_Residual_Error(aCalculatedPhase_float[i + 1], modulus_expected, DELTA_FLOAT) == FAIL)
    {
      Error_Handler();
    }
  }

  /* Correct CORDIC output values: Turn on LD1 */
  BSP_LED_On(LD1);

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
  * @brief CORDIC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CORDIC_Init(void)
{

  /* USER CODE BEGIN CORDIC_Init 0 */

  /* USER CODE END CORDIC_Init 0 */

  /* USER CODE BEGIN CORDIC_Init 1 */

  /* USER CODE END CORDIC_Init 1 */
  hcordic.Instance = CORDIC;
  if (HAL_CORDIC_Init(&hcordic) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CORDIC_Init 2 */

  /* USER CODE END CORDIC_Init 2 */

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
    HAL_NVIC_SetPriority(GPDMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel1_IRQn);
    HAL_NVIC_SetPriority(GPDMA1_Channel2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel2_IRQn);

  /* USER CODE BEGIN GPDMA1_Init 1 */

  /* USER CODE END GPDMA1_Init 1 */
  /* USER CODE BEGIN GPDMA1_Init 2 */

  /* USER CODE END GPDMA1_Init 2 */

}

/* USER CODE BEGIN 4 */
/**
  * @brief  CORDIC calculate complete callback.
  * @param  hcordic pointer to a CORDIC_HandleTypeDef structure that contains
  *         the configuration information for CORDIC module
  * @retval None
  */
void HAL_CORDIC_CalculateCpltCallback(CORDIC_HandleTypeDef *hcordic)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcordic);

  calculate_complete = 1;
}

/**
  * @brief  Check delta between two values is below threshold
  * @param  VarA First input variable
  * @param  VarB Second input variable
  * @param  MaxError Maximum delta allowed between VarA and VarB
  * @retval Status
  *           PASS: Delta is below threshold
  *           FAIL: Delta is above threshold
  */
uint32_t Check_Residual_Error(float VarA, float VarB, float MaxError)
{
  uint32_t status = PASS;

  if ((VarA - VarB) >= 0)
  {
    if ((VarA - VarB) > MaxError)
    {
      status = FAIL;
    }
  }
  else
  {
    if ((VarB - VarA) > MaxError)
    {
      status = FAIL;
    }
  }

  return status;
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
  while (1)
  {
    /* Wrong CORDIC output values: Toggle LD3 */
    BSP_LED_Toggle(LD3);
    HAL_Delay(500);
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
