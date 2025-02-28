/**
  ******************************************************************************
  * @file    jpeg_utils_conf.h
  * @author  MCD Application Team
  * @brief   jpeg_utils configuration file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

 /* Define to prevent recursive inclusion -------------------------------------*/

#ifndef  __JPEG_UTILS_CONF_H__
#define  __JPEG_UTILS_CONF_H__

/* Includes ------------------------------------------------------------------*/

#include "stm32h7rsxx_hal.h"
#include "stm32h7rsxx_hal_jpeg.h"

/* Private define ------------------------------------------------------------*/
/** @addtogroup JPEG_Private_Defines
  * @{
  */
/* RGB Color format definition for JPEG encoding/Decoding : Should not be modified*/
#define JPEG_ARGB8888            0  /* ARGB8888 Color Format */
#define JPEG_RGB888              1  /* RGB888 Color Format   */
#define JPEG_RGB565              2  /* RGB565 Color Format   */

/*
 * Define USE_JPEG_DECODER
 */

#define USE_JPEG_DECODER 1 /* 1 or 0 ********* Value different from default value : 1 ********** */
/*
 * Define USE_JPEG_ENCODER
 */

#define USE_JPEG_ENCODER 1 /* 1 or 0 ********* Value different from default value : 1 ********** */

/*
 * Define JPEG_RGB_FORMAT
 */
#define JPEG_RGB_FORMAT JPEG_ARGB8888 /* JPEG_ARGB8888, JPEG_RGB888, JPEG_RGB565 ********* Value different from default value : 0 ********** */

/*
 * Define JPEG_SWAP_RG
 */
#define JPEG_SWAP_RG 0 /* 0 or 1 ********* Value different from default value : 0 ********** */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __JPEG_UTILS_CONF_H__ */
