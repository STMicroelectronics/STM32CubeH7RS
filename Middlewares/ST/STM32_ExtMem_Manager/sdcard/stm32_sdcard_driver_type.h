/**
  ******************************************************************************
  * @file    stm32_sdcard_type.h
  * @author  MCD Application Team
  * @brief   This file contains the sd card driver definition.
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
#ifndef __STM32_SDCARD_TYPE_H
#define __STM32_SDCARD_TYPE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if EXTMEM_DRIVER_SDCARD == 1

/** @addtogroup SDCARD
  * @ingroup EXTMEM_DRIVER
  * @{
  */
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/** @defgroup SDCARD_Exported_Types Exported Types
  * @{
  */

/**
 * @brief physical link to drive SDCARD communication
 */
typedef enum {
  EXTMEM_DRIVER_SDCARD_LINKSD,  /*!< link SD card */
  EXTMEM_DRIVER_SDCARD_LINKMMC, /*!< link EMMC card */
  EXTMEM_DRIVER_SDCARD_LINKSPI  /*!< link SPI card */
} SDCardLinkTypeDef;
  
/**
 * @brief driver SDCARD instance definition
 */
 typedef struct
 {
  struct 
  {
    SDCardLinkTypeDef Link;   /*!< physical link */
  }sdcard_public;
  
  struct 
  {
    EXTMEM_DRIVER_SDCARD_InfoTypeDef Info;  /*!< sdcard information */
    union
    {
#if EXTMEM_SAL_SD == 1
      SAL_SD_ObjectTypeDef SDObject; /*!< SD Object memory */
#endif                               /* EXTMEM_SAL_SD == 1 */
#if EXTMEM_SAL_MMC == 1
      SAL_MMC_ObjectTypeDef MMCObject; /*!< MMC Object memory */
#endif                                 /* EXTMEM_SAL_MMC == 1 */
    } SALObject;
  }sdcard_private;
  
} EXTMEM_DRIVER_SDCARD_ObjectTypeDef;

/**
  * @}
  */
 
/* Exported functions --------------------------------------------------------*/

/**
  * @}
  */
#endif /* EXTMEM_DRIVER_SDCARD == 1 */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_SDCARD_TYPE_H */
