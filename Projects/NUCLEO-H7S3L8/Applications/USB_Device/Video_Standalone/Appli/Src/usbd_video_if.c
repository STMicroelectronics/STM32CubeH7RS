/**
  ******************************************************************************
  * @file    usbd_video_if.c
  * @author  MCD Application Team
  * @brief   Video Interface application layer functions file
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
#include "usbd_video_if.h"

/* USER CODE BEGIN INCLUDE */
#ifdef UVC_USE_STREAM1
#include "stream1.h"
#endif
#ifdef UVC_USE_STREAM2
#include "stream2.h"
#endif
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device library.
  * @{
  */

/** @addtogroup USBD_VIDEO_IF
  * @{
  */

/** @defgroup USBD_VIDEO_IF_Private_TypesDefinitions USBD_VIDEO_IF_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_VIDEO_IF_Private_Defines USBD_VIDEO_IF_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */

/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_VIDEO_IF_Private_Macros USBD_VIDEO_IF_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_VIDEO_IF_Private_Variables USBD_VIDEO_IF_Private_Variables
  * @brief Private variables.
  * @{
  */

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_VIDEO_IF_Exported_Variables USBD_VIDEO_IF_Exported_Variables
  * @brief Public variables.
  * @{
  */



/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_VIDEO_IF_Private_FunctionPrototypes USBD_VIDEO_IF_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t VIDEO_Itf_Init     (void);
static int8_t VIDEO_Itf_DeInit   (void);
static int8_t VIDEO_Itf_Control  (uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t VIDEO_Itf_Data     (uint8_t** pbuf, uint16_t* psize, uint16_t* pcktidx);


/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_VIDEO_ItfTypeDef USBD_VIDEO_fops =
{
  VIDEO_Itf_Init,
  VIDEO_Itf_DeInit,
  VIDEO_Itf_Control,
  VIDEO_Itf_Data,
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the VIDEO media low layer over USB FS IP
  * @param  VIDEOFreq: VIDEO frequency used to play the VIDEO stream.
  * @param  Volume: Initial volume level (from 0 (Mute) to 100 (Max))
  * @param  options: Reserved for future use
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t VIDEO_Itf_Init(void)
{
  /*
     Add your initialization code here
  */

  return (0);
}

/**
  * @brief  TEMPLATE_DeInit
  *         DeInitializes the UVC media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t VIDEO_Itf_DeInit(void)
{
  /*
     Add your deinitialization code here
  */
  return (0);
}


/**
  * @brief  TEMPLATE_Control
  *         Manage the UVC class requests
  * @param  Cmd: Command code
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t VIDEO_Itf_Control(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  return (0);
}


/**
  * @brief  TEMPLATE_Data
  *         Manage the UVC data packets
  * @param  pbuf: pointer to the buffer data to be filled
  * @param  psize: pointer to the current packet size to be filled
  * @param  pcktidx: pointer to the current packet index in the current image
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t VIDEO_Itf_Data(uint8_t** pbuf, uint16_t* psize, uint16_t* pcktidx)
{
  const uint8_t* (*ImagePtr)= tImagesList;
  uint32_t packet_count = (tImagesSizes[img_count])/((uint16_t)(UVC_PACKET_SIZE-2));
  uint32_t packet_remainder =(tImagesSizes[img_count])%((uint16_t)(UVC_PACKET_SIZE-2));
  static uint8_t packet_index = 0;

  /* Check if end of current image has been reached */
  if (packet_index < packet_count)
  {
    /* Set the current packet size */
    *psize = (uint16_t)UVC_PACKET_SIZE;

    /* Get the pointer to the next packet to be transmitted */
    *pbuf = (uint8_t*)(*(ImagePtr + img_count) + packet_index*((uint16_t)(UVC_PACKET_SIZE-2)));
  }
    else if(packet_index == packet_count)
   {
     /* Get the pointer to the next packet to be transmitted */
    *pbuf=(uint8_t*)(*(ImagePtr+img_count)+packet_index*((uint16_t)(UVC_PACKET_SIZE-2)));

     /* Set the current packet size */
    *psize =(packet_remainder+2);
   }

  else
  {
    /* New image to be started, send only the packet header */
    *psize = 2;
  }

  /* Update the packet index */
  *pcktidx = packet_index;

  /* Increment the packet count and check if it reached the end of current image buffer */
  if(packet_index++ >= (packet_count + 1))
  {
    /* Reset the packet count to zero */
    packet_index = 0U;

    /* Move to the next image in the images table */

    img_count++;
    HAL_Delay(USBD_VIDEO_IMAGE_LAPS);
    /* Check if images count has been reached, then reset to zero (go back to first image in circular loop) */
    if (img_count == IMG_NBR)
    {
      img_count= 0U;
    }
  }

  return (0);
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */
