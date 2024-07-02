/**
  ******************************************************************************
  * @file    stm32_sfdp_data.h
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
#ifndef __STM32_SFDP_DATA_H
#define __STM32_SFDP_DATA_H

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
 * @brief SFDP param table ID
 */
typedef enum {
   SFPD_PARAMID_UNKNOWN                                     = 0x00000u,
   SFPD_PARAMID_VENDOR                                      = 0x00001u,
   SFPD_PARAMID_FUNCTION_VENDOR                             = 0x00002u,
   SFPD_PARAMID_FUNCTION_JEDEC                              = 0x00004u,
   SFPD_PARAMID_BASIC_SPIPROTOCOL                           = 0x00008u,
   SFPD_PARAMID_SECTORMAP                                   = 0x00010u,
   SFPD_PARAMID_RPMC                                        = 0x00020u,
   SFPD_PARAMID_4BYTE_ADDRESS_INSTRUCTION                   = 0x00040u,
   SFPD_PARAMID_XSPI_V1_0                                   = 0x00080u,
   SFPD_PARAMID_XSPI_V2_0                                   = 0x00100u,
   SFPD_PARAMID_STATUS_CONTROL_CONFIG_REGISTER_MAP          = 0x00200u,
   SFPD_PARAMID_STATUS_CONTROL_CONFIG_REGISTER_MAP_MULTICHIP= 0x00400u,
   SFPD_PARAMID_STATUS_CONTROL_CONFIG_XSPI_V2_0             = 0x00800u,
   SFPD_PARAMID_OCTAL_DDR                                   = 0x01000u,
   SFPD_PARAMID_MSPT                                        = 0x02000u,
   SFPD_PARAMID_X4QUAD_DS                                   = 0x04000u,
   SFPD_PARAMID_QUAD_DDR                                    = 0x08000u,
   SFPD_PARAMID_SECURE_PACKET_READ_WRITE                    = 0x10000u,
   SFPD_PARAMID_RESERVED                                    = 0x20000u
} SFDP_ParamID_TypeDef;

/**
 * @brief SFDP status typedef
 */
typedef enum {
      EXTMEM_SFDP_OK,                            /*!< status OK */
      EXTMEM_SFDP_ERROR_PARAM,
      EXTMEM_SFDP_ERROR_NOREADFUNCTION,
      EXTMEM_SFDP_ERROR_SFPDREAD,
      EXTMEM_SFDP_ERROR_SIGNATURE,               /*!< the signature is invalid */
      EXTMEM_SFDP_ERROR_SIGNATUREMTYPE,          /*!< the signature is invalid due to wrong memory type */
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
 * @brief this function get the SFDP header
 * @param Object memory instance object descriptor
 * @return @ref SFDP_StatusTypeDef
 **/
SFDP_StatusTypeDef SFDP_GetHeader(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object,
                                  SFPD_HeaderTypeDef *sfdp_header);

 /**
 * @brief this function read the SFDP header for current configuration
          and adjust the memory type if required
 * @param Object memory instance object descriptor
 * @return @ref SFDP_StatusTypeDef
 **/
SFDP_StatusTypeDef SFDP_ReadHeader(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object,
                                   SFPD_HeaderTypeDef *sfdp_header);

/**
 * @brief collect all the SFPD information
 * @param Object memory instance object descriptor
 * @return @ref SFDP_StatusTypeDef
 */
SFDP_StatusTypeDef SFDP_CollectData(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object);

/**
 * @brief reset the memory
 * @param Object memory instance object descriptor
 * @return @ref SFDP_StatusTypeDef
 */
SFDP_StatusTypeDef SFDP_MemoryReset(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object);

/**
 * @brief build the driver info
 * @param Object memory instance object descriptor
 * @param FreqUpdated set to 1 if the freqence is updated
 * @return @ref SFDP_StatusTypeDef
 */
SFDP_StatusTypeDef SFDP_BuildGenericDriver(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *Object, uint8_t *FreqUpdated);

/**
 * @brief this function check the busy flag
 *
 * @param SFDPObject memory Object
 * @param timeout timeout value
 * @return @ref EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef
 **/
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef driver_check_FlagBUSY(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject, uint32_t Timeout);


/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_SFDP_DATA_H */
