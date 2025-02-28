/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : This example code shows how to use STM32H7RSxx
  *                   SPI HAL API to program an Accelerometer in Polling mode.
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
#include <stdio.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* Please uncomment 1 define (LSM6DSL_ID or IIS3DWB_ID) depending on used accelerometer board */
#define LSM6DSL_ID                0x6A    /* LSM6DSL (Board MIKROELSM6DSL click)   */
//#define IIS3DWB_ID                0x7B    /* IIS3DWB (Board MIKROEAccel14 click    */

#if defined (LSM6DSL_ID)
#define ACCEL_ID                  LSM6DSL_ID
#define ACCEL_CMD_INDEX_0            0    /* ACCEL cmd index 0     */
#define ACCEL_CMD_INDEX_1            1    /* ACCEL cmd index 1     */
#define ACCEL_CMD_WHO_AM_I        0x8F    /* ACCEL Who am i cmd    */
#define ACCEL_CMD_STATUS          0x9E    /* ACCEL Status Reg      */
#define ACCEL_CMD_GET_X_Y_Z       0xA8    /* ACCEL Get X Y Z axis  */
#define ACCEL_CMD_SIZE               1    /* ACCEL size Command    */
#define ACCEL_CMD_WR_SIZE            2    /* ACCEL WR size Command */
#define ACCEL_CMD_GET_X_Y_Z_SIZE     6    /* ACCEL axis size Cmd   */
#define ACCEL_REG_CTRL1           0x10    /* ACCEL CTRL1 reg Addr  */
#define ACCEL_CMD_CTRL1_ODR       0xFC    /* ACCEL ODR  1.66KHz    */
#define ACCEL_STATUS_INDEX           0    /* ACCEL Status index    */
#define ACCEL_STATUS_NEW_DATA     0x01    /* ACCEL Status new Data */
#define ACCEL_STATUS_CLEAR        0x00    /* ACCEL Status clear    */
#define ACCEL_LONG_TIMEOUT        1000    /* ACCEL Long Timeout 1s */
#define ACCEL_FS200_TO_MG         6.1f    /* ACCEL Ratio FS200:MG  */
#define ACCEL_DATA_X_Y_Z_SIZE        3    /* ACCEL Data Raw Size   */
#define ACCEL_DATA_X_POS             0    /* ACCEL Data X position */
#define ACCEL_DATA_Y_POS             1    /* ACCEL Data Y position */
#define ACCEL_DATA_Z_POS             2    /* ACCEL Data Z position */

#elif defined (IIS3DWB_ID)
#define ACCEL_ID                  IIS3DWB_ID
#define ACCEL_CMD_INDEX_0            0    /* ACCEL cmd index 0     */
#define ACCEL_CMD_INDEX_1            1    /* ACCEL cmd index 1     */
#define ACCEL_CMD_WHO_AM_I        0x8F    /* ACCEL Who am i cmd    */
#define ACCEL_CMD_STATUS          0x9E    /* ACCEL Status Reg      */
#define ACCEL_CMD_GET_X_Y_Z       0xA8    /* ACCEL Get X Y Z axis  */
#define ACCEL_CMD_SIZE               1    /* ACCEL size Command    */
#define ACCEL_CMD_WR_SIZE            2    /* ACCEL WR size Command */
#define ACCEL_CMD_GET_X_Y_Z_SIZE     6    /* ACCEL axis size Cmd   */
#define ACCEL_REG_CTRL1           0x10    /* ACCEL CTRL1 reg Addr  */
#define ACCEL_CMD_CTRL1_ODR       0xEC    /* ACCEL ODR  1.66KHz    */
#define ACCEL_STATUS_INDEX           0    /* ACCEL Status index    */
#define ACCEL_STATUS_NEW_DATA     0x01    /* ACCEL Status new Data */
#define ACCEL_STATUS_CLEAR        0x00    /* ACCEL Status clear    */
#define ACCEL_LONG_TIMEOUT        1000    /* ACCEL Long Timeout 1s */
#define ACCEL_FS200_TO_MG         6.1f    /* ACCEL Ratio FS200:MG  */
#define ACCEL_DATA_X_Y_Z_SIZE        3    /* ACCEL Data Raw Size   */
#define ACCEL_DATA_X_POS             0    /* ACCEL Data X position */
#define ACCEL_DATA_Y_POS             1    /* ACCEL Data Y position */
#define ACCEL_DATA_Z_POS             2    /* ACCEL Data Z position */
#else
#warning Please uncomment define LSM6DSL_ID or IIS3DWB_ID depending on used accelerometer
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
/* Buffer used for command transmission data */
uint8_t aTxCmd[] = {0x00, 0x00};

