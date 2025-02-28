/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           usbd_conf.h
  * @author         MCD Application Team
  * @brief          Header for usbd_conf.c file.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CONF_H
#define __USBD_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "stm32h7rsxx.h"
#include "stm32h7rsxx_hal.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/** @addtogroup USBD_OTG_DRIVER
  * @brief Driver for Usb device.
  * @{
  */

/** @defgroup USBD_CONF USBD_CONF
  * @brief Configuration file for Usb otg low level driver.
  * @{
  */

/** @defgroup USBD_CONF_Exported_Variables USBD_CONF_Exported_Variables
  * @brief Public variables.
  * @{
  */
/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* USER CODE END PV */
/**
  * @}
  */

/** @defgroup USBD_CONF_Exported_Defines USBD_CONF_Exported_Defines
  * @brief Defines for configuration of the Usb device.
  * @{
  */

/*---------- -----------*/
#define USBD_MAX_NUM_INTERFACES     1U
/*---------- -----------*/
#define USBD_MAX_NUM_CONFIGURATION     1U
/*---------- -----------*/
#define USBD_MAX_STR_DESC_SIZ     0x100U
/*---------- -----------*/
#define USBD_DEBUG_LEVEL     0U
/*---------- -----------*/
#define USBD_LPM_ENABLED     0U
/*---------- -----------*/
#define USBD_SELF_POWERED     1U

/****************************************/
/* #define for FS and HS identification */
#define DEVICE_FS               0
#define DEVICE_HS               1

/****************************************/
#define UVC_1_1
#define USBD_UVC_FORMAT_UNCOMPRESSED


#ifdef USBD_UVC_FORMAT_UNCOMPRESSED
#define UVC_USE_STREAM1
#define UVC_BITS_PER_PIXEL                          12U
#define UVC_UNCOMPRESSED_GUID                       UVC_GUID_NV12
/* refer to Table 3-18 Color Matching Descriptor video class v1.1 */
#define UVC_COLOR_PRIMARIE                          0x01U
#define UVC_TFR_CHARACTERISTICS                     0x01U
#define UVC_MATRIX_COEFFICIENTS                     0x04U
#else
#define UVC_USE_STREAM2
#endif

/****************************************/
/* Video Stream frame width and height */
#ifdef UVC_USE_STREAM2
#define UVC_WIDTH                                   320U
#define UVC_HEIGHT                                  236U
#endif

#ifdef UVC_USE_STREAM1
#define UVC_WIDTH                                   176U
#define UVC_HEIGHT                                  144U
#endif

   /* bEndpointAddress in Endpoint Descriptor */
#define UVC_IN_EP                                   0x81U

#define UVC_CAM_FPS_FS                              10U
#define UVC_CAM_FPS_HS                              5U

//#define UVC_ISO_FS_MPS                            300U
#define UVC_HEADER_PACKET_CTN                     0x01U
#define UVC_PACKET_SIZE                           (UVC_ISO_FS_MPS * UVC_HEADER_PACKET_CTN)

#define UVC_MAX_FRAME_SIZE                          (UVC_WIDTH * UVC_HEIGHT * 16U / 8U)

/* Time laps between video frames in ms.
   Please adjust this value depending on required speed.
   Please note that this define uses the system HAL_Delay() which uses the systick.
   In case of changes on HAL_Delay, please ensure the values in ms correspond. */
#ifdef USE_USB_HS
#define USBD_VIDEO_IMAGE_LAPS                     160
#else
#define USBD_VIDEO_IMAGE_LAPS                     80U
#endif /* USE_USB_HS */



/**
  * @}
  */

/** @defgroup USBD_CONF_Exported_Macros USBD_CONF_Exported_Macros
  * @brief Aliases.
  * @{
  */
/* Memory management macros make sure to use static memory allocation */
/** Alias for memory allocation. */

#define USBD_malloc         malloc

/** Alias for memory release. */
#define USBD_free           free

/** Alias for memory set. */
#define USBD_memset         memset

/** Alias for memory copy. */
#define USBD_memcpy         memcpy

/** Alias for delay. */
#define USBD_Delay          HAL_Delay

/* DEBUG macros */

#if (USBD_DEBUG_LEVEL > 0)
#define USBD_UsrLog(...)    printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_UsrLog(...)
#endif /* (USBD_DEBUG_LEVEL > 0U) */

#if (USBD_DEBUG_LEVEL > 1)

#define USBD_ErrLog(...)    printf("ERROR: ");\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_ErrLog(...)
#endif /* (USBD_DEBUG_LEVEL > 1U) */

#if (USBD_DEBUG_LEVEL > 2)
#define USBD_DbgLog(...)    printf("DEBUG : ");\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_DbgLog(...)
#endif /* (USBD_DEBUG_LEVEL > 2U) */

/**
  * @}
  */

/** @defgroup USBD_CONF_Exported_Types USBD_CONF_Exported_Types
  * @brief Types.
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_CONF_Exported_FunctionsPrototype USBD_CONF_Exported_FunctionsPrototype
  * @brief Declaration of public functions for Usb device.
  * @{
  */

/* Exported functions -------------------------------------------------------*/

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

#endif /* __USBD_CONF_H */

