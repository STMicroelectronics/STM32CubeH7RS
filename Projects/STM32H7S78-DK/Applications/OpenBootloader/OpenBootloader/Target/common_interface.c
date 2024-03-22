/**
  ******************************************************************************
  * @file    common_interface.c
  * @author  MCD Application Team
  * @brief   Contains common functions used by different interfaces
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
#include "platform.h"
#include "common_interface.h"
#include "flash_interface.h"
#include "openbootloader_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static Function_Pointer ResetCallback;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Assigns the given value to the Main Stack Pointer (MSP).
  * @param  top_of_main_stack Main Stack Pointer value to set.
  * @retval None.
  */
void Common_SetMsp(uint32_t top_of_main_stack)
{
  __set_MSP(top_of_main_stack);
}

/**
  * @brief  Enable IRQ Interrupts.
  * @retval None.
  */
void Common_EnableIrq(void)
{
  __enable_irq();
}

/**
  * @brief  Disable IRQ Interrupts.
  * @retval None.
  */
void Common_DisableIrq(void)
{
  __disable_irq();
}

/**
  * @brief  Checks whether the target Protection Status is set or not.
  * @retval Returns RESET protection is disabled.
  */
FlagStatus Common_GetProtectionStatus(void)
{
  /* NOTE: Always return reset as all commands are allowed when protection is set */
  return RESET;
}

/**
  * @brief  Register a callback function to be called at the end of commands processing.
  * @retval None.
  */
void Common_SetPostProcessingCallback(Function_Pointer callback)
{
  ResetCallback = callback;
}

/**
  * @brief  Start post processing task.
  * @retval None.
  */
void Common_StartPostProcessing()
{
  if (ResetCallback != NULL)
  {
    ResetCallback();

    /* In case there is no system reset, we must reset the callback */
    ResetCallback = NULL;
  }
}
