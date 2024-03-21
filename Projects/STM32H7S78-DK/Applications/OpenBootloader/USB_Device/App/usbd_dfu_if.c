/**
  ******************************************************************************
  * @file           usbd_dfu_if.c
  * @author         MCD Application Team
  * @brief          Memory management layer
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
#include "usbd_conf.h"
#include "usbd_dfu.h"
#include "usbd_dfu_if.h"

#include "common_interface.h"

#include "openbl_core.h"
#include "openbl_mem.h"
#include "openbl_usb_cmd.h"

#include "app_openbootloader.h"
#include "usb_interface.h"
#include "flash_interface.h"
#include "external_memory_interface.h"

/* External variables --------------------------------------------------------*/
uint8_t __IO DfuJumpToUserApp = 0U;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define MEDIA_DESC_STR      "@Internal Flash   /0x08000000/8*008Kg"

#define MEDIA_ERASE_TIME    (uint16_t)5
#define MEDIA_PROGRAM_TIME  (uint16_t)5

static uint16_t USB_DFU_If_Init(void);
static uint16_t USB_DFU_If_Erase(uint32_t Add);
static uint16_t USB_DFU_If_Write(uint8_t *src, uint8_t *dest, uint32_t Len);
static uint8_t *USB_DFU_If_Read(uint8_t *src, uint8_t *dest, uint32_t Len);
static uint16_t USB_DFU_If_DeInit(void);
static uint16_t USB_DFU_If_GetStatus(uint32_t Add, uint8_t Cmd, uint8_t *buffer);
#if (USBD_DFU_VENDOR_CMD_ENABLED == 1U)
uint16_t USB_DFU_If_GetVendorCMD(uint8_t *cmd, uint8_t *cmdlength);
uint16_t USB_DFU_If_VendorDownloadCMD(uint8_t *pbuf, uint32_t BlockNumber, uint32_t wlength, uint32_t *status);
uint16_t USB_DFU_If_VendorUploadCMD(uint32_t Add, uint32_t BlockNumber, uint32_t *status);
#endif /* USBD_DFU_VENDOR_CMD_ENABLED */
#if (USBD_DFU_VENDOR_CHECK_ENABLED == 1U)
uint16_t USB_DFU_If_VendorCheck(uint8_t *Add, uint32_t ReqType, uint32_t *status);
#endif /* USBD_DFU_VENDOR_CHECK_ENABLED */
#if (USBD_DFU_VENDOR_EXIT_ENABLED == 1U)
uint16_t USB_DFU_If_LeaveDFU(uint32_t Addr);
#endif /* USBD_DFU_VENDOR_EXIT_ENABLED */

