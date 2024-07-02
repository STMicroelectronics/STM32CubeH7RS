/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    : USB_Host/HID_Standalone/USB_Host/App/usb_host.c
  * @author  : MCD Application Team
  * @brief   : This file implements the USB Host
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "usbh_core.h"
#include "usbh_hid.h"
#include "usbpd_trace.h"
#include "cmsis_os2.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
typedef enum
{
  HOST_CLASS_APP_IDLE = 0,
  HOST_CLASS_APP_WAIT,
  HOST_CLASS_APP_MOUSE_HID,
  HOST_CLASS_NOT_SUPPORTED
}HOST_CLASS_STATE;
__IO HOST_CLASS_STATE host_class_app_state = HOST_CLASS_APP_IDLE;
extern HID_MOUSE_Info_TypeDef mouse_info;
QueueHandle_t UCPD_Host_MsgQueue;

extern QueueHandle_t UCPD_Device_MsgQueue;
extern HCD_HandleTypeDef hhcd_USB_OTG_HS;

osThreadId_t USBH_TaskHandle;
const osThreadAttr_t USBH_Task_attributes = {
  .name = "USBH_Task",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};;

osThreadId_t UCPD_Host_App_TaskHandle;
const osThreadAttr_t UCPD_Host_App_attributes = {
  .name = "UCPD_Host_App",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};;


#if defined ( __ICCARM__ ) /* IAR Compiler */
  #pragma data_alignment=4
#endif /* defined ( __ICCARM__ ) */
__ALIGN_BEGIN USB_DRD_ModeMsg_TypeDef USB_DRD_State_Msg __ALIGN_END;
uint8_t interface;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void HID_MOUSE_App(USBH_HandleTypeDef *phost);
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
  if (USBH_Start(&hUsbHostHS) != USBH_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_HOST_Init_PostTreatment */

  /* USER CODE END USB_HOST_Init_PostTreatment */
}

/*
 * user callback definition
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
  /* USER CODE BEGIN CALL_BACK_1 */
  switch(id)
  {
  case HOST_USER_SELECT_CONFIGURATION:
  break;

  case HOST_USER_DISCONNECTION:
    Appli_state = APPLICATION_DISCONNECT;
    host_class_app_state = HOST_CLASS_APP_IDLE;
  break;

  case HOST_USER_CLASS_ACTIVE:
    Appli_state = APPLICATION_READY;
    interface = USBH_FindInterface(&hUsbHostHS, hUsbHostHS.pActiveClass->ClassCode, HID_BOOT_CODE, 0xFFU);
    if (hUsbHostHS.device.CfgDesc.Itf_Desc[interface].bInterfaceProtocol  == HID_MOUSE_BOOT_CODE)
    {
      host_class_app_state = HOST_CLASS_APP_MOUSE_HID;
    }
    else
    {
      host_class_app_state = HOST_CLASS_NOT_SUPPORTED;
    }
  break;

  case HOST_USER_CONNECTION:
    Appli_state = APPLICATION_START;
    host_class_app_state = HOST_CLASS_APP_WAIT;
  break;

  default:
  break;
  }
  /* USER CODE END CALL_BACK_1 */
}

/* USER CODE BEGIN 2 */

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
    if(( m_pinfo->x) || (m_pinfo->y))
    {
      char _str[30];
      sprintf(_str, "Mouse : X = %3d, Y = %3d",  m_pinfo->x, m_pinfo->y);
      USBPD_TRACE_Add(USBPD_TRACE_DEBUG, 0, 0, (uint8_t*)_str, strlen(_str));
    }
    if(m_pinfo->buttons[0])
    {
      USBPD_TRACE_Add(USBPD_TRACE_DEBUG, 0, 0, (uint8_t *)"Left Button Pressed", 19);
    }
    if(m_pinfo->buttons[1])
    {
      USBPD_TRACE_Add(USBPD_TRACE_DEBUG, 0, 0, (uint8_t *)"Right Button Pressed", 20);
    }
    if(m_pinfo->buttons[2])
    {
      USBPD_TRACE_Add(USBPD_TRACE_DEBUG, 0, 0, (uint8_t *)"Middle Button Pressed", 21);
    }
  }
}

/**
  * @brief  Function implementing the UCPD_Host_App_Thread.
  * @param  argument: Not used
  * @retval None
  */
void UCPD_Host_App_Thread(void *argument)
{
  while (1)
  {
    /* wait for message queue from callback event */
    if (xQueueReceive(UCPD_Host_MsgQueue, &USB_DRD_State_Msg, portMAX_DELAY) != pdPASS)
    {
      Error_Handler();
    }

    /* Check if received message equal to START_USB_HOST */
    if (USB_DRD_State_Msg.HostState == START_USB_HOST)
    {
      /* Start USB Host */
      MX_USB_HOST_Init();
    }
    /* Check if received message equal to STOP_USB_HOST */
    else if (USB_DRD_State_Msg.HostState == STOP_USB_HOST)
    {
      /* Stop USB Host */
      if (hhcd_USB_OTG_HS.State == HAL_HCD_STATE_READY)
      {
        /* Stop USB Host */
        USBH_Stop(&hUsbHostHS);
        /* Free allocated resource for USBH process */
        osThreadTerminate(hUsbHostHS.thread);
        osMessageQueueDelete(hUsbHostHS.os_event);
      }

      /* Start USB Device */
      if (USB_DRD_State_Msg.DeviceState == START_USB_DEVICE)
      {
        /* Send message to start device */
        if (xQueueSend(UCPD_Device_MsgQueue, &USB_DRD_State_Msg, portMAX_DELAY) != pdPASS)
        {
          Error_Handler();
        }
      }
    }
    /* Else Error */
    else
    {
      /*Error*/
      Error_Handler();
    }
  }
}

/* USER CODE BEGIN Header__USBH_Task */
/**
  * @brief  Function implementing the USBH_Task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header__USBH_Task */
void USBH_Task(void *argument)
{
  /* USER CODE BEGIN 5 */
  while (1)
  {
    /* HID Menu Process */
    if((Appli_state == APPLICATION_READY) && (hUsbHostHS.pActiveClass != NULL))
    {
      if (host_class_app_state == HOST_CLASS_APP_MOUSE_HID)
      {
        HID_MOUSE_App(&hUsbHostHS);
      }
    }
    osDelay(1);
  }
  /* USER CODE END 5 */
}

void USB_HOST_RTOS_Init(void)
{
  USBH_TaskHandle = osThreadNew(USBH_Task, NULL, &USBH_Task_attributes);
  UCPD_Host_App_TaskHandle = osThreadNew(UCPD_Host_App_Thread, NULL, &UCPD_Host_App_attributes);

  /* Create the MsgQueue for UCPD_Host_MsgQueue */
  UCPD_Host_MsgQueue = osMessageQueueNew(30, sizeof(QueueHandle_t),NULL);
  if (UCPD_Host_MsgQueue == NULL)
  {
    Error_Handler();
  }
}
/* USER CODE END 2 */

/**
  * @}
  */

/**
  * @}
  */

