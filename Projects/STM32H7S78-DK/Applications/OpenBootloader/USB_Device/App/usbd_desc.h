/**
  ******************************************************************************
  * @file           usbd_desc.h
  * @author         MCD Application Team
  * @brief          Header for usbd_desc.c file.
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
#ifndef USBD_DESC__H
#define USBD_DESC__H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "usbd_def.h"

#define         DEVICE_ID1          (UID_BASE)
#define         DEVICE_ID2          (UID_BASE + 0x4U)
#define         DEVICE_ID3          (UID_BASE + 0x8U)

#define  USB_SIZ_STRING_SERIAL          0x1AU

extern USBD_DescriptorsTypeDef     DFU_Desc;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* USBD_DESC__H */
