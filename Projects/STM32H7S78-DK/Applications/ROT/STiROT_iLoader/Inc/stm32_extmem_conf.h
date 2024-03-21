/**
  ******************************************************************************
  * @file    stm32_extmem_conf.h
  * @author  MCD Application Team
  * @brief   Header configuration for extmem module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EXTMEM_CONF_H_
#define EXTMEM_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup EXTMEN_CONF
  * @{
  */

/** @defgroup EXTMEN_CONF_Driver_selection External Memory configuration selection of the driver
  * this section is used to select the driver\n
  *              #define EXTMEM_DRIVER_NOR_SFDP   1\n
  *              #define EXTMEM_DRIVER_PSRAM      1\n
  *              #define EXTMEM_DRIVER_SDCARD     1\n
  * @{
  */
#define EXTMEM_DRIVER_NOR_SFDP   1
#define EXTMEM_DRIVER_PSRAM      0
#define EXTMEM_DRIVER_SDCARD     0

/*
  @brief management of the driver layer enable
*/
#define EXTMEM_SAL_XSPI      1
#define EXT_PAGE_SIZE        (0x100U)
/**
  * @}
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_hal.h"
#include "stm32_extmem.h"
#include "stm32_extmem_type.h"

/** @defgroup EXTMEN_CONF_SAL_imported_variable External Memory configuration list of the imported variables
  * this section is used to import the HAL handle variable.
  * @{
  */

/*
  @brief import of the HAL handles for MEMORY_SERIAL_0
*/
extern XSPI_HandleTypeDef       hxspi;

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** @defgroup EXTMEM_CONF_Exported_constants EXTMEM_CONF exported constants
  * @{
  */
enum
{
  MEMORY_SERIAL_0  = 0, /*!< ID 0 of extmem_list_config */
};

/**
  * @}
  */

/* Exported configuration --------------------------------------------------------*/
/** @defgroup EXTMEM_CONF_Exported_configuration EXTMEM_CONF exported configuration definition
  * @{
  */
extern EXTMEM_DefinitionTypeDef extmem_list_config[1];
#if defined(EXTMEM_C)
EXTMEM_DefinitionTypeDef extmem_list_config[1] =
{
  /* EXTMEMORY_1 */
  {
    .MemType = EXTMEM_NOR_SFDP,
    .Handle = (void *) &hxspi,
    .ConfigType = EXTMEM_LINK_CONFIG_8LINES,
#if !defined ( __GNUC__ )
    .NorSfdpObject =
    {
      0u
    }
#endif /* __GNUC__ */
  }
};
#endif /* EXTMEM_C */
/**
  * @}
  */

/* Exported trace --------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif /* EXTMEM_CONF_H_ */
