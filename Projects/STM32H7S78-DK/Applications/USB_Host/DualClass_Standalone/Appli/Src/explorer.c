/**
******************************************************************************
* @file    explorer.c
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

/* Includes ----------------------------------------------------------------- */
#include "main.h"
#include "ff.h"
#include "usbh_core.h"
#include "usbh_msc.h"
/* Private typedef ---------------------------------------------------------- */
/* Private define ----------------------------------------------------------- */
/* Private macro ------------------------------------------------------------ */
/* Private variables -------------------------------------------------------- */
//static int32_t recurseLevel = -1;
extern USBH_HandleTypeDef hUsbHostHS;
/* Private function prototypes ---------------------------------------------- */
FRESULT Explore_Disk(char *path, uint8_t recu_level);
/* Private functions -------------------------------------------------------- */

/**
* @brief  Displays disk content.
* @param  path: Pointer to root path
* @param  recu_level: Disk content level
* @retval Operation result
*/
FRESULT Explore_Disk(char *path, uint8_t recu_level)
{
  FRESULT res = FR_OK;
  FILINFO fno;
  DIR dir;
  char *fn;
  char tmp[14];

  res = f_opendir(&dir, path);
  if(res == FR_OK)
  {
    while(USBH_MSC_IsReady(&hUsbHostHS))
    {
      res = f_readdir(&dir, &fno);
      if(res != FR_OK || fno.fname[0] == 0)
      {
        break;
      }
      if(fno.fname[0] == '.')
      {
        continue;
      }

      fn = fno.fname;
      strcpy(tmp, fn);

      if(recu_level == 1)
      {
        USBH_UsrLog("   |__");
      }
      else if(recu_level == 2)
      {
        USBH_UsrLog("   |   |__");
      }
      if((fno.fattrib & AM_DIR) == AM_DIR)
      {
        strcat(tmp, "\n");
        USBH_UsrLog("%s", tmp);
        Explore_Disk(fn, 2);
      }
      else
      {
        strcat(tmp, "\n");
        USBH_UsrLog("%s", tmp);
      }

      if(((fno.fattrib & AM_DIR) == AM_DIR)&&(recu_level == 2))
      {
        Explore_Disk(fn, 2);
      }
    }
    f_closedir(&dir);
  }
  return res;
}

