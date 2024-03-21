/**
  ******************************************************************************
  * @file    usb_interface.h
  * @author  MCD Application Team
  * @brief   Contains USB protocol commands
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
#ifndef USB_INTERFACE_H
#define USB_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "usbd_def.h"

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern __IO uint8_t DfuJumpToUserApp;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void OPENBL_USB_Configuration(void);
void OPENBL_USB_DeInit(void);
uint8_t OPENBL_USB_ProtocolDetection(void);
uint16_t OPENBL_USB_SendAddressNack(USBD_HandleTypeDef *p_dev);
uint32_t OPENBL_USB_GetPage(uint32_t address);
uint16_t OPENBL_USB_Ext_ProcessDownloadCmd(uint8_t command, uint8_t *buffer, uint32_t w_length, uint32_t *status);
uint16_t OPENBL_USB_MassEraseCommand(void);
void OPENBL_USB_JumpToUserApplication(uint32_t address);
void OPENBL_USB_SetUserApplicationAddress(uint32_t addr);
uint16_t OPENBL_DFU_Check(uint8_t *address, uint32_t request_type, uint32_t *p_status);
uint16_t OPENBL_DFU_DownloadCommands(uint8_t *p_buffer, uint32_t block_number, uint32_t w_length, uint32_t *p_status);
uint16_t OPENBL_DFU_UploadCommands(uint32_t address, uint32_t block_number, uint32_t *p_status);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* USB_INTERFACE_H */
