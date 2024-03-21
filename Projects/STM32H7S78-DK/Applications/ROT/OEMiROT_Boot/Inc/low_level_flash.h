/**
  ******************************************************************************
  * @file    low_level_flash.h
  * @author  MCD Application Team
  * @brief   This file contains device definition for low_level_flash driver
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
#ifndef __LOW_LEVEL_FLASH_H
#define __LOW_LEVEL_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32h7rsxx_hal.h"

struct flash_range
{
  uint32_t base;
  uint32_t limit;
};
struct flash_vect
{
  uint32_t nb;
  struct flash_range *range;
};
struct low_level_device
{
  struct flash_vect erase;
  struct flash_vect write;
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U) && !defined(LOCAL_LOADER_CONFIG)
  struct flash_vect secure;
#endif /* __ARM_FEATURE_CMSE and (__ARM_FEATURE_CMSE == 3U) and (not LOCAL_LOADER_CONFIG) */
#if  !defined(LOCAL_LOADER_CONFIG)
  uint32_t read_error;
#endif /* not LOCAL_LOADER_CONFIG */
};

extern struct low_level_device FLASH0_DEV;

#if !defined(LOCAL_LOADER_CONFIG)
void NMI_Handler(void);
#endif /* not LOCAL_LOADER_CONFIG */

#ifdef __cplusplus
}
#endif

#endif /* __LOW_LEVEL_FLASH_H */


