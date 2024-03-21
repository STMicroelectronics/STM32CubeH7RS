/**
  ******************************************************************************
  * @file    wm8904.c
  * @author  MCD Application Team
  * @brief   This file provides the WM8904 Audio Codec driver.
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
#include "wm8904.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @defgroup WM8904 WM8904
  * @brief     This file provides a set of functions needed to drive the
  *            WM8904 audio codec.
  * @{
  */

/** @defgroup WM8904_Exported_Variables Exported Variables
  * @{
  */
/* Audio codec driver structure initialization */
WM8904_Drv_t WM8904_Driver =
{
  WM8904_Init,
  WM8904_DeInit,
  WM8904_ReadID,
  WM8904_Play,
  WM8904_Pause,
  WM8904_Resume,
  WM8904_Stop,
  WM8904_SetFrequency,
  WM8904_GetFrequency,
  WM8904_SetVolume,
  WM8904_GetVolume,
  WM8904_SetMute,
  WM8904_SetOutputMode,
  WM8904_SetResolution,
  WM8904_GetResolution,
  WM8904_SetProtocol,
  WM8904_GetProtocol,
  WM8904_Reset
};
/**
  * @}
  */

/** @defgroup WM8904_Private_Macros Private Macros
  * @{
  */
#define VOLUME_OUT_CONVERT(Volume)     (((Volume) >= 100U) ? 63U : ((uint8_t)((((Volume) / 2U) + 13U))))
#define VOLUME_OUT_INVERT(Volume)      ((uint8_t)(((Volume) - 13U) * 2U))
/**
  * @}
  */

/** @defgroup WM8904_Private_Variables Private Variables
  * @{
  */
static uint32_t WM8904_CurrentDevices = WM8904_OUT_NONE;
/**
  * @}
  */

/** @defgroup WM8904_Private_Functions Private Functions
  * @{
  */
static int32_t WM8904_ReadRegWrap(void *handle, uint16_t Reg, uint8_t* Data, uint16_t Length);
static int32_t WM8904_WriteRegWrap(void *handle, uint16_t Reg, uint8_t* Data, uint16_t Length);
static int32_t WM8904_Delay(WM8904_Object_t *pObj, uint32_t Delay);
/**
  * @}
  */

/** @defgroup WM8904_Exported_Functions Exported Functions
  * @{
  */
/**
  * @brief Initializes the audio codec and the control interface.
  * @param pObj pointer to component object
  * @param pInit pointer de component init structure
  * @retval 0 if correct communication, else wrong communication
  */
