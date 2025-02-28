/**
  ******************************************************************************
  * @file    usbd_video_if.h
  * @author  MCD Application Team
  * @brief   Header file for the video Interface application layer functions file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_VIDEO_IF_H
#define __USBD_VIDEO_IF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_video.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief For Usb device.
  * @{
  */

/** @defgroup USBD_VIDEO_IF USBD_VIDEO_IF
  * @brief Usb VIDEO interface device module.
  * @{
  */

/** @defgroup USBD_VIDEO_IF_Exported_Defines USBD_VIDEO_IF_Exported_Defines
  * @brief Defines.
  * @{
  */

/* USER CODE BEGIN EXPORTED_DEFINES */

/* USER CODE END EXPORTED_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_VIDEO_IF_Exported_Types USBD_VIDEO_IF_Exported_Types
  * @brief Types.
  * @{
  */

/* USER CODE BEGIN EXPORTED_TYPES */

/* USER CODE END EXPORTED_TYPES */

/**
  * @}
  */

/** @defgroup USBD_VIDEO_IF_Exported_Macros USBD_VIDEO_IF_Exported_Macros
  * @brief Aliases.
  * @{
  */

/* USER CODE BEGIN EXPORTED_MACRO */

/* USER CODE END EXPORTED_MACRO */

/**
  * @}
  */

/** @defgroup USBD_VIDEO_IF_Exported_Variables USBD_VIDEO_IF_Exported_Variables
  * @brief Public variables.
  * @{
  */

/** VIDEO_IF Interface callback. */
extern USBD_VIDEO_ItfTypeDef USBD_VIDEO_fops;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_VIDEO_IF_Exported_FunctionsPrototype USBD_VIDEO_IF_Exported_FunctionsPrototype
  * @brief Public functions declaration.
  * @{
  */

/**
  * @brief  Manages the DMA full transfer complete event.
  * @retval None
  */
void TransferComplete_CallBack(void);

/**
  * @brief  Manages the DMA half transfer complete event.
  * @retval None
  */
void HalfTransfer_CallBack(void);

/* USER CODE BEGIN EXPORTED_FUNCTIONS */

/* USER CODE END EXPORTED_FUNCTIONS */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USBD_VIDEO_IF_H */
