/**
  ******************************************************************************
  * @file    stm32_extmem_type.h
  * @author  MCD Application Team
  * @brief   This file contains the external memory type definition.
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
#ifndef EXTMEM_TYPE_H_
#define EXTMEM_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup EXTMEM
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#if EXTMEM_SAL_XSPI == 1
#include "sal/stm32_sal_xspi_type.h"
#endif /* EXTMEM_SAL_XSPI */

#if EXTMEM_SAL_SD == 1
#include "sal/stm32_sal_sd_type.h"
#endif /* EXTMEM_SAL_SD */

#if EXTMEM_SAL_MMC == 1
#include "sal/stm32_sal_mmc_type.h"
#endif /* EXTMEM_SAL_MMC */

#if EXTMEM_DRIVER_NOR_SFDP == 1
#include "nor_sfdp/stm32_sfdp_driver_type.h"
#endif /* DRIVER_SFDP_ENABLED */

#if EXTMEM_DRIVER_SDCARD == 1
#include "sdcard/stm32_sdcard_driver_type.h"
#endif /* DRIVER_PSRAM_ENABLED */

#if EXTMEM_DRIVER_PSRAM == 1
#include "psram/stm32_psram_driver_type.h"
#endif /* DRIVER_PSRAM_ENABLED */

#if EXTMEM_DRIVER_USER == 1
#include "user/stm32_user_driver_type.h"
#endif /* DRIVER_USER_ENABLED */
    
/* Exported constants --------------------------------------------------------*/
/** @defgroup EXTMEN_Exported_Macro External Memory Exported Macro
  * @{
  */
/**
  * @brief macro to get the minimum value 
  */

#define EXTMEM_MIN(_A_,_B_)    ((_A_)>(_B_))?(_B_):(_A_);
/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/    
/** @defgroup EXTMEN_Exported_Typedef External Memory Exported type definition
  * @{
  */

/**
  * @brief Definition of the memory 
  */
typedef struct
{
  EXTMEM_TypeTypeDef MemType;               /*!< memory driver type */
  void *Handle;                             /*!< HAL handle */
  EXTMEM_LinkConfig_TypeDef ConfigType;     /*!< physical link config */
  union
  {
#if EXTMEM_DRIVER_NOR_SFDP == 1
    EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef  NorSfdpObject;  /*!< NorSfdp object */
#endif /* EXTMEM_DRIVER_NOR_SFDP == 1 */
#if EXTMEM_DRIVER_SDCARD == 1
    EXTMEM_DRIVER_SDCARD_ObjectTypeDef   SdCardObject;    /*!< SdCard object */
#endif /* EXTMEM_DRIVER_PSRAM == 1 */
#if EXTMEM_DRIVER_PSRAM == 1
    EXTMEM_DRIVER_PSRAM_ObjectTypeDef    PsramObject;    /*!< Psram object */
#endif /* EXTMEM_DRIVER_PSRAM == 1 */
#if EXTMEM_DRIVER_USER == 1
    EXTMEM_DRIVER_USER_ObjectTypeDef     UserObject;     /*!< User object */
#endif /* EXTMEM_DRIVER_CUSTOM == 1 */
  };
} EXTMEM_DefinitionTypeDef;

/**
  * @}
  */


/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* EXTMEM_TYPE_H_ */