int32_t WM8904_Init(WM8904_Object_t *pObj, WM8904_Init_t *pInit)
{
  int32_t ret = WM8904_OK;
  uint16_t tmp;

  if (pObj->IsInitialized == 1U)
  {
    ret += WM8904_DeInit(pObj);
  }

  /*********/
  /* Reset */
  /*********/

  tmp = 0x0U; /* Reset registers */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_SW_RESET, &tmp, 2U);

  /**************/
  /* Clock rate */
  /**************/

  tmp = 0x0004U; /* CLK_SYS_ENA = 1 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_CLOCK_RATES2, &tmp, 2U);

  /**************************************/
  /* Reference voltages and master bias */
  /**************************************/

  tmp = 0x0018U; /* ISEL[1:0] = 10b, BIAS_EN = 0 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_BIAS_CONTROL0, &tmp, 2U);

  tmp = 0x0047U; /* VMID_BUF_ENA = 1, VMID_RES[1:0] = 11b, VMID_ENA = 1 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_VMID_CONTROL0, &tmp, 2U);
  WM8904_Delay(pObj, 100U);

  tmp = 0x0043U; /* VMID_BUF_ENA = 1, VMID_RES[1:0] = 01b, VMID_ENA = 1 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_VMID_CONTROL0, &tmp, 2U);

  tmp = 0x0019U; /* ISEL[1:0] = 10b, BIAS_EN = 1 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_BIAS_CONTROL0, &tmp, 2U);

  /********************/
  /* Mic BIAS control */
  /********************/

  tmp = 0x0001U; /* MICBIAS_ENA = 1 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_MIC_BIAS_CONTROL0, &tmp, 2U);

  /**********************/
  /* Power management   */
  /**********************/

  tmp = 0x0002U; /* INL_ENA = 1 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_PWR_MANAGEMENT0, &tmp, 2U);

  tmp = 0x0003U; /* HPL_PGA_ENA = HPR_PGA_ENA = 1 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_PWR_MANAGEMENT2, &tmp, 2U);

  /**************/
  /* Clock rate */
  /**************/

  tmp = 0x0006U; /* CLK_DSP_ENA = CLK_SYS_ENA = 1 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_CLOCK_RATES2, &tmp, 2U);

  /**************/
  /* DAC and ADC*/
  /**************/

  tmp = 0x000EU; /* ADCL_ENA = DACL_ENA = DACR_ENA = 1 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_PWR_MANAGEMENT6, &tmp, 2U);

  tmp = 0x0648U; /* DAC_MUTERATE = DAC_UNMUTE_RAMP = DAC_OSR128 = DAC_MUTE = 1 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_DAC_DIGITAL1, &tmp, 2U);

  tmp = 0x0010U; /* AIFADCR_SRC = 0 (left) */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_AUDIO_INTERFACE0, &tmp, 2U);

  /**********************************************/
  /* Resolution, protocol, frequency and volume */
  /**********************************************/

  tmp = 0x0002U; /* Set resolution to 16B and protocol to I2S*/
  ret +=  wm8904_write_reg(&pObj->Ctx, WM8904_AUDIO_INTERFACE1, &tmp, 2U);

  ret += WM8904_SetFrequency(pObj, pInit->Frequency);

  if ((WM8904_CurrentDevices & WM8904_OUT_HEADPHONE) == WM8904_OUT_HEADPHONE)
  {
    ret += WM8904_SetVolume(pObj, VOLUME_OUTPUT, (uint8_t)pInit->Volume);
  }

  tmp = 0x01FFU; /* ADC_VU = 1, ADC_VOL = +17dB */
  ret +=  wm8904_write_reg(&pObj->Ctx, WM8904_ADC_DIGITAL_VOL_LEFT, &tmp, 2U);

  tmp = 0x009AU; /* LINMUTE = 1, LIN_VOL = +14dB */
  ret +=  wm8904_write_reg(&pObj->Ctx, WM8904_ANALOG_LEFT_INPUT0, &tmp, 2U);

  /***************/
  /* Charge pump */
  /***************/

  tmp = 0x0001U; /* CP_ENA = 1  */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_CHARGE_PUMP0, &tmp, 2U);

  /**********************/
  /* Output signal path */
  /**********************/

  tmp = 0x0011U; /* HPL_ENA = HPR_ENA = 1 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_ANALOG_HP0, &tmp, 2U);

  tmp = 0x0033U; /* HPL_ENA_DLY = HPR_ENA_DLY = HPL_ENA = HPR_ENA = 1 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_ANALOG_HP0, &tmp, 2U);

  /************/
  /* DC Servo */
  /************/

  tmp = 0x0003U; /* Enable channels 0 and 1 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_DC_SERVO0, &tmp, 2U);

  tmp = 0x0030U; /* Startup 0 and 1 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_DC_SERVO1, &tmp, 2U);
  WM8904_Delay(pObj, 300U);

  /**********************/
  /* Output signal path */
  /**********************/

  tmp = 0x0077U; /* HPL_ENA_OUTP= HPL_ENA_OUTR = HPL_ENA_DLY = HPR_ENA_DLY = HPL_ENA = HPR_ENA = 1 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_ANALOG_HP0, &tmp, 2U);

  tmp = 0x00FFU; /* HPL_RMV_SHORT = HPR_RMV_SHORT = HPL_ENA_OUTP= HPL_ENA_OUTR = HPL_ENA_DLY = HPR_ENA_DLY = HPL_ENA = HPR_ENA = 1 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_ANALOG_HP0, &tmp, 2U);

  /***************/
  /* Charge pump */
  /***************/

  tmp = 0x01U; /* CP_DYN_PWR = 1 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_CLASS_W0, &tmp, 2U);

  /* Store current devices */
  WM8904_CurrentDevices = (pInit->OutputDevice | pInit->InputDevice);

  return ret;
}

