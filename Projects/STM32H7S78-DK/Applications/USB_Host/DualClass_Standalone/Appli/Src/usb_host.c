/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file            : usb_host.c
  * @author          : MCD Application Team
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
#include "usbh_msc.h"
#include "usbh_hid.h"

/* USER CODE BEGIN Includes */
#include "keyboard.h"
#include "mouse.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "usbh_diskio.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
char USBDISKPath[4];            /* USB Host logical drive path */
extern FATFS USBH_fatfs;
typedef enum {
  MSC_APP_IDLE = 0,
  MSC_APP_WAIT,
  MSC_APP_FILE_OPERATIONS,
  MSC_APP_EXPLORER,
}MSC_APP_State;
__IO MSC_APP_State MSC_app_state;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* USB Host core handle declaration */
USBH_HandleTypeDef hUsbHostHS;
ApplicationTypeDef Appli_state = APPLICATION_IDLE;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * user callback declaration
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */
extern uint8_t MSC_File_Operations(void);
extern FRESULT Explore_Disk(char *path, uint8_t recu_level);
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
  if (USBH_RegisterClass(&hUsbHostHS, USBH_MSC_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_HOST_Init_PostTreatment */
  USBH_UsrLog(" **** USB OTG HS Host **** \n");
  USBH_UsrLog("USB Host library started.\n");
  USBH_UsrLog("Starting MSC Application");
  USBH_UsrLog("Connect your MSC or HID Device\n");
  MSC_app_state = MSC_APP_WAIT;
  /* USER CODE END USB_HOST_Init_PostTreatment */
}

/*
 * user callback definition
 */
static void USBH_UserProcess (USBH_HandleTypeDef *phost, uint8_t id)
{
  /* USER CODE BEGIN CALL_BACK_1 */
  switch(id)
  {
    case HOST_USER_SELECT_CONFIGURATION:
      break;

    case HOST_USER_DISCONNECTION:
      if (f_mount(NULL, "", 0) != FR_OK)
      {
        USBH_ErrLog("ERROR : Cannot DeInitialize FatFs! \n");
      }
      if (FATFS_UnLinkDriver(USBDISKPath) != 0)
      {
        USBH_ErrLog("ERROR : Cannot UnLink FatFS Driver! \n");
      }
      Appli_state = APPLICATION_DISCONNECT;
      break;

    case HOST_USER_CLASS_ACTIVE:
      Appli_state = APPLICATION_READY;
      break;

    case HOST_USER_CONNECTION:
      Appli_state = APPLICATION_START;
      if (FATFS_LinkDriver(&USBH_Driver, USBDISKPath) == 0)
      {
        if (f_mount(&USBH_fatfs, (TCHAR const*)USBDISKPath, 0) != FR_OK)
        {
          USBH_ErrLog("ERROR : Cannot Initialize FatFs! \n");
        }
      }
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
  USBH_Process(&hUsbHostHS);
  if (hUsbHostHS.pActiveClass == USBH_MSC_CLASS)
  {
    /* MSC Application Process */
    MSC_Process();
  }
  else if (hUsbHostHS.pActiveClass == USBH_HID_CLASS)
  {
    /* HID Application Process */
    HID_Process();
  }
}

/**
  * @brief  Manages MSC Menu Process.
  * @param  None
  * @retval None
  */
void MSC_Process(void)
{
  switch(MSC_app_state)
  {

  case MSC_APP_IDLE:
    if(Appli_state == APPLICATION_START)
    {
      MSC_app_state = MSC_APP_WAIT;
    }
    break;

  case MSC_APP_WAIT:
    MSC_app_state = MSC_APP_FILE_OPERATIONS;
    break;

  case MSC_APP_FILE_OPERATIONS:
    /* Read and Write File Here */
    if(Appli_state == APPLICATION_READY)
    {
      USBH_UsrLog("\n\n*** Files operations ***\n");
      MSC_File_Operations();
      MSC_app_state = MSC_APP_EXPLORER;
    }
    break;

  case MSC_APP_EXPLORER:
    /* Display disk content */
    if(Appli_state == APPLICATION_READY)
    {
      USBH_UsrLog("\n\n*** Explore Disk ***\n");
      Explore_Disk("0:/", 1);
      MSC_app_state = MSC_APP_IDLE;
    }
    break;

  default:
    break;
  }
}

/**
* @brief  Manages HID Application Process.
* @param  None
* @retval None
*/
void HID_Process(void)
{
  /* USB Host Background task */
  USBH_Process(&hUsbHostHS);

  /* HID Application Process */
  if(USBH_HID_GetDeviceType(&hUsbHostHS) == HID_KEYBOARD)
  {
    HID_KEYBRD_App(&hUsbHostHS);
  }
  else if(USBH_HID_GetDeviceType(&hUsbHostHS) == HID_MOUSE)
  {
    HID_MOUSE_App(&hUsbHostHS);
  }
}
/* USER CODE END 2 */
/**
  * @}
  */

/**
  * @}
  */

