/**
  ******************************************************************************
  * @file    low_level_ramecc.c
  * @author  MCD Application Team
  * @brief   Low Level Interface module to use STM32 RAMECC IP
  *
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
#include "low_level_ramecc.h"
#include "low_level_security.h"
#include "boot_hal_flowcontrol.h"

/* Private defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Place data in a specific section*/
#if defined(__ICCARM__)
#pragma default_variable_attributes = @ ".ramecchandle"
#elif defined(__ARMCC_VERSION)
__attribute__((section(".bss.ramecchandle")))
#elif defined(__GNUC__)
__attribute__((section(".ramecchandle")))
#endif /* __ICCARM__ */
RAMECC_HandleTypeDef ahRamecc[5];
/* Stop placing data in specified section*/
#if defined(__ICCARM__)
#pragma default_variable_attributes =
#endif /* __ICCARM__ */
/* Private function prototypes -----------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Configure RAMECC IP
  * @retval None
  */
void LL_RAMECC_Configure(void)
{
#if defined(OEMIROT_USER_SRAM_ECC)
  uint32_t i;
  uint32_t prioritygroup;

  if (uFlowStage == FLOW_STAGE_CFG)
  {
    if (READ_BIT(FLASH->OBW2SR, FLASH_OBW2SRP_ECC_ON_SRAM_Msk) != FLASH_OBW2SRP_ECC_ON_SRAM)
    {
      Error_Handler();
    }

    /* ahRamecc shall be initialized after enabling AHB SRAM2 clock */
    ahRamecc[0].Instance = RAMECC1_Monitor0; /* Monitor AXI SRAM1 ECC */
    ahRamecc[1].Instance = RAMECC1_Monitor2; /* Monitor AXI DTCM0 ECC */
    ahRamecc[2].Instance = RAMECC1_Monitor3; /* Monitor AXI DTCM1 ECC */
    ahRamecc[3].Instance = RAMECC1_Monitor4; /* Monitor AXI SRAM3 ECC */
    ahRamecc[4].Instance = RAMECC2_Monitor1; /* Monitor BKPRAM ECC */

    /* Configure ECC_FPU_IRQn interrupt */
    prioritygroup = NVIC_GetPriorityGrouping();
    NVIC_SetPriority(RAMECC_IRQn, NVIC_EncodePriority(prioritygroup, 0x0U, 0x0U));
    NVIC_EnableIRQ(RAMECC_IRQn);

    for (i = 0U; i < ARRAY_SIZE(ahRamecc); i++)
    {
      if (HAL_RAMECC_DeInit(&ahRamecc[i]) != HAL_OK)
      {
        Error_Handler();
      }

      if (HAL_RAMECC_Init(&ahRamecc[i]) != HAL_OK)
      {
        Error_Handler();
      }

      if (HAL_RAMECC_StartMonitor(&ahRamecc[i]) != HAL_OK)
      {
        Error_Handler();
      }

      if (HAL_RAMECC_EnableNotification(&ahRamecc[i],
                                        RAMECC_IT_MONITOR_DOUBLEERR_R) != HAL_OK)
      {
        Error_Handler();
      }

      if (HAL_RAMECC_EnableNotification(&ahRamecc[i],
                                        RAMECC_IT_MONITOR_DOUBLEERR_W) != HAL_OK)
      {
        Error_Handler();
      }
    }

    if (i != ARRAY_SIZE(ahRamecc))
    {
      Error_Handler();
    }

    /* Flow control */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_RAMECC_EN, FLOW_CTRL_RAMECC_EN);
  }
  else
  {
    if (READ_BIT(FLASH->OBW2SR, FLASH_OBW2SRP_ECC_ON_SRAM_Msk) != FLASH_OBW2SRP_ECC_ON_SRAM)
    {
      Error_Handler();
    }

    /* Check that IRQ is enabled */
    if (NVIC_GetEnableIRQ(RAMECC_IRQn) != 0x1U)
    {
      Error_Handler();
    }

    /* Check RAMECC configuration to detect ECC errors on DTCM, AXISRAM1 and AXISRAM3 */
    for (i = 0U; i < ARRAY_SIZE(ahRamecc); i++)
    {
      /* Check that RAMECC is enabled for this index */
      if ((ahRamecc[i].Instance->CR & RAMECC_CR_ECCELEN_Msk) != RAMECC_CR_ECCELEN)
      {
        Error_Handler();
      }

      /* Check that there is no error for this index */
      if (ahRamecc[i].ErrorCode != HAL_RAMECC_ERROR_NONE)
      {
        Error_Handler();
      }

      /* Check that the notifications are enabled for this index */
      if (__HAL_RAMECC_GET_MONITOR_IT_SOURCE(&ahRamecc[i], RAMECC_IT_MONITOR_DOUBLEERR_R) != SET)
      {
        Error_Handler();
      }
      if (__HAL_RAMECC_GET_MONITOR_IT_SOURCE(&ahRamecc[i], RAMECC_IT_MONITOR_DOUBLEERR_W) != SET)
      {
        Error_Handler();
      }
    }

    if (i != ARRAY_SIZE(ahRamecc))
    {
      Error_Handler();
    }
    /* Flow control */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_RAMECC_CH, FLOW_CTRL_RAMECC_CH);
  }
#endif /* OEMIROT_USER_SRAM_ECC */
}

/**
  * @brief  DeInit RAMECC IP
  * @param  None
  * @retval None
  */
void LL_RAMECC_DeInit(void)
{
#if defined(OEMIROT_USER_SRAM_ECC)
  uint8_t i = 0U;

  if (READ_BIT(FLASH->OBW2SR, FLASH_OBW2SRP_ECC_ON_SRAM_Msk) == FLASH_OBW2SRP_ECC_ON_SRAM)
  {
    /* Disable ECC IRQ */
    NVIC_DisableIRQ(RAMECC_IRQn);

    /* Disable RAMECC monitor */
    for (i = 0U; i < ARRAY_SIZE(ahRamecc); i++)
    {
      ahRamecc[i].Instance->CR &= ~RAMECC_CR_ECCELEN;
    }
  }
#endif /* OEMIROT_USER_SRAM_ECC */
}
