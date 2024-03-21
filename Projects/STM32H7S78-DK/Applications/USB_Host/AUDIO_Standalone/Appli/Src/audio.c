/**
  ******************************************************************************
  * @file    audio.c
  * @author  MCD Application Team
  * @brief   This file provides the Audio Out (playback) interface API
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

/* Includes ------------------------------------------------------------------ */
#include "main.h"
#include "usbh_audio.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "audio.h"
#include "explorer.h"

/* functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* Private define ------------------------------------------------------------ */
#define AUDIO_BLOCK_SIZE  512
#define AUDIO_BLOCK_NBR   33
#define FILEMGR_LIST_DEPDTH                     24
#define FILEMGR_FILE_NAME_SIZE                  40
#define FILEMGR_MAX_LEVEL                        4
#define FILETYPE_DIR                             0
#define FILETYPE_FILE                            1
/* Private typedef ----------------------------------------------------------- */
typedef struct AUDIO_Info_t {
  uint32_t ChunkID;       /* 0  */
  uint32_t FileSize;      /* 4  */
  uint32_t FileFormat;    /* 8  */
  uint32_t SubChunk1ID;   /* 12 */
  uint32_t SubChunk1Size; /* 16 */
  uint16_t AudioFormat;   /* 20 */
  uint16_t NbrChannels;   /* 22 */
  uint32_t SampleRate;    /* 24 */
  uint32_t ByteRate;      /* 28 */
  uint16_t BlockAlign;    /* 32 */
  uint16_t BitPerSample;  /* 34 */
  uint32_t SubChunk2ID;   /* 36 */
  uint32_t SubChunk2Size; /* 40 */
}WAV_InfoTypedef;

typedef struct _FILELIST_LineTypeDef {
  uint8_t type;
  uint8_t name[FILEMGR_FILE_NAME_SIZE];
}FILELIST_LineTypeDef;

typedef struct _FILELIST_FileTypeDef {
  FILELIST_LineTypeDef  file[FILEMGR_LIST_DEPDTH] ;
  uint16_t              ptr;
}FILELIST_FileTypeDef;

typedef enum {
  APPLICATION_IDLE = 0,
  APPLICATION_START,
  APPLICATION_READY,
  APPLICATION_DISCONNECT,
}AUDIO_ApplicationTypeDef;

typedef struct
{
  uint8_t buff[AUDIO_BLOCK_SIZE * AUDIO_BLOCK_NBR];
  int32_t out_ptr;
  uint32_t in_ptr;
} Audio_BufferTypeDef;

/* Private variables --------------------------------------------------------- */
AUDIO_PLAYBACK_StateTypeDef AUDIO_app_state;
AUDIO_PLAYBACK_StateTypeDef AUDIO_app_prev_state;
static WAV_InfoTypedef WavInfo;
static FIL WavFile;
static Audio_BufferTypeDef BufferCtl;
int16_t FilePos = 0;
uint8_t* oldfile;

/* Exported variables ------------------------------------------------------- */
extern AUDIO_ApplicationTypeDef Appli_state;
extern USBH_HandleTypeDef hUsbHostFS;
extern FILELIST_FileTypeDef FileList;

/* Private function prototypes ----------------------------------------------- */
static AUDIO_ErrorTypeDef AUDIO_GetFileInfo(uint16_t file_idx, WAV_InfoTypedef * info);
AUDIO_ErrorTypeDef AUDIO_Start(uint8_t idx);
AUDIO_ErrorTypeDef AUDIO_Process(void);
AUDIO_ErrorTypeDef AUDIO_Stop(void);

/* Exported function prototypes ----------------------------------------------- */
extern uint8_t Audio_ShowWavFiles(void);

/**
  * @brief  Starts Audio streaming.
  * @param  idx: File index
  * @retval Audio error
  */
AUDIO_ErrorTypeDef AUDIO_Start(uint8_t idx)
{
  uint32_t bytesread;

  if (FileList.ptr > idx)
  {
    f_close(&WavFile);

    AUDIO_GetFileInfo(idx, &WavInfo);

    /* Set Frequency */
    USBH_AUDIO_SetFrequency(&hUsbHostFS,
                            WavInfo.SampleRate,
                            WavInfo.NbrChannels, WavInfo.BitPerSample);

    /* Fill whole buffer at first time */
    if (f_read(&WavFile,
               &BufferCtl.buff[0],
               AUDIO_BLOCK_SIZE * AUDIO_BLOCK_NBR, (void *)&bytesread) == FR_OK)
    {
      if (bytesread != 0)
      {
        return AUDIO_ERROR_NONE;
      }
    }

    BufferCtl.in_ptr = 0;
  }
  return AUDIO_ERROR_IO;
}

