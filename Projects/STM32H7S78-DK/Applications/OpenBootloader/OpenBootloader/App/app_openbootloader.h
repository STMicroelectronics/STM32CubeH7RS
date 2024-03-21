/**
  ******************************************************************************
  * @file    app_openbootloader.h
  * @author  MCD Application Team
  * @brief   Header for app_openbootloader.c module
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
#ifndef APP_OPENBOOTLOADER_H
#define APP_OPENBOOTLOADER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "openbl_mem.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define EXTENDED_SPECIAL_CMD_MAX_NUMBER   0x01U           /* Extended special command max length array */
#define SPECIAL_CMD_SYSTEM_RESET          0x02U           /* System reset special opcode */
#define SPECIAL_CMD_MAX_NUMBER            0x03U           /* Special command max length array */
#define SPECIAL_CMD_EXT_MEM_INIT          0xA0U           /* External memory initialization special command */
#define SPECIAL_CMD_EXT_MEM_ERASE         0xA4U           /* External memory erase special command */
#define SPECIAL_CMD_DEFAULT               0xFFU           /* Default special command */

#define SPECIAL_CMD_SUCCESS               0x00U           /* Special command success status */
#define SPECIAL_CMD_ERROR                 0x01U           /* Special command error status */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void OpenBootloader_Init(void);
void OpenBootloader_DeInit(void);
void OpenBootloader_ProtocolDetection(void);

/* External variables --------------------------------------------------------*/
extern OPENBL_MemoryTypeDef FLASH_Descriptor;
extern OPENBL_MemoryTypeDef RAM_Descriptor;
extern OPENBL_MemoryTypeDef DTCM_Descriptor;
extern OPENBL_MemoryTypeDef OB_Descriptor;
extern OPENBL_MemoryTypeDef OTP_Descriptor;
extern OPENBL_MemoryTypeDef ICP_Descriptor;
extern OPENBL_MemoryTypeDef RO_Descriptor;
extern OPENBL_MemoryTypeDef EB_Descriptor;

extern uint16_t SpecialCmdList[SPECIAL_CMD_MAX_NUMBER];
extern uint16_t ExtendedSpecialCmdList[EXTENDED_SPECIAL_CMD_MAX_NUMBER];

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* APP_OPENBOOTLOADER_H */
