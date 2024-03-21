/**
 ******************************************************************************
 * @file    stm32_device_info.h
 * @author  MCD Application Team
 * @brief   header file of stm32_device_info.c
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
#ifndef STM32_LOADER_INFO_H
#define STM32_LOADER_INFO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32_extmemloader_conf.h"

/** @defgroup STM32_LOADER STM32Cube loader
  * @ingroup EXTMEM_LOADER
  * @{
  */

/** @addtogroup STM32_LOADER_Exported_definiton
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @addtogroup STM32_LOADER_Exported_Constants
  * @{
  */

/**
 * @brief different type of supported memory
 */
#define MCU_FLASH 1
#define NAND_FLASH 2
#define NOR_FLASH 3
#define SRAM 4
#define PSRAM 5
#define PC_CARD 6
#define SPI_FLASH 7
#define I2C_FLASH 8
#define SDRAM 9
#define I2C_EEPROM 10

/**
 * @brief Maximum Number of Sectors
 */
#define SECTOR_NUM    10

/**
 * @brief Maximum length for the device name
 */
#define DEVICENAME_MAXLENGHT 100

/**
  * @}
  */

/* Exported typedefs --------------------------------------------------------*/
/** @defgroup STM32_LOADER_Exported_Typedefs Exported typedefs
  * @{
  */

/**
 * @brief Maximum length for the device name
 */
typedef struct
{
  uint32_t SectorNumber;  /*!< Number of Sectors    */
  uint32_t SectorSize;    /*!< Sector Size in Bytes */
} DeviceSectors;

/**
 * @brief Maximum length for the device name
 */   
typedef struct
{
   uint8_t  DeviceName[DEVICENAME_MAXLENGHT]; /*!< Device Name and Description              */
   uint16_t DeviceType;    	              /*!< Device Type: NAND_FLASH, NOR_FLASH, ...  */
   uint32_t DeviceStartAddress;	              /*!< Default Device Start Address             */
   uint32_t DeviceSize;    	              /*!< Total Size of Device                     */
   uint32_t PageSize;  	                      /*!< Programming Page Size                    */
   uint8_t  EraseValue;    	              /*!< Content of Erased Memory                 */
   DeviceSectors Sectors[SECTOR_NUM];         /*!< sector descriptor                        */
} sStorageInfo;

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* STM32_LOADER_INFO_H */