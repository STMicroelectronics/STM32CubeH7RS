/**
  ******************************************************************************
  * @file    user_diskio.c
  * @author  MCD Application Team
  * @brief   User Disk I/O driver
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the st_license.txt
  * file in the root directory of this software component.
  * If no st_license.txt file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static DSTATUS USER_initialize (BYTE);
static DSTATUS USER_status (BYTE);
static DRESULT USER_read (BYTE, BYTE*, LBA_t, UINT);
static DRESULT USER_write (BYTE, const BYTE*, LBA_t, UINT);
static DRESULT USER_ioctl (BYTE, BYTE, void*);

const Diskio_drvTypeDef  USER_Driver =
{
  USER_initialize,
  USER_status,
  USER_read,
  USER_write,
  USER_ioctl
};

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initialize the user Diskio lowlevel driver
 * @param  lun : not used
 * @retval DSTATUS: return 0
 */
static DSTATUS USER_initialize(BYTE lun)
{
  return 0;
}

/**
 * @brief  Gets the Disk Status
 * @param  lun : not used
 * @retval DSTATUS: return 0
 */
static DSTATUS USER_status(BYTE lun)
{
  return 0;
}

/**
 * @brief  Read data from media into a buffer
 * @param  lun : not used
 * @param  *buff: Data buffer to store read data
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to read (1..128)
 * @retval DRESULT: return RES_OK otherwise
 */
static DRESULT USER_read(BYTE lun, BYTE *buff, LBA_t sector, UINT count)
{

  return RES_OK;
}

/**
 * @brief  Write data from media into a buffer
 * @param  lun : not used
 * @param  *buff: Data to be written
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to write (1..128)
 * @retval DRESULT: return RES_OK otherwise
 */
static DRESULT USER_write(BYTE lun, const BYTE *buff, LBA_t sector, UINT count)
{

  return RES_OK;
}

/**
 * @brief  I/O control operation
 * @param  lun : not used
 * @param  cmd: Control code
 * @param  *buff: Buffer to send/receive control data
 * @retval DRESULT: return RES_OK otherwise
 */
static DRESULT USER_ioctl(BYTE lun, BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  switch (cmd)
  {
     /* Make sure that no pending write process */
     case CTRL_SYNC:
       res = RES_OK;
       break;

     /* Get number of sectors on the disk (DWORD) */
     case GET_SECTOR_COUNT:
       res = RES_OK;
       break;

     /* Get R/W sector size (WORD) */
     case GET_SECTOR_SIZE:
       res = RES_OK;
       break;

     /* Get erase block size in unit of sector (DWORD) */
     case GET_BLOCK_SIZE:
       res = RES_OK;
       break;

     default:
       res = RES_PARERR;
  }
  return res;
}
