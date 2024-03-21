/**
  ******************************************************************************
  * @file    sram_diskio.c
  * @author  MCD Application Team
  * @brief   SRAM Disk I/O  driver
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
#include "sram_diskio.h"
#include <string.h>

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static DSTATUS SRAMDISK_initialize (BYTE);
static DSTATUS SRAMDISK_status (BYTE);
static DRESULT SRAMDISK_read (BYTE, BYTE*, LBA_t, UINT);
static DRESULT SRAMDISK_write (BYTE, const BYTE*, LBA_t, UINT);
static DRESULT SRAMDISK_ioctl (BYTE, BYTE, void*);

const Diskio_drvTypeDef SRAMDISK_Driver =
{
  SRAMDISK_initialize,
  SRAMDISK_status,
  SRAMDISK_read,
  SRAMDISK_write,
  SRAMDISK_ioctl
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize the SRAM Diskio lowlevel driver
  * @param  lun : not used
  * @retval DSTATUS: return 0
  */
static DSTATUS SRAMDISK_initialize(BYTE lun)
{
  memset((void *)SRAM_DISK_BASE_ADDR, '\0', SRAM_DISK_SIZE);
  return 0;
}

/**
  * @brief  Gets the Disk Status
  * @param  lun : not used
  * @retval DSTATUS: return 0
  */
static DSTATUS SRAMDISK_status(BYTE lun)
{
  return 0;
}

/**
  * @brief  Read data from sram into a buffer
  * @param  lun : not used
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: return RES_OK otherwise
  */
static DRESULT SRAMDISK_read(BYTE lun, BYTE *buff, LBA_t sector, UINT count)
{
  uint32_t BufferSize = (BLOCK_SIZE * count);
  uint8_t *pMem = (uint8_t *) (SRAM_DISK_BASE_ADDR + (sector * BLOCK_SIZE));

  for(; BufferSize != 0; BufferSize--)
  {
    *buff++ = *(__IO uint8_t *)pMem++;
  }

  return RES_OK;
}

/**
  * @brief  Write data from sram into a buffer
  * @param  lun : not used
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: return RES_OK otherwise
  */
static DRESULT SRAMDISK_write(BYTE lun, const BYTE *buff, LBA_t sector, UINT count)
{
  uint32_t BufferSize = (BLOCK_SIZE * count);
  uint8_t *pMem = (uint8_t *) (SRAM_DISK_BASE_ADDR + (sector * BLOCK_SIZE));

  for(; BufferSize != 0; BufferSize--)
  {
    *(__IO uint8_t *)pMem++ = *buff++;
  }

  return RES_OK;
}


/**
  * @brief  I/O control operation
  * @param  lun : not used
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: return RES_OK on success RES_ERROR otherwise
  */
static DRESULT SRAMDISK_ioctl(BYTE lun, BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;

  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC :
    res = RES_OK;
    break;

  /* Get number of sectors on the disk (DWORD) */
  case GET_SECTOR_COUNT :
    *(DWORD*)buff = SRAM_DISK_SIZE / BLOCK_SIZE;
    res = RES_OK;
    break;

  /* Get R/W sector size (WORD) */
  case GET_SECTOR_SIZE :
    *(WORD*)buff = BLOCK_SIZE;
    res = RES_OK;
    break;

  /* Get erase block size in unit of sector (DWORD) */
  case GET_BLOCK_SIZE :
    *(DWORD*)buff = 1;
    res = RES_OK;
    break;

  default:
    res = RES_PARERR;
  }

  return res;
}