/**
  * @brief  Deinitializes the audio codec.
  * @param  pObj pointer to component object
  * @retval Component status
  */
int32_t WM8904_DeInit(WM8904_Object_t *pObj)
{
  int32_t ret;
  uint16_t tmp;

  /**********************/
  /* Output signal path */
  /**********************/

  tmp = 0x0077U; /* HPL_RMV_SHORT = HPR_RMV_SHORT = 0 */
  ret = wm8904_write_reg(&pObj->Ctx, WM8904_ANALOG_HP0, &tmp, 2U);

  tmp = 0x0000U; /* HPL_ENA_OUTP= HPL_ENA_OUTR = HPL_ENA_DLY = HPR_ENA_DLY = HPL_ENA = HPR_ENA = 0 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_ANALOG_HP0, &tmp, 2U);

  /************/
  /* DC Servo */
  /************/

  tmp = 0x0000U; /* Disable channels 0 and 1 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_DC_SERVO0, &tmp, 2U);

  /***************/
  /* Charge pump */
  /***************/

  tmp = 0x0000U; /* CP_ENA = 0  */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_CHARGE_PUMP0, &tmp, 2U);

  /*******/
  /* DAC */
  /*******/

  tmp = 0x0000U; /* ADCL_ENA = DACL_ENA = DACR_ENA = 0 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_PWR_MANAGEMENT6, &tmp, 2U);

  /**************/
  /* Clock rate */
  /**************/

  tmp = 0x0004U; /* CLK_DSP_ENA = 0, CLK_SYS_ENA = 1 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_CLOCK_RATES2, &tmp, 2U);

  /**********************/
  /* Output signal path */
  /**********************/

  tmp = 0x0000U; /* HPL_PGA_ENA = HPR_PGA_ENA = 0 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_PWR_MANAGEMENT2, &tmp, 2U);

  tmp = 0x0000U; /* INL_ENA = 0 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_PWR_MANAGEMENT0, &tmp, 2U);

  /********************/
  /* Mic BIAS control */
  /********************/

  tmp = 0x0000U; /* MICBIAS_ENA = 0 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_MIC_BIAS_CONTROL0, &tmp, 2U);

  /**************************************/
  /* Reference voltages and master bias */
  /**************************************/

  tmp = 0x0018U; /* ISEL[1:0] = 10b, BIAS_EN = 0 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_BIAS_CONTROL0, &tmp, 2U);

  tmp = 0x0042U; /* VMID_BUF_ENA = 1, VMID_RES[1:0] = 01b, VMID_ENA = 0 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_VMID_CONTROL0, &tmp, 2U);
  WM8904_Delay(pObj, 300U);

  tmp = 0x0042U; /* VMID_BUF_ENA = 1, VMID_RES[1:0] = 01b, VMID_ENA = 0 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_VMID_CONTROL0, &tmp, 2U);

  tmp = 0x0000U; /* VMID_BUF_ENA = 0, VMID_RES[1:0] = 00b, VMID_ENA = 0 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_VMID_CONTROL0, &tmp, 2U);

  tmp = 0x0018U; /* ISEL[1:0] = 10b,  BIAS_EN = 0 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_BIAS_CONTROL0, &tmp, 2U);

  /**************/
  /* Clock rate */
  /**************/

  tmp = 0x0000U; /* CLK_SYS_ENA = 0 */
  ret += wm8904_write_reg(&pObj->Ctx, WM8904_CLOCK_RATES2, &tmp, 2U);

  if (pObj->IsInitialized == 1U)
  {
    pObj->IsInitialized = 0U;
  }

  return ret;
}

/**
  * @brief  Get the WM8904 ID.
  * @param  pObj pointer to component object
  * @param  Id component ID
  * @retval Component status
  */
int32_t WM8904_ReadID(WM8904_Object_t *pObj, uint32_t *Id)
{
  int32_t ret;
  uint16_t wm8904_id;

  /* Initialize the Control interface of the Audio Codec */
  pObj->IO.Init();

  /* Get ID from component */
  ret = wm8904_read_reg(&pObj->Ctx, WM8904_SW_RESET, &wm8904_id, 2U);

  *Id = wm8904_id;

  return ret;
}

