/**
  ******************************************************************************
  * @file    stm32_sfdp.h
  * @author  MCD Application Team
  * @brief   This file contains the sfdp functions prototypes.
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
#ifndef __SFDP_H
#define __SFDP_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if EXTMEM_SAL_XSPI == 1
#include "../sal/stm32_sal_xspi_api.h"
#else
#error "the driver SFDP requires the enable of EXTMEM_SAL_XSPI"
#endif /* EXTMEM_SAL_XSPI_ENABLED */

/** 
  * @addtogroup NOR_SFDP_DATA
  * @ingroup NOR_SFDP
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/** @defgroup NOR_SFDP_DATA_Exported_Types Exported Types
  * @ingroup NOR_SFDP_DATA
  * @{
  */

/**
 * @brief SFDP status typedef
 */
typedef enum {
      EXTMEM_SFDP_OK,                            /*!< status OK */
      EXTMEM_SFDP_ERROR_PARAM,
      EXTMEM_SFDP_ERROR_NOREADFUNCTION,
      EXTMEM_SFDP_ERROR_MEDUIM,
      EXTMEM_SFDP_ERROR_SIGNATURE,
      EXTMEM_SFDP_ERROR_PARAMTABLE_NOTFOUND,
      EXTMEM_SFDP_ERROR_NO_PARAMTABLE_BASIC,
      EXTMEM_SFDP_ERROR_NO_4BITADDRESSING,
      EXTMEM_SFDP_ERROR_JEDECBASIC_D14,
      EXTMEM_SFDP_ERROR_JEDECBASIC_D16,
      EXTMEM_SFDP_ERROR_OCTALMODE,
      EXTMEM_SFDP_ERROR_DRIVER,
      EXTMEM_SFDP_ERROR_SETCLOCK,
      EXTMEM_SFDP_ERROR_CONFIGDUMMY,
      EXTMEM_SFDP_ERROR_NOTYETHANDLED
} SFDP_StatusTypeDef;


/**
 * @brief SFDP header
 */
typedef struct {
  uint32_t  Signature;       /*!< signature value */
  uint8_t  Minor_revision;   /*!< minor revision */
  uint8_t  Major_revision;   /*!< major revision */
  uint8_t  param_number;     /*!< number of param */
  uint8_t  AccessProtocol;   /*!< access protocol */
} SFPD_HeaderTypeDef;

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup NOR_SFDP_Data_Exported_Functions Exported Functions
  * @{
  */

 /**
 * @brief this function gets the SFDP header
 * @param Object
 * @return @ref SFDP_StatusTypeDef
 **/
SFDP_StatusTypeDef SFDP_GetHeader(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object);
/**
 * @brief collect all the SFPD information
 * @param Object memory instance
 * @return @ref SFDP_StatusTypeDef
 */
SFDP_StatusTypeDef SFDP_CollectData(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object);

/**
 * @brief reset the memory
 * @param Object memory instance
 * @return @ref SFDP_StatusTypeDef
 */
SFDP_StatusTypeDef SFDP_MemoryReset(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object);

/**
 * @brief build the driver info
 * @param Object memory instance
 * @return @ref SFDP_StatusTypeDef
 */
SFDP_StatusTypeDef SFDP_BuildGenericDriver(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __SFDP_H */
