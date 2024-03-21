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
#include "RGB565_240x21.h"
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

GFXTIM_HandleTypeDef hgfxtim;

LTDC_HandleTypeDef hltdc;

/* USER CODE BEGIN PV */
/* variables to check DMA2D transfer */
uint32_t tickstart;
uint32_t timeout = 50;
__IO uint32_t transferComplete = 0;
/* Variables for GFXTIM */
uint32_t lineStep = 2;
uint32_t frameFreq = 1;
uint32_t lineCompare;
__IO uint32_t EV1_event = 0;
/*Variables for main application */
uint32_t nLines = 0;
uint32_t srcInsertOffset = 0;
uint32_t dstInsertOffset = 0;
uint32_t cntFrame = 0;
uint32_t nTotal, nPartDisplay, nFullDisplay;
uint32_t srcEraseOffset, dstEraseOffset;
/* LTDC Frame buffer */
uint32_t RGB565_240x130[(LAYER_SIZE_X * LAYER_SIZE_Y * LAYER_BYTE_PER_PIXEL) / 4]__attribute__((aligned(16),section("noncacheable_buffer")));
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
static void MX_DMA2D_Init(void);
static void MX_LTDC_Init(void);
static void MX_GFXTIM_Init(void);
/* USER CODE BEGIN PFP */
static void MPU_AdjustRegionAddressSize(uint32_t Address, uint32_t Size, MPU_Region_InitTypeDef* pInit);
static void MPU_Config(void);

static void TransferError(DMA2D_HandleTypeDef* Dma2dHandle);
static void TransferComplete(DMA2D_HandleTypeDef* Dma2dHandle);
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

  /* Initialize variables */
  nPartDisplay = frames_partition[(lineStep - 1)*2 ];
  nFullDisplay = frames_partition[(lineStep - 1)*2 + 1 ];
  nTotal = (2*nPartDisplay) + nFullDisplay ;
  srcInsertOffset = TEXT_SIZE_Y;
  lineCompare = LCD_VBP + LCD_VSYNC_HEIGHT + lineStep;
  /* Initialize LD1 and LD2*/
  BSP_LED_Init(LD1);
  BSP_LED_Init(LD2);

  for (uint32_t i=0;i<(LAYER_SIZE_X * LAYER_SIZE_Y * LAYER_BYTE_PER_PIXEL) / 4;i++)
  {
    RGB565_240x130[i]=0xFFFFFFFF;
  }
  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */


  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA2D_Init();
  MX_LTDC_Init();
  MX_GFXTIM_Init();
  /* USER CODE BEGIN 2 */

  /* DMA2D Callbacks Configuration */
  hdma2d.XferCpltCallback  = TransferComplete;
  hdma2d.XferErrorCallback = TransferError;

  /* Set LTDC layer1 source address */
  HAL_LTDC_SetAddress(&hltdc, (uint32_t) &RGB565_240x130, LTDC_LAYER_1);

  /* Set GFXTIM Relative frame compare and Absolute line compare values */
  HAL_GFXTIM_RelativeTimer_SetReload(&hgfxtim,GFXTIM_RELATIVE_TIMER1,frameFreq);
  HAL_GFXTIM_AbsoluteTimer_SetLineCompare(&hgfxtim,GFXTIM_ABSOLUTE_LINE_COMPARE1,lineCompare);

  /* Enable GFXTIM complex event */
  HAL_GFXTIM_EventGenerator_Enable(&hgfxtim, GFXTIM_EVENT_GENERATOR_1);

  /* Start GFXTIM absolute and relative counters */
  HAL_GFXTIM_AbsoluteTimer_Start(&hgfxtim);
  HAL_GFXTIM_RelativeTimer_Start(&hgfxtim,GFXTIM_RELATIVE_TIMER1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (EV1_event ==1)
    {
      /* update next line event */
      lineCompare = lineCompare + lineStep;
      HAL_GFXTIM_AbsoluteTimer_SetLineCompare(&hgfxtim,GFXTIM_ABSOLUTE_LINE_COMPARE1,lineCompare);

      /* update RGB565_240x21 new position variables */
      if (cntFrame <= nTotal)
      {
        if (cntFrame<nPartDisplay)
        {
          dstInsertOffset = 0;
          nLines = nLines + lineStep;
          srcInsertOffset = srcInsertOffset - lineStep;
          if (nLines>TEXT_SIZE_Y)
          {
            dstInsertOffset = (nLines + lineStep) - TEXT_SIZE_Y;
            nLines = TEXT_SIZE_Y;
            srcInsertOffset = 0;
          }
        }
        else if (cntFrame <= nPartDisplay+nFullDisplay)
        {
          dstInsertOffset = dstInsertOffset + lineStep;
          nLines = TEXT_SIZE_Y;
          srcInsertOffset = 0;
        }
        else
        {
          dstInsertOffset = dstInsertOffset + lineStep;
          nLines = LAYER_SIZE_Y - dstInsertOffset;
          srcInsertOffset = 0;
        }
        cntFrame = cntFrame + 1;
      }

      transferComplete = 0;
      /* update RGB565_240x21 position */
      HAL_DMA2D_BlendingStart_IT(&hdma2d,
                                 (uint32_t)&RGB565_240x130,
                                 (uint32_t)&RGB565_240x21+ 120 * 4 * srcInsertOffset,
                                 (uint32_t)&RGB565_240x130 + 120 * 4 * dstInsertOffset,
                                 nLines,
                                 240);
      /* Wait until DMA2D transfer completes successfully or exceeds timeout */
      tickstart = HAL_GetTick();
      while(transferComplete == 0)
      {
        if (HAL_GetTick() - tickstart > timeout)
          TransferError(&hdma2d);
      }

      if (cntFrame == nPartDisplay + 1)
      {
        srcEraseOffset=dstInsertOffset + nLines + 5;
        dstEraseOffset=0;
      }
      else if (cntFrame >nPartDisplay+1)
      {
        srcEraseOffset=dstInsertOffset - 2 * lineStep ;
        dstEraseOffset=dstInsertOffset - lineStep ;
      }
      else if (cntFrame == 1)
      {
        srcEraseOffset=dstInsertOffset + nLines +1 ;
        dstEraseOffset=LAYER_SIZE_Y - lineStep ;
      }
      if (cntFrame>nPartDisplay || cntFrame == 1)
      {
        transferComplete = 0;
        /* Erase remaining RGB565_240x21 trace content */
        HAL_DMA2D_BlendingStart_IT(&hdma2d,
                                   (uint32_t)&RGB565_240x130,
                                   (uint32_t)&RGB565_240x130 + 120 * 4 * srcEraseOffset,
                                   (uint32_t)&RGB565_240x130 + 120 * 4 * dstEraseOffset,
                                   lineStep,
                                   240);
        /* Wait until DMA2D transfer completes successfully or exceeds timeout */
        tickstart = HAL_GetTick();
        while(transferComplete == 0)
        {
          if (HAL_GetTick() - tickstart > timeout)
            TransferError(&hdma2d);
        }
      }

      if (cntFrame > nTotal )
      {
        srcEraseOffset=dstInsertOffset - 2 * lineStep ;
        dstEraseOffset=dstInsertOffset - lineStep ;
        nLines=0;
        cntFrame =  0;
        srcInsertOffset = TEXT_SIZE_Y;
        lineCompare = LCD_VBP + LCD_VSYNC_HEIGHT + lineStep;
      }
      EV1_event = 0;
    }
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
  hdma2d.Init.Mode = DMA2D_M2M_BLEND;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
  hdma2d.Init.OutputOffset = 0;
  hdma2d.LayerCfg[1].InputOffset = 0;
  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_REPLACE_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0x00;
  hdma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;
  hdma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR;
  hdma2d.LayerCfg[1].ChromaSubSampling = DMA2D_NO_CSS;
  hdma2d.LayerCfg[0].InputOffset = 0;
  hdma2d.LayerCfg[0].InputColorMode = DMA2D_INPUT_RGB565;
  hdma2d.LayerCfg[0].AlphaMode = DMA2D_REPLACE_ALPHA;
  hdma2d.LayerCfg[0].InputAlpha = 0xFF;
  hdma2d.LayerCfg[0].AlphaInverted = DMA2D_REGULAR_ALPHA;
  hdma2d.LayerCfg[0].RedBlueSwap = DMA2D_RB_REGULAR;
  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA2D_ConfigLayer(&hdma2d, 0) != HAL_OK)
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

