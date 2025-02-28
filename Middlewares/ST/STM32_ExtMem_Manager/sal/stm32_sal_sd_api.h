/**
  ******************************************************************************
  * @file    stm32_sal_sd_api.h
  * @author  MCD Application Team
  * @brief   This file contains the software adaptation layer SD functions
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
#ifndef __STM32_SAL_SD_API_H
#define __STM32_SAL_SD_API_H

#ifdef __cplusplus
 extern "C" {
#endif

/** @addtogroup SAL_SD
  * @ingroup EXTMEM_SAL
  * @{
  */

/** @defgroup SAL_SD
  * @{
  */

/* Includes ------------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/** @defgroup SAL_SD_Exported_types SAL SD exported types
  * @{
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup SAL_SD_Exported_Functions SAL XSP Exported Functions
  * @{
  */

/**
 * @brief This function initializes the SD SAL context
 * @param SalSD SAL SD Object
 * @param HALHandle HAL handle
 * @param Info  information on the SD card
 * @return @ref HAL_StatusTypeDef
 **/
HAL_StatusTypeDef SAL_SD_Init(SAL_SD_ObjectTypeDef* SalSD, void* HALHandle, EXTMEM_DRIVER_SDCARD_InfoTypeDef* Info);

/**
 * @brief This function un-initializes the SD SAL context
 * @param SalSD SAL SD Object
 * @return @ref HAL_StatusTypeDef
 **/
HAL_StatusTypeDef SAL_SD_DeInit(SAL_SD_ObjectTypeDef* SalSD);

/**
 * @brief This function reads data from the SD
 * @param SalSD SAL SD Object
 * @param BlockIdx block index
 * @param Data data pointer
 * @param NumberOfBlock number of block
 * @return @ref HAL_StatusTypeDef
 **/
HAL_StatusTypeDef SAL_SD_ReadData(SAL_SD_ObjectTypeDef* SalSD, uint32_t BlockIdx, uint8_t* Data, uint32_t NumberOfBlock);

/**
 * @brief This function writes data on the SD
 * @param SalSD SAL SD Object
 * @param BlockIdx block index
 * @param Data data pointer
 * @param NumberOfBlock number of block
 * @return @ref HAL_StatusTypeDef
 **/
HAL_StatusTypeDef SAL_SD_WriteData(SAL_SD_ObjectTypeDef* SalSD, uint32_t BlockIdx, const uint8_t* const Data, uint32_t NumberOfBlock);

/**
 * @brief This function erases an amount of blocks on the SD
 * @param SalSD SAL SD Object
 * @param BlockIdx block index
 * @param BlockCount number of block
 * @return @ref HAL_StatusTypeDef
 **/
HAL_StatusTypeDef SAL_SD_EraseBlock(SAL_SD_ObjectTypeDef* SalSD, uint32_t BlockIdx, uint32_t BlockCount);

/**
 * @brief This function erases all the SD
 * @param SalSD SAL SD Object
 * @return @ref HAL_StatusTypeDef
 **/
HAL_StatusTypeDef SAL_SD_MassErase(SAL_SD_ObjectTypeDef* SalSD);

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

#endif /* __STM32_SAL_SD_API_H */