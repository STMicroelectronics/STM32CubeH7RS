/**
  ******************************************************************************
  * @file    Examples/BSP/Src/audio_play.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the audio feature in the
  *          STM32H7S78-DK driver
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

/** @addtogroup STM32H7RSxx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/*Since SysTick is set to 1ms (unless to set it quicker) */
/* to run up to 48khz, a buffer around 1000 (or more) is requested*/
/* to run up to 96khz, a buffer around 2000 (or more) is requested*/
#define AUDIO_DEFAULT_VOLUME    70

/* Audio file size and start address are defined here since the audio file is
   stored in Flash memory as a constant table of 16-bit data */
#define AUDIO_START_OFFSET_ADDRESS    0            /* Offset relative to audio file header size */
#define AUDIO_BUFFER_SIZE             2048

/* Audio file size and start address are defined here since the audio file is
   stored in Flash memory as a constant table of 16-bit data */
#define AUDIO_START_OFFSET_ADDRESS    0            /* Offset relative to audio file header size */
/* Private typedef -----------------------------------------------------------*/

typedef enum {
  BUFFER_OFFSET_NONE = 0,
  BUFFER_OFFSET_HALF,
  BUFFER_OFFSET_FULL,
}BUFFER_StateTypeDef;

typedef struct {
  uint8_t buff[AUDIO_BUFFER_SIZE];
  uint32_t fptr;
  BUFFER_StateTypeDef state;
  uint32_t AudioFileSize;
  uint32_t *SrcAddress;
}AUDIO_BufferTypeDef;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ALIGN_32BYTES (static AUDIO_BufferTypeDef  buffer_ctl);
__IO uint32_t audio_state;
__IO uint32_t uwVolume = 20;
__IO uint32_t uwPauseEnabledStatus = 0;

uint32_t AudioFreq[8] = {96000, 48000, 44100, 32000, 22050, 16000, 11025, 8000};

BSP_AUDIO_Init_t AudioPlayInit;

/* Private function prototypes -----------------------------------------------*/
static void Audio_SetHint(uint32_t Index);
static uint32_t GetData(void *pdata, uint32_t offset, uint8_t *pbuf, uint32_t NbrOfData);
AUDIO_ErrorTypeDef AUDIO_Start(uint32_t *psrc_address, uint32_t file_size);

extern TS_Init_t hTS;
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Audio Play demo
  * @param  None
  * @retval None
  */
