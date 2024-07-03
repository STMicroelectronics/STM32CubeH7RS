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
#ifndef __STM32_SAL_XSPI_TYPE_H
#define __STM32_SAL_XSPI_TYPE_H

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

/**
 * @brief data order information
 */

typedef enum {
 SAL_XSPI_ORDERINVERTED = 0,
} SAL_XSPI_DataOrderTypeDef;

/**
 * @brief list of the supported configuration link
 */

typedef enum {
  PHY_LINK_1S1S1S,    /*!< physical link configure in 1S1S1S */
  PHY_LINK_1S1S2S,    /*!< physical link configure in 1S1S1S */
  PHY_LINK_1S2S2S,    /*!< physical link configure in 1S1S1S */
  PHY_LINK_1S1D1D,    /*!< physical link configure in 1S1D1D */
  PHY_LINK_4S4S4S,    /*!< physical link configure in 4S4S4S */
  PHY_LINK_4S4D4D,    /*!< physical link configure in 4S4D4D */
  PHY_LINK_4D4D4D,    /*!< physical link configure in 4D4D4D */
  PHY_LINK_1S8S8S,    /*!< physical link configure in 1S8S8S */
  PHY_LINK_8S8D8D,    /*!< physical link configure in 8D8D8D */
  PHY_LINK_8D8D8D,    /*!< physical link configure in 8D8D8D */

  PHY_LINK_RAM8,      /*!< physical link configure for RAM  8lines of data */
#if defined(HAL_XSPI_DATA_16_LINES)
  PHY_LINK_RAM16,     /*!< physical link configure for RAM 16lines of data */
#endif /* defined(HAL_XSPI_DATA_16_LINES) */
} SAL_XSPI_PhysicalLinkTypeDef;

typedef struct {
   XSPI_HandleTypeDef           *hxspi;            /*!< handle on the XSPI instance */
   XSPI_RegularCmdTypeDef       Commandbase;       /*!< command base configuration */
   uint8_t                      CommandExtension;  /*!< Flag on the 16-bit command extension 0 inverted 1 the same */
   uint8_t                      SFDPDummyCycle;    /*!< SDPF dummy cycle */
   SAL_XSPI_PhysicalLinkTypeDef PhyLink;           /*!< Only used for data Read in 4S4D4d 2S2D2D 1S1D1D */
   uint8_t                      DTRDummyCycle;     /*!< Specify that DTR read only valid for data read using DTRDummyCycle value */
} SAL_XSPI_ObjectTypeDef;

/**
  * @brief define the list of the parameter
  */
typedef enum {
  PARAM_PHY_LINK,                /*!< physical link parameter */
  PARAM_DUMMY_CYCLES,            /*!< dummy cycle parameter */
  PARAM_ADDRESS_4BITS,           /*!< @4BITS parameter */
  PARAM_DATA_STROBE,             /*!< data strobe parameter */
  PARAM_FLASHSIZE,               /*!< set the flash size on the IP */
} SAL_XSPI_MemParamTypeTypeDef;
/**
  * @}
  */

/* Exported Macro ------------------------------------------------------------*/

#define SAL_XSPI_SET_DTRREADDUMMYCYLE(_OBJ_,_VAL_) (_OBJ_).DTRDummyCycle = (_VAL_)
#define SAL_XSPI_SET_SFDPDUMMYCYLE(_OBJ_,_VAL_)    (_OBJ_).SFDPDummyCycle = (_VAL_)
#define SAL_XSPI_SET_COMMANDEXTENSION(_OBJ_,_VAL_) (_OBJ_).CommandExtension = (_VAL_)

#define SAL_XSPI_FORMAT_COMMANDSEND(_PTR_, _CMD_)                      \
                          _PTR_.CommandType = SAL_XSPI_COMMAND_SEND;   \
                          _PTR_.Command = _CMD_;

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

#endif /* __STM32_SAL_XSPI_TYPE_H */


