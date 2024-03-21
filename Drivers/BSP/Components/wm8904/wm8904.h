/**
  ******************************************************************************
  * @file    wm8904.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the
  *          wm8904.c driver.
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
#ifndef WM8904_H
#define WM8904_H

/* Includes ------------------------------------------------------------------*/
#include "wm8904_reg.h"
#include <stddef.h>

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Component
  * @{
  */

/** @addtogroup WM8904
  * @{
  */

/** @defgroup WM8904_Exported_Types Exported Types
  * @{
  */
typedef int32_t (*WM8904_Init_Func)    (void);
typedef int32_t (*WM8904_DeInit_Func)  (void);
typedef int32_t (*WM8904_GetTick_Func) (void);
typedef int32_t (*WM8904_WriteReg_Func)(uint16_t, uint16_t, uint8_t*, uint16_t);
typedef int32_t (*WM8904_ReadReg_Func) (uint16_t, uint16_t, uint8_t*, uint16_t);

typedef struct
{
  WM8904_Init_Func          Init;
  WM8904_DeInit_Func        DeInit;
  uint16_t                  Address;
  WM8904_WriteReg_Func      WriteReg;
  WM8904_ReadReg_Func       ReadReg;
  WM8904_GetTick_Func       GetTick;
} WM8904_IO_t;


typedef struct
{
  WM8904_IO_t         IO;
  wm8904_ctx_t        Ctx;
  uint8_t             IsInitialized;
} WM8904_Object_t;

typedef struct
{
  uint32_t   InputDevice;
  uint32_t   OutputDevice;
  uint32_t   Frequency;
  uint32_t   Resolution;
  uint32_t   Volume;
} WM8904_Init_t;

 typedef struct
{
  int32_t ( *Init                 ) ( WM8904_Object_t *, WM8904_Init_t* );
  int32_t ( *DeInit               ) ( WM8904_Object_t * );
  int32_t ( *ReadID               ) ( WM8904_Object_t *, uint32_t * );
  int32_t ( *Play                 ) ( WM8904_Object_t * );
  int32_t ( *Pause                ) ( WM8904_Object_t * );
  int32_t ( *Resume               ) ( WM8904_Object_t * );
  int32_t ( *Stop                 ) ( WM8904_Object_t *, uint32_t );
  int32_t ( *SetFrequency         ) ( WM8904_Object_t *, uint32_t );
  int32_t ( *GetFrequency         ) ( WM8904_Object_t *, uint32_t*);
  int32_t ( *SetVolume            ) ( WM8904_Object_t *, uint32_t, uint8_t );
  int32_t ( *GetVolume            ) ( WM8904_Object_t *, uint32_t, uint8_t*);
  int32_t ( *SetMute              ) ( WM8904_Object_t *, uint32_t );
  int32_t ( *SetOutputMode        ) ( WM8904_Object_t *, uint32_t );
  int32_t ( *SetResolution        ) ( WM8904_Object_t *, uint32_t );
  int32_t ( *GetResolution        ) ( WM8904_Object_t *, uint32_t*);
  int32_t ( *SetProtocol          ) ( WM8904_Object_t *, uint32_t );
  int32_t ( *GetProtocol          ) ( WM8904_Object_t *, uint32_t*);
  int32_t ( *Reset                ) ( WM8904_Object_t * );
} WM8904_Drv_t;
/**
  * @}
  */

/** @defgroup WM8904_Exported_Constants Exported Constants
  * @{
  */
#define WM8904_OK                (0)
#define WM8904_ERROR             (-1)

/******************************************************************************/
/***************************  Codec User defines ******************************/
/******************************************************************************/
/* Audio Input Device */
#define WM8904_IN_NONE           0x0000U
#define WM8904_IN_MIC1           0x0100U
#define WM8904_IN_MIC2           0x0200U
#define WM8904_IN_LINE1          0x0300U
#define WM8904_IN_LINE2          0x0400U
#define WM8904_IN_MIC1_MIC2      0x0500U

/* Audio Output Device */
#define WM8904_OUT_NONE          0x0000U
#define WM8904_OUT_HEADPHONE     0x0001U
#define WM8904_OUT_LINE          0x0002U
#define WM8904_OUT_BOTH          0x0003U
#define WM8904_OUT_AUTO          0x0004U

/* AUDIO FREQUENCY */
#define WM8904_FREQUENCY_192K    192000U
#define WM8904_FREQUENCY_176K    176400U
#define WM8904_FREQUENCY_96K     96000U
#define WM8904_FREQUENCY_88K     88200U
#define WM8904_FREQUENCY_48K     48000U
#define WM8904_FREQUENCY_44K     44100U
#define WM8904_FREQUENCY_32K     32000U
#define WM8904_FREQUENCY_22K     22050U
#define WM8904_FREQUENCY_16K     16000U
#define WM8904_FREQUENCY_11K     11025U
#define WM8904_FREQUENCY_8K      8000U

/* AUDIO RESOLUTION */
#define WM8904_RESOLUTION_16B    0x00U
#define WM8904_RESOLUTION_20B    0x01U
#define WM8904_RESOLUTION_24B    0x02U
#define WM8904_RESOLUTION_32B    0x03U

/* Codec stop options */
#define WM8904_PDWN_HW           0x00U
#define WM8904_PDWN_SW           0x01U

/* Volume Input Output selection */
#define VOLUME_INPUT                  0U
#define VOLUME_OUTPUT                 1U

/* MUTE commands */
#define WM8904_MUTE_ON                1U
#define WM8904_MUTE_OFF               0U

/* AUDIO PROTOCOL */
#define WM8904_PROTOCOL_R_JUSTIFIED    0x0000U
#define WM8904_PROTOCOL_L_JUSTIFIED    0x0001U
#define WM8904_PROTOCOL_I2S            0x0002U
#define WM8904_PROTOCOL_DSP            0x0003U

/**
  * @brief  WM8904 ID
  */
#define  WM8904_ID      0x8904U
#define  WM8904_ID_MASK 0xFFFFU
/**
  * @}
  */

/** @addtogroup WM8904_Exported_Variables
  * @{
  */
/* Audio driver structure */
extern WM8904_Drv_t WM8904_Driver;
/**
  * @}
  */

/** @addtogroup WM8904_Exported_Functions
  * @{
  */
/*------------------------------------------------------------------------------
                           Audio Codec functions
------------------------------------------------------------------------------*/
/* High Layer codec functions */
int32_t WM8904_RegisterBusIO (WM8904_Object_t *pObj, WM8904_IO_t *pIO);
int32_t WM8904_Init(WM8904_Object_t *pObj, WM8904_Init_t *pInit);
int32_t WM8904_DeInit(WM8904_Object_t *pObj);
int32_t WM8904_ReadID(WM8904_Object_t *pObj, uint32_t *Id);
int32_t WM8904_Play(WM8904_Object_t *pObj);
int32_t WM8904_Pause(WM8904_Object_t *pObj);
int32_t WM8904_Resume(WM8904_Object_t *pObj);
int32_t WM8904_Stop(WM8904_Object_t *pObj, uint32_t CodecPdwnMode);
int32_t WM8904_SetVolume(WM8904_Object_t *pObj, uint32_t InputOutput, uint8_t Volume);
int32_t WM8904_GetVolume(WM8904_Object_t *pObj, uint32_t InputOutput, uint8_t *Volume);
int32_t WM8904_SetMute(WM8904_Object_t *pObj, uint32_t Cmd);
int32_t WM8904_SetOutputMode(WM8904_Object_t *pObj, uint32_t Output);
int32_t WM8904_SetResolution(WM8904_Object_t *pObj, uint32_t Resolution);
int32_t WM8904_GetResolution(WM8904_Object_t *pObj, uint32_t *Resolution);
int32_t WM8904_SetProtocol(WM8904_Object_t *pObj, uint32_t Protocol);
int32_t WM8904_GetProtocol(WM8904_Object_t *pObj, uint32_t *Protocol);
int32_t WM8904_SetFrequency(WM8904_Object_t *pObj, uint32_t AudioFreq);
int32_t WM8904_GetFrequency(WM8904_Object_t *pObj, uint32_t *AudioFreq);
int32_t WM8904_Reset(WM8904_Object_t *pObj);
/**
  * @}
  */

#endif /* WM8904_H */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
