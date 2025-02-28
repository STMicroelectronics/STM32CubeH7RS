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
static const uint8_t wtext[] = "This is STM32 working with FatFs uSD diskio DMA RTOS driver";

typedef enum {
  APPLICATION_IDLE = 0,
  APPLICATION_INIT,
  APPLICATION_RUNNING,
  APPLICATION_SD_UNPLUGGED,
  APPLICATION_SD_PLUGGED,
} FS_FileOperationsTypeDef;
osThreadId_t StartThreadHandle;
osThreadId_t ConcurrentThreadHandle;

static osThreadAttr_t uSDThread_attributes = {
  .name = "uSDThread",
  .priority = osPriorityNormal,
  .stack_size = 8 * 512,
};
static uint32_t   DiskQueueMsg = 0;
osMessageQueueId_t DiskEvent;

FATFS SDFatFs;         /* File system object for SD logical drive */
FIL File1, File2 ;     /* File  object for SD */
static char SDPath[4]; /* SD logical drive path */
static const MKFS_PARM OptParm = {FM_ANY, 0, 0, 0, 0};
static uint32_t disk_op = 0;
static uint32_t DiskReadyEvent = 0x1;
static uint32_t DiskRemoveEvent = 0x2;
FS_FileOperationsTypeDef Appli_state = APPLICATION_IDLE;
static uint8_t workBuffer[FF_MAX_SS];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void StartThread(void *argument);
static void ConcurrentThread(void *argument);
static int32_t FS_File1Operations(void);
static int32_t FS_File2Operations(void);
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
    Appli_state = APPLICATION_INIT;

    /* Create the thread(s) */
    /* definition and creation of USER_Thread */
    uSDThread_attributes.name = "USER_Thread";
    StartThreadHandle = osThreadNew(StartThread, NULL, (const osThreadAttr_t *)&uSDThread_attributes);
    /* definition and creation of USER_ConcurrentThread */
    uSDThread_attributes.name = "USER_ConcurrentThread";
    ConcurrentThreadHandle = osThreadNew(ConcurrentThread, NULL, (const osThreadAttr_t *)&uSDThread_attributes);
    /* Create Disk Queue */
    DiskEvent = osMessageQueueNew (1U, sizeof(uint16_t), NULL);

  }

  /* USER CODE END Init */
}

/* USER CODE BEGIN Application */
/**
  * @brief FatFs Deinitialization
  * @param  None
  * @retval Deinitialization result
  */
int32_t MX_FATFS_DeInit(void)
{
  /* FatFS: UnLink the disk I/O driver(s) */
  if (FATFS_UnLinkDriver(SDPath) == 0)
  {
    Appli_state = APPLICATION_INIT;
    return APP_OK;
  }
  else
  {
    return APP_ERROR;
  }
}

/**
  * @brief  Function implementing the StartThread thread.
  * @param  argument: Not used
  * @retval None
  */
static void StartThread(void *argument)
{
  int32_t ffStatus ;
  /* Infinite loop */
  while (1)
  {
    ffStatus = MX_FATFS_Process();
    if( ffStatus == APP_SD_UNPLUGGED)
    {
      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
      HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
      osDelay(200);
    }
    else if (ffStatus == APP_ERROR)
    {
      Error_Handler();
    }
    else if (ffStatus ==APP_OK)
    {
      osThreadSuspend (StartThreadHandle);
    }
  }
}

/**
  * @brief Function implementing the ConcurrentThread thread.
  * @param argument: Not used
  * @retval None
  */
static void ConcurrentThread(void *argument)
{
  /* USER CODE BEGIN ConcurrentThread */
  osStatus_t status;

  while (1)
  {
    status = osMessageQueueGet(DiskEvent, &DiskQueueMsg, NULL, osWaitForever);
    if(status == osOK)
    {
      switch(DiskQueueMsg)
      {
      case DISK_READY_EVENT:
        FS_File2Operations();
        break;

      case DISK_REMOVE_EVENT:
        /* Unlink the USB disk I/O driver */
        MX_FATFS_DeInit();
        Success_Handler();
        break;

      default:
        break;
      }
    }
  }
}

