/**
  ******************************************************************************
  * @file    openbootloader_conf.h
  * @author  MCD Application Team
  * @brief   Contains Open Bootloader configuration
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
#ifndef OPENBOOTLOADER_CONF_H
#define OPENBOOTLOADER_CONF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* -------------------------------- Device ID --------------------------------*/
#define DEVICE_ID_MSB                     0x04U  /* MSB byte of device ID */
#define DEVICE_ID_LSB                     0x85U  /* LSB byte of device ID */

/*--------------------------- Definitions for Memories -----------------------*/
#define FLASH_MEM_SIZE                    (64U * 1024U)                           /* Size of FLASH 64 kByte */
#define FLASH_MEM_START_ADDRESS           0x08000000U                             /* Start of Flash */
#define FLASH_MEM_END_ADDRESS             (FLASH_BASE + FLASH_MEM_SIZE)           /* End of Flash */

#define RAM_MEM_SIZE                      (456U * 1024U)                          /* Size of RAM 456 kByte */
#define RAM_MEM_START_ADDRESS             0x24000000U                             /* Start of SRAM */
#define RAM_MEM_END_ADDRESS               (RAM_MEM_START_ADDRESS + RAM_MEM_SIZE)  /* End of SRAM */

#define ICP_MEM_SIZE                      (32U * 1024U)                           /* Size of ICP 32 kByte */
#define ICP_MEM_START_ADDRESS             0x1FF18000U                             /* System memory start address */
#define ICP_MEM_END_ADDRESS               (ICP_MEM_START_ADDRESS + ICP_MEM_SIZE)  /* System memory end address*/

#define OB_MEM_SIZE                       112U                                    /* Size of OB 112 bytes */
#define OB_MEM_START_ADDRESS              0x52002200U                             /* Option bytes start address */
#define OB_MEM_END_ADDRESS                (OB_MEM_START_ADDRESS + OB_MEM_SIZE)    /* Option bytes end address */

#define OTP_MEM_SIZE                      (1024U)                                 /* Size of OTP 1024 bytes */
#define OTP_MEM_START_ADDRESS             0x08FFF000U                             /* OTP start address */
#define OTP_MEM_END_ADDRESS               (OTP_MEM_START_ADDRESS + OTP_MEM_SIZE)  /* OTP end address */

#define RO_MEM_SIZE                       (2U * 1024U)                            /* Size of Read Only memory 2 kByte */
#define RO_MEM_START_ADDRESS              0x08FFF800U                             /* Read only memory start address */
#define RO_MEM_END_ADDRESS                (RO_MEM_START_ADDRESS + RO_MEM_SIZE)    /* Read Only memory end address */

#define EB_MEM_SIZE                       108U                                    /* Size of Engi bytes 108 bytes */
#define EB_MEM_START_ADDRESS              0x52002800U                             /* Engi bytes start address */
#define EB_MEM_END_ADDRESS                (EB_MEM_START_ADDRESS + EB_MEM_SIZE)    /* Engi bytes end address */

#define OPENBL_RAM_SIZE                   0x00004100U                    /* RAM used by the Open Bootloader 20 kBytes */

#define OPENBL_DEFAULT_MEM                FLASH_MEM_START_ADDRESS        /* Used for Erase and Write protect CMD */

#define RDP_LEVEL_0                       0xEEEEEEEEU
#define RDP_LEVEL_1                       0xEEEEEEFFU

#define AREA_ERROR                        0x0U  /* Error Address Area */
#define FLASH_AREA                        0x1U  /* Flash Address Area */
#define RAM_AREA                          0x2U  /* RAM Address area */
#define ICP_AREA                          0x3U  /* BIF Address area */
#define OB_AREA                           0x4U  /* Option bytes Address area */
#define OTP_AREA                          0x5U  /* OTP Address area */
#define RO_AREA                           0x6U  /* RO Address area */
#define EB_AREA                           0x7U  /* Engi bytes Address area */
#define EXTERNAL_MEMORY_AREA              0x8U  /* External Memory Area */

#define FLASH_MASS_ERASE                  0xFFFFU

#define INTERFACES_SUPPORTED              7U

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OPENBOOTLOADER_CONF_H */