/**
  * @brief  Manages Audio process.
  * @param  None
  * @retval Audio error
  */
AUDIO_ErrorTypeDef AUDIO_Process(void)
{
  uint8_t str[16];
  int32_t diff;
  uint32_t bytesread, elapsed_time;
  static uint32_t prev_elapsed_time = 0xFFFFFFFF;
  AUDIO_ErrorTypeDef error_state = AUDIO_ERROR_NONE;

  switch(AUDIO_app_state)
  {
  case AUDIO_STATE_PLAYBACK_IDLE:
    if (Appli_state == APPLICATION_READY)
    {
      AUDIO_app_state = AUDIO_STATE_PLAYBACK_EXPLORE;
      AUDIO_app_prev_state = AUDIO_STATE_PLAYBACK_IDLE;
    }
    break;

  case AUDIO_STATE_PLAYBACK_EXPLORE:
    if (Appli_state == APPLICATION_READY)
    {
      /* Init SD Storage */
      if (SD_StorageInit() == 0)
      {
        USBH_UsrLog("SD Parsing .........");
        SD_StorageParse();
      }
      /* Show Wav Files */
      if (Audio_ShowWavFiles() == 0)
      {
        AUDIO_app_state = AUDIO_STATE_PLAYBACK_WAIT;
        AUDIO_app_prev_state = AUDIO_STATE_PLAYBACK_EXPLORE;
        USBH_UsrLog("To start Playback, please press the user button..");
      }
      else
      {
        AUDIO_app_state = AUDIO_STATE_PLAYBACK_IDLE;
        USBH_ErrLog("\nPlease check your SD card\n");
      }
    }
    break;

  case AUDIO_STATE_PLAYBACK_WAIT:
    break;

  case AUDIO_STATE_PLAYBACK_CONFIG:
    if(Appli_state == APPLICATION_READY)
    {
      AUDIO_app_state = AUDIO_STATE_PLAYBACK_START;
      AUDIO_app_prev_state = AUDIO_STATE_PLAYBACK_CONFIG;
    }
    break;

  case AUDIO_STATE_PLAYBACK_START:
    /* Read and Write File Here */
    if(Appli_state == APPLICATION_READY)
    {
      USBH_UsrLog("*** Playing audio ***\n");
      if (AUDIO_Start(0) == AUDIO_ERROR_IO)
      {
        USBH_UsrLog("*** AUDIO_ERROR_IO 1 ***");
      }
      AUDIO_app_state = AUDIO_STATE_PLAYBACK_PLAY;
      AUDIO_app_prev_state = AUDIO_STATE_PLAYBACK_START;
    }
    break;

  case AUDIO_STATE_PLAYBACK_PLAY:
    if ((BufferCtl.out_ptr = USBH_AUDIO_GetOutOffset(&hUsbHostFS)) < 0) /* End of file */
    {
      AUDIO_app_state = AUDIO_STATE_PLAYBACK_NEXT;
    }
    else if (BufferCtl.out_ptr >= (AUDIO_BLOCK_SIZE * AUDIO_BLOCK_NBR)) /* End of buffer */
    {
      USBH_AUDIO_ChangeOutBuffer(&hUsbHostFS, &BufferCtl.buff[0]);
    }
    else
    {
      diff = BufferCtl.out_ptr - BufferCtl.in_ptr;

      if (diff < 0)
      {
        diff = AUDIO_BLOCK_SIZE * AUDIO_BLOCK_NBR + diff;
      }

      if (diff >= (AUDIO_BLOCK_SIZE * AUDIO_BLOCK_NBR / 2))
      {
        BufferCtl.in_ptr += AUDIO_BLOCK_SIZE;

        if (BufferCtl.in_ptr >= (AUDIO_BLOCK_SIZE * AUDIO_BLOCK_NBR))
        {
          BufferCtl.in_ptr = 0;
        }

        if (f_read(&WavFile,
                   &BufferCtl.buff[BufferCtl.in_ptr],
                   AUDIO_BLOCK_SIZE, (void *)&bytesread) != FR_OK)
        {
          f_close(&WavFile);
          error_state = AUDIO_ERROR_NONE;
        }
      }
    }
    /* Display elapsed time */
    elapsed_time = WavFile.fptr / WavInfo.ByteRate;
    if(prev_elapsed_time != elapsed_time)
    {
      prev_elapsed_time = elapsed_time;
      sprintf((char *)str, "[%02lu:%02lu]", (unsigned long)(elapsed_time /60), (unsigned long)(elapsed_time%60));
      USBH_UsrLog("%s",str);
    }
    AUDIO_app_prev_state = AUDIO_STATE_PLAYBACK_PLAY;
    break;

  case AUDIO_STATE_PLAYBACK_NEXT:
    USBH_UsrLog("[NEXT]");
    if (++FilePos >= FileList.ptr)
    {
      FilePos = 0;
    }
    USBH_AUDIO_Stop(&hUsbHostFS);
    AUDIO_Start(FilePos);
    AUDIO_app_state = AUDIO_STATE_PLAYBACK_CONFIG;
    break;

  default:
    break;
  }
  if(Appli_state == APPLICATION_DISCONNECT)
  {
    Appli_state = APPLICATION_IDLE;
    USBH_ErrLog("USB device disconnected !!! \n");
    AUDIO_Stop();
    AUDIO_app_prev_state = AUDIO_STATE_PLAYBACK_IDLE;
  }
  return error_state;
}

