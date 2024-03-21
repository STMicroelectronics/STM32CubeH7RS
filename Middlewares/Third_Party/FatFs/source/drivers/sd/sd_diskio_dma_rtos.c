/**
  ******************************************************************************
  * @file    sd_diskio_dma_rtos.c
  * @author  MCD Application Team
  * @brief   SD Disk I/O DMA with RTOS driver
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
#include "sd_diskio_dma_rtos.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define QUEUE_SIZE         (uint32_t) 10
#define READ_CPLT_MSG      (uint32_t) 1
#define WRITE_CPLT_MSG     (uint32_t) 2
#define RW_ABORT_MSG       (uint32_t) 3

/* Private variables ---------------------------------------------------------*/
#ifndef BLOCKSIZE
#define BLOCKSIZE   512
#endif

#if (ENABLE_SD_DMA_CACHE_MAINTENANCE == 1)
static uint8_t scratch[BLOCKSIZE]__attribute__ ((aligned (32))); // 32-Byte aligned for cache maintenance
#else
__ALIGN_BEGIN static uint8_t scratch[BLOCKSIZE] __ALIGN_END;
#endif

/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;
static osMessageQueueId_t SDQueueID = NULL;

/* Private function prototypes -----------------------------------------------*/
static int SD_check_status_with_timeout (uint32_t);
static DSTATUS SD_check_status (BYTE);
static DSTATUS SD_DMA_initialize (BYTE);
static DSTATUS SD_DMA_status (BYTE);
static DRESULT SD_DMA_read (BYTE, BYTE*, LBA_t, UINT);
static DRESULT SD_DMA_write (BYTE, const BYTE*, LBA_t, UINT);
static DRESULT SD_DMA_ioctl (BYTE, BYTE, void*);

const Diskio_drvTypeDef SD_DMA_Driver =
{
  SD_DMA_initialize,
  SD_DMA_status,
  SD_DMA_read,
  SD_DMA_write,
  SD_DMA_ioctl
};

/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Check the status of the sd card
 * @param  lun : not used
 * @retval DSTATUS: return 0 if the sd card is ready and -1 otherwise
 */
static int SD_check_status_with_timeout(uint32_t timeout)
{
  uint32_t timer;

    timer = osKernelGetTickCount();
  /* block until SDIO IP is ready or a timeout occur */
  while(osKernelGetTickCount() - timer < timeout)
  {
    if (HAL_SD_GetCardState(&sdmmc_handle) == HAL_SD_CARD_TRANSFER)
    {
      return 0;
    }
  }

  return -1;
}

/**
 * @brief  Check the status of the sd card
 * @param  lun : not used
 * @retval DSTATUS: return 0 if the sd card is ready and 1 otherwise
 */
static DSTATUS SD_check_status(BYTE lun)
{
  Stat = STA_NOINIT;

  if (HAL_SD_GetCardState(&sdmmc_handle) == HAL_SD_CARD_TRANSFER)
  {
    Stat &= ~STA_NOINIT;
  }

  return Stat;
}

/**
 * @brief  Initialize the SD Diskio lowlevel driver
 * @param  lun : not used
 * @retval DSTATUS: return 0 on Success STA_NOINIT otherwise
 */
static DSTATUS SD_DMA_initialize(BYTE lun)
{
  Stat = STA_NOINIT;

  if(osKernelGetState() == osKernelRunning)
  {
#if (ENABLE_SD_INIT == 1)

  sdmmc_sd_init();
#endif

  Stat = SD_check_status(lun);

    /*
     * if the SD is correctly initialized, create the operation queue
     */

    if (Stat != STA_NOINIT)
    {
      if (SDQueueID == NULL)
      {
      SDQueueID = osMessageQueueNew(QUEUE_SIZE, 2, NULL);
      }

      if (SDQueueID == NULL)
      {
        Stat |= STA_NOINIT;
      }
    }
  }

  return Stat;
}

/**
 * @brief  Get the Disk Status
 * @param  lun : not used
 * @retval DSTATUS: return 0 if the sd card is ready and 1 otherwise status
 */
static DSTATUS SD_DMA_status(BYTE lun)
{
  return SD_check_status(lun);
}

/**
 * @brief  Read data from sd card into a buffer
 * @param  lun : not used
 * @param  *buff: Data buffer to store read data
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to read (1..128)
 * @retval DRESULT: return RES_OK otherwise
 */
