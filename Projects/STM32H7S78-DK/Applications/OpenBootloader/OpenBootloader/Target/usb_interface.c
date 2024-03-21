/**
  ******************************************************************************
  * @file    usb_interface.c
  * @author  MCD Application Team
  * @brief   Contains USB protocol commands
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
#include "openbl_core.h"
#include "usb_interface.h"
#include "usbd_core.h"
#include "platform.h"
#include "usbd_def.h"
#include "usbd_dfu.h"
#include "usbd_ioreq.h"
#include "usbd_dfu_if.h"
#include "usbd_desc.h"
#include "usb_device.h"
#include "external_memory_interface.h"
#include "app_openbootloader.h"
#include "openbl_usb_cmd.h"
#include "flash_interface.h"
#include "usbd_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USB_RAM_BUFFER_SIZE             1164U     /* Size of USB buffer used to store received data from the host */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t UsbDetected = 0U;
static uint8_t USB_RAM_Buffer[USB_RAM_BUFFER_SIZE];
static uint32_t UserAppAddress = 0U;

/* Exported variables --------------------------------------------------------*/
uint8_t UsbSofDetected = 0U;

/* Private function prototypes -----------------------------------------------*/
static void OPENBL_USB_Loop(void);
static void OPENBL_USB_SystemResetCommand(void);

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Vendor specific commands
  * @param  address Pointer to the address.
  * @param  request_type Request Type.
  * @param  p_status Pointer that contains the command status.
  * @retval USBD_OK if operation is successful otherwise returns USBD_FAIL
  */
uint16_t OPENBL_DFU_Check(uint8_t *address, uint32_t request_type, uint32_t *p_status)
{
  *p_status = DFU_ERROR_NONE;

  return USBD_OK;
}

/**
  * @brief  This function is used to configure USB pins and then initialize the used USB instance.
  * @retval None.
  */
void OPENBL_USB_Configuration(void)
{
  /* Initialization USB device Library, add supported class and start the library */
  MX_USB_Device_Init();
}

/**
  * @brief  This function is used to De-initialize the USB pins and instance.
  * @retval None.
  */
void OPENBL_USB_DeInit(void)
{
  /* Only de-initialize the USB if it is not the current detected interface */
  if (UsbDetected == 0U)
  {
    __HAL_RCC_USB_OTG_FS_CLK_DISABLE();

    HAL_NVIC_DisableIRQ(OTG_FS_IRQn);

    __HAL_RCC_USB_OTG_FS_FORCE_RESET();
    __HAL_RCC_USB_OTG_FS_RELEASE_RESET();
  }
}

/**
  * @brief  This function is used to detect if there is any activity on USB protocol.
  * @retval None.
  */
uint8_t OPENBL_USB_ProtocolDetection(void)
{
  if (UsbSofDetected == 1U)
  {
    UsbDetected = 1U;

    /* Disable the other interfaces */
    OPENBL_InterfacesDeInit();

    OPENBL_USB_Loop();
  }
  else
  {
    UsbDetected = 0U;
  }

  return UsbDetected;
}

/**
  * @brief  This function is used to send a NACK when the address is not valid by changing
  *         the state of the USB to DFU error.
  * @param  p_dev Pointer to the USBD_HandleTypeDef structure.
  * @retval Returns USBD_FAIL.
  */
uint16_t OPENBL_USB_SendAddressNack(USBD_HandleTypeDef *p_dev)
{
  USBD_DFU_HandleTypeDef *hdfu;

  hdfu = (USBD_DFU_HandleTypeDef *)p_dev->pClassData;

  if (hdfu->dev_state == DFU_STATE_DNLOAD_BUSY)
  {
    hdfu->dev_state     = DFU_ERROR_TARGET;
    hdfu->dev_status[0] = DFU_ERROR_TARGET;
    hdfu->dev_status[1] = 0U;
    hdfu->dev_status[2] = 0U;
    hdfu->dev_status[3] = 0U;
    hdfu->dev_status[4] = DFU_STATE_ERROR;
    hdfu->dev_status[5] = 0U;
  }

  return (uint16_t)USBD_FAIL;
}

/**
  * @brief  Gets the sector that corresponds to a given address.
  * @param  address Address of the FLASH Memory.
  * @retval The sector of a given address.
  */