/**
  * @brief FatFs application main process
  * @param  None
  * @retval Process result
  */
int32_t MX_FATFS_Process(void)
{
  /* USER CODE BEGIN FATFS_Process */
  int32_t process_res = APP_OK;
  /* Mass Storage Application State Machine */
  switch(Appli_state)
  {
  case APPLICATION_INIT:
    if(SD_IsDetected())
    {
#if FATFS_MKFS_ALLOWED
      FRESULT res;
     /* Register the file system object to the FatFs module */

      res = f_mkfs(SDPath, &OptParm, workBuffer, sizeof(workBuffer));

      if (res != FR_OK)
      {
        process_res = APP_ERROR;
      }
      else
      {
        process_res = APP_INIT;
        Appli_state = APPLICATION_RUNNING;
      }
#else
      process_res = APP_INIT;
      Appli_state = APPLICATION_RUNNING;
#endif
    }
    else
    {
    process_res = APP_SD_UNPLUGGED;
    Appli_state = APPLICATION_SD_UNPLUGGED;

    }

    break;
  case APPLICATION_RUNNING:
    process_res = FS_File1Operations();
    Appli_state = APPLICATION_IDLE;
    break;

  case APPLICATION_SD_UNPLUGGED:
    f_mount(NULL, (TCHAR const*)"", 0);
    process_res = APP_SD_UNPLUGGED;
    break;

  case APPLICATION_IDLE:
  default:
    break;
  }
  return process_res;
  /* USER CODE END FATFS_Process */
}
/**
  * @brief  Gets Time from RTC (generated when FS_NORTC==0; see ff.c)
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */
}

/**
  * @brief File system : file operation
  * @retval File operation result
  */
static int32_t FS_File1Operations(void)
{
  FRESULT res;            /* FatFs function common result code */
  uint32_t byteswritten;  /* File write/read counts */

  /* Register the file system object to the FatFs module */
  if (f_mount(&SDFatFs, (TCHAR const*)SDPath, 1) == FR_OK)
  {
    /* Create and Open a new text file object with write access */
    if (f_open(&File1, "STM32_1.TXT", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
    {
      /* Allow Second task to have access to FatFs */
      osMessageQueuePut(DiskEvent, &DiskReadyEvent, 100, osWaitForever);
      osDelay(100);
      /* Write data to the text file */
      res = f_write(&File1, wtext, sizeof(wtext), (void *)&byteswritten);

      if ((byteswritten > 0) && (res == FR_OK))
      {
        /* Close the open text file */
        f_close(&File1);
        disk_op ++;
        /* If last access to Disk, unlink drive */
        while (disk_op < 2);
        osMessageQueuePut(DiskEvent, &DiskRemoveEvent, 100, 0);
        disk_op = 0;
        return 0;
      }
    }
  }
  /* Error */
  return -1;
}

/**
  * @brief File system : file operation
  * @retval File operation result
  */
static int32_t FS_File2Operations(void)
{
  FRESULT res;            /* FatFs function common result code */
  uint32_t byteswritten ; /* File write/read counts */

  /* Create and Open a new text file object with write access */
  if (f_open(&File2, "STM32_2.TXT", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
  {
    /* Write data to the text file */
    res = f_write(&File2, wtext, sizeof(wtext), (void *)&byteswritten);

    if ((byteswritten > 0) && (res == FR_OK))
    {
      /* Close the open text file */
      f_close(&File2);
      disk_op ++;
      return 0;
    }
  }
  /* Error */
  return -1;
}

/**
  * @brief  Detects if SD card is correctly plugged in the memory slot or not.
  * @retval Returns if SD is detected or not
  */
static uint8_t SD_IsDetected(void)
{
    uint8_t ret;

    if (HAL_GPIO_ReadPin(SD_GPIO_PORT, PinDetect) == GPIO_PIN_SET)
    {
      ret = (uint8_t)SD_No_Present;
    }
    else
    {
      ret = (uint8_t)SD_Present;
    }
      return ret;
}

/* USER CODE END Application */
