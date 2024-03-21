/**
  ******************************************************************************
  * @file    low_level_ext_ram.c
  * @author  MCD Application Team
  * @brief   Low Level Interface module to access the XSPI external RAM
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
#include "low_level_ext_ram.h"
#include "low_level_security.h"
#include "boot_hal_flowcontrol.h"
#include "flash_layout.h"
#include "stm32_extmem_conf.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
XSPI_HandleTypeDef XSPI_RUN_HANDLE;
/* Private function prototypes -----------------------------------------------*/
#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
uint32_t RCCEx_GetXspiCLKFreq(uint32_t PeriphClk);
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */
/* Functions Definition ------------------------------------------------------*/
#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)

/**
  * @brief  Return the peripheral clock frequency for a given peripheral
  * @note   Return 0 if peripheral clock identifier not managed by this API or
  *         if the selected clock source is not enabled (HSI, PLLs clock output..)
  * @param  PeriphClk: Peripheral clock identifier
  *         This parameter can be one of the following values:
  *            @arg RCC_PERIPHCLK_XSPI1       : Xspi1 peripheral clock
  *            @arg RCC_PERIPHCLK_XSPI2       : Xspi2 peripheral clock
  * @retval Frequency in KHz
  */
__weak uint32_t RCCEx_GetXspiCLKFreq(uint32_t PeriphClk)
{
  while (1U);
}

/**
  * @brief  @brief  Configure "HSLV" for the external ram support
  * @param  None
  * @retval HAL status
  */
static HAL_StatusTypeDef Ext_Ram_SetHslv(void)
{
  static FLASH_OBProgramInitTypeDef flash_option_bytes = {0};
  uint32_t user_config = OB_XSPI1_HSLV_ENABLE | OB_XSPI2_HSLV_ENABLE;

  /* Set the OB needed for "XSPI High-speed at low voltage" */
  /* Both XSPI must be configured when XSPIM is used */
  HAL_FLASHEx_OBGetConfig(&flash_option_bytes);

  if ((flash_option_bytes.USERConfig1 & user_config) != user_config)
  {
    if (HAL_FLASH_Unlock() != HAL_OK)
    {
      return HAL_ERROR;
    }
    if (HAL_FLASH_OB_Unlock() != HAL_OK)
    {
      return HAL_ERROR;
    }
    flash_option_bytes.OptionType = OPTIONBYTE_USER;
    flash_option_bytes.USERType = OB_USER_XSPI1_HSLV | OB_USER_XSPI2_HSLV;
    flash_option_bytes.USERConfig1 = user_config;
    if (HAL_FLASHEx_OBProgram(&flash_option_bytes) != HAL_OK)
    {
      return HAL_ERROR;
    }
    if (HAL_FLASH_OB_Lock() != HAL_OK)
    {
      return HAL_ERROR;
    }
    if (HAL_FLASH_Lock() != HAL_OK)
    {
      return HAL_ERROR;
    }
  }

  /* Configure "high speed low voltage" */
  /* Both XSPI must be configured when XSPIM is used */
  HAL_SBS_EnableIOSpeedOptimize(SBS_IO_XSPI1_HSLV);
  HAL_SBS_EnableIOSpeedOptimize(SBS_IO_XSPI2_HSLV);

  return HAL_OK;
}
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

/**
  * @brief Initialize the external RAM
  * @retval HAL status
  */
HAL_StatusTypeDef Ext_Ram_Initialize(void)
{
#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
  HAL_StatusTypeDef status;
  XSPIM_CfgTypeDef xspi_manager_cfg = {0};

  status = Ext_Ram_SetHslv();
  if (status != HAL_OK)
  {
    return status;
  }

  XSPI_RUN_HANDLE.Instance = EXT_RAM_XSPI_INSTANCE;

  XSPI_RUN_HANDLE.Init.ChipSelectBoundary      = HAL_XSPI_BONDARYOF_8KB;
  XSPI_RUN_HANDLE.Init.ChipSelectHighTimeCycle = 1U;
  XSPI_RUN_HANDLE.Init.ClockMode               = HAL_XSPI_CLOCK_MODE_0;
  XSPI_RUN_HANDLE.Init.ClockPrescaler          = 0U;
  XSPI_RUN_HANDLE.Init.DelayHoldQuarterCycle   = HAL_XSPI_DHQC_ENABLE;
  XSPI_RUN_HANDLE.Init.FifoThresholdByte       = 2U;
  XSPI_RUN_HANDLE.Init.FreeRunningClock        = HAL_XSPI_FREERUNCLK_DISABLE;
  XSPI_RUN_HANDLE.Init.MemoryMode              = HAL_XSPI_SINGLE_MEM;
  XSPI_RUN_HANDLE.Init.MemorySize              = HAL_XSPI_SIZE_32GB;
  XSPI_RUN_HANDLE.Init.MemoryType              = HAL_XSPI_MEMTYPE_APMEM_16BITS;
  XSPI_RUN_HANDLE.Init.SampleShifting          = HAL_XSPI_SAMPLE_SHIFT_NONE;
  XSPI_RUN_HANDLE.Init.WrapSize                = HAL_XSPI_WRAP_NOT_SUPPORTED;
  XSPI_RUN_HANDLE.Init.MaxTran                 = 0U;
  XSPI_RUN_HANDLE.Init.Refresh                 = 0U;
  XSPI_RUN_HANDLE.Init.MemorySelect            = HAL_XSPI_CSSEL_NCS1;
  status = HAL_XSPI_Init(&XSPI_RUN_HANDLE);
  if (status != HAL_OK)
  {
    return status;
  }

  /* Connect Instance2(XSPI2/MCE2) to port1 */
  xspi_manager_cfg.IOPort = HAL_XSPIM_IOPORT_1;
  xspi_manager_cfg.nCSOverride = HAL_XSPI_CSSEL_OVR_DISABLED;
  if (HAL_XSPIM_Config(&XSPI_RUN_HANDLE, &xspi_manager_cfg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Initialize the EXTMEM */
  if (EXT_RAM_XSPI_INSTANCE == XSPI1)
  {
    if (EXTMEM_Init(MEMORY_PSRAM_0, RCCEx_GetXspiCLKFreq(RCC_PERIPHCLK_XSPI1)) != EXTMEM_OK)
    {
      return HAL_ERROR;
    }
  }
  else if (EXT_RAM_XSPI_INSTANCE == XSPI2)
  {
    if (EXTMEM_Init(MEMORY_PSRAM_0, RCCEx_GetXspiCLKFreq(RCC_PERIPHCLK_XSPI2)) != EXTMEM_OK)
    {
      return HAL_ERROR;
    }
  }
  else
  {
    return HAL_ERROR;
  }

  /* Enable default memory mapped mode */
  if (EXTMEM_MemoryMappedMode(MEMORY_PSRAM_0, EXTMEM_ENABLE) != EXTMEM_OK)
  {
    return HAL_ERROR;
  }
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

  return HAL_OK;
}

/**
  * @brief DeInitialize the external RAM
  * @retval HAL status
  */
HAL_StatusTypeDef Ext_Ram_Uninitialize(void)
{
#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
  /* De-initialize the EXTMEM */
  if (EXTMEM_DeInit(MEMORY_PSRAM_0) != EXTMEM_OK)
  {
    return HAL_ERROR;
  }

  return HAL_XSPI_DeInit(&XSPI_RUN_HANDLE);
#else /* OEMIROT_LOAD_AND_RUN != LOAD_AND_RUN_EXT_RAM */
  return HAL_OK;
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */
}
