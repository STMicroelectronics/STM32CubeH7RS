/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    PSSI/PSSI_Master_Com/Appli/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32H7RSxx PSSI HAL API to
             manage a Reception and Transmission procedure in polling mode.
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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Timer defines */
#define PERIOD_VALUE        (uint32_t)(40 - 1)              /* Period Value  */
#define PULSE_VALUE         (uint32_t)(40 / 2)              /* Capture Compare 1 Value  */
/* Compute the prescaler value to have TIM1 counter clock equal to 1MHz */
#define PRESCALER_VALUE     (uint32_t)(((SystemCoreClock) / 80000000) - 1)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

PSSI_HandleTypeDef hpssi;

TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */
/* Buffer data to transmit to the Slave */
static uint8_t aTxBuffer[36];

/* Buffer data received from Master */
static uint8_t aRxBuffer[16];

/* Buffer data expected at reception */
uint8_t aRxBufferExp[] = "Hello from Slave";
uint32_t Request_received = 0 ;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
static void MX_PSSI_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */
static void MPU_AdjustRegionAddressSize(uint32_t Address, uint32_t Size, MPU_Region_InitTypeDef* pInit);
static void MPU_Config(void);

/* PWM generation functions */
static void Start_Clock_Generation(void);
static void Stop_Clock_Generation(void);

static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);
static uint32_t Fetch_Slave_Request(uint8_t *pRxBuffer);
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
  MX_PSSI_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  /* Configure LEDs */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD2);

  Start_Clock_Generation();

  while (Request_received != 1)
  {
    /* Start receiving data from the Slave */
    if (HAL_PSSI_Receive(&hpssi, (uint8_t*)aRxBuffer, sizeof(aRxBuffer), 10000) != HAL_OK)
    {
      Error_Handler();
    }

    /* Check if the data was received to exit the loop */
    if (Fetch_Slave_Request(aRxBuffer) == 1)
    {
      Request_received = 1;
    }

    HAL_Delay(2);
  }

  /* Reception is done */
  BSP_LED_Toggle(LD1);

  /* Start transmitting the received & modified data */
  if (HAL_PSSI_Transmit(&hpssi, (uint8_t*)aTxBuffer, sizeof(aTxBuffer), 10000) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_Delay(1);

  /* Transmission is done */
  BSP_LED_Toggle(LD1);

  /* Stop PWM generation */
  Stop_Clock_Generation();

  /* Compare buffer received with buffer sent */
  if (Buffercmp(aRxBuffer, aRxBufferExp, sizeof(aRxBufferExp)) != 0)
  {
    Error_Handler();
  }

  BSP_LED_Toggle(LD1);

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
  hpssi.Init.DataWidth = HAL_PSSI_8BITS;
  hpssi.Init.BusWidth = HAL_PSSI_8LINES;
  hpssi.Init.ControlSignal = HAL_PSSI_DE_RDY_ALT_ENABLE;
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
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = PRESCALER_VALUE;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = PERIOD_VALUE;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = PULSE_VALUE;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

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
  __HAL_RCC_GPIOA_CLK_ENABLE();

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
  * @brief This function starts the generation of the PWM signal generation.
  * @param None
  * @retval None
  */
static void Start_Clock_Generation(void)
{
  /* Start PWM signal generation */
  if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK)
  {
    /* PWM Generation Error */
    Error_Handler();
  }
}

/**
  * @brief This function stops the generation of the PWM signal generation.
  * @param None
  * @retval None
  */
static void Stop_Clock_Generation(void)
{
  /* Stop PWM signal generation */
  if (HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1) != HAL_OK)
  {
    /* PWM Generation Error */
    Error_Handler();
  }
}

/**
  @brief Compares two buffers.
  @param pBuffer1, pBuffer2: buffers to be compared.
  @param BufferLength: buffer's length
  @retval 0 : pBuffer1 identical to pBuffer2
         >0 : pBuffer1 differs from pBuffer2
  */
static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength)
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
  * @brief This function retrieves the received buffer and fill the buffer to transmit with it.
  * @param pRxBuffer: buffer received
  * @retval 1 : the modification done successfully
            0 : expected buffer not received
  */
uint32_t Fetch_Slave_Request(uint8_t *pRxBuffer)
{
  uint32_t i;
  uint32_t j;
  char aReplyMsg[21] = ": received by Master" ;

  if (strstr((char*)pRxBuffer,"Slave") != 0)
  {
    /* Fill the buffer to transmit with the received data */
    for (i=0; i<strlen((char*)pRxBuffer); i++)
    {
      aTxBuffer[i] = pRxBuffer[i];
    }

    /* Complete the buffer to transmit with a reply */
    for (j=0; j<strlen(aReplyMsg); j++)
    {
      aTxBuffer[i] = aReplyMsg[j];
      i++;
    }
    return 1;
  }
  else
  {
    return 0;
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
  Error_Handler();
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
