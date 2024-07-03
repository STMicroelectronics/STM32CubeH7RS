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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DATA_SIZE              ((uint32_t)0x01800000U) /* Data Size 24MB */

#define BUFFER_SIZE            ((uint32_t)0x00001000U) /* 1KB */
#define BUFFER_WORD_SIZE       (BUFFER_SIZE>>2)        /* Buffer size in Word */
#define NB_BLOCK_BUFFER        BUFFER_SIZE / BLOCKSIZE /* Number of Block (512B) by Buffer */
#define BLOCK_WORD_SIZE        (BLOCKSIZE>>2) /* Block size in words */



#define DATA_ADDRESS           ((uint32_t)0x00000400U) /* SD Card Address to write/read data */
#define DATA_PATTERN0          ((uint32_t)0x01000000U) /* Data pattern for buffer0*/
#define DATA_PATTERN1          ((uint32_t)0x02000000U) /* Data pattern for buffer1 */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

SD_HandleTypeDef hsd1;

/* USER CODE BEGIN PV */
ALIGN_32BYTES(uint32_t Buffer0[BUFFER_WORD_SIZE]);
ALIGN_32BYTES(uint32_t Buffer1[BUFFER_WORD_SIZE]);

void SystemClock_Config(void);
ALIGN_32BYTES(static SDMMC_DMALinkNodeTypeDef pLinkNode[4]);
static SDMMC_DMALinkedListTypeDef Read_LinkedList;
static SDMMC_DMALinkedListTypeDef Write_LinkedList;
SD_DMALinkNodeConfTypeDef LinkNodeConf;
__IO uint8_t TxLnkLstBufCplt, RxLnkLstBufCplt,TxNodeBufCplt,RxNodeBufCplt;

uint32_t Node,RBuff0,RBuff1,WBuff0,WBuff1;
uint32_t ReadError = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
static void MX_SDMMC1_SD_Init(void);
/* USER CODE BEGIN PFP */
static void Fill_Buffer(uint32_t *pBuffer, uint16_t BufferLength, uint32_t Offset);
static HAL_StatusTypeDef Buffercmp(uint32_t* pBuffer, uint32_t BufferLength, uint32_t Offset);
void Dynamic_buffer_fill(void);
void Dynamic_buffer_check(void);
void MPU_AdjustRegionAddressSize(uint32_t Address, uint32_t Size, MPU_Region_InitTypeDef* pInit);
void MPU_Config(void);