/**
  * @brief Start the audio Codec play feature.
  * @note For this codec no Play options are required.
  * @param  pObj pointer to component object
  * @retval Component status
  */
int32_t WM8904_Play(WM8904_Object_t *pObj)
{
  int32_t ret = WM8904_OK;
  uint16_t tmp;

  if ((WM8904_CurrentDevices & WM8904_OUT_HEADPHONE) == WM8904_OUT_HEADPHONE)
  {
    tmp = 0x0640U; /* DAC_MUTE= 0 */
    ret += wm8904_write_reg(&pObj->Ctx, WM8904_DAC_DIGITAL1, &tmp, 2U);
  }


  if ((WM8904_CurrentDevices & WM8904_IN_MIC1) == WM8904_IN_MIC1)
  {
    tmp = 0x001AU; /* LINMUTE = 0 */
    ret += wm8904_write_reg(&pObj->Ctx, WM8904_ANALOG_LEFT_INPUT0, &tmp, 2U);
  }

  return ret;
}

/**
  * @brief Pauses playing on the audio codec.
  * @param  pObj pointer to component object
  * @retval Component status
  */
int32_t WM8904_Pause(WM8904_Object_t *pObj)
{
  /* Pause the audio file playing */
  return WM8904_SetMute(pObj, WM8904_MUTE_ON);
}

/**
  * @brief Resumes playing on the audio codec.
  * @param  pObj pointer to component object
  * @retval Component status
  */
int32_t WM8904_Resume(WM8904_Object_t *pObj)
{
  /* Resumes the audio file playing */
  return WM8904_SetMute(pObj, WM8904_MUTE_OFF);
}

/**
  * @brief Stops audio Codec playing. It powers down the codec.
  * @param  pObj pointer to component object
  * @param CodecPdwnMode  selects the  power down mode.
  *          - WM8904_PDWN_SW: only mutes the audio codec. When resuming from this
  *                           mode the codec keeps the previous initialization
  *                           (no need to re-Initialize the codec registers).
  *          - WM8904_PDWN_HW: Physically power down the codec. When resuming from this
  *                           mode, the codec is set to default configuration
  *                           (user should re-Initialize the codec in order to
  *                            play again the audio stream).
  * @retval 0 if correct communication, else wrong communication
  */
int32_t WM8904_Stop(WM8904_Object_t *pObj, uint32_t CodecPdwnMode)
{
  int32_t ret;

  /* Mute the output */
  ret = WM8904_SetMute(pObj, WM8904_MUTE_ON);

  if (CodecPdwnMode == WM8904_PDWN_HW)
  {
    ret += WM8904_DeInit(pObj);
  }

  return ret;
}

/**
  * @brief Set higher or lower the codec volume level.
  * @param  pObj pointer to component object
  * @param  InputOutput Input or Output volume
  * @param  Volume  a byte value from 0 to 100.
  *         (refer to codec registers description for more details).
  * @retval Component status
  */
int32_t WM8904_SetVolume(WM8904_Object_t *pObj, uint32_t InputOutput, uint8_t Volume)
{
  uint8_t  convertedvol;
  int32_t  ret;
  uint16_t tmp;

  /* Only output volume supported */
  if (InputOutput == VOLUME_OUTPUT)
  {
    convertedvol = VOLUME_OUT_CONVERT(Volume);

    tmp = (uint16_t) convertedvol;

    /* Left Headphone Volume */
    ret = wm8904_write_reg(&pObj->Ctx, WM8904_ANALOG_OUTPUT1_LEFT, &tmp, 2U);
    /* Right Headphone Volume */
    tmp |= 0x80U; /* Volume update */
    ret += wm8904_write_reg(&pObj->Ctx, WM8904_ANALOG_OUTPUT1_RIGHT, &tmp, 2U);
  }
  else /* Input volume */
  {
    ret = WM8904_ERROR;
  }

  return ret;
}

/**
  * @brief Get higher or lower the codec volume level.
  * @param  pObj pointer to component object
  * @param  InputOutput Input or Output volume
  * @param  Volume audio volume
  * @retval Component status
  */
