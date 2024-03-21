/**
  ******************************************************************************
  * @file    stm32_sdcard_driver.h
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
#ifndef __STM32_SDCARD_DRIVER_H
#define __STM32_SDCARD_DRIVER_H

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

/** @addtogroup SDCARD_Exported_Types
  * @{
  */

/**
 * @brief list of error code of the SDCARD driver
 */
typedef enum  {
  EXTMEM_DRIVER_SDCARD_OK               =   0, /*!< status ok */
  EXTMEM_DRIVER_SDCARD_ERROR_LINKTYPE   =  -1, /*!< error on the link type */
  EXTMEM_DRIVER_SDCARD_ERROR_READ       =  -2, /*!< error on the read operation */
  EXTMEM_DRIVER_SDCARD_ERROR_WRITE      =  -3, /*!< error on the write operation */
  EXTMEM_DRIVER_SDCARD_ERROR_PARAM      =  -4, /*!< error on the parameter */
  EXTMEM_DRIVER_SDCARD_ERROR            =-128, /*!< error */
} EXTMEM_DRIVER_SDCARD_StatusTypeDef;

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/**
  * @addtogroup SDCARD_Exported_Functions exported functions
  * @{
  */
/**
 * @brief this function initializes the driver SDCARD
 * @param SDCARDObject object SDCARD
 * @param ClockInput timeout value
 * @return @ref EXTMEM_DRIVER_SDCARD_StatusTypeDef
 **/
EXTMEM_DRIVER_SDCARD_StatusTypeDef EXTMEM_DRIVER_SDCARD_Init(void *IP,
                                                      EXTMEM_DRIVER_SDCARD_ObjectTypeDef* SDCARDObject);

/**
 * @brief this function un-initializes the driver SDCARD
 * @param SDCARDObject object SDCARD
 * @return @ref EXTMEM_DRIVER_SDCARD_StatusTypeDef
 **/
EXTMEM_DRIVER_SDCARD_StatusTypeDef EXTMEM_DRIVER_SDCARD_DeInit(EXTMEM_DRIVER_SDCARD_ObjectTypeDef* SDCARDObject);

/**
 * @brief this function reads SDCARD memory
 * @param SDCARDObject object SDCARD
 * @param Address memory address
 * @param Data pointer on the data
 * @param Size data size to read
 * @return @ref EXTMEM_DRIVER_SDCARD_StatusTypeDef
 **/
EXTMEM_DRIVER_SDCARD_StatusTypeDef EXTMEM_DRIVER_SDCARD_Read(EXTMEM_DRIVER_SDCARD_ObjectTypeDef* SDCARDObject,
                                                             uint32_t Address, uint8_t* Data, uint32_t Size);
/**
 * @brief this function writes data on the SDCARD memory
 * @param SDCARDObject object SDCARD
 * @param Address memory address
 * @param Data pointer on the data
 * @param Size data size to write
 * @return @ref EXTMEM_DRIVER_SDCARD_StatusTypeDef
 **/
EXTMEM_DRIVER_SDCARD_StatusTypeDef EXTMEM_DRIVER_SDCARD_Write(EXTMEM_DRIVER_SDCARD_ObjectTypeDef* SDCARDObject,
                                                              uint32_t Address, const uint8_t* Data, uint32_t Size);

/**
 * @brief this function erases SDCARD blocks
 * @param SDCARDObject object SDCARD
 * @param Address memory address
 * @param Size data size to erase
 * @return @ref EXTMEM_DRIVER_SDCARD_StatusTypeDef
 **/
EXTMEM_DRIVER_SDCARD_StatusTypeDef EXTMEM_DRIVER_SDCARD_EraseBlock(EXTMEM_DRIVER_SDCARD_ObjectTypeDef* SDCARDObject,
                                                                   uint32_t Address, uint32_t Size);

/**
 * @brief this function erases the SDCARD memory
 * @param SDCARDObject object SDCARD
 * @return @ref EXTMEM_DRIVER_SDCARD_StatusTypeDef
 **/
EXTMEM_DRIVER_SDCARD_StatusTypeDef EXTMEM_DRIVER_SDCARD_Erase(EXTMEM_DRIVER_SDCARD_ObjectTypeDef* SDCARDObject);

/**
 * @brief this function get the SDCARD information
 * @param SDCARDObject SD card object
 * @param Info pointer on SDcard information struct
 * @return @ref EXTMEM_DRIVER_SDCARD_StatusTypeDef
 **/
EXTMEM_DRIVER_SDCARD_StatusTypeDef EXTMEM_DRIVER_SDCARD_GetInfo(EXTMEM_DRIVER_SDCARD_ObjectTypeDef* SDCARDObject, EXTMEM_DRIVER_SDCARD_InfoTypeDef *Info);

/**
  * @}
  */

/**
  * @}
  */
#endif /* EXTMEM_DRIVER_SDCARD == 1 */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_SDCARD_DRIVER_H */
