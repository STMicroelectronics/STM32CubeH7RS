/**
******************************************************************************
* @file    USB_Host/MSC_Standalone/Src/explorer.c
* @author  MCD Application Team
* @brief   Explore the USB flash disk content
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

/* Includes ------------------------------------------------------------------ */
#include "main.h"
#include "ff.h"
#include "usbh_conf.h"
#include "ff_gen_drv.h"
#include "explorer.h"
#include "sd_diskio_dma_standalone.h"
/* Private define ------------------------------------------------------------ */
#define FILEMGR_LIST_DEPDTH                     24
#define FILEMGR_FILE_NAME_SIZE                  40
#define FILETYPE_FILE                            1

/* Private typedef ----------------------------------------------------------- */
typedef struct _FILELIST_LineTypeDef {
  uint8_t type;
  uint8_t name[FILEMGR_FILE_NAME_SIZE];
} FILELIST_LineTypeDef;

typedef struct _FILELIST_FileTypeDef {
  FILELIST_LineTypeDef  file[FILEMGR_LIST_DEPDTH] ;
  uint16_t              ptr;
} FILELIST_FileTypeDef;
/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
extern SD_HandleTypeDef hsd1;
FATFS SD_FatFs;
char SD_Path[4];
FILELIST_FileTypeDef FileList;
/* Private function prototypes ----------------------------------------------- */
void SDMMC1_SD_Init(void);
uint8_t SD_StorageInit(void);
FRESULT SD_StorageParse(void);
uint8_t Audio_ShowWavFiles(void);
extern void MX_SDMMC1_SD_Init(void);
/* Private functions --------------------------------------------------------- */

const MKFS_PARM OptParm = {FM_ANY, 0, 0, 0, 0};
uint8_t workBuffer[FF_MAX_SS];
/**
  * @brief  Initializes the SD Storage.
  * @param  None
  * @retval Status
  */
uint8_t SD_StorageInit(void)
{
  /* Initializes the SD card device */
  MX_SDMMC1_SD_Init();

  if(HAL_SD_Init(&hsd1) != HAL_OK)
  {
    USBH_ErrLog("SD card NOT plugged \n");
    return 1;
  }
  else
  {
    /* Link the SD Card disk I/O driver */
    if (FATFS_LinkDriver(&SD_DMA_Driver, SD_Path) == 0)
    {
      if ((f_mount(&SD_FatFs, (TCHAR const *)SD_Path, 0) != FR_OK))
      {
        /* FatFs Initialization Error */
        USBH_ErrLog("Cannot Initialize FatFs !");
        return 1;
      }
      else
      {
         USBH_UsrLog("INFO : FatFs Initialized !");
      }
    }
  }
  return 0;
}

/**
  * @brief  Copies disk content in the explorer list.
  * @param  None
  * @retval Operation result
  */
FRESULT SD_StorageParse(void)
{
  FRESULT res;
  FILINFO fno;
  DIR dir;
  char *fn;

#if _USE_LFN
  static char lfn[_MAX_LFN];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
#endif

  res = f_opendir(&dir, SD_Path);
  FileList.ptr = 0;

  if (res == FR_OK)
  {
    while (1)
    {
      res = f_readdir(&dir, &fno);

      if (res != FR_OK || fno.fname[0] == 0)
      {
        break;
      }
      if (fno.fname[0] == '.')
      {
        continue;
      }
#if _USE_LFN
      fn = *fno.lfname ? fno.lfname : fno.fname;
#else
      fn = fno.fname;
#endif

      if (FileList.ptr < FILEMGR_LIST_DEPDTH)
      {
        if ((fno.fattrib & AM_DIR) == 0)
        {
          if ((strstr(fn, "wav")) || (strstr(fn, "WAV")))
          {
            strncpy((char *)FileList.file[FileList.ptr].name, (char *)fn,
                    FILEMGR_FILE_NAME_SIZE);

            FileList.file[FileList.ptr].type = FILETYPE_FILE;
            FileList.ptr++;
          }
        }
      }
    }
  }
  else
  {
    USBH_ErrLog("There is no WAV file on the microSD.\n");
  }
  f_closedir(&dir);
  return res;
}

/**
  * @brief  Shows audio file (*.wav) on the root
  * @param  None
  * @retval None
  */
uint8_t Audio_ShowWavFiles(void)
{
  uint8_t i = 0;

  if (FileList.ptr > 0)
  {
    USBH_UsrLog("\nAudio file(s) :");
    for (i = 0; i < FileList.ptr; i++)
    {
      USBH_UsrLog("%s\n",FileList.file[i].name);
    }
    USBH_UsrLog("End of files list.\n");
    return 0;
  }
  else
  {
    return 1;
  }
}
