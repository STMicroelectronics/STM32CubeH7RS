/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file            : usb_host.c
  * @version         : v1.1_Cube
  * @brief           : This file implements the USB Host
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

#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_hid.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
typedef enum {
  HID_APP_IDLE = 0,
  HID_APP_WAIT,
  HID_APP_MOUSE,
  HID_APP_KEYBOARD,
}HID_APP_State;
__IO HID_APP_State hid_app_state;
extern HID_MOUSE_Info_TypeDef mouse_info;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void HID_KEYBRD_App(USBH_HandleTypeDef *phost);
void HID_MOUSE_App(USBH_HandleTypeDef *phost);
static void HID_MOUSE_ProcessData(HID_MOUSE_Info_TypeDef *data);
/* USER CODE END PFP */

/* USB Host core handle declaration */
USBH_HandleTypeDef hUsbHostHS;
ApplicationTypeDef Appli_state = APPLICATION_IDLE;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */
#if defined (USE_USB_HS)
#define hUsbHost hUsbHostHS
#else
#define hUsbHost hUsbHostFS
#endif
/* USER CODE END 0 */

/*
 * user callback declaration
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Init USB host library, add supported class and start the library
  * @retval None
  */
void MX_USB_HOST_Init(void)
{
  /* USER CODE BEGIN USB_HOST_Init_PreTreatment */

  /* USER CODE END USB_HOST_Init_PreTreatment */

  /* Init host Library, add supported class and start the library. */
  if (USBH_Init(&hUsbHostHS, USBH_UserProcess, HOST_HS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_RegisterClass(&hUsbHostHS, USBH_HID_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_HOST_Init_PostTreatment */
#if defined(USE_USB_HS)
  USBH_UsrLog(" **** USB OTG HS Host **** \n");
#else
  USBH_UsrLog(" **** USB OTG FS Host **** \n");
#endif
  USBH_UsrLog("USB Host library started.\n");
  USBH_UsrLog("Starting HID Application");
  USBH_UsrLog("Connect your HID Device\n");
  hid_app_state = HID_APP_WAIT;
  /* USER CODE END USB_HOST_Init_PostTreatment */
}

/*
 * user callback definition
 */
static void USBH_UserProcess  (USBH_HandleTypeDef *phost, uint8_t id)
{
  /* USER CODE BEGIN CALL_BACK_1 */
  switch(id)
  {
  case HOST_USER_SELECT_CONFIGURATION:
  break;

  case HOST_USER_DISCONNECTION:
  Appli_state = APPLICATION_DISCONNECT;
  break;

  case HOST_USER_CLASS_ACTIVE:
  Appli_state = APPLICATION_READY;
  break;

  case HOST_USER_CONNECTION:
  Appli_state = APPLICATION_START;
  break;

  default:
  break;
  }
  /* USER CODE END CALL_BACK_1 */
}

/* USER CODE BEGIN 2 */

/**
  * @brief  Manages USB HOST Process.
  * @param  None
  * @retval None
  */
void MX_USB_HOST_Process(void)
{
  /* USB Host Background task */
  USBH_Process(&hUsbHost);
}

/**
  * @brief  Manages HID Menu Process.
  * @param  None
  * @retval None
  */
void HID_Process(void)
{
  switch(hid_app_state)
  {
  case HID_APP_WAIT:
    if(Appli_state == APPLICATION_READY)
    {
      if(USBH_HID_GetDeviceType(&hUsbHost) == HID_KEYBOARD)
      {
        hid_app_state = HID_APP_KEYBOARD;
        USBH_UsrLog("Use Keyboard to tape characters:");
      }

      else if(USBH_HID_GetDeviceType(&hUsbHost) == HID_MOUSE)
      {
        hid_app_state = HID_APP_MOUSE;
        USBH_UsrLog("USB HID Host Mouse App...");
      }
    }
    break;

  case HID_APP_MOUSE:
    if(Appli_state == APPLICATION_READY)
    {
      HID_MOUSE_App(&hUsbHost);
    }
    break;

  case HID_APP_KEYBOARD:
    if(Appli_state == APPLICATION_READY)
    {
      HID_KEYBRD_App(&hUsbHost);
    }
    break;

  default:
    break;
  }

  if(Appli_state == APPLICATION_DISCONNECT)
  {
    Appli_state = APPLICATION_IDLE;
    USBH_ErrLog("USB device disconnected !!! \n");
    hid_app_state = HID_APP_WAIT;
  } 
}

/**
  * @brief  Main routine for Mouse application
  * @param  phost: Host handle
  * @retval None
  */
void HID_MOUSE_App(USBH_HandleTypeDef *phost)
{
  HID_MOUSE_Info_TypeDef *m_pinfo;

  m_pinfo = USBH_HID_GetMouseInfo(phost);

  if(m_pinfo != NULL)
  {
    /* Handle Mouse data position */
    HID_MOUSE_ProcessData(&mouse_info);

    if(m_pinfo->buttons[0])
    {
      USBH_UsrLog("Left Button Pressed");
    }
    if(m_pinfo->buttons[1])
    {
      USBH_UsrLog("Right Button Pressed");
    }
    if(m_pinfo->buttons[2])
    {
      USBH_UsrLog("Middle Button Pressed");
    }
  }
}

/**
  * @brief  Processes Mouse data.
  * @param  data: Mouse data to be displayed
  * @retval None
  */
static void HID_MOUSE_ProcessData(HID_MOUSE_Info_TypeDef *data)
{
  if((data->x != 0) || (data->y != 0))
  {
    USBH_UsrLog("Mouse : X = %3d, Y = %3d", data->x, data->y);
  }

}

/**
  * @brief  Main routine for Keyboard application
  * @param  phost: Host handle
  * @retval None
  */
void HID_KEYBRD_App(USBH_HandleTypeDef *phost)
{
  HID_KEYBD_Info_TypeDef *k_pinfo;
  char c;
  k_pinfo = USBH_HID_GetKeybdInfo(phost);

  if(k_pinfo != NULL)
  {
    c = USBH_HID_GetASCIICode(k_pinfo);
    if(c != 0)
    {
      USBH_UsrLog("%c", c);
    }
  }
}
/* USER CODE END 2 */

/**
  * @}
  */

/**
  * @}
  */

