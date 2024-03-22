/**
  ******************************************************************************
  * @file    i2c_interface.h
  * @author  MCD Application Team
  * @brief   Header for i2c_interface.c module
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
#ifndef I2C_INTERFACE_H
#define I2C_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "openbl_core.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void OPENBL_I2C_Configuration(void);
void OPENBL_I2C_DeInit(void);
uint8_t OPENBL_I2C_ProtocolDetection(void);

uint8_t OPENBL_I2C_GetCommandOpcode(void);
uint8_t OPENBL_I2C_ReadByte(void);
void OPENBL_I2C_SendByte(uint8_t byte);
void OPENBL_I2C_WaitAddress(void);
void OPENBL_I2C_SendAcknowledgeByte(uint8_t acknowledge);
void OPENBL_I2C_SpecialCommandProcess(OPENBL_SpecialCmdTypeDef *p_special_cmd);
void OPENBL_Enable_BusyState_Sending(void);
void OPENBL_Disable_BusyState_Sending(void);

#if defined (__ICCARM__)
__ramfunc void OPENBL_I2C_WaitNack(void);
__ramfunc void OPENBL_I2C_WaitStop(void);
__ramfunc void OPENBL_I2C_SendBusyByte(void);
#else
__attribute__((section(".ramfunc"))) void OPENBL_I2C_WaitNack(void);
__attribute__((section(".ramfunc"))) void OPENBL_I2C_WaitStop(void);
__attribute__((section(".ramfunc"))) void OPENBL_I2C_SendBusyByte(void);
#endif /* (__ICCARM__) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* I2C_INTERFACE_H */
