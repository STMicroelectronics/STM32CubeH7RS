/**
  ******************************************************************************
  * @file    stm32h7s78_discovery_conf_template.h
  * @author  MCD Application Team
  * @brief   STM32H7S78-DK board configuration file.
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
#ifndef __STM32H7S78_DK_CONF_H
#define __STM32H7S78_DK_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_hal.h"

/* COM define */
#define USE_COM_LOG                         0U
#define USE_BSP_COM_FEATURE                 0U

/* Touch controllers defines */
#define USE_FT5336_TS_CTRL                 1U


#define LCD_LAYER_0_ADDRESS                 0x90000000U
#define LCD_LAYER_1_ADDRESS                 0x90200000U

/* Default Audio IN internal buffer size */
#define DEFAULT_AUDIO_IN_BUFFER_SIZE        2048U
/* TS supported features defines */
#define USE_TS_GESTURE                      1U
#define USE_TS_MULTI_TOUCH                  1U

/* Default TS touch number */
#define TS_TOUCH_NBR                        2U


/* IRQ priorities */
#define BSP_SDRAM_IT_PRIORITY               15U
#define BSP_BUTTON_USER_IT_PRIORITY         15U
#define BSP_AUDIO_OUT_IT_PRIORITY           14U
#define BSP_AUDIO_IN_IT_PRIORITY            15U
#define BSP_SD_IT_PRIORITY                  14U
#define BSP_SD_RX_IT_PRIORITY               14U
#define BSP_SD_TX_IT_PRIORITY               15U
#define BSP_TS_IT_PRIORITY                  15U
#define BSP_XSPI_RAM_IT_PRIORITY            15U
#define BSP_XSPI_RAM_DMA_IT_PRIORITY        15U

#ifdef __cplusplus
}
#endif

#endif /* __STM32H7S78_DK_CONF_H */