/**
  * @brief GFXTIM Initialization Function
  * @param None
  * @retval None
  */
static void MX_GFXTIM_Init(void)
{

  /* USER CODE BEGIN GFXTIM_Init 0 */

  /* USER CODE END GFXTIM_Init 0 */

  GFXTIM_ClockGeneratorConfigTypeDef pClockGeneratorConfig = {0};
  GFXTIM_AbsoluteTimerConfigTypeDef pAbsoluteTimerConfig = {0};
  GFXTIM_EventGeneratorConfigTypeDef pEventGeneratorConfig = {0};
  GFXTIM_RelativeTimerConfigTypeDef pRelativeTimerConfig = {0};

  /* USER CODE BEGIN GFXTIM_Init 1 */

  /* USER CODE END GFXTIM_Init 1 */
  hgfxtim.Instance = GFXTIM;
  hgfxtim.Init.SynchroSrc = GFXTIM_SYNC_SRC_HSYNC_VSYNC_0;
  if (HAL_GFXTIM_Init(&hgfxtim) != HAL_OK)
  {
    Error_Handler();
  }
  pClockGeneratorConfig.LCCReloadValue = 149;
  pClockGeneratorConfig.FCCReloadValue = 1;
  pClockGeneratorConfig.LineClockSrc = GFXTIM_LINE_CLK_SRC_HSYNC_RISING;
  pClockGeneratorConfig.FrameClockSrc = GFXTIM_FRAME_CLK_SRC_VSYNC_RISING;
  pClockGeneratorConfig.LineClockCalib = GFXTIM_LINE_CLK_CALIB_DISABLE;
  pClockGeneratorConfig.FrameClockCalib = GFXTIM_FRAME_CLK_CALIB_DISABLE;
  if (HAL_GFXTIM_ClockGenerator_Config(&hgfxtim, &pClockGeneratorConfig) != HAL_OK)
  {
    Error_Handler();
  }
  pAbsoluteTimerConfig.FrameCompare1Value = 10;
  pAbsoluteTimerConfig.LineCompare1Value = 499;
  pAbsoluteTimerConfig.LineCompare2Value = 0;
  pAbsoluteTimerConfig.FrameCounterValue = 0;
  pAbsoluteTimerConfig.LineCounterValue = 0;
  pAbsoluteTimerConfig.FrameOverflowInterrupt = DISABLE;
  pAbsoluteTimerConfig.FrameCompare1Interrupt = DISABLE;
  pAbsoluteTimerConfig.LineOverflowInterrupt = DISABLE;
  pAbsoluteTimerConfig.LineCompare1Interrupt = DISABLE;
  pAbsoluteTimerConfig.LineCompare2Interrupt = DISABLE;
  if (HAL_GFXTIM_AbsoluteTimer_Config(&hgfxtim, &pAbsoluteTimerConfig) != HAL_OK)
  {
    Error_Handler();
  }
  pEventGeneratorConfig.LineEvent = GFXTIM_LINE_EVENT_ALC1_COMPARE;
  pEventGeneratorConfig.FrameEvent = GFXTIM_FRAME_EVENT_RFC1_RELOAD;
  pEventGeneratorConfig.EventInterrupt = ENABLE;
  if (HAL_GFXTIM_EventGenerator_Config(&hgfxtim, GFXTIM_EVENT_GENERATOR_1, &pEventGeneratorConfig) != HAL_OK)
  {
    Error_Handler();
  }
  pRelativeTimerConfig.AutoReloadValue = 10;
  pRelativeTimerConfig.CounterMode = GFXTIM_MODE_CONTINUOUS;
  pRelativeTimerConfig.ReloadInterrupt = DISABLE;
  if (HAL_GFXTIM_RelativeTimer_Config(&hgfxtim, &pRelativeTimerConfig, GFXTIM_RELATIVE_TIMER1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN GFXTIM_Init 2 */

  /* USER CODE END GFXTIM_Init 2 */

}

/**
  * @brief LTDC Initialization Function
  * @param None
  * @retval None
  */
static void MX_LTDC_Init(void)
{

  /* USER CODE BEGIN LTDC_Init 0 */

  /* USER CODE END LTDC_Init 0 */

  LTDC_LayerCfgTypeDef pLayerCfg = {0};

  /* USER CODE BEGIN LTDC_Init 1 */

  /* USER CODE END LTDC_Init 1 */
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 3;
  hltdc.Init.VerticalSync = 3;
  hltdc.Init.AccumulatedHBP = 11;
  hltdc.Init.AccumulatedVBP = 11;
  hltdc.Init.AccumulatedActiveW = 811;
  hltdc.Init.AccumulatedActiveH = 491;
  hltdc.Init.TotalWidth = 819;
  hltdc.Init.TotalHeigh = 499;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg.WindowX0 = 120;
  pLayerCfg.WindowX1 = 360;
  pLayerCfg.WindowY0 = 70;
  pLayerCfg.WindowY1 = 200;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg.FBStartAdress = 0;
  pLayerCfg.ImageWidth = LAYER_SIZE_X;
  pLayerCfg.ImageHeight = LAYER_SIZE_Y;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LTDC_Init 2 */

  /* USER CODE END LTDC_Init 2 */

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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_ON_OFF_GPIO_Port, LCD_ON_OFF_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_BACKLIGHT_GPIO_Port, LCD_BACKLIGHT_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : LCD_ON_OFF_Pin */
  GPIO_InitStruct.Pin = LCD_ON_OFF_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_ON_OFF_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_BACKLIGHT_Pin */
  GPIO_InitStruct.Pin = LCD_BACKLIGHT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_BACKLIGHT_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_GFXTIM_EventGenerator_EV1Callback(GFXTIM_HandleTypeDef *hgfxtim)
{
  EV1_event = 1;
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
static void MPU_AdjustRegionAddressSize(uint32_t Address, uint32_t Size, MPU_Region_InitTypeDef* pInit)
{
  /* Compute the MPU region size */
  pInit->Size = ((31 - __CLZ(Size)) - 1);
  if (Size > (1 << (pInit->Size + 1)))
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

/**
* @brief  DMA2D Transfer completed callback
* @param  hdma2d: DMA2D handle.
* @note   This example shows a simple way to report end of DMA2D transfer, and
*         you can add your own implementation.
* @retval None
*/
static void TransferComplete(DMA2D_HandleTypeDef *hdma2d)
{
  transferComplete = 1;
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
  /* Turn LD2 On */
  BSP_LED_On(LD2);
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
  BSP_LED_On(LD2);
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
