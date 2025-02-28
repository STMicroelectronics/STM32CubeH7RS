/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : stm32_extmem_conf.h
  * @version        : 1.0.0
  * @brief          : Header for extmem.c file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_EXTMEM_CONF__H__
#define __STM32_EXTMEM_CONF__H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/*
  @brief management of the driver layer enable
*/
#define EXTMEM_DRIVER_NOR_SFDP   1
#define EXTMEM_DRIVER_PSRAM      1
#define EXTMEM_DRIVER_SDCARD     0
#define EXTMEM_DRIVER_USER       0

/*
  @brief management of the sal layer enable
*/
#define EXTMEM_SAL_XSPI   1
#define EXTMEM_SAL_SD     0

/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_hal.h"
#include "stm32_extmem.h"
#include "stm32_extmem_type.h"
#include "boot/stm32_boot_xip.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */
/* Private variables ---------------------------------------------------------*/
extern XSPI_HandleTypeDef hxspi2;
extern XSPI_HandleTypeDef hxspi1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Exported constants --------------------------------------------------------*/
/** @defgroup EXTMEM_CONF_Exported_constants EXTMEM_CONF exported constante
  * @{
  */
enum {
  EXTMEMORY_1  = 0, /*!< ID=0 for the first external memory  */
  EXTMEMORY_2  = 1, /*!< ID=1 for the second external memory */
};

/*
  @brief management of the boot layer
*/
#define EXTMEM_MEMORY_BOOTXIP  EXTMEMORY_1

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Exported configuration --------------------------------------------------------*/
/** @defgroup EXTMEM_CONF_Exported_configuration EXTMEM_CONF exported configuration definition
  * @{
  */
extern EXTMEM_DefinitionTypeDef extmem_list_config[2];
#if defined(EXTMEM_C)
EXTMEM_DefinitionTypeDef extmem_list_config[2] =
{
  /* EXTMEMORY_1 */
  {
    .MemType = EXTMEM_NOR_SFDP,
    .Handle = (void*)&hxspi2,
    .ConfigType = EXTMEM_LINK_CONFIG_8LINES,
#if !defined ( __GNUC__ )
    .NorSfdpObject =
    {
      0u
    }
#endif /* __GNUC__ */
  }
  /* EXTMEMORY_2 */
  ,{
    .MemType = EXTMEM_PSRAM,
    .Handle = (void*)&hxspi1,
    .ConfigType = EXTMEM_LINK_CONFIG_16LINES,
    .PsramObject =
    {
      .psram_public = {
      .MemorySize = HAL_XSPI_SIZE_256MB,
      .FreqMax = 200 * 1000000u,
      .NumberOfConfig = 1u,
      /* Config */
      {
        {.WriteMask = 0x40u, .WriteValue = 0x40u, .REGAddress = 0x08u},
      },
      /* Memory command configuration */
      .ReadREG           = 0x40u,
      .WriteREG          = 0xC0u,
      .ReadREGSize       = 2u,
      .REG_DummyCycle    = 4u,
      .Write_command     = 0xA0u,
      .Write_DummyCycle  = 4u,
      .Read_command      = 0x20u,
      .WrapRead_command  = 0x00u,
      .Read_DummyCycle   = 4u,
      }
    }
  }
};
#endif /* EXTMEM_C */

/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN VARIABLES */

/* USER CODE END VARIABLES */

/*
 * -- Insert functions declaration here --
 */
/* USER CODE BEGIN FD */

/* USER CODE END FD */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_EXTMEM_CONF__H__ */