int32_t WM8904_GetVolume(WM8904_Object_t *pObj, uint32_t InputOutput, uint8_t *Volume)
{
  int32_t  ret;
  uint16_t tmp;

  /* only output volume supported */
  if (InputOutput == VOLUME_OUTPUT)
  {
    /* Left Headphone Volume */
    ret = wm8904_read_reg(&pObj->Ctx, WM8904_ANALOG_OUTPUT1_LEFT, &tmp, 2U);
    tmp &= 0x3FU;

    if (ret == WM8904_OK)
    {
      *Volume = (uint8_t) VOLUME_OUT_INVERT(tmp);
    }
  }
  else /* Input volume */
  {
    ret = WM8904_ERROR;
  }

  return ret;
}

/**
  * @brief Enables or disables the mute feature on the audio codec.
  * @param  pObj pointer to component object
  * @param Cmd  WM8904_MUTE_ON to enable the mute or WM8904_MUTE_OFF to disable the
  *             mute mode.
  * @retval 0 if correct communication, else wrong communication
  */
int32_t WM8904_SetMute(WM8904_Object_t *pObj, uint32_t Cmd)
{
  int32_t ret = WM8904_OK;
  uint16_t tmp;

  if ((WM8904_CurrentDevices & WM8904_OUT_HEADPHONE) == WM8904_OUT_HEADPHONE)
  {
    /* Read DAC digital 1 */
    ret += wm8904_read_reg(&pObj->Ctx, WM8904_DAC_DIGITAL1, &tmp, 2U);

    if(Cmd == WM8904_MUTE_ON)
    {
      /* Mute the DAC Digital 1 */
      tmp |= 0x0008U;
    }
    else
    {
      /* Unmute the the DAC Digital 1 */
      tmp &= ~(0x0008U);
    }
    ret += wm8904_write_reg(&pObj->Ctx, WM8904_DAC_DIGITAL1, &tmp, 2U);
  }

  if ((WM8904_CurrentDevices & WM8904_IN_MIC1) == WM8904_IN_MIC1)
  {
    if(Cmd == WM8904_MUTE_ON)
    {
      tmp = 0x009AU; /* LINMUTE = 1 */
    }
    else
    {
      tmp = 0x001AU; /* LINMUTE = 0 */
    }
    ret +=  wm8904_write_reg(&pObj->Ctx, WM8904_ANALOG_LEFT_INPUT0, &tmp, 2U);
  }

  return ret;
}

/**
  * @brief Switch dynamically (while audio file is played) the output target
  *         (speaker or headphone).
  * @param  pObj pointer to component object
  * @param Output  specifies the audio output target: WM8904_OUT_SPEAKER,
  *         WM8904_OUT_HEADPHONE, WM8904_OUT_BOTH or WM8904_OUT_AUTO
  * @retval 0 if correct communication, else wrong communication
  */
int32_t WM8904_SetOutputMode(WM8904_Object_t *pObj, uint32_t Output)
{
  (void)(pObj);
  (void)(Output);

  /* This feature is currently not supported */
  return WM8904_ERROR;
}

/**
  * @brief Set Audio resolution.
  * @param pObj pointer to component object
  * @param Resolution  Audio resolution. Can be:
  *                    WM8904_RESOLUTION_16b, WM8904_RESOLUTION_20b,
  *                    WM8904_RESOLUTION_24b or WM8904_RESOLUTION_32b
  * @retval Component status
  */
int32_t WM8904_SetResolution(WM8904_Object_t *pObj, uint32_t Resolution)
{
  (void)(pObj);
  (void)(Resolution);

  /* This feature is currently not supported */
  return WM8904_ERROR;
}

/**
  * @brief Get Audio resolution.
  * @param pObj pointer to component object
  * @retval Audio resolution
  */
