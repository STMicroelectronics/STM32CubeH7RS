/**
  ******************************************************************************
  * @file    wm8904_reg.h
  * @author  MCD Application Team
  * @brief   Header of wm8904_reg.c
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
#ifndef WM8904_REG_H
#define WM8904_REG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/** @addtogroup BSP
  * @{
  */

/** @addtogroup Component
  * @{
  */

/** @addtogroup WM8904
  * @{
  */


/** @addtogroup WM8904_Exported_Constants
  * @{
  */
/******************************************************************************/
/****************************** REGISTER MAPPING ******************************/
/******************************************************************************/
/* SW Reset */
#define WM8904_SW_RESET               0x0000U

/* Bias Control */
#define WM8904_BIAS_CONTROL0          0x0004U
#define WM8904_VMID_CONTROL0          0x0005U
#define WM8904_MIC_BIAS_CONTROL0      0x0006U
#define WM8904_MIC_BIAS_CONTROL1      0x0007U

#define WM8904_ANALOG_ADC0            0x000AU

/* Power Management */
#define WM8904_PWR_MANAGEMENT0        0x000CU
#define WM8904_PWR_MANAGEMENT2        0x000EU
#define WM8904_PWR_MANAGEMENT3        0x000FU
#define WM8904_PWR_MANAGEMENT6        0x0012U

/* Clock Rates */
#define WM8904_CLOCK_RATES0           0x0014U
#define WM8904_CLOCK_RATES1           0x0015U
#define WM8904_CLOCK_RATES2           0x0016U

/* Audio Interface */
#define WM8904_AUDIO_INTERFACE0       0x0018U
#define WM8904_AUDIO_INTERFACE1       0x0019U
#define WM8904_AUDIO_INTERFACE2       0x001AU
#define WM8904_AUDIO_INTERFACE3       0x001BU

/* DAC */
#define WM8904_DAC_DIGITAL_VOL_LEFT   0x001EU
#define WM8904_DAC_DIGITAL_VOL_RIGHT  0x001FU
#define WM8904_DAC_DIGITAL0           0x0020U
#define WM8904_DAC_DIGITAL1           0x0021U

/* ADC */
#define WM8904_ADC_DIGITAL_VOL_LEFT   0x0024U
#define WM8904_ADC_DIGITAL_VOL_RIGHT  0x0025U
#define WM8904_ADC_DIGITAL0           0x0026U

#define WM8904_DIGITAL_MICROPHONE0    0x0027U

/* DRC */
#define WM8904_DRC0                   0x0028U
#define WM8904_DRC1                   0x0029U
#define WM8904_DRC2                   0x002AU
#define WM8904_DRC3                   0x002BU

/* Analog */
#define WM8904_ANALOG_LEFT_INPUT0     0x002CU
#define WM8904_ANALOG_RIGHT_INPUT0    0x002DU
#define WM8904_ANALOG_LEFT_INPUT1     0x002EU
#define WM8904_ANALOG_RIGHT_INPUT1    0x002FU
#define WM8904_ANALOG_OUTPUT1_LEFT    0x0039U
#define WM8904_ANALOG_OUTPUT1_RIGHT   0x003AU
#define WM8904_ANALOG_OUTPUT2_LEFT    0x003BU
#define WM8904_ANALOG_OUTPUT2_RIGHT   0x003CU
#define WM8904_ANALOG_OUTPUT12_ZC     0x003DU

/* DC Servo */
#define WM8904_DC_SERVO0              0x0043U
#define WM8904_DC_SERVO1              0x0044U
#define WM8904_DC_SERVO2              0x0045U
#define WM8904_DC_SERVO4              0x0047U
#define WM8904_DC_SERVO5              0x0048U
#define WM8904_DC_SERVO6              0x0049U
#define WM8904_DC_SERVO7              0x004AU
#define WM8904_DC_SERVO8              0x004BU
#define WM8904_DC_SERVO9              0x004CU
#define WM8904_DC_SERVO_READBACK0     0x004DU

