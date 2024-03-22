/**
  ******************************************************************************
  * @file    stm32_psram_driver.h
  * @author  MCD Application Team
  * @brief   This file contains the psram driver definition.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_PSRAM_DRIVER_H
#define __STM32_PSRAM_DRIVER_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if EXTMEM_DRIVER_PSRAM == 1
   
/** @addtogroup PSRAM
  * @ingroup EXTMEM_DRIVER
  * @{
  */
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/** @defgroup PSRAM_Exported_Types Exported Types
  * @{
  */

/**
 * @brief list of error code of the PSRAM driver
 */
typedef enum  {
  EXTMEM_DRIVER_PSRAM_OK                           =   0,
  EXTMEM_DRIVER_PSRAM_ERROR_READREG                =  -1,
  EXTMEM_DRIVER_PSRAM_ERROR_WRITEREG               =  -2,
  EXTEM_DRIVER_PSRAM_ERROR_READ                    =  -3,
  EXTEM_DRIVER_PSRAM_ERROR_WRITE                   =  -4,
  EXTMEM_DRIVER_PSRAM_ERROR_MAP_ENABLE             =  -5,
  EXTMEM_DRIVER_PSRAM_ERROR_MAP_DISABLE            =  -6,
  EXTMEM_DRIVER_PSRAM_ERROR_REGSIZE                =  -7,
  EXTMEM_DRIVER_PSRAM_ERROR                        = -128,
} EXTMEM_DRIVER_PSRAM_StatusTypeDef;

/* Exported functions --------------------------------------------------------*/
/**
  * @addtogroup PSRAM_Exported_Functions Exported functions
  * @{
  */
/**
 * @brief this function initialize the driver PSRAM
 *
 * @param IP ptr on the ip handle
 * @param Config link configuration
 * @param ClockInput timeout value
 * @param PsramObject object PSRAM
 * @return @ref EXTMEM_DRIVER_PSRAM_StatusTypeDef
 **/
EXTMEM_DRIVER_PSRAM_StatusTypeDef EXTMEM_DRIVER_PSRAM_Init(void *IP, EXTMEM_LinkConfig_TypeDef Config,
                                                           uint32_t ClockInput,
                                                           EXTMEM_DRIVER_PSRAM_ObjectTypeDef *PsramObject);

/**
 * @brief this function un-initialize the driver PSRAM
 *
 * @param PsramObject IP instance
 * @return @ref EXTMEM_DRIVER_PSRAM_StatusTypeDef
 **/
EXTMEM_DRIVER_PSRAM_StatusTypeDef EXTMEM_DRIVER_PSRAM_DeInit(EXTMEM_DRIVER_PSRAM_ObjectTypeDef *PsramObject);

/**
 * @brief this function enables the memory mapped mode
 *
 * @param PsramObject memory instance
 * @return @ref EXTMEM_DRIVER_PSRAM_StatusTypeDef
 **/
EXTMEM_DRIVER_PSRAM_StatusTypeDef EXTMEM_DRIVER_PSRAM_Enable_MemoryMappedMode(EXTMEM_DRIVER_PSRAM_ObjectTypeDef *PsramObject);

/**
 * @brief this function disables the memory mapped mode
 *
 * @param PsramObject memory instance
 * @return @ref EXTMEM_DRIVER_PSRAM_StatusTypeDef
 **/
EXTMEM_DRIVER_PSRAM_StatusTypeDef EXTMEM_DRIVER_PSRAM_Disable_MemoryMappedMode(EXTMEM_DRIVER_PSRAM_ObjectTypeDef *PsramObject);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
#endif /* EXTMEM_DRIVER_PSRAM == 1 */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_PSRAM_DRIVER_H */