int32_t WM8904_GetResolution(WM8904_Object_t *pObj, uint32_t *Resolution)
{
  int32_t  ret;
  uint16_t tmp;

  ret = wm8904_read_reg(&pObj->Ctx, WM8904_AUDIO_INTERFACE1, &tmp, 2U);

  switch((tmp >> 2U) & 0x0003U)
  {
    case 1:
      *Resolution = WM8904_RESOLUTION_20B;
      break;
    case 2:
      *Resolution = WM8904_RESOLUTION_24B;
      break;
    case 3:
      *Resolution = WM8904_RESOLUTION_32B;
      break;
    case 0:
    default:
      *Resolution = WM8904_RESOLUTION_16B;
      break;
  }

  return ret;
}

/**
  * @brief Set Audio Protocol.
  * @param pObj pointer to component object
  * @param Protocol Audio Protocol. Can be:
  *                  WM8904_PROTOCOL_R_JUSTIFIED, WM8904_PROTOCOL_L_JUSTIFIED,
  *                  WM8904_PROTOCOL_I2S or WM8904_PROTOCOL_DSP
  * @retval Component status
  */
int32_t WM8904_SetProtocol(WM8904_Object_t *pObj, uint32_t Protocol)
{
  (void)(pObj);
  (void)(Protocol);

  /* This feature is currently not supported */
  return WM8904_ERROR;
}

/**
  * @brief Get Audio Protocol.
  * @param pObj pointer to component object
  * @retval Component status
  */
int32_t WM8904_GetProtocol(WM8904_Object_t *pObj, uint32_t *Protocol)
{
  int32_t  ret;
  uint16_t tmp;

  ret = wm8904_read_reg(&pObj->Ctx, WM8904_AUDIO_INTERFACE1, &tmp, 2U);

  *Protocol = (tmp & 0x0003U);

  return ret;
}

/**
  * @brief Sets new frequency.
  * @param pObj pointer to component object
  * @param AudioFreq Audio frequency
  * @retval Component status
  */
int32_t WM8904_SetFrequency(WM8904_Object_t *pObj, uint32_t AudioFreq)
{
  int32_t ret;
  uint16_t tmp;

  switch (AudioFreq)
  {
  case  WM8904_FREQUENCY_8K:
    /* Sample Rate = 8 (kHz), ratio=256 */
    tmp = 0x0C00U;
    ret = wm8904_write_reg(&pObj->Ctx, WM8904_CLOCK_RATES1, &tmp, 2U);
    break;

  case  WM8904_FREQUENCY_11K:
    /* Sample Rate = 11.025 (kHz), ratio=256 */
    tmp = 0x0C01U;
    ret = wm8904_write_reg(&pObj->Ctx, WM8904_CLOCK_RATES1, &tmp, 2U);
    break;

  case  WM8904_FREQUENCY_16K:
    /* Sample Rate = 16 (kHz), ratio=256 */
    tmp = 0x0C02U;
    ret = wm8904_write_reg(&pObj->Ctx, WM8904_CLOCK_RATES1, &tmp, 2U);
    break;

  case  WM8904_FREQUENCY_22K:
    /* Sample Rate = 22.050 (kHz), ratio=256 */
    tmp = 0x0C03U;
    ret = wm8904_write_reg(&pObj->Ctx, WM8904_CLOCK_RATES1, &tmp, 2U);
    break;

  case  WM8904_FREQUENCY_32K:
    /* Sample Rate = 32 (kHz), ratio=256 */
    tmp = 0x0C04U;
    ret = wm8904_write_reg(&pObj->Ctx, WM8904_CLOCK_RATES1, &tmp, 2U);
    break;

  case  WM8904_FREQUENCY_44K:
  case  WM8904_FREQUENCY_48K:
    /* Sample Rate = 44.1 (kHz) or 48 (kHz), ratio=256 */
    tmp = 0x0C05U;
    ret = wm8904_write_reg(&pObj->Ctx, WM8904_CLOCK_RATES1, &tmp, 2U);
    break;

  default:
    ret = WM8904_ERROR;
    break;
  }

  return ret;
}

/**
  * @brief Get frequency.
  * @param pObj pointer to component object
  * @param AudioFreq Audio frequency
  * @retval Component status
  */
