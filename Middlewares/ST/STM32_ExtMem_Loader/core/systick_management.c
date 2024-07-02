/**
  ******************************************************************************
  * @file    systick_management.c
  * @author  MCD Application Team
  * @brief   This file defines the systick timer management
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
/* Includes ------------------------------------------------------------------*/
#include "stm32_extmemloader_conf.h"

/** @defgroup EXTMEM_LOADER_CORE Core
  * @ingroup EXTMEM_LOADER
  * @{
  */

/** @defgroup EXTMEM_LOADER_SYSTICK Systick manager
  * @ingroup EXTMEM_LOADER_CORE
  * @{
  */

/** @defgroup EXTMEM_LOADER_SYSTICK_Exported_Functions Exported Functions
  * @{
  */
#if !defined(STM32_EXTMEMLOADER_STM32CUBEOPENBLTARGET)
/**
  * @brief This function configures the source of the time base:
  *        The time source is configured to have 1ms time base with a dedicated
  *        Tick interrupt priority.
  * @note The function is an override of the HAL function to have tick
  *       management functional without interrupt
  * @param TickPriority  Tick interrupt priority.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  HAL_StatusTypeDef retr = HAL_ERROR;
  /* Check uwTickFreq for MisraC 2012 (even if uwTickFreq is a enum type that doesn't take the value zero)*/
  if ((uint32_t)uwTickFreq != 0U)
  {
    uint32_t ticks = SystemCoreClock / (1000U / (uint32_t)uwTickFreq);
    SysTick->LOAD  = (uint32_t)(ticks - 1UL);      /* Set reload register */
    SysTick->VAL   = 0UL;                          /* Load the SysTick Counter Value */
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |  /* Set processor clock */
                     SysTick_CTRL_ENABLE_Msk;      /* Enable SysTick Timer */
    retr = HAL_OK;
  }
  return retr;
}
#endif
/**
  * @brief Provide a tick value in millisecond.
  * @note The function is an override of the HAL function to increment the
  *       tick on a count flag event.
  * @retval tick value
  */
uint32_t HAL_GetTick(void)
{
  if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == SysTick_CTRL_COUNTFLAG_Msk)
  {
    uwTick++;
  }
  return uwTick;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */