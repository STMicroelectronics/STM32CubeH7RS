/**
  ******************************************************************************
  * @file    Examples/BSP/Src/audio_record.c
  * @author  MCD Application Team
  * @brief   This example describes how to use DFSDM HAL API to realize
  *          audio recording.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <string.h>

/** @addtogroup STM32H7RSxx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
  BUFFER_OFFSET_NONE = 0,
  BUFFER_OFFSET_HALF = 1,
  BUFFER_OFFSET_FULL = 2,
}BUFFER_StateTypeDef;

/* Private define ------------------------------------------------------------*/
#define AUDIO_FREQUENCY        16000U
#define AUDIO_IN_BUFFER_SIZE   2048
#define AUDIO_OUT_BUFFER_SIZE  4*AUDIO_IN_BUFFER_SIZE
#define AUDIO_NB_BLOCKS    ((uint32_t)4)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ALIGN_32BYTES (uint16_t RecBuffer[AUDIO_IN_BUFFER_SIZE]);
ALIGN_32BYTES (uint16_t RecPlayback[AUDIO_OUT_BUFFER_SIZE]);

/* Pointer to record_data */
uint32_t PlaybackPtr = 0;

uint32_t  InState = 0;
uint32_t  OutState = 0;

BUFFER_StateTypeDef AudioBufferOffset;
BSP_AUDIO_Init_t AudioInInit;
BSP_AUDIO_Init_t AudioOutInit;

static uint32_t AudioFreq[9] = {8000, 11025, 16000, 22050, 32000, 44100, 48000, 96000, 192000};
uint32_t *AudioFreq_ptr;

uint32_t VolumeLevel = 70, AudioOutState, AudioInState, MuteState;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Audio Record and Playback multi-buffer mode test
  *         Record:
  *          - Audio IN instance: 1 (DFSDM)
  *          - Audio IN Device  : digital MIC1 and MIC2
  *          - Audio IN number of channel  : 2
  *         Playback:
  *          - Audio OUT instance: 0 (SAI)
  *          - Audio OUT Device  : HDMI
  * @retval None
  */
void AudioRecord_demo(void)
{
  uint32_t channel_nbr = 2;
  uint32_t x_size, y_size;

  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  /* Clear the LCD */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
  /* Set Audio Demo description */
  UTIL_LCD_FillRect(0, 0, x_size, 90, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *)"AUDIO RECORD I2S EXAMPLE", CENTER_MODE);
  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_DisplayStringAt(0, 40,  (uint8_t *)"Press USER button for next menu", CENTER_MODE);
  /* Set the LCD Text Color */
  UTIL_LCD_DrawRect(10, 100, x_size - 20, y_size - 110, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DrawRect(11, 101, x_size - 22, y_size - 112, UTIL_LCD_COLOR_BLUE);

  AudioFreq_ptr = AudioFreq+2; /* AUDIO_FREQUENCY_16K; */

  AudioOutInit.Device = AUDIO_OUT_HEADPHONE;
  AudioOutInit.ChannelsNbr = channel_nbr;
  AudioOutInit.SampleRate = *AudioFreq_ptr;
  AudioOutInit.BitsPerSample = AUDIO_RESOLUTION_16B;
  AudioOutInit.Volume = VolumeLevel;

  AudioInInit.Device = AUDIO_IN_ANALOG_MIC;
  AudioInInit.ChannelsNbr = 1;
  AudioInInit.SampleRate = *AudioFreq_ptr;
  AudioInInit.BitsPerSample = AUDIO_RESOLUTION_16B;
  AudioInInit.Volume = VolumeLevel;

  /* Initialize Audio Recorder with 2 channels to be used */
  BSP_AUDIO_IN_Init(1, &AudioInInit);
  BSP_AUDIO_IN_GetState(1, &InState);
  BSP_AUDIO_IN_SetSampleRate(1, AudioInInit.SampleRate);

  BSP_AUDIO_OUT_Init(0, &AudioOutInit);

  UTIL_LCD_SetFont(&Font20);
  /* Start Recording */
  UTIL_LCD_DisplayStringAt(0, 190, (uint8_t *)"Start Recording ", CENTER_MODE);
  BSP_AUDIO_IN_Record(1, (uint8_t*)&RecBuffer, 2*AUDIO_IN_BUFFER_SIZE);

  /* Play the recorded buffer*/
  UTIL_LCD_DisplayStringAt(0, 220, (uint8_t *)"Play the recorded buffer... ", CENTER_MODE);
  BSP_AUDIO_OUT_Play(0, (uint8_t*)&RecPlayback[0], 2*AUDIO_OUT_BUFFER_SIZE);

  while (1)
  {
    if (CheckForUserInput() > 0)
    {
      ClearUserInput();
      BSP_AUDIO_OUT_Stop(0);
      BSP_AUDIO_OUT_DeInit(0);
      BSP_AUDIO_IN_Stop(1);
      BSP_AUDIO_IN_DeInit(1);
      return;
    }
  }
}

/**
  * @brief Calculates the remaining file size and new position of the pointer.
  * @retval None
  */
void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance)
{
  uint32_t i;

  if(Instance == 1U)
  {
    /* Store values on Play buff */
    for (i = 0; i < (AUDIO_IN_BUFFER_SIZE / 2); i++)
    {
      RecPlayback[PlaybackPtr + (2*i)]     = RecBuffer[(AUDIO_IN_BUFFER_SIZE/2) + i];
      RecPlayback[PlaybackPtr + (2*i) + 1] = RecBuffer[(AUDIO_IN_BUFFER_SIZE/2) + i];
    }

    PlaybackPtr += AUDIO_IN_BUFFER_SIZE;///4/2;
    if(PlaybackPtr >= AUDIO_OUT_BUFFER_SIZE)
    {  PlaybackPtr = 0;
    }
  }
  else
  {
    AudioBufferOffset = BUFFER_OFFSET_FULL;
  }
}

/**
  * @brief  Manages the DMA Half Transfer complete interrupt.
  * @retval None
  */
void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance)
{
  uint32_t i;

  if(Instance == 1U)
  {
    /* Store values on Play buff */
    for (i = 0; i < (AUDIO_IN_BUFFER_SIZE / 2); i++)
    {
      RecPlayback[PlaybackPtr + (2*i)]     = RecBuffer[i];
      RecPlayback[PlaybackPtr + (2*i) + 1] = RecBuffer[i];
    }

    PlaybackPtr += AUDIO_IN_BUFFER_SIZE;///4/2;
    if(PlaybackPtr >= AUDIO_OUT_BUFFER_SIZE)
    {
      PlaybackPtr = 0;
    }
  }
  else
  {
    AudioBufferOffset = BUFFER_OFFSET_HALF;
  }
}

/**
  * @}
  */

/**
  * @}
  */

