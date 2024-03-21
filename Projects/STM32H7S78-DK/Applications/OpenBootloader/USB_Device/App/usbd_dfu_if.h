/**
  ******************************************************************************
  * @file           usbd_dfu_it.h
  * @author         MCD Application Team
  * @brief          Header for usbd_dfu_flash.c file.
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
#ifndef __USBD_DFU_FLASH_H__
#define __USBD_DFU_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_dfu.h"

/** MEDIA Interface callback. */
extern USBD_DFU_MediaTypeDef USBD_DFU_Media_fops;
extern USBD_HandleTypeDef hUsbDeviceFS;

#ifdef __cplusplus
}
#endif

#endif /* __USBD_DFU_FLASH_H__ */
