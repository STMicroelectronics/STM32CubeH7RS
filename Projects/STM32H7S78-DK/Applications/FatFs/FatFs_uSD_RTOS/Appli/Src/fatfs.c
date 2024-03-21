/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
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
#include "fatfs.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
static uint32_t PinDetect = {SD_DETECT_Pin};
static GPIO_TypeDef* SD_GPIO_PORT = {SD_DETECT_GPIO_Port};
const static uint8_t wtext[] = "This is STM32 working with FatFs uSD + FreeRTOS"; /* File write buffer */

uint32_t   osQueueMsg;

osThreadId_t FSAppThreadHandle;
const osThreadAttr_t uSDThread_attributes = {
  .name = "uSDThread",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 8
};

/* Definitions for Mutex */
osMessageQueueId_t QueueHandle;
const osMessageQueueAttr_t Queue_attributes = {
  .name = "osqueue"
};

FATFS SDFatFs;    /* File system object for SD logical drive */
FIL SDFile;       /* File  object for SD */
char SDPath[4];   /* SD logical drive path */
const MKFS_PARM OptParm = {FM_ANY, 0, 0, 0, 0};

static uint32_t CARD_CONNECTED= 0;
static uint32_t CARD_DISCONNECTED= 1;
static uint32_t CARD_STATUS_CHANGED= 2;

static uint8_t isFsCreated = 0;
static __IO uint8_t statusChanged = 0;
static uint8_t workBuffer[2 * FF_MAX_SS];
static  uint8_t rtext[100]; /* File read buffer */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void uSDThread_Entry(void *argument);
static void FS_FileOperations(void);
static uint8_t SD_IsDetected(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

void MX_FATFS_Init(void)
{
  /* USER CODE BEGIN Init */
  /* additional user code for init */
  /*## FatFS: Link the disk I/O driver(s)  ###########################*/
  if (FATFS_LinkDriver(&SD_DMA_Driver, SDPath) == 0)
  {
    /* creation of uSDThread */
    FSAppThreadHandle = osThreadNew(uSDThread_Entry, NULL, &uSDThread_attributes);

    /* Create Storage Message Queue */
    QueueHandle = osMessageQueueNew(1U, sizeof(uint16_t), NULL);
  }
  /* USER CODE END Init */
}

/* USER CODE BEGIN Application */
/**
  * @brief  Start task
  * @param  pvParameters not used
  * @retval None
  */
static void uSDThread_Entry(void *argument)
{
  osStatus_t status;

    if(SD_IsDetected())
    {
      osMessageQueuePut (QueueHandle, &CARD_CONNECTED, 100, 0U);
    }

  /* Infinite Loop */
  for( ;; )
  {
    status = osMessageQueueGet(QueueHandle, &osQueueMsg, NULL, 100);

    if ((status == osOK) && (osQueueMsg== CARD_STATUS_CHANGED))
    {
        if (SD_IsDetected())
        {
          osMessageQueuePut (QueueHandle, &CARD_CONNECTED, 100, 0U);
        }
        else
        {
          osMessageQueuePut (QueueHandle, &CARD_DISCONNECTED, 100, 0U);
        }
     }

     if ((status == osOK) && (osQueueMsg== CARD_CONNECTED))
     {
        HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
        FS_FileOperations();
        statusChanged = 0;
     }

     if ((status == osOK) && (osQueueMsg== CARD_DISCONNECTED))
     {
        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
        HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
        osDelay(200);

        f_mount(NULL, (TCHAR const*)"", 0);
        statusChanged = 0;
     }
  }
}

/**
  * @brief File system : file operation
  * @retval File operation result
  */
static void FS_FileOperations(void)
{
  FRESULT res;                                          /* FatFs function common result code */
  uint32_t byteswritten, bytesread;                     /* File write/read counts */


  /* Register the file system object to the FatFs module */
  if(f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) == FR_OK)
  {
    /* check whether the FS has been already created */
    if (isFsCreated == 0)
    {
      if(f_mkfs(SDPath, &OptParm, workBuffer, sizeof(workBuffer)) != FR_OK)
      {
        HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
        return;
      }
      isFsCreated = 1;
    }
    /* Create and Open a new text file object with write access */
    if(f_open(&SDFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
    {
      /* Write data to the text file */
      res = f_write(&SDFile, (const void *)wtext, sizeof(wtext), (void *)&byteswritten);

      if((byteswritten > 0) && (res == FR_OK))
      {
        /* Close the open text file */
        f_close(&SDFile);

        /* Open the text file object with read access */
        if(f_open(&SDFile, "STM32.TXT", FA_READ) == FR_OK)
        {
          /* Read data from the text file */
          res = f_read(&SDFile, ( void *)rtext, sizeof(rtext), (void *)&bytesread);

          if((bytesread > 0) && (res == FR_OK))
          {
            /* Close the open text file */
            f_close(&SDFile);

            /* Compare read data with the expected data */
            if(bytesread == byteswritten)
            {
              /* Success of the demo: no error occurrence */
              HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
              return;
            }
          }
        }
      }
    }
  }
  /* Error */
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
}

static uint8_t SD_IsDetected(void)
{
    uint8_t status;

    if (HAL_GPIO_ReadPin(SD_GPIO_PORT, PinDetect) == GPIO_PIN_RESET)
    {
      status = HAL_ERROR;
    }
    else
    {
      status = HAL_OK;
    }
      return status;
}

/**
  * @brief  EXTI line detection callback.
  * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
  * @retval None.
  */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == SD_DETECT_Pin)
  {
     if (statusChanged == 0)
     {
       statusChanged = 1;
       osMessageQueuePut ( QueueHandle, &CARD_STATUS_CHANGED, 100, 0U);
     }
  }
}
/* USER CODE END Application */