/* Buffer used for reception */
uint8_t aRxStatus;
int16_t aAccelDataRaw[ACCEL_DATA_X_Y_Z_SIZE];
float aAccelMgFormat[ACCEL_DATA_X_Y_Z_SIZE];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */
static float_t Accel_Convert_fs200_To_mg(int16_t lsb);
static void Accel_Data_Treatment(int16_t *pAccelData);
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

  /* Configure LEDs */
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD2);
  BSP_LED_Init(LD3);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  /*##-1- Verify WHO_AM_I ID from Accel ########################################*/
  /* Set SPI CS to low state to select ACCEL */
  HAL_GPIO_WritePin(Accel_CS_GPIO_Port, Accel_CS_Pin, GPIO_PIN_RESET);

  /* Prepare WHO_AM_I command */
  aTxCmd[ACCEL_CMD_INDEX_0] = ACCEL_CMD_WHO_AM_I;

  /* Send WHO_AM_I command to ACCEL */
  if (HAL_SPI_Transmit(&hspi1, (uint8_t *)aTxCmd, ACCEL_CMD_SIZE, ACCEL_LONG_TIMEOUT) != HAL_OK)
  {
    /* Transfer error in transmission process */
    Error_Handler();
  }

  /* Read WHO_AM_I ID from ACCEL */
  if (HAL_SPI_Receive(&hspi1, &aRxStatus, ACCEL_CMD_SIZE, ACCEL_LONG_TIMEOUT) != HAL_OK)
  {
    /* Writing process Error */
    Error_Handler();
  }

  /* Set SPI CS to high state to unselect ACCEL */
  HAL_GPIO_WritePin(Accel_CS_GPIO_Port, Accel_CS_Pin, GPIO_PIN_SET);

  if (aRxStatus != ACCEL_ID)
  {
     /* Device not found */
    Error_Handler();
  }

  /*##-2- Set Output DataRate ################################################*/
  /* Set SPI CS to low state to select ACCEL */
  HAL_GPIO_WritePin(Accel_CS_GPIO_Port, Accel_CS_Pin, GPIO_PIN_RESET);

  /* Prepare ODR command */
  aTxCmd[ACCEL_CMD_INDEX_0] = ACCEL_REG_CTRL1;
  aTxCmd[ACCEL_CMD_INDEX_1] = ACCEL_CMD_CTRL1_ODR;

   /* Send ODR Status Command command to ACCEL */
  if (HAL_SPI_Transmit(&hspi1, (uint8_t *)aTxCmd, ACCEL_CMD_WR_SIZE, ACCEL_LONG_TIMEOUT) != HAL_OK)
  {
    /* Transfer error in transmission process */
    Error_Handler();
  }

  /* Set SPI CS to high state to unselect ACCEL */
  HAL_GPIO_WritePin(Accel_CS_GPIO_Port, Accel_CS_Pin, GPIO_PIN_SET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /*##-3- Check Output Status ##############################################*/
    /* Set SPI CS to low state to select ACCEL */
    HAL_GPIO_WritePin(Accel_CS_GPIO_Port, Accel_CS_Pin, GPIO_PIN_RESET);

    /* Prepare Status command */
    aTxCmd[ACCEL_CMD_INDEX_0] = ACCEL_CMD_STATUS;

    /* Send Status command to ACCEL */
    if (HAL_SPI_Transmit(&hspi1, (uint8_t *)aTxCmd, ACCEL_CMD_SIZE, ACCEL_LONG_TIMEOUT) != HAL_OK)
    {
      /* Transfer error in transmission process */
      Error_Handler();
    }

    /* Read Status from ACCEL */
    if (HAL_SPI_Receive(&hspi1, &aRxStatus, ACCEL_CMD_SIZE, ACCEL_LONG_TIMEOUT) != HAL_OK)
    {
      /* Writing process Error */
      Error_Handler();
    }

    /* Set SPI CS to high state to unselect ACCEL */
    HAL_GPIO_WritePin(Accel_CS_GPIO_Port, Accel_CS_Pin, GPIO_PIN_SET);

    /*##-4- Retrieve Output data #############################################*/
    /* Read Output only if new value is available */
    if ((aRxStatus & ACCEL_STATUS_NEW_DATA) == ACCEL_STATUS_NEW_DATA)
    {
      /* Set SPI CS to low state to select ACCEL */
      HAL_GPIO_WritePin(Accel_CS_GPIO_Port, Accel_CS_Pin, GPIO_PIN_RESET);

      /* Prepare Read X_Y_Z command */
      aTxCmd[ACCEL_CMD_INDEX_0] = ACCEL_CMD_GET_X_Y_Z;

      /* Send Read X_Y_Z command to ACCEL */
      if (HAL_SPI_Transmit(&hspi1, (uint8_t *)aTxCmd, ACCEL_CMD_SIZE, ACCEL_LONG_TIMEOUT) != HAL_OK)
      {
        /* Transfer error in transmission process */
        Error_Handler();
      }

      /* Read X_Y_Z Output from ACCEL */
      if (HAL_SPI_Receive(&hspi1, (uint8_t *)aAccelDataRaw, ACCEL_DATA_X_Y_Z_SIZE*2, ACCEL_LONG_TIMEOUT) != HAL_OK)
      {
        /* Writing process Error */
        Error_Handler();
      }

      /* Set SPI CS to high state to unselect ACCEL */
      HAL_GPIO_WritePin(Accel_CS_GPIO_Port, Accel_CS_Pin, GPIO_PIN_SET);

      /*##-5- Data treatment and LEDs management ##############################*/
      Accel_Data_Treatment((int16_t *)aAccelDataRaw);
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x7;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  hspi1.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
  hspi1.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Accel_CS_GPIO_Port, Accel_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : Accel_CS_Pin */
  GPIO_InitStruct.Pin = Accel_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(Accel_CS_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief Function to convert acceleration fs200 format to mg format
  * @param lsb: Acceleration in fs200 lsb format
  * @retval Acceleration in mg format
  */
static float_t Accel_Convert_fs200_To_mg(int16_t lsb)
{
  return ((float_t)lsb * ACCEL_FS200_TO_MG);
}

/**
  * @brief Function to convert data and manage LEDs
  * @param pAccelData: pointer on Accel Data Buffer in fs200 lsb format
  * @retval None
  */
static void Accel_Data_Treatment(int16_t *pAccelData)
{
  /* Convert X_Y_Z output to mg format */
  aAccelMgFormat[ACCEL_DATA_X_POS] = Accel_Convert_fs200_To_mg(pAccelData[ACCEL_DATA_X_POS]);
  aAccelMgFormat[ACCEL_DATA_Y_POS] = Accel_Convert_fs200_To_mg(pAccelData[ACCEL_DATA_Y_POS]);
  aAccelMgFormat[ACCEL_DATA_Z_POS] = Accel_Convert_fs200_To_mg(pAccelData[ACCEL_DATA_Z_POS]);

  /* Check which LED must be enabled */
  if ((fabs(aAccelMgFormat[ACCEL_DATA_X_POS]) > fabs(aAccelMgFormat[ACCEL_DATA_Y_POS])) &&
      (fabs(aAccelMgFormat[ACCEL_DATA_X_POS]) > fabs(aAccelMgFormat[ACCEL_DATA_Z_POS])))
  {
    /* Enable LD1 if X axis is the most significant */
    BSP_LED_On(LD1);
    BSP_LED_Off(LD2);
    BSP_LED_Off(LD3);
  }
  else if ((fabs(aAccelMgFormat[ACCEL_DATA_Y_POS]) > fabs(aAccelMgFormat[ACCEL_DATA_X_POS])) &&
           (fabs(aAccelMgFormat[ACCEL_DATA_Y_POS]) > fabs(aAccelMgFormat[ACCEL_DATA_Z_POS])))
  {
  /* Enable LD2 if Y axis is the most significant */
    BSP_LED_Off(LD1);
    BSP_LED_On(LD2);
    BSP_LED_Off(LD3);
  }
  else
  {
    /* Enable LD3 if Z axis is the most significant */
    BSP_LED_Off(LD1);
    BSP_LED_Off(LD2);
    BSP_LED_On(LD3);
  }
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
    /* Toggle LD1 for error */
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
  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
