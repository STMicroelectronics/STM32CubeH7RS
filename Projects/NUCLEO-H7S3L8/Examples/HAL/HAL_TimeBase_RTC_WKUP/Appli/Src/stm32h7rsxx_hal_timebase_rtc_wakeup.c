/**
  ******************************************************************************
  * @file    stm32h7rsxx_hal_timebase_rtc_wakeup.c
  * @author  MCD Application Team
  * @brief   HAL time base based on the hardware RTC_WAKEUP.
  *
  *          This file overrides the native HAL time base functions (defined as weak)
  *          to use the RTC WAKEUP for the time base generation:
  *           + Initializes the RTC peripheral and configures the wakeup timer to be
  *             incremented each 1ms
  *           + The wakeup feature is configured to assert an interrupt each 1ms
  *           + HAL_IncTick is called inside the HAL_RTCEx_WakeUpTimerEventCallback
  *           + HSE (default), LSE or LSI can be selected as RTC clock source
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
    [..]
    This file must be copied to the application folder and modified as follows:
    (#) Rename it to 'stm32h7rsxx_hal_timebase_rtc_wakeup.c'
    (#) Add this file and the RTC HAL drivers to your project and uncomment
       HAL_RTC_MODULE_ENABLED define in stm32h7rsxx_hal_conf.h

    [..]
    (@) HAL RTC alarm and HAL RTC wakeup drivers can not be used with low power modes:
        The wake up capability of the RTC may be intrusive in case of prior low power mode
        configuration requiring different wake up sources.
        Application/Example behavior is no more guaranteed
    (@) The stm32h7rsxx_hal_timebase_tim use is recommended for the Applications/Examples
          requiring low power modes
  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_hal.h"

/** @addtogroup STM32H7RSxx_HAL_Driver
  * @{
  */

/** @defgroup HAL_TimeBase_RTC_WakeUp  HAL TimeBase RTC WakeUp
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Uncomment the line below to select the appropriate RTC Clock source for your application:
  + RTC_CLOCK_SOURCE_HSE: can be selected for applications requiring timing precision.
  + RTC_CLOCK_SOURCE_LSE: can be selected for applications with low constraint on timing
                          precision.
  + RTC_CLOCK_SOURCE_LSI: can be selected for applications with low constraint on timing
                          precision.
  */
//#define RTC_CLOCK_SOURCE_HSE
#define RTC_CLOCK_SOURCE_LSE
/* #define RTC_CLOCK_SOURCE_LSE */
/* #define RTC_CLOCK_SOURCE_LSI */

/* The time base should be 1ms
   Time base = ((RTC_ASYNCH_PREDIV + 1) * (RTC_SYNCH_PREDIV + 1)) / RTC_CLOCK
    HSE as RTC clock
     Time base = ((99 + 1) * (4 + 1)) / 500kHz
               = 1ms
    LSE as RTC clock
     Time base = ((32 + 1) * (0 + 1)) / 32.768kHz
               = ~1ms
    LSI as RTC clock
     Time base = ((31 + 1) * (0 + 1)) / 32kHz
               = 1ms
*/
#if defined (RTC_CLOCK_SOURCE_HSE)
#define RTC_ASYNCH_PREDIV      99U
#define RTC_SYNCH_PREDIV        4U
#elif defined (RTC_CLOCK_SOURCE_LSE)
#define RTC_ASYNCH_PREDIV       0U
#define RTC_SYNCH_PREDIV       32U
#elif defined (RTC_CLOCK_SOURCE_LSI)
#define RTC_ASYNCH_PREDIV       0U
#define RTC_SYNCH_PREDIV       31U
#else
#error Please select the RTC Clock source
#endif /* RTC_CLOCK_SOURCE_HSE */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static RTC_HandleTypeDef        hRTC_Handle;

/* Private function prototypes -----------------------------------------------*/
void RTC_IRQHandler(void);
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1U)
void TimeBase_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc);
#endif /* USE_HAL_RTC_REGISTER_CALLBACKS */
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function configures the RTC wakeup timer as a time base source.
  *         The time source is configured to have 1ms time base with a dedicated
  *         Tick interrupt priority.
  * @note   This function is called  automatically at the beginning of program after
  *         reset by HAL_Init() or at any time when clock is configured, by HAL_RCC_ClockConfig().
  * @param  TickPriority Tick interrupt priority.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  HAL_StatusTypeDef  Status;

  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  /* Disable bkup domain protection */
  HAL_PWR_EnableBkUpAccess();

  /* Force and Release the Backup domain reset */
  __HAL_RCC_BACKUPRESET_FORCE();
  __HAL_RCC_BACKUPRESET_RELEASE();

  /* Enable RTC Clock */
  __HAL_RCC_RTC_ENABLE();
  __HAL_RCC_RTCAPB_CLK_ENABLE();

