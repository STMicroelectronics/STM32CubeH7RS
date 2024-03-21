/**
******************************************************************************
* @file    file_operations.c
* @author  MCD Application Team
* @brief   Write/read file on the disk.
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
#include "usbh_core.h"

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
FATFS USBH_fatfs;
FIL MyFile;
FRESULT res;
uint32_t bytesWritten;
uint8_t rtext[200];
uint8_t wtext[] = "USB Host Library : Mass Storage application";

/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* Private function prototypes ----------------------------------------------- */
uint8_t MSC_File_Operations(void);
/* Private functions --------------------------------------------------------- */

/**
* @brief  Files operations: Read/Write and compare
* @param  None
* @retval None
*/
uint8_t MSC_File_Operations(void)
{
  uint32_t bytesread;
  uint8_t retvalue = 0;

  USBH_UsrLog("INFO : FatFs Initialized \n");

  if(f_open(&MyFile, "0:USBHost.txt",FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
  {
    USBH_ErrLog("Cannot Open 'USBHost.txt' file \n");
    retvalue = 1;
  }
  else
  {
    USBH_UsrLog("INFO : 'USBHost.txt' opened for write  \n");
    res= f_write (&MyFile, wtext, sizeof(wtext), (void *)&bytesWritten);
    f_close(&MyFile);

    if((bytesWritten == 0) || (res != FR_OK)) /*EOF or Error*/
    {
      USBH_ErrLog("Cannot Write on the  'USBHost.txt' file \n");
      retvalue = 1;
    }
    else
    {
      if(f_open(&MyFile, "0:USBHost.txt", FA_READ) != FR_OK)
      {
        USBH_ErrLog("Cannot Open 'USBHost.txt' file for read.\n");
        retvalue = 1;
      }
      else
      {
        USBH_UsrLog("INFO : Text written on the 'USBHost.txt' file \n");

        res = f_read(&MyFile, rtext, sizeof(rtext), (void *)&bytesread);

        if((bytesread == 0) || (res != FR_OK)) /*EOF or Error*/
        {
          USBH_ErrLog("Cannot Read from the  'USBHost.txt' file \n");
          retvalue = 1;
        }
        else
        {
          USBH_UsrLog("Read Text : \n");
          USBH_UsrLog("%s", rtext);
          USBH_UsrLog("\n");
        }
        f_close(&MyFile);
      }
      /* Compare read data with the expected data */
      if((bytesread == bytesWritten))
      {
        USBH_UsrLog("INFO : FatFs data compare SUCCESS");
        USBH_UsrLog("\n");
      }
      else
      {
        USBH_ErrLog("FatFs data compare ERROR");
        USBH_ErrLog("\n");
        retvalue = 1;
      }
    }
  }
  return (retvalue);
}