int32_t WM8904_GetFrequency(WM8904_Object_t *pObj, uint32_t *AudioFreq)
{
  int32_t  ret;
  uint16_t tmp;

  ret = wm8904_read_reg(&pObj->Ctx, WM8904_CLOCK_RATES1, &tmp, 2U);

  switch(tmp & 0x0007U)
  {
  case 0:
    *AudioFreq = WM8904_FREQUENCY_8K;
    break;
  case 1:
    /* 11.025k or 12k */
    *AudioFreq = WM8904_FREQUENCY_11K;
    break;
  case 2:
    *AudioFreq = WM8904_FREQUENCY_16K;
    break;
  case 3:
    /* 22.05k or 24k */
    *AudioFreq = WM8904_FREQUENCY_22K;
    break;
  case 4:
    *AudioFreq = WM8904_FREQUENCY_32K;
    break;
  case 5:
    /* 44.1k or 48k */
    *AudioFreq = WM8904_FREQUENCY_44K;
    break;
  default:
    ret = WM8904_ERROR;
    break;
  }

  return ret;
}

/**
  * @brief Resets wm8904 registers.
  * @param pObj pointer to component object
  * @retval Component status if correct communication, else wrong communication
  */
int32_t WM8904_Reset(WM8904_Object_t *pObj)
{
  uint16_t tmp;
  int32_t  ret;

  /* Reset Codec by writing in 0x0000 address register */
  tmp = 0x0U; /* Reset registers */
  ret = wm8904_write_reg(&pObj->Ctx, WM8904_SW_RESET, &tmp, 2U);

  return ret;
}

/**
  * @brief  Function
  * @param  Component object pointer
  * @retval error status
  */
int32_t WM8904_RegisterBusIO(WM8904_Object_t *pObj, WM8904_IO_t *pIO)
{
  int32_t ret;

  if (pObj == NULL)
  {
    ret = WM8904_ERROR;
  }
  else
  {
    pObj->IO.Init      = pIO->Init;
    pObj->IO.DeInit    = pIO->DeInit;
    pObj->IO.Address   = pIO->Address;
    pObj->IO.WriteReg  = pIO->WriteReg;
    pObj->IO.ReadReg   = pIO->ReadReg;
    pObj->IO.GetTick   = pIO->GetTick;

    pObj->Ctx.ReadReg  = WM8904_ReadRegWrap;
    pObj->Ctx.WriteReg = WM8904_WriteRegWrap;
    pObj->Ctx.handle   = pObj;

    if(pObj->IO.Init != NULL)
    {
      ret = pObj->IO.Init();
    }
    else
    {
      ret = WM8904_ERROR;
    }
  }

  return ret;
}
/**
  * @}
  */

/******************** Static functions ****************************************/
/** @addtogroup WM8904_Private_Functions
  * @{
  */
/**
  * @brief This function provides accurate delay (in milliseconds)
  * @param pObj pointer to component object
  * @param Delay: specifies the delay time length, in milliseconds
  * @retval Component status
  */
static int32_t WM8904_Delay(WM8904_Object_t *pObj, uint32_t Delay)
{
  uint32_t tickstart;

  tickstart = pObj->IO.GetTick();
  while((pObj->IO.GetTick() - tickstart) < Delay)
  {
  }
  return WM8904_OK;
}

/**
  * @brief  Function
  * @param  handle  Component object handle
  * @param  Reg     The target register address to write
  * @param  pData   The target register value to be written
  * @param  Length  buffer size to be written
  * @retval error status
  */
static int32_t WM8904_ReadRegWrap(void *handle, uint16_t Reg, uint8_t* pData, uint16_t Length)
{
  WM8904_Object_t *pObj = (WM8904_Object_t *)handle;

  return pObj->IO.ReadReg(pObj->IO.Address, Reg, pData, Length);
}

/**
  * @brief  Function
  * @param  handle Component object handle
  * @param  Reg    The target register address to write
  * @param  pData  The target register value to be written
  * @param  Length buffer size to be written
  * @retval error status
  */
static int32_t WM8904_WriteRegWrap(void *handle, uint16_t Reg, uint8_t* pData, uint16_t Length)
{
  WM8904_Object_t *pObj = (WM8904_Object_t *)handle;

  return pObj->IO.WriteReg(pObj->IO.Address, Reg, pData, Length);
}
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
