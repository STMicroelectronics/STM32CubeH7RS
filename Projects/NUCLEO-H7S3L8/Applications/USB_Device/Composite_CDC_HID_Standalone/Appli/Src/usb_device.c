/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           usb_device.c
  * @author         MCD Application Team
  * @brief          This file implements the USB Device
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_hid.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "usbd_composite_builder.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */


/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t HID_Buffer[4];
extern PCD_HandleTypeDef hpcd_USB_OTG_HS;
#define CURSOR_STEP 5
/* Classes ID */
uint8_t CDC_InstID, HID_InstID = 0;
/* HID Endpoint Address */
uint8_t HID_EpAdress = HID_EPIN_ADDR;
uint8_t CDC_EpAdd_Inst1[3] = {CDC_IN_EP, CDC_OUT_EP, CDC_CMD_EP}; /* CDC Endpoint Address First Instance */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void GetPointerData(uint8_t *pbuf);
extern void SystemClockConfig_Resume(void);
/* USER CODE END PFP */

/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUsbDeviceHS;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */
/**
  * @brief  Gets Pointer Data.
  * @param  pbuf: Pointer to report
  * @retval None
  */
void GetPointerData(uint8_t * pbuf)
{
  static int8_t cnt = 0;
  int8_t x = 0, y = 0;

  if (cnt++ > 0)
  {
    x = CURSOR_STEP;
  }
  else
  {
    x = -CURSOR_STEP;
  }
  pbuf[0] = 0;
  pbuf[1] = x;
  pbuf[2] = y;
  pbuf[3] = 0;
}
/**
  * @brief  HAL_GPIO_EXTI_Callback
  *         EXTI line detection callback.
  * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == USER_BUTTON_Pin)
  {
    if ((((USBD_HandleTypeDef *) hpcd_USB_OTG_HS.pData)->dev_remote_wakeup == 1) &&
        (((USBD_HandleTypeDef *) hpcd_USB_OTG_HS.pData)->dev_state ==
         USBD_STATE_SUSPENDED))
    {
      if ((&hpcd_USB_OTG_HS)->Init.low_power_enable)
      {
        HAL_ResumeTick();
        SystemCoreClockUpdate();
      }
      /* Activate Remote wakeup */
      HAL_PCD_ActivateRemoteWakeup((&hpcd_USB_OTG_HS));

      /* Remote wakeup delay */
      HAL_Delay(10);

      /* Disable Remote wakeup */
      HAL_PCD_DeActivateRemoteWakeup((&hpcd_USB_OTG_HS));

      /* change state to configured */
      ((USBD_HandleTypeDef *) hpcd_USB_OTG_HS.pData)->dev_state = USBD_STATE_CONFIGURED;

      /* Change remote_wakeup feature to 0 */
      ((USBD_HandleTypeDef *) hpcd_USB_OTG_HS.pData)->dev_remote_wakeup = 0;
    }
    else if (((USBD_HandleTypeDef *) hpcd_USB_OTG_HS.pData)->dev_state ==
         USBD_STATE_CONFIGURED)
    {
      GetPointerData(HID_Buffer);
      USBD_HID_SendReport(&hUsbDeviceHS, HID_Buffer, 4, 0);
    }
    else
    {
      /* ... */
    }
  }
}
/* USER CODE END 1 */

/**
  * Init USB device Library, add supported class and start the library
  * @retval None
  */
void MX_USB_DEVICE_Init(void)
{
  /* USER CODE BEGIN USB_DEVICE_Init_PreTreatment */

  /* USER CODE END USB_DEVICE_Init_PreTreatment */

  /* Init Device Library */
  USBD_Init(&hUsbDeviceHS, &CMPST_Desc, DEVICE_HS);

  /* Store HID instance Class ID */
  HID_InstID = hUsbDeviceHS.classId;

  /* Register the HID  class */
  USBD_RegisterClassComposite(&hUsbDeviceHS, USBD_HID_CLASS, CLASS_TYPE_HID, &HID_EpAdress);

  /* Store CDC instance Class ID */
  CDC_InstID = hUsbDeviceHS.classId;

  /* Register CDC class First Instance */
  USBD_RegisterClassComposite(&hUsbDeviceHS, USBD_CDC_CLASS, CLASS_TYPE_CDC, CDC_EpAdd_Inst1);

    /* Add CDC Interface Class First Instance */
  if (USBD_CMPSIT_SetClassID(&hUsbDeviceHS, CLASS_TYPE_CDC, 0) != 0xFF)
  {
    USBD_CDC_RegisterInterface(&hUsbDeviceHS, &USBD_Interface_fops_HS);
  }

  /* USER CODE BEGIN USB_DEVICE_Init_PostTreatment */

  /* USER CODE END USB_DEVICE_Init_PostTreatment */
}

/**
  * @}
  */

/**
  * @}
  */

