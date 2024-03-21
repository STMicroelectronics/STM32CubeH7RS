/**
  ******************************************************************************
  * @file    boot_hal_mce.h
  * @author  MCD Application Team
  * @brief   Header for mce services in boot_hal.c module
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
#ifndef BOOT_HAL_MCE_H
#define BOOT_HAL_MCE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
/* Exported functions ------------------------------------------------------- */

/** @defgroup BOOT_MCE_Exported_Functions Exported Functions
  * @{
  */
bool boot_is_in_primary(uint8_t fa_id, uint32_t offset_in_flash, size_t len);
bool boot_is_in_primary_and_erased(uint8_t fa_id, uint32_t offset_in_flash, size_t len, uint8_t erased_val);
/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif /* BOOT_HAL_MCE_H */