//void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd);
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
  /* Add your application code here
     */
 /*##-1- Initialize LEDs mounted on STM32H7S78-DK board #####################*/
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD3);

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SDMMC1_SD_Init();
  /* USER CODE BEGIN 2 */
  /* Try to switch to High Speed Mode , if supported by the card */
  HAL_SD_ConfigSpeedBusOperation(&hsd1,SDMMC_SPEED_MODE_HIGH);

  if(HAL_SD_Erase(&hsd1,DATA_ADDRESS,DATA_SIZE/BLOCKSIZE) != HAL_OK)
  {
    Error_Handler();
  }
  /* Wait for the Card Transfer status */
  while(HAL_SD_GetCardState(&hsd1) != HAL_SD_CARD_TRANSFER)
  {
  }
  /* Fill the buffer to send */
  Fill_Buffer(Buffer0, BUFFER_WORD_SIZE, DATA_PATTERN0);
  SCB_CleanDCache_by_Addr (Buffer0, BUFFER_SIZE);
  
  Fill_Buffer(Buffer1, BUFFER_WORD_SIZE, DATA_PATTERN1);
  SCB_CleanDCache_by_Addr (Buffer1, BUFFER_SIZE);

  /*  Write steps */
  /*Node 0 preparation */
  LinkNodeConf.BufferAddress = (uint32_t) Buffer0;
  LinkNodeConf.BufferSize = BUFFER_SIZE;
  HAL_SDEx_DMALinkedList_BuildNode(&pLinkNode[0], &LinkNodeConf );
  HAL_SDEx_DMALinkedList_InsertNode(&Write_LinkedList, NULL, &pLinkNode[0] );

  /*Node 1 preparation */
  LinkNodeConf.BufferAddress = (uint32_t) Buffer1;
  LinkNodeConf.BufferSize = BUFFER_SIZE;
  HAL_SDEx_DMALinkedList_BuildNode(&pLinkNode[1], &LinkNodeConf );
  HAL_SDEx_DMALinkedList_InsertNode(&Write_LinkedList ,&pLinkNode[0], &pLinkNode[1] );

   

  /*Enable Circular Linked list mode */
  HAL_SDEx_DMALinkedList_EnableCircularMode(&Write_LinkedList );

  SCB_CleanDCache_by_Addr (pLinkNode, sizeof(pLinkNode)); 
  TxLnkLstBufCplt=0;
  TxNodeBufCplt=0;
  BSP_LED_On(LD1);
  /* Write 24MB  on uSD card using DMA transfer */

  HAL_SDEx_DMALinkedList_WriteBlocks(&hsd1,&Write_LinkedList, DATA_ADDRESS, DATA_SIZE/BLOCKSIZE);
  
 
  /* Dynamic buffer filling Process*/
  Dynamic_buffer_fill();

  /* Remove write nodes*/
  HAL_SDEx_DMALinkedList_RemoveNode(&Write_LinkedList,&pLinkNode[1] );
  HAL_SDEx_DMALinkedList_RemoveNode(&Write_LinkedList,&pLinkNode[0] );

  /* Wait for the Card Transfer status */
  while(HAL_SD_GetCardState(&hsd1) != HAL_SD_CARD_TRANSFER)
  {
  }

  /*  Read steps */
  /*Node 0 preparation */
  LinkNodeConf.BufferAddress = (uint32_t) Buffer0;
  LinkNodeConf.BufferSize = BUFFER_SIZE;
  HAL_SDEx_DMALinkedList_BuildNode(&pLinkNode[0], &LinkNodeConf );
  HAL_SDEx_DMALinkedList_InsertNode(&Read_LinkedList, NULL, &pLinkNode[0] );

  /*Node 1 preparation */
  LinkNodeConf.BufferAddress = (uint32_t) Buffer1;
  LinkNodeConf.BufferSize = BUFFER_SIZE;
  HAL_SDEx_DMALinkedList_BuildNode(&pLinkNode[1], &LinkNodeConf );
  HAL_SDEx_DMALinkedList_InsertNode(&Read_LinkedList, &pLinkNode[0], &pLinkNode[1]);

  /*Enable Circular Linked list mode */
  HAL_SDEx_DMALinkedList_EnableCircularMode(&Read_LinkedList );
  SCB_CleanDCache_by_Addr (pLinkNode, sizeof(pLinkNode)); 
  /* Read 24MB  from uSD card using DMA transfer */

  HAL_SDEx_DMALinkedList_ReadBlocks(&hsd1,&Read_LinkedList,DATA_ADDRESS,DATA_SIZE/BLOCKSIZE);

  /* Dynamic buffer Check Process*/
  Dynamic_buffer_check();

  BSP_LED_Off(LD1);

  /* Check the correctness of written dada */

  if (ReadError == 0)
  {

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      BSP_LED_Toggle(LD1);
      HAL_Delay(200);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  }
  else
  {
    while(1)
    {
      BSP_LED_Toggle(LD3);
      HAL_Delay(200);
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief SDMMC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDMMC1_SD_Init(void)
{

  /* USER CODE BEGIN SDMMC1_Init 0 */

  /* USER CODE END SDMMC1_Init 0 */

  /* USER CODE BEGIN SDMMC1_Init 1 */

  /* USER CODE END SDMMC1_Init 1 */
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_FALLING;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 1;
  if (HAL_SD_Init(&hsd1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SDMMC1_Init 2 */

  /* USER CODE END SDMMC1_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief Dynamic buffer fill
  * @param None
  * @retval None
  */
void Dynamic_buffer_fill(void)
{

  while ((WBuff0+WBuff1) <= (DATA_SIZE/BUFFER_SIZE))
  {
    /* Wait for Node transfer completed or the whole transfer completed */
    while ((TxNodeBufCplt == 0) && (TxLnkLstBufCplt ==0));

    TxNodeBufCplt = 0;

    if(Node == 0)
    {
      WBuff0++;
      /*Start processing of Node0 buffer, we should lock the node*/
      HAL_SDEx_DMALinkedList_LockNode(&pLinkNode[0]);

       Fill_Buffer(Buffer0, BUFFERSIZE, DATA_PATTERN0 + (WBuff0 * (uint32_t)0x00001000));
       SCB_CleanDCache_by_Addr (Buffer0, BUFFER_SIZE);

      /*Finish processing of Node1 buffer, we can unlock the node*/
      HAL_SDEx_DMALinkedList_UnlockNode(&pLinkNode[0]);

      /*Set Next Node*/
      Node = 1;
    }
    else if(Node == 1)
    {
      WBuff1++;
      /*Start processing of Node1 buffer, we should lock the node*/
      HAL_SDEx_DMALinkedList_LockNode(&pLinkNode[1]);
     
       Fill_Buffer(Buffer1, BUFFERSIZE, DATA_PATTERN1 + (WBuff1 * (uint32_t)0x00001000));
       SCB_CleanDCache_by_Addr (Buffer1, BUFFER_SIZE);
       
      /*Finish processing of Node1 buffer, we can unlock the node*/
      HAL_SDEx_DMALinkedList_UnlockNode(&pLinkNode[1]);

      /*Set Next Node*/
      Node = 0;
    }
  }
  TxLnkLstBufCplt = 0;
  Node = 0;
}



/**
  * @brief Dynamic buffer check
  * @param None
  * @retval None
  */
void Dynamic_buffer_check(void)
{

  while ((RBuff0+RBuff1) < (DATA_SIZE/BUFFER_SIZE))
  {
    /* Wait for Node transfer completed or the whole transfer completed */
    while ((RxNodeBufCplt == 0) && (RxLnkLstBufCplt ==0));

    RxNodeBufCplt = 0;

    if(Node == 0)
    {
      /*Start processing of Node0 buffer, we should lock the node*/
      HAL_SDEx_DMALinkedList_LockNode(&pLinkNode[0]);
     
      SCB_InvalidateDCache_by_Addr (Buffer0, BUFFER_SIZE);
      
      ReadError += Buffercmp(Buffer0, BUFFERSIZE, DATA_PATTERN0 + (RBuff0 * (uint32_t)0x00001000) );

      /*Finish processing of Node0 buffer, we can unlock the node*/
      HAL_SDEx_DMALinkedList_UnlockNode(&pLinkNode[0]);

      RBuff0++;
      /*Next Node*/
      Node = 1;
    }
    else if(Node == 1)
    {
      /*Start processing of Node0 buffer, we should lock the node*/
      HAL_SDEx_DMALinkedList_LockNode(&pLinkNode[1]);

      SCB_InvalidateDCache_by_Addr (Buffer1, BUFFER_SIZE);
      
      ReadError += Buffercmp(Buffer1, BUFFERSIZE, DATA_PATTERN1 + (RBuff1 * (uint32_t)0x00001000) );

      /*Finish processing of Node1 buffer, we can unlock the node*/
      HAL_SDEx_DMALinkedList_UnlockNode(&pLinkNode[1]);

      RBuff1++;
      /*Next Node*/
      Node = 0;
    }
  }
  RxLnkLstBufCplt = 0;
  Node = 0;
}

/**
  * @brief Rx Transfer completed callbacks
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SDEx_DMALinkedList_ReadCpltCallback(SD_HandleTypeDef *hsd)
{
  RxLnkLstBufCplt = 1;
}

/**
  * @brief Tx Transfer completed callbacks
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SDEx_DMALinkedList_WriteCpltCallback(SD_HandleTypeDef *hsd)
{
  TxLnkLstBufCplt = 1;
}

/**
  * @brief Read DMA Linked list node Transfer completed callbacks
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SDEx_Read_DMALnkLstBufCpltCallback(SD_HandleTypeDef *hsd)
{
  RxNodeBufCplt = 1;
}

/**
  * @brief Read DMA Linked list node Transfer completed callbacks
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SDEx_Write_DMALnkLstBufCpltCallback(SD_HandleTypeDef *hsd)
{
  TxNodeBufCplt = 1;
}

/**
  * @brief SD error callbacks
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_ErrorCallback(SD_HandleTypeDef *hsd)
{
  Error_Handler();
}



/**
  * @brief  Fills the goal 32-bit length buffer
  * @param  pBuffer: pointer on the Buffer to fill
  * @param  BufferLength: length of the buffer to fill
  * @param  Offset: first value to fill on the Buffer
  * @retval None
  */
static void Fill_Buffer(uint32_t *pBuffer, uint16_t BufferLength, uint32_t Offset)
{
  uint16_t IndexTmp;

  /* Put in global buffer same values */
  for ( IndexTmp = 0; IndexTmp < BufferLength; IndexTmp++ )
  {
    pBuffer[IndexTmp] = IndexTmp + Offset;
  }

}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer, pBuffer1: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer identical to pBuffer1
  *         FAILED: pBuffer differs from pBuffer1
  */
static HAL_StatusTypeDef Buffercmp(uint32_t* pBuffer, uint32_t BufferLength, uint32_t Offset)
{
  uint32_t counter = 0;
  while(BufferLength--)
  {
    if(pBuffer[counter] != counter + Offset)
    {
      return HAL_ERROR;
    }
    counter++;
  }
  return HAL_OK;
}


/**
  * @brief  This function configures the MPU context of the application.
  * @retval None
  */
void MPU_Config(void)
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
    MPU_InitStruct.SubRegionDisable = 0U;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
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
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
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
void MPU_AdjustRegionAddressSize(uint32_t Address, uint32_t Size, MPU_Region_InitTypeDef* pInit)
{
  /* Compute the MPU region size */
  pInit->Size = ((31 - __CLZ(Size)) - 1);
 
  if (Size > (1 << (pInit->Size+1 )))
  {
    pInit->Size++;
  }
   

  uint32_t Modulo = Address % (1 << (pInit->Size - 1));

  if (0 != Modulo)
  {
    /* Align address with MPU region size considering there is no need to increase the size */
    pInit->BaseAddress = Address - Modulo;
  }
  else
  {
    pInit->BaseAddress = Address;
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
  BSP_LED_Off(LD1);
  __disable_irq();
    /* Toggle LD3: Error */
    BSP_LED_On(LD3);
  while(1)
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
  while(1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
