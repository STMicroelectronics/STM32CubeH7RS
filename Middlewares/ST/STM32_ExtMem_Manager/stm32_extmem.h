/**
  ******************************************************************************
  * @file    stm32_extmem.h
  * @author  MCD Application Team
  * @brief   This file contains the external memory functions prototypes.
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
#ifndef __STM32_EXTMEM_H_
#define __STM32_EXTMEM_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup EXTMEM
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/** @defgroup EXTMEM_Exported_Types External Memory Exported Types
  * @{
  */

/**
  * @brief Middleware version number
  */
#define EXTMEM_VERSION 0x00010100 /* version v1.1.0 */

/**
  * @brief Types of supported memories
  */
typedef enum
{
  EXTMEM_NOR_SFDP,
  EXTMEM_NOR_CFI,
  EXTMEM_PSRAM,
  EXTMEM_SDCARD,
  EXTMEM_USER
} EXTMEM_TypeTypeDef;

/**
  * @brief EXTMEM module function call return values
  */
typedef enum
{
  EXTMEM_OK,                      /*!< action performed with a status ok */
  EXTMEM_ERROR_NOTSUPPORTED = -1, /*!< action not supported by the memory type */
  EXTMEM_ERROR_UNKNOWNMEMORY= -2, /*!< unknown memory type */
  EXTMEM_ERROR_DRIVER       = -3, /*!< error return by the driver layer */
  EXTMEM_ERROR_SECTOR_SIZE  = -4, /*!< inconsistency between the size an the sector size of the memory */
  EXTMEM_ERROR_INVALID_ID   = -5, /*!< the memory ID is invalid */
  EXTMEM_ERROR_PARAM        = -6, /*!< parameter value error */
} EXTMEM_StatusTypeDef;

/**
  * @brief Enable/disable state of the module
  */
typedef enum
{
  EXTMEM_ENABLE,               /*!< state enable */
  EXTMEM_DISABLE               /*!< state disable */
} EXTMEM_StateTypeDef;

/**
 * @brief NOR SFDP memory information structure
 */
typedef struct {
  uint8_t  FlashSize;         /*!< Flash size in power of 2 */
  uint32_t PageSize;          /*!< Page size */
  uint32_t EraseType1Size;    /*!< erase 1 size */
  uint32_t EraseType2Size;    /*!< erase 2 size */
  uint32_t EraseType3Size;    /*!< erase 3 size */
  uint32_t EraseType4Size;    /*!< erase 4 size */
} EXTMEM_NOR_SFDP_FlashInfoTypeDef;


/**
 * @brief USER memory information structure
 */
typedef struct {
  uint8_t  MemSize;            /*!< mem size in power of 2 */
  uint32_t MpuCache;          /*!< */
}  EXTMEM_USER_MemInfoTypeDef;

/**
 * @brief SD card information structure
 */
 typedef struct {
  uint32_t CardType;        /*!< Specifies the card type                         */
  uint32_t CardVersion;     /*!< Specifies the card version                      */
  uint32_t Class;           /*!< Specifies the class of the card                 */
  uint32_t RelCardAdd;      /*!< Specifies the celative card Address             */
  uint32_t BlockNbr;        /*!< Specifies the card capacity in blocks           */
  uint32_t BlockSize;       /*!< Specifies one block size in bytes               */
  uint32_t LogBlockNbr;     /*!< Specifies the card logical capacity in blocks   */
  uint32_t LogBlockSize;    /*!< Specifies logical block size in bytes           */
  uint32_t CardSpeed;       /*!< Specifies the card speed                        */
} EXTMEM_DRIVER_SDCARD_InfoTypeDef;

/**
 * @brief Number of physical I/O line(s) connected with the memory
 */
