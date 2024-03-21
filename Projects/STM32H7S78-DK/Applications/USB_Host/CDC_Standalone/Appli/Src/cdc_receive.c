/**
  ******************************************************************************
  * @file    cdc_receive.c
  * @author  MCD Application Team
  * @brief   CDC Receive state machine
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
#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_cdc.h"
#include "cdc_receive.h"
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern USBH_HandleTypeDef hUsbHostFS;
uint8_t CDC_RX_Buffer[RX_BUFF_SIZE];
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void DumpReceivedData(void);

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/**
  * @brief  CDC data receive callback.
  * @param  phost: Host handle
  * @retval None
  */
void USBH_CDC_ReceiveCallback(USBH_HandleTypeDef *phost)
{
  USBH_CDC_Receive(&hUsbHostFS, CDC_RX_Buffer, RX_BUFF_SIZE);
  DumpReceivedData();
}

/**
  * @brief  Displays received data
  * @param  data: Keyboard data to be displayed
  * @retval None
  */
static void DumpReceivedData(void)
{
  uint16_t size;
  uint8_t *ptr = CDC_RX_Buffer;
  uint16_t count = 0;
  size = USBH_CDC_GetLastReceivedDataSize(&hUsbHostFS);
  /* Display the received data */
  for (count = 0; count < size ; count++)
  {
    USBH_UsrLog("%c", *ptr);
    ptr++;
  }
  /* Move to next reception */
  ptr++;
}

