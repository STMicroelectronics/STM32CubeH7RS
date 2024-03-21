/**
  ******************************************************************************
  * @file    low_level_extmem_device.c
  * @author  MCD Application Team
  * @brief   This file contains device definition for low_level_extmem_device
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

#include "flash_layout.h"
#include "low_level_ext_flash.h"

XSPI_HandleTypeDef XSPI_HANDLE;

static struct ext_flash_range erase_vect[] =
{
  { FLASH_AREA_BEGIN_OFFSET, FLASH_AREA_END_OFFSET - 1},
#if !defined(MCUBOOT_OVERWRITE_ONLY)
  { FLASH_AREA_SCRATCH_OFFSET, FLASH_AREA_SCRATCH_OFFSET + FLASH_AREA_SCRATCH_SIZE - 1}
#endif /* not MCUBOOT_OVERWRITE_ONLY */
};
static struct ext_flash_range write_vect[] =
{
  { FLASH_AREA_BEGIN_OFFSET, FLASH_AREA_END_OFFSET - 1},
#if !defined(MCUBOOT_OVERWRITE_ONLY)
  { FLASH_AREA_SCRATCH_OFFSET, FLASH_AREA_SCRATCH_OFFSET + FLASH_AREA_SCRATCH_SIZE - 1}
#endif /* not MCUBOOT_OVERWRITE_ONLY */
};

struct low_level_ext_flash_device EXT_FLASH0_DEV =
{
  .erase = { .nb = sizeof(erase_vect) / sizeof(struct ext_flash_range), .range = erase_vect},
  .write = { .nb = sizeof(write_vect) / sizeof(struct ext_flash_range), .range = write_vect},
  .read_error = 1
};
