/**
  ******************************************************************************
  * @file    wm8904_reg.c
  * @author  MCD Application Team
  * @brief   This file provides unitary register function to control the WM8904
  *          Audio Codec driver.
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
#include "wm8904_reg.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup wm8904
  * @brief     This file provides a set of functions needed to drive the
  *            WM8904 audio codec.
  * @{
  */

/************** Generic Function  *******************/
/*******************************************************************************
* Function Name : wm8904_read_reg
* Description   : Generic Reading function. It must be full-filled with either
*                 I2C or SPI reading functions
* Input         : Register Address, length of buffer
* Output        : data Read
*******************************************************************************/
int32_t wm8904_read_reg(wm8904_ctx_t *ctx, uint16_t reg, uint16_t* data, uint16_t length)
{
  int32_t ret;
  uint16_t tmp;

  ret = ctx->ReadReg(ctx->handle, reg, (uint8_t *)data, length);

  if(ret >= 0)
  {
    tmp = ((uint16_t)(*data >> 8) & 0x00FFU);
    tmp |= ((uint16_t)(*data << 8) & 0xFF00U);
    *data = tmp;
  }
  return ret;
}

/*******************************************************************************
* Function Name : wm8904_write_reg
* Description   : Generic Writing function. It must be full-filled with either
*                 I2C or SPI writing function
* Input         : Register Address, data to be written, length of buffer
* Output        : None
*******************************************************************************/
int32_t wm8904_write_reg(wm8904_ctx_t *ctx, uint16_t reg, uint16_t *data, uint16_t length)
{
  uint16_t tmp;
  tmp = ((uint16_t)(*data >> 8) & 0x00FFU);
  tmp |= ((uint16_t)(*data << 8) & 0xFF00U);

  return ctx->WriteReg(ctx->handle, reg, (uint8_t *)&tmp, length);
}

/******************************************************************************/
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
