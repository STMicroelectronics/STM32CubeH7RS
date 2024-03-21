/**
  ******************************************************************************
  * @file    Examples/BSP/Src/sd.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the BSP SD Driver
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
#include "main.h"

/** @addtogroup STM32H7RSxx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SD_START_ADDRESS       ((uint32_t)0x00000400U)     /* SD Address to write/read data */
#define NB_BLOCK_BUFFER        (BUFFER_SIZE*4) / BLOCKSIZE /* Number of Block (512b) by Buffer */
#define BUFFER_SIZE            ((uint32_t)1024)            /* 16Ko */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t aTxBuffer[BUFFER_SIZE];
uint32_t aRxBuffer[BUFFER_SIZE];
__IO uint32_t SDWriteStatus = 0, SDReadStatus = 0, SDDetectStatus = 0;

/* Private function prototypes -----------------------------------------------*/
static void SD_SetHint(void);
static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset);
static uint8_t Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint32_t BufferLength);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  SD Demo
  * @param  None
  * @retval None
  */
void SD_demo (void)
{
  int32_t SD_state;
  uint32_t prev_status = 2;  /* Undefined state */

  SD_SetHint();

  SD_state = BSP_SD_Init(0);
  BSP_SD_DetectITConfig(0);

  UTIL_LCD_SetFont(&Font20);

  if(SD_state != BSP_ERROR_NONE)
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
    if (SD_state == BSP_ERROR_SD_NOT_DETECTED)
    {
      UTIL_LCD_DisplayStringAt(20, 100, (uint8_t *)"SD CARD NOT DETECTED", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(20, 115, (uint8_t *)"SD shall be inserted before running test", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(20, 130, (uint8_t *)"SD Test Aborted", LEFT_MODE);
    }
    else
    {
      UTIL_LCD_DisplayStringAt(20, 100, (uint8_t *)"SD INITIALIZATION : FAIL", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(20, 115, (uint8_t *)"SD Test Aborted", LEFT_MODE);
    }
  }
  else
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
    UTIL_LCD_DisplayStringAt(20, 100, (uint8_t *)"SD Initialization : OK", LEFT_MODE);
    UTIL_LCD_DisplayStringAt(20, 300, (uint8_t *)"     SD Connected", RIGHT_MODE);
    SDDetectStatus = SD_PRESENT;
    prev_status = SD_PRESENT;

    SD_state = BSP_SD_Erase(0,SD_START_ADDRESS, (SD_START_ADDRESS+BUFFER_SIZE));
    while(BSP_SD_GetCardState(0) != SD_TRANSFER_OK)
    {
    }

    if(SD_state != BSP_ERROR_NONE)
    {
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
      UTIL_LCD_DisplayStringAt(20, 115, (uint8_t *)"SD ERASE : FAILED", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(20, 130, (uint8_t *)"SD Test Aborted", LEFT_MODE);
    }
    else
    {
      UTIL_LCD_DisplayStringAt(20, 115, (uint8_t *)"SD ERASE : OK", LEFT_MODE);

      /* Fill the buffer to write */
      Fill_Buffer(aTxBuffer, BUFFER_SIZE, 0x22FF);
      SD_state = BSP_SD_WriteBlocks(0,(uint32_t *)aTxBuffer, SD_START_ADDRESS, NB_BLOCK_BUFFER);

      /* Wait until SD cards are ready to use for new operation */
      while(BSP_SD_GetCardState(0) != SD_TRANSFER_OK)
      {
      }
      if(SD_state != BSP_ERROR_NONE)
      {
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
        UTIL_LCD_DisplayStringAt(20, 130, (uint8_t *)"SD WRITE : FAILED", LEFT_MODE);
        UTIL_LCD_DisplayStringAt(20, 145, (uint8_t *)"SD Test Aborted", LEFT_MODE);
      }
      else
      {
        UTIL_LCD_DisplayStringAt(20, 130, (uint8_t *)"SD WRITE : OK", LEFT_MODE);
        SD_state = BSP_SD_ReadBlocks(0,(uint32_t *)aRxBuffer, SD_START_ADDRESS, NB_BLOCK_BUFFER);

        /* Wait until SD cards are ready to use for new operation */
        while(BSP_SD_GetCardState(0) != SD_TRANSFER_OK)
        {
        }
        if(SD_state != BSP_ERROR_NONE)
        {
          UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
          UTIL_LCD_DisplayStringAt(20, 145, (uint8_t *)"SD READ : FAILED", LEFT_MODE);
          UTIL_LCD_DisplayStringAt(20, 160, (uint8_t *)"SD Test Aborted", LEFT_MODE);
        }
        else
        {
          UTIL_LCD_DisplayStringAt(20, 145, (uint8_t *)"SD READ : OK", LEFT_MODE);
          if(Buffercmp(aTxBuffer, aRxBuffer, BUFFER_SIZE) > 0)
          {
            UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
            UTIL_LCD_DisplayStringAt(20, 160, (uint8_t *)"SD COMPARE : FAILED", LEFT_MODE);
            UTIL_LCD_DisplayStringAt(20, 175, (uint8_t *)"SD Test Aborted", LEFT_MODE);
          }
          else
          {
            UTIL_LCD_DisplayStringAt(20, 160, (uint8_t *)"SD TEST : OK", LEFT_MODE);
          }
        }
      }
    }
  }

  while (1)
  {

    /* Check if the SD card is plugged in the slot */
    if (SDDetectStatus != SD_PRESENT)
    {
      if (prev_status != SD_NOT_PRESENT)
      {
        prev_status = SD_NOT_PRESENT;
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
        UTIL_LCD_DisplayStringAt(20, 300, (uint8_t *)"SD NOT Connected", RIGHT_MODE);
      }
    }
    else if (prev_status != SD_PRESENT)
    {
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
      UTIL_LCD_DisplayStringAt(20, 300, (uint8_t *)"     SD Connected", RIGHT_MODE);
      prev_status = SD_PRESENT;
    }

    if(CheckForUserInput() > 0)
    {
      ClearUserInput();
      if(SD_state != BSP_ERROR_SD_NOT_DETECTED)
      {
        BSP_SD_DeInit(0);
      }
      return;
    }
  }
}


/**
  * @brief Tx Transfer completed callback
  * @param  Instance SD Instance
  * @retval None
  */
void BSP_SD_WriteCpltCallback(uint32_t Instance)
{
  SDWriteStatus = 1;

}

/**
  * @brief Rx Transfer completed callback
  * @param  Instance SD Instance
  * @retval None
  */
void BSP_SD_ReadCpltCallback(uint32_t Instance)
{
  SDReadStatus = 1;
}

/**
  * @brief  BSP SD Callback.
  * @param  Instance SD Instance
  * @param  Status   Pin status
  * @retval None
  */
void BSP_SD_DetectCallback(uint32_t Instance, uint32_t Status)
{
  SDDetectStatus = Status;
}

/**
  * @brief  Display SD Demo Hint
  * @param  None
  * @retval None
  */
static void SD_SetHint(void)
{
  uint32_t x_size, y_size;

  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);
  /* Clear the LCD */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  /* Set LCD Demo description */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_FillRect(0, 0, x_size, 80, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *)"SD", CENTER_MODE);
  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_DisplayStringAt(0, 45, (uint8_t *)"This example shows how to detect the presence of the card", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"and how to write and read data on the microSD", CENTER_MODE);

  /* Set the LCD Text Color */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DrawRect(10, 90, x_size - 20, y_size - 100, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DrawRect(11, 91, x_size - 22, y_size - 102, UTIL_LCD_COLOR_BLUE);

  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLACK);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
 }

/**
  * @brief  Fills buffer with user predefined data.
  * @param  pBuffer: pointer on the buffer to fill
  * @param  uwBufferLenght: size of the buffer to fill
  * @param  uwOffset: first value to fill on the buffer
  * @retval None
  */
static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
{
  uint32_t tmpIndex = 0;

  /* Put in global buffer different values */
  for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++ )
  {
    pBuffer[tmpIndex] = tmpIndex + uwOffset;
  }
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 1: pBuffer identical to pBuffer1
  *         0: pBuffer differs from pBuffer1
  */
static uint8_t Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint32_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return 1;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}
/**
  * @}
  */

/**
  * @}
  */

