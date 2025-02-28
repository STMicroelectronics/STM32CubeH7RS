/**
  ******************************************************************************
  * @file    stm32h7rsxx_hal_timebase_tim.c
  * @author  MCD Application Team
  * @brief   Template for HAL time base based on the peripheral hardware TIM17.
  *
  *          This file override the native HAL time base functions (defined as weak)
  *          the TIM time base:
  *           + Initializes the TIM17 peripheral to generate a Period elapsed Event each 1ms
  *           + HAL_IncTick is called inside HAL_TIM_PeriodElapsedCallback ie each 1ms
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_hal.h"

/** @addtogroup STM32H7RSxx_HAL_Driver
  * @{
  */

/** @addtogroup HAL_TimeBase
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIM_CNT_FREQ 1000000U /* Timer counter frequency : 1 MHz */
#define TIM_FREQ     1000U      /* Timer frequency : 1 kHz => to have 1 ms interrupt */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim17;

/* Private function prototypes -----------------------------------------------*/
#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1U)
void TimeBase_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
#endif /* USE_HAL_TIM_REGISTER_CALLBACKS */
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function configures the TIM17 as a time base source.
  *         The time source is configured  to have 1ms time base with a dedicated
  *         Tick interrupt priority.
  * @note   This function is called  automatically at the beginning of program after
  *         reset by HAL_Init() or at any time when clock is configured, by HAL_RCC_ClockConfig().
  * @param  TickPriority Tick interrupt priority.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  RCC_ClkInitTypeDef    clkconfig;
  uint32_t              uwTimclock;
  uint32_t              uwAPB2Prescaler;
  uint32_t              uwPrescalerValue;
  uint32_t              pFLatency;
  HAL_StatusTypeDef     Status;

  /* Enable TIM17 clock */
  __HAL_RCC_TIM17_CLK_ENABLE();

  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

  /* Get APB2 prescaler */
  uwAPB2Prescaler = clkconfig.APB2CLKDivider;

  /* Compute TIM17 clock */
  if (uwAPB2Prescaler == RCC_APB2_DIV1)
  {
    uwTimclock = HAL_RCC_GetPCLK2Freq();
  }
  else if (uwAPB2Prescaler == RCC_APB2_DIV2)
  {
    uwTimclock = 2UL * HAL_RCC_GetPCLK2Freq();
  }
  else
  {
    if (__HAL_RCC_GET_TIMCLKPRESCALER() == RCC_TIMPRES_DISABLE)
    {
      uwTimclock = 2UL * HAL_RCC_GetPCLK2Freq();
    }
    else
    {
      uwTimclock = 4UL * HAL_RCC_GetPCLK2Freq();
    }
  }

  /* Compute the prescaler value to have TIM17 counter clock equal to TIM_CNT_FREQ */
  uwPrescalerValue = (uint32_t)((uwTimclock / TIM_CNT_FREQ) - 1U);

  /* Initialize TIM17 */
  htim17.Instance = TIM17;

  /* Initialize TIMx peripheral as follow:
  + Period = [uwTickFreq * (TIM_CNT_FREQ/TIM_FREQ) - 1]. to have a (uwTickFreq/TIM_FREQ) s time base.
  + Prescaler = (uwTimclock/TIM_CNT_FREQ - 1) to have a TIM_CNT_FREQ counter clock.
  + ClockDivision = 0
  + Counter direction = Up
  */
  htim17.Init.Period = ((uint32_t)uwTickFreq * (TIM_CNT_FREQ / TIM_FREQ)) - 1U;
  htim17.Init.Prescaler = uwPrescalerValue;
  htim17.Init.ClockDivision = 0;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  Status = HAL_TIM_Base_Init(&htim17);
  if (Status == HAL_OK)
  {
#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1U)
  HAL_TIM_RegisterCallback(&htim17, HAL_TIM_PERIOD_ELAPSED_CB_ID, TimeBase_TIM_PeriodElapsedCallback);
#endif /* USE_HAL_TIM_REGISTER_CALLBACKS */

    /* Start the TIM time Base generation in interrupt mode */
    Status = HAL_TIM_Base_Start_IT(&htim17);
    if (Status == HAL_OK)
    {
      if (TickPriority < (1UL << __NVIC_PRIO_BITS))
      {
        /* Configure the TIM17 global Interrupt priority */
        HAL_NVIC_SetPriority(TIM17_IRQn, TickPriority, 0);

        /* Enable the TIM17 global Interrupt */
        HAL_NVIC_EnableIRQ(TIM17_IRQn);

        uwTickPrio = TickPriority;
      }
      else
      {
        Status = HAL_ERROR;
      }
    }
  }

  /* Return function status */
  return Status;
}

/**
  * @brief  Suspend Tick increment.
  * @note   Disable the tick increment by disabling TIM17 update interrupt.
  * @retval None
  */
void HAL_SuspendTick(void)
{
  /* Disable TIM17 update interrupt */
  __HAL_TIM_DISABLE_IT(&htim17, TIM_IT_UPDATE);
}

/**
  * @brief  Resume Tick increment.
  * @note   Enable the tick increment by enabling TIM17 update interrupt.
  * @retval None
  */
void HAL_ResumeTick(void)
{
  /* Enable TIM17 update interrupt */
  __HAL_TIM_ENABLE_IT(&htim17, TIM_IT_UPDATE);
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM17 interrupt took place, inside
  * HAL_TIM17_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim TIM handle
  * @retval None
  */
#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1U)
void TimeBase_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  HAL_IncTick();
}
#endif /* USE_HAL_TIM_REGISTER_CALLBACKS */
/**
  * @}
  */

/**
  * @}
  */
