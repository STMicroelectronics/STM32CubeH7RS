/**
  ******************************************************************************
  * @file    usbh_diskio_config.h
  * @author  MCD Application Team
  * @brief   Template for the usbh_diskio_config.h. This file should be copied and
             under project.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the st_license.txt
  * file in the root directory of this software component.
  * If no st_license.txt file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef USBH_DISKIO_CONFIG_H
#define USBH_DISKIO_CONFIG_H


#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include "usbh_msc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Block size */
#define USB_BLOCK_SIZE 512

extern USBH_HandleTypeDef hUsbHost;
/* Default handle used in usbh_diskio.c file */
#define hUsb_Host hUsbHost

#ifdef __cplusplus
}
#endif

#endif /* USBH_DISKIO_CONFIG_H */