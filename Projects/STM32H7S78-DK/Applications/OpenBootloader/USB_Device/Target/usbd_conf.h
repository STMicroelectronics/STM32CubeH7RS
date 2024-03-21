/**
  ******************************************************************************
  * @file    usbd_conf.h
  * @author  MCD Application Team
  * @brief   General low level driver configuration
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
#ifndef USBD_CONF_H
#define USBD_CONF_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Exported types ------------------------------------------------------------*/
/** @addtogroup USBD_OTG_DRIVER
  * @brief Driver for Usb device.
  * @{
  */
#define USBD_CLASS_USER_STRING_DESC            1U
/* Exported constants --------------------------------------------------------*/


/* Common Config */
#define USBD_MAX_NUM_INTERFACES               1U
#define USBD_MAX_NUM_CONFIGURATION            1U
#define USBD_MAX_STR_DESC_SIZ                 128U
#define USBD_SUPPORT_USER_STRING_DESC         1U
#define USBD_SELF_POWERED                     1U
#define USBD_DEBUG_LEVEL                      0U

#define USBD_DFU_VENDOR_CMD_ENABLED           1U
#define USBD_DFU_VENDOR_EXIT_ENABLED          1U
#define USBD_DFU_VENDOR_CHECK_ENABLED         1U

/* DFU Class Config */
#define USBD_DFU_MAX_ITF_NUM                   1U
#define USBD_DFU_XFER_SIZE                     1024U      /* Max DFU Packet Size = 1024 bytes */
#define USBD_DFU_APP_DEFAULT_ADD               0x08000000U /* The first sector (32 KB) is reserved for DFU code */
#define USBD_DFU_MAX_NB_OF_SECTORS             8U         /* Max number of sectors */

/* #define for FS and HS identification */
#define DEVICE_FS     0U

/* Exported macro ------------------------------------------------------------*/
/* Memory management macros */
/** Alias for memory allocation. */
#define USBD_malloc         (void *)USBD_static_malloc

/** Alias for memory release. */
#define USBD_free           USBD_static_free

/** Alias for memory set. */
#define USBD_memset         memset

/** Alias for memory copy. */
#define USBD_memcpy         memcpy
/** Alias for delay. */
#define USBD_Delay          HAL_Delay

/* DEBUG macros */
#if (USBD_DEBUG_LEVEL > 0U)
#define  USBD_UsrLog(...)   printf(__VA_ARGS__);\
  printf("\n");
#else
#define USBD_UsrLog(...)
#endif /* if (USBD_DEBUG_LEVEL > 0U) */

#if (USBD_DEBUG_LEVEL > 1U)

#define  USBD_ErrLog(...)   printf("ERROR: ") ;\
  printf(__VA_ARGS__);\
  printf("\n");
#else
#define USBD_ErrLog(...)
#endif /* if (USBD_DEBUG_LEVEL > 1U) */

#if (USBD_DEBUG_LEVEL > 2U)
#define  USBD_DbgLog(...)   printf("DEBUG : ") ;\
  printf(__VA_ARGS__);\
  printf("\n");
#else
#define USBD_DbgLog(...)
#endif /* if (USBD_DEBUG_LEVEL > 2U) */

extern PCD_HandleTypeDef USBFS_Hpcd;
extern uint8_t UsbSofDetected;

/* Exported functions -------------------------------------------------------*/
void *USBD_static_malloc(uint32_t size);
void USBD_static_free(void *p);

#endif /* __USBD_CONF_H */
