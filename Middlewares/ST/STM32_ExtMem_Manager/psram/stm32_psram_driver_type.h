/**
  ******************************************************************************
  * @file    stm32_psram_type.h
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
#ifndef __STM32_PSRAM_TYPE_H
#define __STM32_PSRAM_TYPE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/** @addtogroup PSRAM
  * @ingroup EXTMEM_DRIVER
  * @{
  */

#if EXTMEM_DRIVER_PSRAM == 1

/* Exported constants --------------------------------------------------------*/
/** @defgroup PSRAM_Exported_constants Exported constants
  * @{
  */

/**
 * @brief driver PSRAM maximum number of command
 */
#define PSRAM_MAX_COMMAND 3u

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup PSRAM_Exported_Types Exported Types
  * @{
  */

/**
 * @brief driver PSRAM object definition
 */
typedef struct {
  struct {
  SAL_XSPI_ObjectTypeDef   SALObject;     /*!< SAL object */
  } psram_private;                        /*!< private data of the object */

  struct {
  uint32_t MemorySize;                    /*!< memory size @ref XSPI_MemorySize */
  uint32_t FreqMax;                       /*!< maximum frequency supported by the memory */

  /* Configuration */
  uint8_t NumberOfConfig;                /*!< Number of config, each config consists to perform read and write operation */
  struct {
    uint8_t WriteMask;                   /*!< write mask */
    uint8_t WriteValue;                  /*!< write value */
    uint8_t REGAddress;                  /*!< register address */
  } config[PSRAM_MAX_COMMAND];

  /* Command REG */
  uint8_t ReadREG;
  uint8_t WriteREG;
  uint8_t ReadREGSize;
  uint8_t REG_DummyCycle;

  /* Command read write */
  uint8_t Write_command;
  uint8_t Write_DummyCycle;
  uint8_t Read_command;
  uint8_t WrapRead_command;
  uint8_t Read_DummyCycle;
  } psram_public;                        /*!< public data of the object */
} EXTMEM_DRIVER_PSRAM_ObjectTypeDef;

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

#endif /* __STM32_PSRAM_TYPE_H */
