/**
  ******************************************************************************
  * @file    low_level_ramecc.h
  * @author  MCD Application Team
  * @brief   Header for low_level_ramecc.c module
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
#ifndef LOW_LEVEL_RAMECC_H
#define LOW_LEVEL_RAMECC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_hal.h"
#include "boot_hal_cfg.h"

/* Exported functions ------------------------------------------------------- */
void LL_RAMECC_DeInit(void);
void LL_RAMECC_Configure(void);

#ifdef __cplusplus
}
#endif

#endif /* LOW_LEVEL_RAMECC_H */

