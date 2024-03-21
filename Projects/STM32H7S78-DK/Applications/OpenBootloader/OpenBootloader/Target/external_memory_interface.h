/**
  ******************************************************************************
  * @file    external_memory_interface.h
  * @author  MCD Application Team
  * @brief   Header for external_memory_interface.c module
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

#ifndef EXTERNAL_MEMORY_INTERFACE_H
#define EXTERNAL_MEMORY_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "openbl_core.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
ErrorStatus OPENBL_ExtMem_Init(uint32_t address);
uint8_t OPENBL_ExtMem_Read(uint32_t address);
void OPENBL_ExtMem_Write(uint32_t address, uint8_t *p_data, uint32_t data_length);
void OPENBL_ExtMem_JumpToAddress(uint32_t address);
ErrorStatus OPENBL_ExtMem_Erase(OPENBL_SpecialCmdTypeDef *p_special_cmd);

#ifdef __cplusplus
}
#endif

#endif /* EXTERNAL_MEMORY_INTERFACE_H */
