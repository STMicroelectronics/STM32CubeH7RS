/*
 * Copyright (c) 2010-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 /**
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#if defined(STM32_EXTMEMLOADER_MDKARMTARGET)

/* Includes ------------------------------------------------------------------*/
#include "FlashOS.h"
#include "stm32_extmemloader_conf.h"

/** @addtogroup MDKARM_LOADER_Private_Variables
  * @ingroup MDKARM_LOADER
  * @{
  */

/**
 * @brief debug state machine
 */
struct FlashDevice const FlashDevice __attribute__((section("device_info"), used)) = {
    FLASH_DRV_VERS,                        // Driver Version, do not modify!
    STM32EXTLOADER_DEVICE_NAME,            // Device Name
    EXTSPI,                                // Device Type
    STM32EXTLOADER_DEVICE_ADDR,            // Device Start Address
    STM32EXTLOADER_DEVICE_SIZE,            // Device Size in Bytes
    STM32EXTLOADER_DEVICE_PAGE_SIZE,       // Programming Page Size
    0x00,                                  // Reserved, must be 0
    STM32EXTLOADER_DEVICE_INITIAL_CONTENT, // Initial Content of Erased Memory
    STM32EXTLOADER_DEVICE_PAGE_TIMEOUT,    // Program Page Timeout 100 mSec
    STM32EXTLOADER_DEVICE_SECTOR_TIMEOUT,  // Erase Sector Timeout 6000 mSec

    // Specify Size and Address of Sectors
    STM32EXTLOADER_DEVICE_SECTOR_SIZE, 0x000000, // Sector Size
    SECTOR_END};

/**
  * @}
  */

#endif /* STM32_EXTMEMLOADER_MDKARMTARGET */
