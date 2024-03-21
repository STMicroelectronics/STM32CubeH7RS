/**
  ******************************************************************************
  * @file           usb_device.h
  * @author         MCD Application Team
  * @brief          Header for usb_device.c file.
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
#ifndef USB_DEVICE__H
#define USB_DEVICE__H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "usbd_def.h"

/* Private variables ---------------------------------------------------------*/
extern USBD_DescriptorsTypeDef DFU_Desc;

/* Private function prototypes -----------------------------------------------*/
void MX_USB_Device_Init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* USB_DEVICE__H */
