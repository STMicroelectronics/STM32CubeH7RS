/**
  ******************************************************************************
  * @file    stm32_psram_driver.c
  * @author  MCD Application Team
  * @brief   This file includes a driver for psram support
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
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32_extmem.h"
#include "stm32_extmem_conf.h"
#if EXTMEM_DRIVER_PSRAM == 1
#if EXTMEM_SAL_XSPI == 1
#include "../sal/stm32_sal_xspi_api.h"
#else
#error "the driver PSRAM requires the enable of EXTMEM_SAL_XSPI"
#endif /* EXTMEM_SAL_XSPI */
#include "stm32_psram_driver_api.h"
#if EXTMEM_DRIVER_PSRAM_DEBUG_LEVEL > 0 && defined(EXTMEM_MACRO_DEBUG)
#include <stdio.h>
#endif /* #if EXTMEM_DRIVER_PSRAM_DEBUG_LEVEL > 0 && defined(EXTMEM_MACRO_DEBUG) */

/** @defgroup PSRAM PSRAM driver
  * @ingroup EXTMEM_DRIVER
  * @{
  */

/* Private Macro ------------------------------------------------------------*/
/** @defgroup PSRAM_Private_Macro Private Macro
  * @{
  */

/**
 * @brief default timeout
 */
#define DRIVER_DEFAULT_TIMEOUT 300

#if EXTMEM_DRIVER_PSRAM_DEBUG_LEVEL > 0 && defined(EXTMEM_MACRO_DEBUG)
/**
 * @brief debug macro for a string
 */
#define DEBUG_STR(_STR_)  {                      \
           EXTMEM_MACRO_DEBUG("\tPSRAM::");  \
           EXTMEM_MACRO_DEBUG(_STR_);        \
           EXTMEM_MACRO_DEBUG("\n\r");       \
           }
#else
/**
 * @brief debug macro for a string
 */
#define DEBUG_STR(_STR_)
#endif /* EXTMEM_DRIVER_PSRAM_DEBUG_LEVEL > 0 && defined(EXTMEM_MACRO_DEBUG) */

/**
  * @}
  */

/* Private typedefs ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup PSRAM_Private_Functions Private Functions
  * @{
  */
EXTMEM_DRIVER_PSRAM_StatusTypeDef PSRAM_ExecuteCommand(EXTMEM_DRIVER_PSRAM_ObjectTypeDef *PsramObject, uint8_t Index);
/**
  * @}
  */

/** @defgroup PSRAM_Exported_Functions Exported Functions
  * @{
  */

EXTMEM_DRIVER_PSRAM_StatusTypeDef EXTMEM_DRIVER_PSRAM_Init(void *IP, EXTMEM_LinkConfig_TypeDef Config,
                                                           uint32_t ClockInput,
                                                           EXTMEM_DRIVER_PSRAM_ObjectTypeDef *PsramObject)
{
  EXTMEM_DRIVER_PSRAM_StatusTypeDef retr = EXTMEM_DRIVER_PSRAM_OK;
  uint32_t ClockOut;
  SAL_XSPI_PhysicalLinkTypeDef linkvalue;

  /* initialize the instance */
  DEBUG_STR("initialize the instance")
  
  /* Initialize XSPI low layer */
  (void)SAL_XSPI_Init(&PsramObject->psram_private.SALObject, IP);

  /* Abort any ongoing XSPI action */
  (void)SAL_XSPI_DisableMapMode(&PsramObject->psram_private.SALObject);

  /* Set the frequency prescaler */
  DEBUG_STR("set memory speed according freqIn and freqMax supported by the memory")
  if (HAL_OK != SAL_XSPI_SetClock(&PsramObject->psram_private.SALObject, ClockInput, PsramObject->psram_public.FreqMax, &ClockOut))
  {
    retr = EXTMEM_DRIVER_PSRAM_ERROR;
    goto error;
  }

  /* Set the memory size */
  DEBUG_STR("set memory size according")
  (void)SAL_XSPI_MemoryConfig(&PsramObject->psram_private.SALObject, PARAM_FLASHSIZE, &PsramObject->psram_public.MemorySize);

  /* Set the memory size */
  DEBUG_STR("set xspi link config")
  linkvalue = PHY_LINK_RAM8;
  (void)SAL_XSPI_MemoryConfig(&PsramObject->psram_private.SALObject, PARAM_PHY_LINK, &linkvalue);

  /* Set the configuration to perform register operation */
  (void)SAL_XSPI_MemoryConfig(&PsramObject->psram_private.SALObject, PARAM_DUMMY_CYCLES, &PsramObject->psram_public.REG_DummyCycle);

  /* Execute the command sequence */
  for (uint8_t command_index = 0u; command_index < PsramObject->psram_public.NumberOfConfig; command_index++)
  {
    retr = PSRAM_ExecuteCommand(PsramObject, command_index);
    if (retr != EXTMEM_DRIVER_PSRAM_OK)
    {
      goto error;
    }
  }

  switch(Config)
  {
  case EXTMEM_LINK_CONFIG_16LINES:
    linkvalue = PHY_LINK_RAM16;
    (void)SAL_XSPI_MemoryConfig(&PsramObject->psram_private.SALObject, PARAM_PHY_LINK, &linkvalue);
    break;

  case EXTMEM_LINK_CONFIG_8LINES:
  default:
    retr = EXTMEM_DRIVER_PSRAM_ERROR;
    goto error;
    break;
  }

error:
  return retr;
}