typedef enum {
   EXTMEM_LINK_CONFIG_1LINE,     /*!< Configuration using  1 line  */
   EXTMEM_LINK_CONFIG_2LINES,    /*!< Configuration using  2 lines */
   EXTMEM_LINK_CONFIG_4LINES,    /*!< Configuration using  4 lines */
   EXTMEM_LINK_CONFIG_8LINES,    /*!< Configuration using  8 lines */
   EXTMEM_LINK_CONFIG_16LINES,   /*!< Configuration using 16 lines */
} EXTMEM_LinkConfig_TypeDef;

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup EXTMEM_Exported_Functions External Memory Exported Functions
  * @{
  */

/**
 * @brief This function initializes a memory
 *
 * @param MemId memory id
 * @param ClockInput clock in input of the memory
 * @return @ref EXTMEM_StatusTypeDef
 **/
EXTMEM_StatusTypeDef EXTMEM_Init(uint32_t MemId, uint32_t ClockInput);

/**
 * @brief This function un-initializes a memory
 *
 * @param MemId memory id
 * @return @ref EXTMEM_StatusTypeDef
 **/
EXTMEM_StatusTypeDef EXTMEM_DeInit(uint32_t MemId);

/**
 * @brief This function reads a buffer from the memory
 *
 * @param MemId memory id
 * @param Address location of the data memory
 * @param Data data pointer
 * @param Size data size in bytes
 * @return @ref EXTMEM_StatusTypeDef
 **/
EXTMEM_StatusTypeDef EXTMEM_Read(uint32_t MemId, uint32_t Address, uint8_t* Data, uint32_t Size);

/**
 * @brief This function writes data to the memory
 *
 * @param MemId memory id
 * @param Address location of the data memory
 * @param Data data pointer
 * @param Size data size in bytes
 * @return @ref EXTMEM_StatusTypeDef
 **/
EXTMEM_StatusTypeDef EXTMEM_Write(uint32_t MemId, uint32_t Address, const uint8_t* Data, uint32_t Size);

/**
 * @brief This function writes data in memory mapped mode 
 *
 * @param MemId memory id
 * @param Address location of the data memory
 * @param Data data pointer aligned 32 bits (this assumption is really important for the writing process)
 * @param Size data size in bytes
 * @return @ref EXTMEM_StatusTypeDef
 * 
 * @note this function perform write in mapped mode need to perform a specific processing due to the management of 
 *       the write enable which can be manage when the memory is mapped. The operation consists to cut the write 
 *       in packet of page size and for each packet perform a write before enter mapped mode.
 * 
 * @note to avoid any issue on the execution of this processing, its mandatory to have targeted memory area 
 *       this the following characteristic: the write region must be not bufferable, not shareable and not cacheable.
 **/
EXTMEM_StatusTypeDef EXTMEM_WriteInMappedMode(uint32_t MemId, uint32_t Address, const uint8_t* const Data, uint32_t Size);

/**
 * @brief This function erases a number of sector
 *
 * @param MemId memory id
 * @param Address location of the data memory
 * @param Size data size in bytes
 * @return @ref EXTMEM_StatusTypeDef
 **/
EXTMEM_StatusTypeDef EXTMEM_EraseSector(uint32_t MemId, uint32_t Address, uint32_t Size);

/**
 * @brief This function erases all the memory
 *
 * @param MemId memory id
 * @return @ref EXTMEM_StatusTypeDef
 **/
EXTMEM_StatusTypeDef EXTMEM_EraseAll(uint32_t MemId);

/**
 * @brief This function returns information about the memory
 *
 * @param MemId memory id
 * @param Info  structure information of the memory
 * @return @ref EXTMEM_StatusTypeDef
 **/
EXTMEM_StatusTypeDef EXTMEM_GetInfo(uint32_t MemId, void *Info);

/**
 * @brief This function enables/disables the memory mapped mode 
 *
 * @param MemId memory id
 * @param State @ref EXTMEM_StateTypeDef
 * @return @ref EXTMEM_StatusTypeDef
 **/
EXTMEM_StatusTypeDef EXTMEM_MemoryMappedMode(uint32_t MemId, EXTMEM_StateTypeDef State);

/**
 * @brief This function gets the memory mapped address 
 *
 * @param MemId memory id
 * @param BaseAddress memory map address
 * @return @ref EXTMEM_StatusTypeDef
 **/
EXTMEM_StatusTypeDef EXTMEM_GetMapAddress(uint32_t MemId, uint32_t *BaseAddress);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_EXTMEM_H_ */
