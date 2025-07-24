/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    extmem_manager.c
  * @author  MCD Application Team
  * @brief   This file implements the extmem configuration
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
#include "extmem_manager.h"
#include <string.h>

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Init External memory manager
  * @retval None
  */
void MX_EXTMEM_MANAGER_Init(void)
{

  /* USER CODE BEGIN MX_EXTMEM_Init_PreTreatment */

  /* USER CODE END MX_EXTMEM_Init_PreTreatment */
  HAL_RCCEx_EnableClockProtection(RCC_CLOCKPROTECT_XSPI);

  /* Initialization of the memory parameters */
  memset(extmem_list_config, 0x0, sizeof(extmem_list_config));

  /* EXTMEMORY_1 */
  extmem_list_config[0].MemType = EXTMEM_NOR_SFDP;
  extmem_list_config[0].Handle = (void*)&hxspi2;
  extmem_list_config[0].ConfigType = EXTMEM_LINK_CONFIG_8LINES;

  /* EXTMEMORY_2 */
  extmem_list_config[1].MemType = EXTMEM_PSRAM;
  extmem_list_config[1].Handle = (void*)&hxspi1;
  extmem_list_config[1].ConfigType = EXTMEM_LINK_CONFIG_16LINES;

  extmem_list_config[1].PsramObject.psram_public.MemorySize = HAL_XSPI_SIZE_256MB;
  extmem_list_config[1].PsramObject.psram_public.FreqMax = 200 * 1000000u;

  /* Required configuration steps */
  extmem_list_config[1].PsramObject.psram_public.NumberOfConfig = 3u;
  extmem_list_config[1].PsramObject.psram_public.REG_DummyCycle = 6u;

  /* 1 - Configuration of X16 mode + 64 Byte/Word Hybrid Wrap in MR8 : value bx1xxx110 */
  extmem_list_config[1].PsramObject.psram_public.config[0].WriteMask  = 0x47u;
  extmem_list_config[1].PsramObject.psram_public.config[0].WriteValue = 0x46u;
  extmem_list_config[1].PsramObject.psram_public.config[0].REGAddress = 0x08u;

  /* 2 - Configuration of Write Latency codes in MR4 : value b001xxxxx => 7 cycles (as in AP specs, 6 in real) */
  extmem_list_config[1].PsramObject.psram_public.config[1].WriteMask  = 0xE0u;
  extmem_list_config[1].PsramObject.psram_public.config[1].WriteValue = 0x20u;
  extmem_list_config[1].PsramObject.psram_public.config[1].REGAddress = 0x04u;
  extmem_list_config[1].PsramObject.psram_public.Write_DummyCycle     = 6u;

  /* 3 - Configuration of Latency Type = FL + Read Latency codes in MR0 + Drive Strength 50 Ohms :
         value bxx110001 => 14 cycles (as in AP specs, 13 in real) */
  extmem_list_config[1].PsramObject.psram_public.config[2].WriteMask  = 0x3Fu;
  extmem_list_config[1].PsramObject.psram_public.config[2].WriteValue = 0x31u;
  extmem_list_config[1].PsramObject.psram_public.config[2].REGAddress = 0x00u;
  extmem_list_config[1].PsramObject.psram_public.Read_DummyCycle      = 13u;

  /* Memory command configuration */
  extmem_list_config[1].PsramObject.psram_public.ReadREG              = 0x40u;
  extmem_list_config[1].PsramObject.psram_public.WriteREG             = 0xC0u;
  extmem_list_config[1].PsramObject.psram_public.ReadREGSize          = 2u;
  extmem_list_config[1].PsramObject.psram_public.Write_command        = 0xA0u;
  extmem_list_config[1].PsramObject.psram_public.Read_command         = 0x20u;
  extmem_list_config[1].PsramObject.psram_public.WrapRead_command     = 0x00u;

  EXTMEM_Init(EXTMEMORY_1, HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_XSPI2));
  EXTMEM_Init(EXTMEMORY_2, HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_XSPI1));

  /* USER CODE BEGIN MX_EXTMEM_Init_PostTreatment */

  /* USER CODE END MX_EXTMEM_Init_PostTreatment */
}
