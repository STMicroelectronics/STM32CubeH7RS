/**
  ******************************************************************************
  * @file    stm32h7s78_discovery_conf.h
  * @author  MCD Application Team
  * @brief   STM32H7S78-DK board configuration file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32H7S78_DISCOVERY_CONF_H
#define STM32H7S78_DISCOVERY_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_hal.h"

/* COM defines */
#define USE_COM_LOG                         0U
#define USE_BSP_COM_FEATURE                 0U

/* Touch Sensing controller defines */
#define USE_FT5336_TS_CTRL                  1U
/* TS supported features defines */
#define USE_TS_GESTURE                      1U
#define USE_TS_MULTI_TOUCH                  1U
/* Default TS touch number */
#define TS_TOUCH_NBR                        2U

/* LCD defines */
#define LCD_LAYER_0_ADDRESS                 0x90000000U
#define LCD_LAYER_1_ADDRESS                 0x90200000U

/* Default Audio IN internal buffer size */
#define DEFAULT_AUDIO_IN_BUFFER_SIZE        2048U

/* IRQ priorities (Default is 15 as lowest priority level) */
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

#endif /* STM32H7S78_DISCOVERY_CONF_H */
