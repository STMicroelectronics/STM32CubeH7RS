/**
  ******************************************************************************
  * @file    usbh_diskio.c
  * @author  MCD Application Team
  * @brief   USB Host Disk I/O  driver.
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
#include "usbh_diskio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static DWORD scratch[FF_MAX_SS / 4];

/* Private function prototypes -----------------------------------------------*/
static DSTATUS USBH_initialize (BYTE);
static DSTATUS USBH_status (BYTE);
static DRESULT USBH_read (BYTE, BYTE*, LBA_t, UINT);
static DRESULT USBH_write (BYTE, const BYTE*, LBA_t, UINT);
static DRESULT USBH_ioctl (BYTE, BYTE, void*);

const Diskio_drvTypeDef  USBH_Driver =
{
  USBH_initialize,
  USBH_status,
  USBH_read,
  USBH_write,
  USBH_ioctl
};

/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Initialize the USBH Diskio lowlevel driver
 * @param  lun : lun id
 * @retval DSTATUS: return RES_OK otherwise
 */
static DSTATUS USBH_initialize(BYTE lun)
{
  /* CAUTION : USB Host library has to be initialized in the application */

  return RES_OK;
}

/**
* @brief  Get the Disk Status
 * @param  lun : lun id
 * @retval DSTATUS: return RES_OK otherwise
 */
static DSTATUS USBH_status(BYTE lun)
{
  DRESULT res = RES_ERROR;

  if (USBH_MSC_UnitIsReady(&hUsb_Host, lun))
  {
    res = RES_OK;
  }
  else
  {
    res = RES_ERROR;
  }

  return res;
}

/**
 * @brief  Read data from usbh into a buffer
 * @param  lun : lun id
 * @param  *buff: Data buffer to store read data
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to read (1..128)
 * @retval DRESULT: return RES_OK otherwise
 */
static DRESULT USBH_read(BYTE lun, BYTE *buff, LBA_t sector, UINT count)
{
  DRESULT res = RES_ERROR;
  MSC_LUNTypeDef info;
  USBH_StatusTypeDef  status = USBH_OK;

  if (buff == NULL)
  {
    USBH_ErrLog("The Data buffer buff should be different to NULL");
  }

  if (!((DWORD)buff & 3) && (((HCD_HandleTypeDef *)hUsb_Host.pData)->Init.dma_enable))
  {
    while ((count--) && (status == USBH_OK))
    {
      status = USBH_MSC_Read(&hUsb_Host, lun, sector + count, (uint8_t *)scratch, 1);

      if (status == USBH_OK)
      {
        USBH_memcpy(&buff[count * FF_MAX_SS] ,scratch, FF_MAX_SS);
      }
      else
      {
        break;
      }
    }
  }
  else
  {
    status = USBH_MSC_Read(&hUsb_Host, lun, sector, buff, count);
  }

  if (status == USBH_OK)
  {
    res = RES_OK;
  }
  else
  {
    USBH_MSC_GetLUNInfo(&hUsb_Host, lun, &info);

    switch (info.sense.asc)
    {
    case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
    case SCSI_ASC_MEDIUM_NOT_PRESENT:
    case SCSI_ASC_NOT_READY_TO_READY_CHANGE:
      USBH_ErrLog ("USB Disk is not ready!");
      res = RES_NOTRDY;
      break;

    default:
      res = RES_ERROR;
      break;
    }
  }

  return res;
}

/**
 * @brief  Writes data from usbh into a buffer
 * @param  lun : lun id
 * @param  *buff: Data to be written
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to write (1..128)
 * @retval DRESULT: return RES_OK otherwise
 */
static DRESULT USBH_write(BYTE lun, const BYTE *buff, LBA_t sector, UINT count)
{
  DRESULT res = RES_ERROR;
  MSC_LUNTypeDef info;
  USBH_StatusTypeDef  status = USBH_OK;

  if (buff == NULL)
  {
    USBH_ErrLog("The Data buffer buff should be different to NULL");
  }

  if (!((DWORD)buff & 3) && (((HCD_HandleTypeDef *)hUsb_Host.pData)->Init.dma_enable))
  {

    while (count--)
    {
      memcpy (scratch, &buff[count * FF_MAX_SS], FF_MAX_SS);

      status = USBH_MSC_Write(&hUsb_Host, lun, sector + count, (BYTE *)scratch, 1) ;
      if (status == USBH_FAIL)
      {
        break;
      }
    }
  }
  else
  {
    status = USBH_MSC_Write(&hUsb_Host, lun, sector, (BYTE *)buff, count);
  }

  if (status == USBH_OK)
  {
    res = RES_OK;
  }
  else
  {
    USBH_MSC_GetLUNInfo(&hUsb_Host, lun, &info);

    switch (info.sense.asc)
    {
    case SCSI_ASC_WRITE_PROTECTED:
      USBH_ErrLog("USB Disk is Write protected!");
      res = RES_WRPRT;
      break;

    case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
    case SCSI_ASC_MEDIUM_NOT_PRESENT:
    case SCSI_ASC_NOT_READY_TO_READY_CHANGE:
      USBH_ErrLog("USB Disk is not ready!");
      res = RES_NOTRDY;
      break;

    default:
      res = RES_ERROR;
      break;
    }
  }

  return res;
}

/**
 * @brief  I/O control operation
 * @param  lun : lun id
 * @param  cmd: Control code
 * @param  *buff: Buffer to send/receive control data
 * @retval DRESULT: return RES_OK otherwise
 */
static DRESULT USBH_ioctl(BYTE lun, BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  MSC_LUNTypeDef info;

  if (buff == NULL)
  {
    USBH_ErrLog("The Data buffer buff should be different to NULL");
  }

  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC:
    res = RES_OK;
    break;

  /* Get number of sectors on the disk (DWORD) */
  case GET_SECTOR_COUNT :
    if (USBH_MSC_GetLUNInfo(&hUsb_Host, lun, &info) == USBH_OK)
    {
      *(DWORD*)buff = info.capacity.block_nbr;
      res = RES_OK;
    }
    else
    {
      res = RES_ERROR;
    }
    break;

  /* Get R/W sector size (WORD) */
  case GET_SECTOR_SIZE :
    if (USBH_MSC_GetLUNInfo(&hUsb_Host, lun, &info) == USBH_OK)
    {
      *(WORD*)buff = info.capacity.block_size;
      res = RES_OK;
    }
    else
    {
      res = RES_ERROR;
    }
    break;

    /* Get erase block size in unit of sector (DWORD) */
  case GET_BLOCK_SIZE :

    if (USBH_MSC_GetLUNInfo(&hUsb_Host, lun, &info) == USBH_OK)
    {
      *(DWORD*)buff = info.capacity.block_size / USB_BLOCK_SIZE;
      res = RES_OK;
    }
    else
    {
      res = RES_ERROR;
    }
    break;

  default:
    res = RES_PARERR;
  }

  return res;
}
