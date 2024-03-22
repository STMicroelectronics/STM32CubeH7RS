/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    I3C/I3C_Target_WakeUpFromStop/Boot/Src/stm32h7rsxx_it.c
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
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
#include "stm32h7rsxx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */

  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32H7RSxx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h7rsxx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles I3C1 wakeup global interrupt.
  */
void I3C1_WKUP_IRQHandler(void)
{
  /* USER CODE BEGIN I3C1_WKUP_IRQn 0 */
  /* Mask wakeup interrupt, as any access to I3C will generate
     a clock kernel request which generate a Wakeup interrupt
     Interrupt unmask will be done when necessary */
  LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_25);

  /* USER CODE END I3C1_WKUP_IRQn 0 */
  /* USER CODE BEGIN I3C1_WKUP_IRQn 1 */
  /* USER CODE END I3C1_WKUP_IRQn 1 */
}

/**
  * @brief This function handles I3C1 event interrupt.
  */
void I3C1_EV_IRQHandler(void)
{
  /* USER CODE BEGIN I3C1_EV_IRQn 0 */
  /* Check RX FIFO not empty flag in EVR register */
  if (LL_I3C_IsActiveFlag_RXFNE(I3C1))
  {
    /* Call the associated callback */
    Target_Reception_Callback();
  }

  /* Check Tx FIFO not full flag in EVR register */
  else if (LL_I3C_IsActiveFlag_TXFNF(I3C1))
  {
    /* Call the associated callback */
    Target_Transmit_Callback();
  }

  /* I3C wakeup flag event Check */
  else if (LL_I3C_IsActiveFlag_WKP(I3C1))
  {
    /* Clear wakeup flag flag */
    LL_I3C_ClearFlag_WKP(I3C1);
  }

  /* I3C frame complete event Check */
  else if (LL_I3C_IsActiveFlag_FC(I3C1))
  {
    /* Clear frame complete flag */
    LL_I3C_ClearFlag_FC(I3C1);

    /* Call the associated callback */
    Target_Complete_Callback();
  }
  else
  {
    /* Call the associated callback */
    Target_Event_Treatment_Callback();
  }
  /* USER CODE END I3C1_EV_IRQn 0 */

  /* USER CODE BEGIN I3C1_EV_IRQn 1 */

  /* USER CODE END I3C1_EV_IRQn 1 */
}

/**
  * @brief This function handles I3C1 error interrupt.
  */
void I3C1_ER_IRQHandler(void)
{
  /* USER CODE BEGIN I3C1_ER_IRQn 0 */
  /* Call the associated callback */
  Error_Handler();
  /* USER CODE END I3C1_ER_IRQn 0 */

  /* USER CODE BEGIN I3C1_ER_IRQn 1 */

  /* USER CODE END I3C1_ER_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
