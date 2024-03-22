/**
  ******************************************************************************
  * @file    usart_interface.h
  * @author  MCD Application Team
  * @brief   Header for usart_interface.c module
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
#ifndef USART_INTERFACE_H
#define USART_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void OPENBL_USART_Configuration(void);
void OPENBL_USART_DeInit(void);
uint8_t OPENBL_USART_ProtocolDetection(void);

uint8_t OPENBL_USART_GetCommandOpcode(void);
uint8_t OPENBL_USART_ReadByte(void);
void OPENBL_USART_SendByte(uint8_t byte);
void OPENBL_USART_SpecialCommandProcess(OPENBL_SpecialCmdTypeDef *p_special_cmd);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* USART_INTERFACE_H */
