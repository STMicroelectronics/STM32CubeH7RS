/**
  ******************************************************************************
  * @file    stm32_sal_xspi_type.h
  * @author  MCD Application Team
  * @brief   This file contains the software adaptation layer XSPI functions
  *          prototypes.
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
#ifndef __SAL_XSPI_TYPE_H
#define __SAL_XSPI_TYPE_H

#ifdef __cplusplus
 extern "C" {
#endif

/** @addtogroup SAL_XSPI
  * @ingroup EXTMEM_SAL
  * @{
  */

/* Includes ------------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/** @defgroup SAL_XSPI_Exported_types SAL XSPI exported types
  * @{
  */

typedef struct {
   XSPI_HandleTypeDef     *hxspi;            /*!< handle on the XSPI instance */
   XSPI_RegularCmdTypeDef commandbase;       /*!< command base configuration */
   uint8_t                commandExtension;  /*!< Flag on the 16-bit command extension 0 inverted 1 the same */
} SAL_XSPI_ObjectTypeDef;

/**
 * @brief list of the supported configuration link
 */

typedef enum {
  PHY_LINK_1S1S1S,    /*!< physical link configure in 1S1S1S */
  PHY_LINK_2S2S2S,    /*!< physical link configure in 2S2S2S */
  PHY_LINK_4S4S4S,    /*!< physical link configure in 4S4S4S */
  PHY_LINK_4S4D4D,    /*!< physical link configure in 4S4D4D */
  PHY_LINK_1S8S8S,    /*!< physical link configure in 1S8S8S */
  PHY_LINK_8D8D8D,    /*!< physical link configure in 8D8D8D */

  PHY_LINK_RAM8,      /*!< physical link configure for RAM  8lines of data */
  PHY_LINK_RAM16,     /*!< physical link configure for RAM 16lines of data */
} SAL_XSPI_PhysicalLinkTypeDef;


/**
  * @brief define the list of the parameter
  */
typedef enum {
  PARAM_PHY_LINK,                /*!< physical link parameter */
  PARAM_DUMMY_CYCLES,            /*!< dmmy cycle parameter */
  PARAM_ADDRESS_4BITS,           /*!< @4BITS parameter */
  PARAM_DATA_STROBE,             /*!< data strobe parameter */
  PARAM_COMMAND_EXTENSION,       /*!< command extension parameter */
  PARAM_FLASHSIZE,               /*!< set the flash size on the IP */
} SAL_XSPI_MemParamTypeTypeDef;
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup SAL_XSPI_Exported_Functions SAL XSP Exported Functions
  * @{
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __SAL_XSPI_TYPE_H */


