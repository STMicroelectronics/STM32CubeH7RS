/**
  ******************************************************************************
  * @file    stm32_sfdp_type.h
  * @author  MCD Application Team
  * @brief   This file contains the sfdp driver definition.
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
#ifndef __STM32_SFDP_TYPE_H
#define __STM32_SFDP_TYPE_H

#ifdef __cplusplus
 extern "C" {
#endif

   
/* Includes ------------------------------------------------------------------*/
/** @addtogroup NOR_SFDP
  * @ingroup EXTMEM_DRIVER
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/** @defgroup DRIVER_SFDP_Exported_Types DRIVER SFDP Memory Exported Types
  * @{
  */

/**
 * @brief driver data used to manage the flash
 */
typedef struct {

  SAL_XSPI_PhysicalLinkTypeDef SpiPhyLink;           /*!< physical link information */
  uint32_t ClockIn;                                  /*!< clock d'entree */

  /* Table of instructions an instruction equal to zero means not supported */
  /* WIP : write in progress */
  uint8_t ReadWIPCommand;                            /*!< read write in progress command */
  uint8_t WIPPosition;                               /*!< write in progress byte position */
  uint8_t WIPBusyPolarity;                           /*!< 0: Positive (WIP=1 means write is in progress) 1: Inverted (WIP=0 means write is in progress) */
  uint8_t WIPAddress;                                /*!< read write in progress address */

  /* WEL: write enable */
  uint8_t WriteWELCommand;                           /*!< write enable command */

  /* WEL : read write enable */
  uint8_t ReadWELCommand;                            /*!< read write enable status command */
  uint8_t WELPosition;                               /*!< write enable position */
  uint8_t WELBusyPolarity;                           /*!< 0: Positive (WIP=1 means write is in progress) 1: Inverted (WIP=0 means write is in progress) */
  uint8_t WELAddress;                                /*!< write enable address */

  /* Page Program */
  uint8_t PageProgramInstruction;                    /*!< page program command */

  /* Read management */
  uint8_t ReadInstruction;                           /*!< read command */
  uint8_t ReadDTRInstruction;                        /*!< read DTR command */

  /* Erase management */
  uint8_t EraseType1Size;                            /*!< erase 1 size */
  uint8_t EraseType1Command;                         /*!< erase 1 command */
  uint8_t EraseType2Size;                            /*!< erase 2 size */
  uint8_t EraseType2Command;                         /*!< erase 2 command */
  uint8_t EraseType3Size;                            /*!< erase 3 size */
  uint8_t EraseType3Command;                         /*!< erase 3 command */
  uint8_t EraseType4Size;                            /*!< erase 4 size */
  uint8_t EraseType4Command;                         /*!< erase 4 command */

  uint32_t EraseType1Timing;                         /*!< erase 1 timing */
  uint32_t EraseType2Timing;                         /*!< erase 2 timing */
  uint32_t EraseType3Timing;                         /*!< erase 3 timing */
  uint32_t EraseType4Timing;                         /*!< erase 4 timing */
  uint32_t EraseChipTiming;                          /*!< erase chip timing */
} EXTMEM_DRIVER_NOR_SFDP_InfoTypeDef;


/**
 * @brief driver SFDP Object definition
 */
typedef struct {
  struct {
  SAL_XSPI_ObjectTypeDef        SALObject;         /*!< Instance of the memory */
  EXTMEM_LinkConfig_TypeDef       Config;          /*!< memory configuration */
  uint8_t                         FlashSize;       /*!< Flash size in power of two */
  uint32_t                        PageSize;        /*!< Page size */
  EXTMEM_DRIVER_NOR_SFDP_InfoTypeDef  DriverInfo;  /*!< driver information */
  uint32_t                        Sfdp_table_mask; /*!< sfdp table mask */
  uint32_t                        Reset_info;      /*!< this bit is a copy of JEDEC Basic 16 Reset/Rescue info */
  } sfpd_private;
} EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef;

/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/**
  * @addtogroup DRIVER_SFDP_Exported_Functions DRIVER SFDP exported functions
  * @{
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

#endif /* __STM32_SFDP_TYPE_H */