/**
  * @brief  Stops Audio streaming.
  * @param  None
  * @retval Audio error
  */
AUDIO_ErrorTypeDef AUDIO_Stop(void)
{
  AUDIO_app_state = AUDIO_STATE_PLAYBACK_IDLE;
  USBH_AUDIO_Stop(&hUsbHostFS);
  FilePos = 0;
  f_close(&WavFile);
  return AUDIO_ERROR_NONE;
}

/**
  * @brief  Gets the file info.
  * @param  file_idx: File index
  * @param  info: Pointer to WAV file info
  * @retval Audio error
  */
static AUDIO_ErrorTypeDef AUDIO_GetFileInfo(uint16_t file_idx,
                                            WAV_InfoTypedef * info)
{
  uint32_t bytesread;

  if (f_open
      (&WavFile, (char *)FileList.file[file_idx].name,
       FA_OPEN_EXISTING | FA_READ) == FR_OK)
  {
    /* Fill the buffer to Send */
    if (f_read(&WavFile, info, sizeof(WAV_InfoTypedef), (void *)&bytesread) ==
        FR_OK)
    {
      if( oldfile != FileList.file[file_idx].name)
      {
        USBH_UsrLog("Playing file : ");
        USBH_UsrLog("%s",FileList.file[file_idx].name);
        oldfile = FileList.file[file_idx].name;
        USBH_UsrLog("Press the USER BUTTON to play the next wav file.");
      }
      return AUDIO_ERROR_NONE;
    }
    f_close(&WavFile);
  }
  return AUDIO_ERROR_IO;
}

/**
  * @brief  Informs user that settings have been changed.
  * @param  phost: Host Handle
  * @retval None
  */
void USBH_AUDIO_FrequencySet(USBH_HandleTypeDef * phost)
{
  if (AUDIO_app_state == AUDIO_STATE_PLAYBACK_CONFIG)
  {
    /* Start first read */
    USBH_AUDIO_Play(&hUsbHostFS, &BufferCtl.buff[0], WavInfo.FileSize);
    AUDIO_app_state = AUDIO_STATE_PLAYBACK_PLAY;
  }
}

/**
  * @brief  GPIO EXTI Callback function
  *         Handle remote-wakeup through key button
  * @param  GPIO_Pin
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == USER_BUTTON_Pin)
  {
    if (AUDIO_app_prev_state == AUDIO_STATE_PLAYBACK_EXPLORE)
    {
      AUDIO_app_state = AUDIO_STATE_PLAYBACK_CONFIG;
    }
    else if (AUDIO_app_prev_state == AUDIO_STATE_PLAYBACK_PLAY)
    {
      AUDIO_app_state = AUDIO_STATE_PLAYBACK_NEXT;
    }
  }
}