uint32_t OPENBL_USB_GetPage(uint32_t address)
{
  uint32_t sector;

  if (address < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    sector = (address - FLASH_BASE) / FLASH_SECTOR_SIZE;
  }
  else
  {
    sector = ((address - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_SECTOR_SIZE) + 8U;
  }

  return sector;
}

/**
  * @brief  Call adequate download command.
  * @param  p_buffer Pointer to data buffer that contains commands opcodes and .
  * @param  w_length Request length.
  * @param  p_status Used to return command status value.
  * @retval Returns USBD_OK if operation is successful else USBD_FAIL.
  */
uint16_t OPENBL_DFU_DownloadCommands(uint8_t *p_buffer, uint32_t block_number, uint32_t w_length, uint32_t *p_status)
{
  if (block_number == 0U)
  {
    if (w_length == 1U)
    {
      if (p_buffer[0] == DFU_CMD_ERASE)
      {
        if (Common_GetProtectionStatus() != RESET)
        {
          return (uint16_t)USBD_FAIL;
        }

        if (OPENBL_USB_MassEraseCommand() != USBD_OK)
        {
          return (uint16_t)USBD_FAIL;
        }
      }
    }
    else if (w_length == 5U)
    {
      /* Check if the received opcode is for special commands */
      if ((p_buffer[0] == SPECIAL_CMD_SYSTEM_RESET)
          || (p_buffer[0] == SPECIAL_CMD_EXT_MEM_INIT)
          || (p_buffer[0] == SPECIAL_CMD_EXT_MEM_ERASE))
      {
        OPENBL_USB_Ext_ProcessDownloadCmd(p_buffer[0], p_buffer, w_length, p_status);
      }
    }
    else
    {
      if (p_buffer[0] == SPECIAL_CMD_EXT_MEM_ERASE)
      {
        OPENBL_USB_Ext_ProcessDownloadCmd(p_buffer[0], p_buffer, w_length, p_status);
      }
    }
  }
  else
  {
    *p_status = DFU_ERROR_TARGET;

    return (uint16_t)USBD_FAIL;
  }

  return USBD_OK;
}

/**
  * @brief  Used to implement vendor specific upload commands.
  * @param  address Contains device memory address.
  * @param  block_number Request length.
  * @param  p_status Pointer that contains the command status.
  * @retval Returns USBD_OK if operation is successful otherwise returns USBD_FAIL.
  */
uint16_t OPENBL_DFU_UploadCommands(uint32_t address, uint32_t block_number, uint32_t *p_status)
{
  /* NOTE: Always return "IS_DFU_PHY_ADDRESS" as there are no extended special commands */

  *p_status = IS_DFU_PHY_ADDRESS;

  return USBD_OK;
}

/**
  * @brief  DFU download extended commands selection function.
  * @param  command The command opcode.
  * @param  p_buffer Pointer to the USB data buffer.
  * @param  p_status Pointer to the status request.
  * @retval Returns USBD_OK if operation was successful, or USBD_FAIL if there was an error.
  */
uint16_t OPENBL_USB_Ext_ProcessDownloadCmd(uint8_t command, uint8_t *p_buffer, uint32_t w_length, uint32_t *p_status)
{
  uint16_t usbd_status = USBD_OK ;
  uint32_t address;
  OPENBL_SpecialCmdTypeDef *p_special_cmd;

  switch (command)
  {
    case SPECIAL_CMD_EXT_MEM_INIT:
      address     = ((p_buffer[1] << 24U) | (p_buffer[2] << 16U) | (p_buffer[3] << 8U) | (p_buffer[4]));
      usbd_status = OPENBL_ExtMem_Init(address);

      break;

    case SPECIAL_CMD_EXT_MEM_ERASE:
      p_special_cmd              = (OPENBL_SpecialCmdTypeDef *)(uint32_t) USB_RAM_Buffer;
      p_special_cmd->SizeBuffer1 = w_length - 1U ;

      for (uint8_t index = 0U; index < (p_special_cmd->SizeBuffer1 + 1U); index++)
      {
        p_special_cmd->Buffer1[index] = p_buffer[index + 1];
      }

      usbd_status = OPENBL_ExtMem_Erase(p_special_cmd);

      break;

    case SPECIAL_CMD_SYSTEM_RESET:
      OPENBL_USB_SystemResetCommand();

      break;

    default:
      *p_status   = DFU_ERROR_VENDOR;
      usbd_status = USBD_FAIL;

      break;
  }

  return usbd_status;
}

/**
  * @brief  This function is used to perform a FLASH mass erase operation.
  * @retval Returns USBD_OK if operation is successful, else USBD_FAIL.
  */
uint16_t OPENBL_USB_MassEraseCommand(void)
{
  return USBD_FAIL;
}

/**
  * @brief  This function is used to set the address that will be used when jumping to user application.
  * @param  address User application address.
  * @retval None.
  */

void OPENBL_USB_SetUserApplicationAddress(uint32_t address)
{
  UserAppAddress = address;
}

/**
  * @brief This command is used to launch system reset.
  * @retval None.
  */
static void OPENBL_USB_SystemResetCommand(void)
{
  NVIC_SystemReset();
}

/**
  * @brief  This function is used to jump to the user application.
  * @retval None.
  */
static void OPENBL_USB_Loop(void)
{
  while (1U)
  {
    /* Jump to user application if a Go command is executed else continue the USB-DFU loop */
    if (DfuJumpToUserApp == 1U)
    {
      OPENBL_USB_Jump(UserAppAddress);
    }
  }
}
