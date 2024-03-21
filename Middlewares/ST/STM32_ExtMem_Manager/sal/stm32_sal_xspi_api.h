/**
  ******************************************************************************
  * @file    stm32_sal_xspi.h
  * @author  MCD Application Team
  * @brief   This file contains the software adaptation layer XSPI functions
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
#ifndef __SAL_XSPI_H
#define __SAL_XSPI_H

#ifdef __cplusplus
 extern "C" {
#endif

/** @defgroup SAL_XSPI
  * @ingroup EXTMEM_SAL
  * @{
  */

/* Includes ------------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/** @defgroup SAL_XSPI_Exported_types SAL XSPI exported types
  * @{
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup SAL_XSPI_Exported_Functions SAL XSP Exported Functions
  * @{
  */
/**
 * @brief this function initialize a memory
 * @param SalXspi SAL XSPI handle
 * @return @ref HAL_StatusTypeDef
 **/
HAL_StatusTypeDef SAL_XSPI_Init(SAL_XSPI_ObjectTypeDef* SalXspi, void* HALHandle);

/**
 * @brief this function get SFDP data
 * @param SalXspi SAL XSPI handle
 * @param Address address to read the data
 * @param Data Data pointer
 * @param DataSize size of the data ti read
 * @return @ref HAL_StatusTypeDef
 **/
HAL_StatusTypeDef SAL_XSPI_GetSFDP(SAL_XSPI_ObjectTypeDef* SalXspi, uint32_t Address, uint8_t* Data, uint32_t DataSize);

/**
 * @brief this function read data form the flash
 * @param SalXspi SAL XSPI handle
 * @param Command command to execute
 * @param Address address to read the data
 * @param Data Data pointer
 * @param DataSize size of the data ti read
 * @return @ref HAL_StatusTypeDef
 **/
HAL_StatusTypeDef SAL_XSPI_Read(SAL_XSPI_ObjectTypeDef* SalXspi, uint8_t Command, uint32_t Address, uint8_t* Data, uint32_t DataSize);

/**
 * @brief this function reads the flash ID
 * @param SalXspi SAL XSPI handle
 * @param Data data pointer
 * @param DataSize number of data to read
 * @return @ref HAL_StatusTypeDef
 **/
HAL_StatusTypeDef SAL_XSPI_GetId(SAL_XSPI_ObjectTypeDef* SalXspi, uint8_t* Data, uint32_t DataSize);

/**
 * @brief this function send a command on read the data
 * @param SalXspi SAL XSPI handle
 * @param Command command to execute
 * @param Data Data pointer
 * @param DataSize size of the data to read
 * @return @ref HAL_StatusTypeDef
 **/
HAL_StatusTypeDef SAL_XSPI_SendCommand(SAL_XSPI_ObjectTypeDef* SalXspi, uint8_t Command,
                                       uint8_t* Data, uint16_t DataSize);

/**
 * @brief this function controls the status register
 * @param SalXspi SAL XSPI handle
 * @param Command command to execute
 * @param Address specify the address
 * @param MatchValue  expected value
 * @param MatchMask   mask used to control the expected value
 * @param Timeout timeout parameter
 * @return @ref HAL_StatusTypeDef
 **/
HAL_StatusTypeDef SAL_XSPI_CheckStatusRegister(SAL_XSPI_ObjectTypeDef* SalXspi, uint8_t Command, uint32_t Address, uint8_t MatchValue, uint8_t MatchMask, uint32_t Timeout);

/**
 * @brief this function write data at an Address
 * @param SalXspi SAL XSPI handle
 * @param Command command to execute
 * @param Address address to write the data
 * @param Data Data pointer
 * @param DataSize size of the data to write
 * @return @ref HAL_StatusTypeDef
 **/
HAL_StatusTypeDef SAL_XSPI_Write(SAL_XSPI_ObjectTypeDef* SalXspi, uint8_t Command, uint32_t Address, const uint8_t* Data, uint32_t DataSize);

/**
 * @brief this function sends a command and an address
 * @param SalXspi SAL XSPI handle
 * @param Command command to execute
 * @param Address address to write the data
 * @return @ref HAL_StatusTypeDef
 **/
HAL_StatusTypeDef SAL_XSPI_SendCommandAddress(SAL_XSPI_ObjectTypeDef* SalXspi, uint8_t Command, uint32_t Address);

/**
 * @brief this function set the clock according the clock in and the expected clock
 * @param SalXspi SAL XSPI handle
 * @param ClockIn clock in input
 * @param ClockRequested clock requested
 * @param ClockReal pointer on the value of the real clock used
 * @return @ref HAL_StatusTypeDef
 **/
HAL_StatusTypeDef SAL_XSPI_SetClock(SAL_XSPI_ObjectTypeDef* SalXspi, uint32_t ClockIn, uint32_t ClockRequested, uint32_t* ClockReal);

/**
 * @brief this funstion set a configuration parameter
 * @param SalXspi SAL XSPI handle
 * @param ParmetersType @ref SAL_XSPI_MemParamTypeTypeDef
 * @param ParamVal pointer on the parameter value
 * @return @ref HAL_StatusTypeDef
 **/
HAL_StatusTypeDef SAL_XSPI_MemoryConfig(SAL_XSPI_ObjectTypeDef* SalXspi, SAL_XSPI_MemParamTypeTypeDef ParmetersType, void* ParamVal);

/**
 * @brief this function enables the memory mapped mode
 * @param SalXspi SAL XSPI handle
 * @param WrapCommand wrap command to execute in case of write operation
 * @param WrapDummy number of dummy cycle for the read operation
 * @return @ref HAL_StatusTypeDef
 **/
HAL_StatusTypeDef SAL_XSPI_ConfigureWrappMode(SAL_XSPI_ObjectTypeDef* SalXspi, uint8_t WrapCommand, uint8_t WrapDummy);

/**
 * @brief this function enables the memory mapped mode
 * @param SalXspi SAL XSPI handle
 * @param CommandRead command to execute in case of read operation
 * @param DummyRead number of dummy cycle for the read operation
 * @param CommandWrite command to execute in case of write operation
 * @param DummyWrite number of dummy cycle for the read operation
 * @return @ref HAL_StatusTypeDef
 **/
HAL_StatusTypeDef SAL_XSPI_EnableMapMode(SAL_XSPI_ObjectTypeDef *SalXspi, uint8_t CommandRead, uint8_t DummyRead,
                                         uint8_t CommandWrite, uint8_t DummyWrite);

/**
 * @brief this function disables the memory mapped mode
 * @param SalXspi SAL XSPI handle
 * @return @ref HAL_StatusTypeDef
 **/
HAL_StatusTypeDef SAL_XSPI_DisableMapMode(SAL_XSPI_ObjectTypeDef *SalXspi);

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

#endif /* __SAL_XSPI_H */