void AudioPlay_demo(void)
{
  uint32_t *AudioFreq_ptr;
  uint32_t y_size,x_size;
  uint32_t CurrentTickDetect =0;
  uint32_t LastTSTick = 0;
  uint32_t TS_Available = 1;
  uint16_t x1, y1;
  TS_State_t  TS_State;
  uint8_t VolStr[256] = {0};
  uint8_t FreqStr[256] = {0};
  int32_t ts_status = BSP_ERROR_NONE;
  int32_t nor_status = BSP_ERROR_NONE;
  Point Points2[] = {{226, 196}, {265, 223}, {226, 248}};
  BSP_XSPI_NOR_Init_t NorInit;

  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  AudioFreq_ptr = &AudioFreq[1]; /*48K*/
  UserButtonPressed = 0;

  uwPauseEnabledStatus = 1; /* 0 when audio is running, 1 when Pause is on */
  uwVolume = 60;

  Audio_SetHint(0);
  UTIL_LCD_SetFont(&Font20);

  UserButtonPressed = 0;

  /* External Nor Flash initialization to get audio file from */
  NorInit.InterfaceMode = MX66UW1G45G_OPI_MODE;
  NorInit.TransferRate = MX66UW1G45G_DTR_TRANSFER;

  if (BSP_XSPI_NOR_Init(0, &NorInit) != BSP_ERROR_NONE)
  {
    nor_status = BSP_ERROR_NO_INIT;
  }
  else if (BSP_XSPI_NOR_EnableMemoryMappedMode(0) != BSP_ERROR_NONE)
  {
    nor_status = BSP_ERROR_PERIPH_FAILURE;
  }

  if (nor_status != BSP_ERROR_NONE)
  {
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
    UTIL_LCD_DisplayStringAt(0, y_size - 95, (uint8_t *)"ERROR", CENTER_MODE);
    UTIL_LCD_DisplayStringAt(0, y_size - 80, (uint8_t *)"NOR flash XSPI cannot be initialized", CENTER_MODE);
  }

  hTS.Width = x_size;
  hTS.Height = y_size;
  hTS.Orientation = TS_SWAP_NONE;
  hTS.Accuracy = 10;

  /* Touchscreen initialization */
  ts_status = BSP_TS_Init(0, &hTS);

  if (ts_status != BSP_ERROR_NONE)
  {
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
    UTIL_LCD_DisplayStringAt(0, y_size - 95, (uint8_t *)"ERROR", CENTER_MODE);
    UTIL_LCD_DisplayStringAt(0, y_size - 80, (uint8_t *)"Touch Screen cannot be initialized", CENTER_MODE);
  }

  AudioPlayInit.Device = AUDIO_OUT_HEADPHONE;
  AudioPlayInit.ChannelsNbr = 2;
  AudioPlayInit.SampleRate = *AudioFreq_ptr;
  AudioPlayInit.BitsPerSample = AUDIO_RESOLUTION_16B;
  AudioPlayInit.Volume = uwVolume;

  if(BSP_AUDIO_OUT_Init(0, &AudioPlayInit) != 0)
  {
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
    UTIL_LCD_DisplayStringAt(0, y_size - 95, (uint8_t *)"  AUDIO CODEC  FAIL ", CENTER_MODE);
    UTIL_LCD_DisplayStringAt(0, y_size - 80, (uint8_t *)" Try to reset board ", CENTER_MODE);
  }

  /*
  Start playing the file from a circular buffer, once the DMA is enabled, it is
  always in running state. Application has to fill the buffer with the audio data
  using Transfer complete and/or half transfer complete interrupts callbacks
  (BSP_AUDIO_OUT_TransferComplete_CallBack() or BSP_AUDIO_OUT_HalfTransfer_CallBack()...
  */
  AUDIO_Start((uint32_t *)AUDIO_SRC_FILE_ADDRESS + PLAY_HEADER, (uint32_t)AUDIO_FILE_SIZE);

  /* Display the state on the screen */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DisplayStringAt(0, LINE(15), (uint8_t *)"       PLAYING...     ", CENTER_MODE);
  sprintf((char*)VolStr, "VOL:%u", uwVolume);
  UTIL_LCD_DisplayStringAt(0,  LINE(16), (uint8_t *)VolStr, CENTER_MODE);

  sprintf((char*)FreqStr, "FREQ:%u", *AudioFreq_ptr);
  UTIL_LCD_DisplayStringAt(0, LINE(17), (uint8_t *)FreqStr, CENTER_MODE);
  UTIL_LCD_SetFont(&Font20);

  /*******VOL +******/
  UTIL_LCD_DisplayStringAt(400,160,(uint8_t *)"VOL", LEFT_MODE);
  UTIL_LCD_FillCircle(420,120, 25, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_FillCircle(420,120, 23, UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_DisplayStringAt(413,113, (uint8_t *)"+", LEFT_MODE);
  /*******VOL -******/
  UTIL_LCD_FillCircle(420,220, 25, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_FillCircle(420,220,  23, UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_DisplayStringAt(413,213, (uint8_t *)"-", LEFT_MODE);
  /********PLAY/STOP***********/
  UTIL_LCD_FillCircle(240,220, 37, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_FillCircle(240,220, 35, UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_FillRect(227, 200, 10 , 45, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_FillRect(244, 200, 10 , 45, UTIL_LCD_COLOR_BLUE);
  /*******FREQ +******/
  UTIL_LCD_DisplayStringAt(710,160, (uint8_t *)"FREQ",RIGHT_MODE );
  UTIL_LCD_FillCircle(60,120, 25, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_FillCircle(60,120,  23, UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_DisplayStringAt(730,113, (uint8_t *)"+", RIGHT_MODE);
  /*******FREQ -******/
  UTIL_LCD_FillCircle(60,220, 25, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_FillCircle(60,220, 23, UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_DisplayStringAt(730,213, (uint8_t *)"-", RIGHT_MODE);

  /* IMPORTANT:
  AUDIO_Process() is called by the SysTick Handler, as it should be called
  within a periodic process */

  /* Infinite loop */
  while (1)
  {
    CurrentTickDetect = HAL_GetTick();
    AUDIO_Process();

    BSP_TS_GetState(0, &TS_State);

    if(CurrentTickDetect>(LastTSTick + 600)) /* 600ms delay on TS */
    {
      TS_Available = 1;
      BSP_AUDIO_OUT_SetVolume(0,uwVolume);
    }

    if((TS_State.TouchDetected)&&(TS_Available))
    {
      x1 = TS_State.TouchX;
      y1 = TS_State.TouchY;
      if((y1<255)&&(y1>185))
      {
        if((205<x1)&&(x1<275))
        {
          /**Audio PLAY or AUDIO Resume*/
          BSP_AUDIO_OUT_GetState(0,(uint32_t *)&audio_state);

          if(audio_state==AUDIO_OUT_STATE_PLAYING)
          {
            BSP_AUDIO_OUT_SetVolume(0,0);
            BSP_AUDIO_OUT_Pause(0);
            UTIL_LCD_FillCircle(240,220, 35, UTIL_LCD_COLOR_WHITE);
            UTIL_LCD_FillPolygon(Points2, 3, UTIL_LCD_COLOR_BLUE);
            LastTSTick = HAL_GetTick();
          }
          else
          {
            BSP_AUDIO_OUT_Resume(0);
            UTIL_LCD_FillCircle(240,220, 35, UTIL_LCD_COLOR_WHITE);
            UTIL_LCD_FillRect(227, 200, 10 , 45, UTIL_LCD_COLOR_BLUE);
            UTIL_LCD_FillRect(244, 200, 10 , 45, UTIL_LCD_COLOR_BLUE);
            LastTSTick = HAL_GetTick();
          }
          TS_Available = 0;
        }
        else
        {
          if((y1<245)&&(y1>195))
          {
            if((395<x1)&&(x1<445))
            {
              BSP_AUDIO_OUT_SetVolume(0, 0);
              /**VOL -*/
              if (uwVolume > 5)
              {
                uwVolume -= 5;
              }
              else
              {
                uwVolume = 0;
              }

              sprintf((char*)VolStr, "VOL:%u", uwVolume);
              BSP_AUDIO_OUT_SetVolume(0, uwVolume);
              UTIL_LCD_DisplayStringAt(0,  LINE(16), (uint8_t *)VolStr, CENTER_MODE);
              LastTSTick = HAL_GetTick();
              TS_Available = 0;
            }
            else
            {
              if((35<x1)&&(x1<85))
              {
                /*FREQ -*/
                if((35<x1)&&(x1<85))
                {
                  /*FREQ +*/
                  if (*AudioFreq_ptr != 8000)
                  {
                    AudioFreq_ptr++;
                    BSP_AUDIO_OUT_SetVolume(0, 0);
                    BSP_AUDIO_OUT_Stop(0);
                    sprintf((char*)FreqStr, "FREQ:%u", *AudioFreq_ptr);
                    UTIL_LCD_DisplayStringAt(0, LINE(17), (uint8_t *)FreqStr, CENTER_MODE);
                    BSP_AUDIO_OUT_SetSampleRate(0, *AudioFreq_ptr);
                    AUDIO_Start((uint32_t *)AUDIO_SRC_FILE_ADDRESS, (uint32_t)AUDIO_FILE_SIZE);
                    LastTSTick = HAL_GetTick();
                    TS_Available = 0;
                  }
                }
              }
            }
          }
        }
      }
      else
      {

        if((95<y1)&&(y1<145))
        {
          if((395<x1)&&(x1<445))
          {
            BSP_AUDIO_OUT_SetVolume(0, 0);
            /**VOL +*/
            if (uwVolume < 95)
            {
              uwVolume += 5;
            }
            else
            {
              uwVolume = 100;
            }
            sprintf((char*)VolStr, "VOL:%u", uwVolume);
            BSP_AUDIO_OUT_SetVolume(0, uwVolume);
            UTIL_LCD_DisplayStringAt(0,  LINE(16), (uint8_t *)VolStr, CENTER_MODE);
            LastTSTick = HAL_GetTick();
            TS_Available = 0;
          }
          else
          {
            if (*AudioFreq_ptr != 96000)
            {
              BSP_AUDIO_OUT_SetVolume(0, 0);
              AudioFreq_ptr--;
              BSP_AUDIO_OUT_Stop(0);
              BSP_AUDIO_OUT_SetSampleRate(0, *AudioFreq_ptr);
              AUDIO_Start((uint32_t *)AUDIO_SRC_FILE_ADDRESS, (uint32_t)AUDIO_FILE_SIZE);
              sprintf((char*)FreqStr, "FREQ:%u", *AudioFreq_ptr);
              UTIL_LCD_DisplayStringAt(0, LINE(17), (uint8_t *)FreqStr, CENTER_MODE);
              LastTSTick = HAL_GetTick();
              TS_Available = 0;
            }
          }
        }
        else
        {}
      }
      /**Audio PLAY or AUDIO Resume*/
      BSP_AUDIO_OUT_GetState(0,(uint32_t *)&audio_state);
    }
    if (CheckForUserInput() > 0)
    {
      ClearUserInput();
      BSP_AUDIO_OUT_Stop(0);
      BSP_AUDIO_OUT_DeInit(0);
      BSP_XSPI_NOR_DisableMemoryMappedMode(0);
      BSP_XSPI_NOR_DeInit(0);
      return;
    }
  }
}

/**
  * @brief  Display Audio demo hint
  * @param  None
  * @retval None
  */
static void Audio_SetHint(uint32_t Index)
{
  uint32_t x_size, y_size;

  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  /* Clear the LCD */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  /* Set Audio Demo description */
  UTIL_LCD_FillRect(0, 0, x_size, 80, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetFont(&Font24);
  if(Index == 0)
  {
    UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *)"AUDIO PLAY EXAMPLE", CENTER_MODE);
    UTIL_LCD_SetFont(&Font16);
    UTIL_LCD_DisplayStringAt(0, 45, (uint8_t *)"Press USER button for next menu", CENTER_MODE);
    UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"Hear nothing? Have you copied the audio file with STM32CubeProgrammer ?", CENTER_MODE);
  }

   UTIL_LCD_DrawRect(10, 90, x_size - 20, y_size - 100, UTIL_LCD_COLOR_BLUE);
   UTIL_LCD_DrawRect(11, 91, x_size - 22, y_size - 102, UTIL_LCD_COLOR_BLUE);
}

/**
  * @brief  Starts Audio streaming.
  * @param  None
  * @retval Audio error
  */
AUDIO_ErrorTypeDef AUDIO_Start(uint32_t *psrc_address, uint32_t file_size)
{
  uint32_t bytesread;

  buffer_ctl.state = BUFFER_OFFSET_NONE;
  buffer_ctl.AudioFileSize = file_size;
  buffer_ctl.SrcAddress = psrc_address;

  bytesread = GetData( (void *)psrc_address,
                       0,
                       &buffer_ctl.buff[0],
                       AUDIO_BUFFER_SIZE);
  if(bytesread > 0)
  {
    BSP_AUDIO_OUT_Play(0,(uint8_t *)&buffer_ctl.buff[0], AUDIO_BUFFER_SIZE);
    audio_state = AUDIO_OUT_STATE_PLAYING;
    buffer_ctl.fptr = bytesread;
    return AUDIO_ERROR_NONE;
  }
  return AUDIO_ERROR_IO;
}

/**
  * @brief  Gets Data from storage unit.
  * @param  None
  * @retval None
  */
static uint32_t GetData(void *pdata, uint32_t offset, uint8_t *pbuf, uint32_t NbrOfData)
{
  uint8_t *lptr = pdata;
  uint32_t ReadDataNbr;

  ReadDataNbr = 0;
  while(((offset + ReadDataNbr) < buffer_ctl.AudioFileSize) && (ReadDataNbr < NbrOfData))
  {
    pbuf[ReadDataNbr]= lptr [offset + ReadDataNbr];
    ReadDataNbr++;
  }

  return ReadDataNbr;
}

/**
  * @brief  Manages Audio process.
  * @param  None
  * @retval Audio error
  */
uint8_t AUDIO_Process(void)
{
  uint32_t bytesread;
  AUDIO_ErrorTypeDef error_state = AUDIO_ERROR_NONE;

  switch(audio_state)
  {
  case AUDIO_OUT_STATE_PLAYING:

    if(buffer_ctl.fptr >= buffer_ctl.AudioFileSize)
    {
      /* Play audio sample again ... */
      buffer_ctl.fptr = 0;
      error_state = AUDIO_ERROR_EOF;
    }

    /* 1st half buffer played; so fill it and continue playing from bottom*/
    if(buffer_ctl.state == BUFFER_OFFSET_HALF)
    {
      bytesread = GetData((void *)buffer_ctl.SrcAddress,
                          buffer_ctl.fptr,
                          &buffer_ctl.buff[0],
                          AUDIO_BUFFER_SIZE /2);

      if( bytesread >0)
      {
        buffer_ctl.state = BUFFER_OFFSET_NONE;
        buffer_ctl.fptr += bytesread;
      }
    }

    /* 2nd half buffer played; so fill it and continue playing from top */
    if(buffer_ctl.state == BUFFER_OFFSET_FULL)
    {
      bytesread = GetData((void *)buffer_ctl.SrcAddress,
                          buffer_ctl.fptr,
                          &buffer_ctl.buff[AUDIO_BUFFER_SIZE /2],
                          AUDIO_BUFFER_SIZE /2);
      if( bytesread > 0)
      {
        buffer_ctl.state = BUFFER_OFFSET_NONE;
        buffer_ctl.fptr += bytesread;
      }
    }
    break;

  default:
    error_state = AUDIO_ERROR_NOTREADY;
    break;
  }
  return (uint8_t) error_state;
}

/*------------------------------------------------------------------------------
       Callbacks implementation:
           the callbacks API are defined __weak in the stm32h7s78_discovery_audio.c file
           and their implementation should be done the user code if they are needed.
           Below some examples of callback implementations.
  ----------------------------------------------------------------------------*/
/**
  * @brief  Manages the full Transfer complete event.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_OUT_TransferComplete_CallBack(uint32_t Interface)
{
  if(audio_state == AUDIO_OUT_STATE_PLAYING)
  {
    /* allows AUDIO_Process() to refill 2nd part of the buffer  */
    buffer_ctl.state = BUFFER_OFFSET_FULL;
  }
}

/**
  * @brief  Manages the DMA Half Transfer complete event.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_OUT_HalfTransfer_CallBack(uint32_t Interface)
{
  if(audio_state == AUDIO_OUT_STATE_PLAYING)
  {
    /* allows AUDIO_Process() to refill 1st part of the buffer  */
    buffer_ctl.state = BUFFER_OFFSET_HALF;
  }
}

/**
  * @brief  Manages the DMA FIFO error event.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_OUT_Error_CallBack(uint32_t Interface)
{
  /* Display message on the LCD screen */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_RED);
  UTIL_LCD_DisplayStringAt(0, LINE(14), (uint8_t *)"       DMA  ERROR     ", CENTER_MODE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);

  /* Stop the program with an infinite loop */
  while (BSP_PB_GetState(BUTTON_USER) != RESET)
  {
    return;
  }

  /* could also generate a system reset to recover from the error */
  /* .... */
}
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
