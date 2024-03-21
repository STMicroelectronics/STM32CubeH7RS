/**
  ******************************************************************************
  * @file    Examples/BSP/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAIN_H
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "string.h"
#include "stm32h7rsxx_hal.h"
#include "stm32h7s78_discovery.h"
#include "stm32h7s78_discovery_audio.h"
#include "stm32h7s78_discovery_ts.h"
#include "stm32h7s78_discovery_lcd.h"
#include "stm32h7s78_discovery_sd.h"
#include "stm32h7s78_discovery_xspi.h"
#include "stm32_lcd.h"

/* Exported variables --------------------------------------------------------*/
extern unsigned char stlogo[];
extern __IO uint32_t SRAMTest;
extern __IO uint32_t UserButtonPressed ;

typedef enum {
  AUDIO_ERROR_NONE = 0,
  AUDIO_ERROR_NOTREADY,
  AUDIO_ERROR_IO,
  AUDIO_ERROR_EOF,
}AUDIO_ErrorTypeDef;

typedef struct
{
  void     (*DemoFunc)(void);
  uint8_t  DemoName[50];
  uint32_t DemoIndex;
} BSP_DemoTypedef;

/* Exported constants --------------------------------------------------------*/

#define SDRAM_WRITE_READ_ADDR        ((uint32_t)0x700010000)

#define SDRAM_WRITE_READ_ADDR_OFFSET ((uint32_t)0x0800)

/* Defines for the Audio playing process */
#define PAUSE_STATUS     ((uint32_t)0x00) /* Audio Player in Pause Status */
#define RESUME_STATUS    ((uint32_t)0x01) /* Audio Player in Resume Status */
#define IDLE_STATUS      ((uint32_t)0x02) /* Audio Player in Idle Status */


#define LED_GREEN      LD1
#define LED_RED        LD2

#define AUDIO_REC_START_ADDR         SDRAM_WRITE_READ_ADDR
#define AUDIO_REC_TOTAL_SIZE         ((uint32_t) 0x0000E000)
#define AUDIO_RECPDM_START_ADDR      (AUDIO_REC_START_ADDR+AUDIO_REC_TOTAL_SIZE)

/* The Audio file is flashed with STM32CubeProgrammer @ flash address =  AUDIO_SRC_FILE_ADDRESS */
#define AUDIO_SRC_FILE_ADDRESS       0x700A0000   /* Audio file address in flash */
#define AUDIO_FILE_SIZE              0x50000
#define PLAY_HEADER                  0x2C

#define AUDIO_PLAY_SAMPLE        0
#define AUDIO_PLAY_RECORDED      1

/* Exported macro ------------------------------------------------------------*/
#define COUNT_OF_EXAMPLE(x)    (sizeof(x)/sizeof(BSP_DemoTypedef))

/* Exported functions ------------------------------------------------------- */
uint8_t CheckForUserInput(void);
void ClearUserInput(void);
void Touchscreen_demo1(void);
void Touchscreen_demo2(void);
void Touchscreen_demo3(void);
void LCD_demo(void);
void AudioPlay_demo(void);
void AudioRecord_demo(void);
void SD_demo (void);
void OSPI_RAM_demo (void);
uint8_t AUDIO_Process(void);
void Error_Handler(void);
uint8_t TouchScreen_GetTouchPosition(void);
void Touchscreen_DrawBackground_Circles(uint8_t state);
#endif /* MAIN_H */
