/**
  ******************************************************************************
  * @file    cdc_send.c
  * @author  MCD Application Team
  * @brief   CDC Send state machine
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

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"
#include "usbh_cdc.h"
#include "cdc_send.h"
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern USBH_HandleTypeDef hUsbHostFS;
/* Private functions ---------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
uint8_t CDC_TX_Buffer[] = "USB_STM32_Host_CDC_ACM \n";
/**
  * @brief  CDC Send Data.
  * @param  None
  * @retval None
  */
void CDC_Send_Data(void)
{
  if (hUsbHostFS.pActiveClass == USBH_CDC_CLASS)
  {
    /* Start Sending data */
    USBH_UsrLog("Sending data ...");
    USBH_StatusTypeDef status = USBH_CDC_Transmit(&hUsbHostFS, CDC_TX_Buffer, sizeof(CDC_TX_Buffer));
    if (status == USBH_OK)
    {
      USBH_UsrLog("Data sent successfully");
    }
    else
    {
      USBH_ErrLog("Unable to send data");
    }
  }
}
