/**
  ******************************************************************************
  * @file    stm32_boot_lrun.h
  * @author  MCD Application Team
  * @brief   Header for stm32_boot_lrun.c file.
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
#ifndef __STM32_BOOT_LRUN_H__
#define __STM32_BOOT_LRUN_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32_extmem_conf.h"

/** @addtogroup BOOT_LRUN
  * @{
  */

/* Exported defines ---------------------------------------------------------*/
/**
  *  @defgroup BOOT_LRUN_Private_Defines Boot LRUN exported definitions
  * @{
  */
/**
 * @brief List of status codes for LRUN
 */
typedef enum {
     BOOT_OK,
     BOOT_ERROR_UNSUPPORTED_MEMORY, /* !< unsupported memory type         */
     BOOT_ERROR_NOBASEADDRESS,      /* !< not base address for the memory */
     BOOT_ERROR_MAPPEDMODEFAIL,     /* !< */
     BOOT_ERROR_COPY,
}BOOTStatus_TypeDef;

/**
  * @}
  */


/* Private function prototypes -----------------------------------------------*/
/**
  *  @defgroup BOOT_LRUN_Exported_Functions Boot LRUN exported functions
  * @{
  */

/**
 * @brief This function boots on the application, the operation consists in mapping 
 *        the memories, loading the code and jumping in the application. 
 *
 * @return @ref BOOTStatus_TypeDef
 **/
 BOOTStatus_TypeDef BOOT_Application(void);
 
 uint32_t BOOT_GetApplicationSize(uint32_t img_addr);
 uint32_t BOOT_GetApplicationVectorTable(void);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_BOOT_LRUN_H__ */