__ALIGN_BEGIN USBD_DFU_MediaTypeDef USBD_DFU_Media_fops __ALIGN_END =
{
  (uint8_t *)MEDIA_DESC_STR,
  USB_DFU_If_Init,
  USB_DFU_If_DeInit,
  USB_DFU_If_Erase,
  USB_DFU_If_Write,
  USB_DFU_If_Read,
  USB_DFU_If_GetStatus,
#if (USBD_DFU_VENDOR_CMD_ENABLED == 1U)
  USB_DFU_If_GetVendorCMD,
  USB_DFU_If_VendorDownloadCMD,
  USB_DFU_If_VendorUploadCMD,
#endif /* USBD_DFU_VENDOR_CMD_ENABLED */
#if (USBD_DFU_VENDOR_CHECK_ENABLED == 1U)
  USB_DFU_If_VendorCheck,
#endif /* USBD_DFU_VENDOR_CHECK_ENABLED */
#if (USBD_DFU_VENDOR_EXIT_ENABLED == 1U)
  USB_DFU_If_LeaveDFU
#endif /* USBD_DFU_VENDOR_EXIT_ENABLED */
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Memory initialization routine.
  * @retval USBD_OK if operation is successful, MAL_FAIL else.
  */
uint16_t USB_DFU_If_Init(void)
{
  return 0U;
}

/**
  * @brief  De-Initializes Memory
  * @retval USBD_OK if operation is successful, MAL_FAIL else
  */
uint16_t USB_DFU_If_DeInit(void)
{
  return 0U;
}

/**
  * @brief  Erase sector.
  * @param  Add: Address of sector to be erased.
  * @retval 0 if operation is successful, MAL_FAIL else.
  */
uint16_t USB_DFU_If_Erase(uint32_t Add)
{
  uint16_t status;

  if (OPENBL_MEM_GetAddressArea((uint32_t)Add) == AREA_ERROR)
  {
    status = OPENBL_USB_SendAddressNack(&hUsbDeviceFS);
  }
  else
  {
    status = OPENBL_USB_EraseMemory(Add);
  }

  return status;
}

/**
  * @brief  Memory write routine.
  * @param  src: Pointer to the source buffer. Address to be written to.
  * @param  dest: Pointer to the destination buffer.
  * @param  Len: Number of data to be written (in bytes).
  * @retval USBD_OK if operation is successful, MAL_FAIL else.
  */
uint16_t USB_DFU_If_Write(uint8_t *pSrc, uint8_t *pDest, uint32_t Len)
{
  uint16_t status;

  if (OPENBL_MEM_GetAddressArea((uint32_t)pDest) == AREA_ERROR)
  {
    status = OPENBL_USB_SendAddressNack(&hUsbDeviceFS);
  }
  else
  {
    OPENBL_USB_WriteMemory(pSrc, ((uint8_t *)(&(pDest))), Len);

    status = 0U;
  }

  return status;
}

/**
  * @brief  Memory read routine.
  * @param  src: Pointer to the source buffer. Address to be written to.
  * @param  dest: Pointer to the destination buffer.
  * @param  Len: Number of data to be read (in bytes).
  * @retval Pointer to the physical address where data should be read.
  */
uint8_t *USB_DFU_If_Read(uint8_t *pSrc, uint8_t *pDest, uint32_t Len)
{
  return OPENBL_USB_ReadMemory(((uint8_t *)(&(pSrc))), pDest, Len);
}

/**
  * @brief  Get status routine
  * @param  Add: Address to be read from
  * @param  Cmd: Number of data to be read (in bytes)
  * @param  pBuffer: used for returning the time necessary for a program or an erase operation
  * @retval USBD_OK if operation is successful
  */
uint16_t USB_DFU_If_GetStatus(uint32_t Add, uint8_t Cmd, uint8_t *pBuffer)
{
  switch (Cmd)
  {
    case DFU_MEDIA_PROGRAM:
      pBuffer[1] = (uint8_t)MEDIA_PROGRAM_TIME;
      pBuffer[2] = (uint8_t)(MEDIA_PROGRAM_TIME << 8U);
      pBuffer[3] = 0U;

      break;

    case DFU_MEDIA_ERASE:
    default:
      pBuffer[1] = (uint8_t)MEDIA_ERASE_TIME;
      pBuffer[2] = (uint8_t)(MEDIA_ERASE_TIME << 8U);
      pBuffer[3] = 0U;

      break;
  }

  return (USBD_OK);
}



#if (USBD_DFU_VENDOR_CMD_ENABLED == 1U)
/**
  * @brief  Get supported vendor specific commands.
  * @param  cmd Pointer to supported vendor commands.
  * @param  cmdlength Pointer to length of supported vendor commands.
  * @retval Always returns USBD_OK.
  */
uint16_t USB_DFU_If_GetVendorCMD(uint8_t *cmd, uint8_t *cmdlength)
{
  /* Fill the commands buffer with the supported commands opcodes */
  cmd[0U] = SPECIAL_CMD_SYSTEM_RESET;
  cmd[1U] = SPECIAL_CMD_EXT_MEM_INIT;
  cmd[2U] = SPECIAL_CMD_EXT_MEM_ERASE;

  /* Set the number of supported commands */
  *cmdlength = 3U;

  return USBD_OK;
}

/**
  * @brief  Used to implement vendor specific download commands.
  * @param  pbuf Pointer to supported vendor commands.
  * @param  wlength Request wlength.
  * @param  status Used to return status value.
  * @retval USBD_OK if operation is successful else USBD_FAIL.
  */
uint16_t USB_DFU_If_VendorDownloadCMD(uint8_t *pbuf, uint32_t BlockNumber, uint32_t wlength, uint32_t *status)
{
  uint16_t state;

  state = OPENBL_DFU_DownloadCommands(pbuf, BlockNumber, wlength, status);

  return state;
}

/**
  * @brief  Used to implement vendor specific upload commands.
  * @param  Add Pointer to address.
  * @param  block_number Request length.
  * @param  status Pointer that contains the command status.
  * @retval USBD_OK if operation is successful otherwise returns USBD_FAIL.
  */
uint16_t USB_DFU_If_VendorUploadCMD(uint32_t Add, uint32_t BlockNumber, uint32_t *status)
{
  uint16_t state;

  state = OPENBL_DFU_UploadCommands(Add, BlockNumber, status);

  return state;
}
#endif /* USBD_DFU_VENDOR_CMD_ENABLED */


#if (USBD_DFU_VENDOR_CHECK_ENABLED == 1U)
/**
  * @brief  Vendor specific commands
  * @param  Add Pointer to the data buffer.
  * @param  ReqType Request Type.
  * @param  status Pointer that contains the command status.
  * @retval USBD_OK if operation is successful otherwise returns USBD_FAIL
  */
uint16_t USB_DFU_If_VendorCheck(uint8_t *Add, uint32_t ReqType, uint32_t *status)
{
  uint32_t state;

  state = OPENBL_DFU_Check(Add, ReqType, status);

  return state;
}
#endif /* USBD_DFU_VENDOR_CHECK_ENABLED */

#if (USBD_DFU_VENDOR_EXIT_ENABLED == 1U)
/**
  * @brief  Vendor Leave DFU
  * @param  Addr address
  * @retval 0 if operation is successful
  */
uint16_t USB_DFU_If_LeaveDFU(uint32_t Addr)
{
  /* Set USB jump token */
  DfuJumpToUserApp = 1U ;

  OPENBL_USB_SetUserApplicationAddress(Addr);

  return 0U;
}
#endif /* USBD_DFU_VENDOR_EXIT_ENABLED */