EXTMEM_DRIVER_PSRAM_StatusTypeDef EXTMEM_DRIVER_PSRAM_DeInit(EXTMEM_DRIVER_PSRAM_ObjectTypeDef *PsramObject)
{
  /* Abort any ongoing XSPI action */
  (void)SAL_XSPI_DisableMapMode(&PsramObject->psram_private.SALObject);
  return EXTMEM_DRIVER_PSRAM_OK;
}

EXTMEM_DRIVER_PSRAM_StatusTypeDef EXTMEM_DRIVER_PSRAM_Enable_MemoryMappedMode(EXTMEM_DRIVER_PSRAM_ObjectTypeDef *PsramObject)
{
  EXTMEM_DRIVER_PSRAM_StatusTypeDef retr = EXTMEM_DRIVER_PSRAM_OK;

  /* configure the read wrap mode */
  if (HAL_OK != SAL_XSPI_ConfigureWrappMode(&PsramObject->psram_private.SALObject, 
                                            PsramObject->psram_public.WrapRead_command, 
                                            PsramObject->psram_public.Write_DummyCycle))
  {
    retr = EXTMEM_DRIVER_PSRAM_ERROR_MAP_ENABLE;
  }
   
  /* launch the memory mapped mode */
  if (HAL_OK != SAL_XSPI_EnableMapMode(&PsramObject->psram_private.SALObject, 
                                       PsramObject->psram_public.Read_command, 
                                       PsramObject->psram_public.Read_DummyCycle,
                                       PsramObject->psram_public.Write_command, 
                                       PsramObject->psram_public.Write_DummyCycle))
  {
    retr = EXTMEM_DRIVER_PSRAM_ERROR_MAP_ENABLE;
  }
  return retr;
}

EXTMEM_DRIVER_PSRAM_StatusTypeDef EXTMEM_DRIVER_PSRAM_Disable_MemoryMappedMode(EXTMEM_DRIVER_PSRAM_ObjectTypeDef *PsramObject)
{
  EXTMEM_DRIVER_PSRAM_StatusTypeDef retr = EXTMEM_DRIVER_PSRAM_OK;

  /* launch mass erase command */
  if (HAL_OK != SAL_XSPI_DisableMapMode(&PsramObject->psram_private.SALObject))
  {
    retr = EXTMEM_DRIVER_PSRAM_ERROR_MAP_DISABLE;
  }
  return retr;
}

/**
  * @}
  */

/** @addtogroup PSRAM_Private_Functions
  * @{
  */

/**
 * @brief this function executes a command 
 *
 * @param PsramObject psram memory object
 * @param Index command index
 * @return @ref EXTMEM_DRIVER_PSRAM_StatusTypeDef
 **/
EXTMEM_DRIVER_PSRAM_StatusTypeDef PSRAM_ExecuteCommand(EXTMEM_DRIVER_PSRAM_ObjectTypeDef *PsramObject, uint8_t Index)
{
  EXTMEM_DRIVER_PSRAM_StatusTypeDef retr = EXTMEM_DRIVER_PSRAM_OK;
  uint8_t regval[2];

  if (PsramObject->psram_public.ReadREGSize > 2u)
  {
    retr = EXTMEM_DRIVER_PSRAM_ERROR_REGSIZE;
    goto error;
  }

  if (HAL_OK != SAL_XSPI_Read(&PsramObject->psram_private.SALObject, 
                              PsramObject->psram_public.ReadREG,
                              PsramObject->psram_public.config[Index].REGAddress, 
                              regval, PsramObject->psram_public.ReadREGSize))
  {
    retr = EXTMEM_DRIVER_PSRAM_ERROR_READREG;
    goto error;
  }

  MODIFY_REG(regval[0], 
             PsramObject->psram_public.config[Index].WriteMask, 
             PsramObject->psram_public.config[Index].WriteValue);

  if (HAL_OK != SAL_XSPI_Write(&PsramObject->psram_private.SALObject, 
                               PsramObject->psram_public.WriteREG,
                               PsramObject->psram_public.config[Index].REGAddress, 
                               regval, PsramObject->psram_public.ReadREGSize))
  {
    retr = EXTMEM_DRIVER_PSRAM_ERROR_WRITEREG;
    goto error;
  }

error:
  return retr;
}

/**
  * @}
  */

/**
  * @}
  */
#endif /* EXTMEM_DRIVER_PSRAM == 1 */
