/**
  ******************************************************************************
  * @file    audio.h
  * @author  MCD Application Team
  * @brief          : Header for audio.c file.
  *                   This file contains the common defines of the application.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AUDIO_H
#define __AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "audio.h"

/* Private typedef ----------------------------------------------------------- */
typedef enum {
  AUDIO_ERROR_NONE = 0,
  AUDIO_ERROR_IO,
  AUDIO_ERROR_EOF,
}AUDIO_ErrorTypeDef;

typedef enum {
  AUDIO_STATE_PLAYBACK_IDLE = 0,
  AUDIO_STATE_PLAYBACK_WAIT,
  AUDIO_STATE_PLAYBACK_EXPLORE,
  AUDIO_STATE_PLAYBACK_CONFIG,
  AUDIO_STATE_PLAYBACK_START,
  AUDIO_STATE_PLAYBACK_PLAY,
  AUDIO_STATE_PLAYBACK_NEXT,
}AUDIO_PLAYBACK_StateTypeDef;

/* Exported variables --------------------------------------------------------- */
extern AUDIO_PLAYBACK_StateTypeDef AUDIO_app_state;
extern AUDIO_PLAYBACK_StateTypeDef AUDIO_app_prev_state;
extern USBH_HandleTypeDef hUsbHostFS;

#ifdef __cplusplus
}
#endif

#endif /* __AUDIO_H */
