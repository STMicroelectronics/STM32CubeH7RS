/**
  ******************************************************************************
  * @file    flash_interface.h
  * @author  MCD Application Team
  * @brief   Header for flash_interface.c module
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
#ifndef FLASH_INTERFACE_H
#define FLASH_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "common_interface.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define FLASH_BUSY_STATE_ENABLED       (0xAAAA0000U)
#define FLASH_BUSY_STATE_DISABLED      (0x0000DDDDU)
#define PROGRAM_TIMEOUT                (0x000FFFFFU)

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void OPENBL_FLASH_JumpToAddress(uint32_t address);
void OPENBL_FLASH_Lock(void);
void OPENBL_FLASH_OB_Unlock(void);
uint8_t OPENBL_FLASH_Read(uint32_t address);
void OPENBL_FLASH_Write(uint32_t address, uint8_t *p_data, uint32_t data_length);
void OPENBL_FLASH_Unlock(void);
ErrorStatus OPENBL_FLASH_MassErase(uint8_t *p_data, uint32_t data_length);
ErrorStatus OPENBL_FLASH_Erase(uint8_t *p_data, uint32_t data_length);
uint32_t OPENBL_FLASH_GetProductState(void);
ErrorStatus OPENBL_FLASH_SetWriteProtection(FunctionalState state, uint8_t *p_sectors, uint32_t length);
void OPENBL_Enable_BusyState_Flag(void);
void OPENBL_Disable_BusyState_Flag(void);

#if defined (__ICCARM__)
__ramfunc HAL_StatusTypeDef OPENBL_FLASH_WaitForLastOperation(uint32_t timeout);
__ramfunc HAL_StatusTypeDef OPENBL_FLASH_ExtendedErase(FLASH_EraseInitTypeDef *p_erase_init, uint32_t *p_page_error);
#else
__attribute__((section(".ramfunc"))) HAL_StatusTypeDef OPENBL_FLASH_WaitForLastOperation(uint32_t timeout);
__attribute__((section(".ramfunc"))) HAL_StatusTypeDef OPENBL_FLASH_ExtendedErase(
  FLASH_EraseInitTypeDef *p_erase_init, uint32_t *p_page_error);
#endif /* (__ICCARM__) */

ErrorStatus OPENBL_FLASH_EnableWriteProtection(uint8_t *list_of_pages, uint32_t length);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FLASH_INTERFACE_H */