static DRESULT SD_DMA_read(BYTE lun, BYTE *buff, LBA_t sector, UINT count)
{
  DRESULT res = RES_ERROR;
  uint32_t timer;
  uint16_t event;
  osStatus_t status;
  uint8_t ret;
  int i;

  /*
  * ensure the SDCard is ready for a new operation
  */

  if (SD_check_status_with_timeout(SD_TIMEOUT) < 0)
  {
    return res;
  }
  /* Check if buffer currently used is aligned on 32 bytes address */
  if (!((uint32_t)buff & 0x1F))
  {
   if(HAL_SD_ReadBlocks_DMA(&sdmmc_handle, (uint8_t*)buff, sector, count) == HAL_OK)
   {
    status = osMessageQueueGet(SDQueueID, (void *)&event, NULL, SD_TIMEOUT);
    if ((status == osOK) && (event == READ_CPLT_MSG))
    {
      timer = osKernelGetTickCount();
      /* block until SDIO IP is ready or a timeout occur */
      while(osKernelGetTickCount() - timer < SD_TIMEOUT)
      {
        if (HAL_SD_GetCardState(&sdmmc_handle) == HAL_SD_CARD_TRANSFER)
        {
          res = RES_OK;
#if (ENABLE_SD_DMA_CACHE_MAINTENANCE == 1)
          /*
            the SCB_InvalidateDCache_by_Addr() requires a 32-Byte aligned address,
            adjust the address and the D-Cache size to invalidate accordingly.
           */
          SCB_InvalidateDCache_by_Addr((uint32_t*)buff, count*BLOCKSIZE);
#endif
          break;
        }
      }
    }
   }
  }
  else
  {
    /* Slow path, fetch each sector a part and memcpy to destination buffer */
    for (i = 0; i < count; i++)
    {
      ret = HAL_SD_ReadBlocks_DMA(&sdmmc_handle, (uint8_t*)scratch, (uint32_t)sector++, 1);
      if (ret == HAL_OK)
      {
        /* wait until the read is successful or a timeout occurs */
        status = osMessageQueueGet(SDQueueID, (void *)&event, NULL, SD_TIMEOUT);
        if ((status == osOK) && (event == READ_CPLT_MSG))
        {
          timer = osKernelGetTickCount();
          /* block until SDIO IP is ready or a timeout occur */
          while(osKernelGetTickCount() - timer < SD_TIMEOUT)
          {
            if ( HAL_SD_GetCardState(&sdmmc_handle) == HAL_SD_CARD_TRANSFER)
            {
              break;
            }
          }
        }
#if (ENABLE_SD_DMA_CACHE_MAINTENANCE == 1)
        /*
         *
         * invalidate the scratch buffer before the next read to get the actual data instead of the cached one
         */
        SCB_InvalidateDCache_by_Addr((uint32_t*)scratch, BLOCKSIZE);
#endif
        memcpy(buff, scratch, BLOCKSIZE);
        buff += BLOCKSIZE;
      }
      else
      {
        break;
      }
    }
    if ((i == count) && (ret == HAL_OK))
      res = RES_OK;
  }

  return res;
}

/**
 * @brief  Write data from sd card into a buffer
 * @param  lun : not used
 * @param  *buff: Data to be written
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to write (1..128)
 * @retval DRESULT: return RES_OK otherwise
 */