#if defined (RTC_CLOCK_SOURCE_LSE)
  /* Configure LSE as RTC clock source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_NONE;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
#elif defined (RTC_CLOCK_SOURCE_LSI)
  /* Configure LSI as RTC clock source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_NONE;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
#elif defined (RTC_CLOCK_SOURCE_HSE)
  /* Configure HSE as RTC clock source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_NONE;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_HSE_DIV48;
#else
#error Please select the RTC Clock source
#endif /* RTC_CLOCK_SOURCE_LSE */

  Status = HAL_RCC_OscConfig(&RCC_OscInitStruct);

  if (Status == HAL_OK)
  {
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    Status = HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
  }

  if (Status == HAL_OK)
  {
    hRTC_Handle.Instance = RTC;
    hRTC_Handle.Init.HourFormat     = RTC_HOURFORMAT_24;
    hRTC_Handle.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV;
    hRTC_Handle.Init.SynchPrediv    = RTC_SYNCH_PREDIV;
    hRTC_Handle.Init.OutPut         = RTC_OUTPUT_DISABLE;
    hRTC_Handle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hRTC_Handle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
    hRTC_Handle.Init.BinMode        = RTC_BINARY_NONE;

    Status = HAL_RTC_Init(&hRTC_Handle);

#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1U)
    HAL_RTC_RegisterCallback(&hRTC_Handle, HAL_RTC_WAKEUPTIMER_EVENT_CB_ID, TimeBase_RTCEx_WakeUpTimerEventCallback);
#endif /* USE_HAL_RTC_REGISTER_CALLBACKS */
  }

  if (Status == HAL_OK)
  {
    uint32_t WakeUpCounter = ((uint32_t)uwTickFreq / (uint32_t)HAL_TICK_FREQ_1KHZ) - 1U;
    Status = HAL_RTCEx_SetWakeUpTimer_IT(&hRTC_Handle, WakeUpCounter, RTC_WAKEUPCLOCK_CK_SPRE_16BITS, 0);
  }

  if (TickPriority < (1UL << __NVIC_PRIO_BITS))
  {
    /* Enable the RTC global Interrupt */
    HAL_NVIC_SetPriority(RTC_IRQn, TickPriority, 0U);
    uwTickPrio = TickPriority;
  }
  else
  {
    Status = HAL_ERROR;
  }

  HAL_NVIC_EnableIRQ(RTC_IRQn);

  return Status;
}

/**
  * @brief  Suspend Tick increment.
  * @note   Disable the tick increment by disabling RTC_WKUP interrupt.
  * @retval None
  */
void HAL_SuspendTick(void)
{
  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(&hRTC_Handle);
  /* Disable RTC WAKE UP TIMER Interrupt */
  __HAL_RTC_WAKEUPTIMER_DISABLE_IT(&hRTC_Handle, RTC_IT_WUT);
  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(&hRTC_Handle);
}

/**
  * @brief  Resume Tick increment.
  * @note   Enable the tick increment by enabling RTC_WKUP interrupt.
  * @retval None
  */
void HAL_ResumeTick(void)
{
  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(&hRTC_Handle);
  /* Enable RTC WAKE UP TIMER  interrupt */
  __HAL_RTC_WAKEUPTIMER_ENABLE_IT(&hRTC_Handle, RTC_IT_WUT);
  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(&hRTC_Handle);
}

/**
  * @brief  Wake Up Timer Event Callback in non blocking mode
  * @note   This function is called  when RTC_WKUP interrupt took place, inside
  * RTC_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  hrtc RTC handle
  * @retval None
  */
#if (USE_HAL_RTC_REGISTER_CALLBACKS == 1U)
void TimeBase_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
#else
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
#endif /* USE_HAL_RTC_REGISTER_CALLBACKS == 1U */
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hrtc);

  HAL_IncTick();
}

/**
  * @brief  This function handles RTC WAKE UP TIMER interrupt request.
  * @retval None
  */
void RTC_IRQHandler(void)
{
  HAL_RTCEx_WakeUpTimerIRQHandler(&hRTC_Handle);
}

/**
  * @}
  */

/**
  * @}
  */