/* Analog */
#define WM8904_ANALOG_HP0             0x005AU
#define WM8904_ANALOG_LINEOUT0        0x005EU

/* Charge pump */
#define WM8904_CHARGE_PUMP0           0x0062U

/* Class W/G */
#define WM8904_CLASS_W0               0x0068U

/* Write Sequencer */
#define WM8904_WRITE_SEQUENCER0       0x006CU
#define WM8904_WRITE_SEQUENCER1       0x006DU
#define WM8904_WRITE_SEQUENCER2       0x006EU
#define WM8904_WRITE_SEQUENCER3       0x006FU
#define WM8904_WRITE_SEQUENCER4       0x0070U

/* FLL Control */
#define WM8904_FLL_CONTROL1           0x0074U
#define WM8904_FLL_CONTROL2           0x0075U
#define WM8904_FLL_CONTROL3           0x0076U
#define WM8904_FLL_CONTROL4           0x0077U
#define WM8904_FLL_CONTROL5           0x0078U

/* GPIO Control */
#define WM8904_GPIO_CONTROL1          0x0079U
#define WM8904_GPIO_CONTROL2          0x007AU
#define WM8904_GPIO_CONTROL3          0x007BU
#define WM8904_GPIO_CONTROL4          0x007CU

#define WM8904_DIGITAL_PULLS          0x007EU

/* Interrupt */
#define WM8904_INTERRUPT_STATUS       0x007FU
#define WM8904_INTERRUPT_STATUS_MASK  0x0080U
#define WM8904_INTERRUPT_POLARITY     0x0081U
#define WM8904_INTERRUPT_DEBOUNCE     0x0082U

/* EQ */
#define WM8904_EQ1                    0x0086U
#define WM8904_EQ2                    0x0087U
#define WM8904_EQ3                    0x0088U
#define WM8904_EQ4                    0x0089U
#define WM8904_EQ5                    0x008AU
#define WM8904_EQ6                    0x008BU
#define WM8904_EQ7                    0x008CU
#define WM8904_EQ8                    0x008DU
#define WM8904_EQ9                    0x008EU
#define WM8904_EQ10                   0x008FU
#define WM8904_EQ11                   0x0090U
#define WM8904_EQ12                   0x0091U
#define WM8904_EQ13                   0x0092U
#define WM8904_EQ14                   0x0093U
#define WM8904_EQ15                   0x0094U
#define WM8904_EQ16                   0x0095U
#define WM8904_EQ17                   0x0096U
#define WM8904_EQ18                   0x0097U
#define WM8904_EQ19                   0x0098U
#define WM8904_EQ20                   0x0099U
#define WM8904_EQ21                   0x009AU
#define WM8904_EQ22                   0x009BU
#define WM8904_EQ23                   0x009CU
#define WM8904_EQ24                   0x009DU

#define WM8904_ADC_TEST0              0x00C6U

#define WM8904_FLL_TEST0              0x00F7U
#define WM8904_FLL_TEST1              0x00F8U
/**
  * @}
  */

/************** Generic Function  *******************/

typedef int32_t (*WM8904_Write_Func)(void *, uint16_t, uint8_t*, uint16_t);
typedef int32_t (*WM8904_Read_Func) (void *, uint16_t, uint8_t*, uint16_t);

typedef struct
{
  WM8904_Write_Func   WriteReg;
  WM8904_Read_Func    ReadReg;
  void                *handle;
} wm8904_ctx_t;

/*******************************************************************************
* Register      : Generic - All
* Address       : Generic - All
* Bit Group Name: None
* Permission    : W
*******************************************************************************/
int32_t wm8904_write_reg(wm8904_ctx_t *ctx, uint16_t reg, uint16_t *data, uint16_t length);
int32_t wm8904_read_reg(wm8904_ctx_t *ctx, uint16_t reg, uint16_t* data, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* WM8904_REG_H */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