static DRESULT SD_DMA_write(BYTE lun, const BYTE *buff, LBA_t sector, UINT count)
{
  DRESULT res = RES_ERROR;
  uint32_t timer;
  uint16_t event;
  osStatus_t status;
  uint8_t ret;
  int i;
  /*
  * ensure the SDCard is ready for a new operation
  */

  if (SD_check_status_with_timeout(SD_TIMEOUT) < 0)
  {
    return res;
  }
  /* Check if buffer currently used is aligned on 32 bytes address */
  if (!((uint32_t)buff & 0x1F))
  {
#if (ENABLE_SD_DMA_CACHE_MAINTENANCE == 1)
  /*
    the SCB_CleanDCache_by_Addr() requires a 32-Byte aligned address
    adjust the address and the D-Cache size to clean accordingly.
  */
  SCB_CleanDCache_by_Addr((uint32_t*)buff, count*BLOCKSIZE);
#endif

   if(HAL_SD_WriteBlocks_DMA(&sdmmc_handle, (uint8_t*)buff, sector, count) == HAL_OK)
   {
    status = osMessageQueueGet(SDQueueID, (void *)&event, NULL, SD_TIMEOUT);
    if ((status == osOK) && (event == WRITE_CPLT_MSG))
    {
       timer = osKernelGetTickCount();
       /* block until SDIO IP is ready or a timeout occur */
       while(osKernelGetTickCount() - timer  < SD_TIMEOUT)
       {
         if (HAL_SD_GetCardState(&sdmmc_handle) == HAL_SD_CARD_TRANSFER)
         {
           res = RES_OK;
           break;
         }
       }
    }
   }
  }
  else
  {
    /* Slow path, fetch each sector a part and memcpy to destination buffer */

#if (ENABLE_SD_DMA_CACHE_MAINTENANCE == 1)
    /*
     * invalidate the scratch buffer before the next write to get the actual data instead of the cached one
     */
    SCB_InvalidateDCache_by_Addr((uint32_t*)scratch, BLOCKSIZE);
#endif
    for (i = 0; i < count; i++)
    {
      memcpy((void *)scratch, buff, BLOCKSIZE);
      buff += BLOCKSIZE;

      ret = HAL_SD_WriteBlocks_DMA(&sdmmc_handle, (uint8_t*)scratch, (uint32_t)sector++, 1);
      if (ret == HAL_OK )
      {
        /* wait until the read is successful or a timeout occurs */
        status = osMessageQueueGet(SDQueueID, (void *)&event, NULL, SD_TIMEOUT);
        if ((status == osOK) && (event == WRITE_CPLT_MSG))
        {
           timer = osKernelGetTickCount();
           /* block until SDIO IP is ready or a timeout occur */
           while(osKernelGetTickCount() - timer < SD_TIMEOUT)
           {
             if (HAL_SD_GetCardState(&sdmmc_handle) == HAL_SD_CARD_TRANSFER)
             {
               break;
             }
           }
        }
      }
      else
      {
        break;
      }
    }
    if ((i == count) && (ret == HAL_OK))
      res = RES_OK;
  }

  return res;
}

/**
 * @brief  I/O control operation
 * @param  lun : not used
 * @param  cmd: Control code
 * @param  *buff: Buffer to send/receive control data
 * @retval DRESULT: return RES_OK otherwise
 */
static DRESULT SD_DMA_ioctl(BYTE lun, BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  HAL_SD_CardInfoTypeDef CardInfo;

  if (Stat & STA_NOINIT) return RES_NOTRDY;

  switch (cmd)
  {
     /* Make sure that no pending write process */
     case CTRL_SYNC:
       res = RES_OK;
       break;

     /* Get number of sectors on the disk (DWORD) */
     case GET_SECTOR_COUNT:
       HAL_SD_GetCardInfo(&sdmmc_handle, &CardInfo);
       *(DWORD*)buff = CardInfo.LogBlockNbr;
       res = RES_OK;
       break;

     /* Get R/W sector size (WORD) */
     case GET_SECTOR_SIZE:
       HAL_SD_GetCardInfo(&sdmmc_handle, &CardInfo);
       *(WORD*)buff = CardInfo.LogBlockSize;
       res = RES_OK;
       break;

     /* Get erase block size in unit of sector (DWORD) */
     case GET_BLOCK_SIZE:
       HAL_SD_GetCardInfo(&sdmmc_handle, &CardInfo);
       *(DWORD*)buff = CardInfo.LogBlockSize / BLOCKSIZE;
       res = RES_OK;
       break;

     default:
       res = RES_PARERR;
  }

  return res;
}

/**
  * @brief Tx Transfer completed callbacks
  * @param hsd: SD handle
  * @retval None
  */

void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
  /*
   * No need to add an "osKernelRunning()" check here, as the SD_initialize()
   * is always called before any SD_Read()/SD_Write() call
   */
   const uint16_t msg = WRITE_CPLT_MSG;
   osMessageQueuePut(SDQueueID, (const void *)&msg, 0, 0);
}

void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
  /*
   * No need to add an "osKernelRunning()" check here, as the SD_initialize()
   * is always called before any SD_Read()/SD_Write() call
   */
    const uint16_t msg = READ_CPLT_MSG;
    osMessageQueuePut(SDQueueID, (const void *)&msg, 0, 0);
}
