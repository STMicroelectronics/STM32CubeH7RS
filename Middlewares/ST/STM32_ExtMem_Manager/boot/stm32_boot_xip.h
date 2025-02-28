/**
  ******************************************************************************
  * @file    stm32_boot_xip.h
  * @author  MCD Application Team
  * @brief   Header for stm32_boot_xip.c file.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_BOOT_XIP_H__
#define __STM32_BOOT_XIP_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/** @addtogroup BOOT_XIP
  * @{
  */

/* Exported defines ---------------------------------------------------------*/
/**
  *  @defgroup BOOT_XIP_Private_Defines Boot XIP exported definitions
  * @{
  */
/**
 * @brief List of status codes for XIP
 */
typedef enum {
     BOOT_OK,
     BOOT_ERROR_UNSUPPORTED_MEMORY, /* !< unsupported memory type         */
     BOOT_ERROR_NOBASEADDRESS,      /* !< not base address for the memory */
     BOOT_ERROR_MAPPEDMODEFAIL,     /* !< error during map processing */
     BOOT_ERROR_INCOMPATIBLEMEMORY, /* !< selected memory not compatible with XIP boot */
     BOOT_ERROR_DRIVER,
}BOOTStatus_TypeDef;

/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/**
  *  @defgroup BOOT_XIP_Exported_Functions Boot XIP exported functions
  * @{
  */

/**
 * @brief This function boots on the application, the operation consists in mapping
 *        the memory and jumping in the application.
 *
 * @return @ref BOOTStatus_TypeDef
 **/
BOOTStatus_TypeDef BOOT_Application(void);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_BOOT_XIP_H__ */