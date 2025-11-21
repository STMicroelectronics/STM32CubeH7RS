/* ########### HEADER GENERATED AUTOMATICALLY DONT TOUCH IT ########### */

/*
 * Copyright (c) 2018 Arm Limited. All rights reserved.
 * Copyright (c) 2023 STMicroelectronics. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef APPLI_FLASH_LAYOUT_H
#define APPLI_FLASH_LAYOUT_H

#ifdef __cplusplus
extern "C" {
#endif

#define MCUBOOT_OVERWRITE_ONLY                                /* Defined: the FW installation uses overwrite method.
                                                                 UnDefined: The FW installation uses swap mode. */

#define MCUBOOT_APP_IMAGE_NUMBER         1

/* app image primary slot offset */
#define FLASH_PRIMARY_APP_SLOT_OFFSET                   0x0

/* app image primary slot size */
#define FLASH_PRIMARY_APP_SLOT_SIZE                     0x0

/* app image secondary slot offset */
#define FLASH_SECONDARY_APP_SLOT_OFFSET                 0x0

/* app image secondary slot size */
#define FLASH_SECONDARY_APP_SLOT_SIZE                   0x0

/* App image header size */
#define APP_IMAGE_HEADER_SIZE                             0x400

/* app image secondary magic install request offset */
#define FLASH_SECONDARY_APP_INSTALL_REQ_OFFSET          0x0

#if !defined(OVERWRITE_ONLY)
/* app image primary confirmation flag offset */
#define FLASH_PRIMARY_APP_CONFIRM_OFFSET                0x0
#endif

/* app image primary version offset */
#define FLASH_PRIMARY_APP_VERSION_OFFSET                0x0

/******************************************************************************/
/* THESE DEFINITIONS ARE RoT SW RELATED (CONFIG INDEPENDANT)                  */
/******************************************************************************/

/* Magic install request */
static const uint32_t MagicInstallRequestValue[4] =
{
  0xf395c277, 0x7fefd260, 0x0f505235, 0x8079b62c
};
#define IMAGE_INSTALL_REQ_SIZE                            sizeof(MagicInstallRequestValue)

/* Confirm flag size */
#define IMAGE_CONFIRM_SIZE                                0x1

/* Version size */
#define IMAGE_VERSION_SIZE                                0x8

/******************************************************************************/
/* THESE DEFINITIONS ARE HW RELATED                                           */
/******************************************************************************/

/* Flash total size */
#define FLASH_TOTAL_SIZE                                  0x0


#ifdef __cplusplus
}
#endif

#endif /* APPLI_FLASH_LAYOUT_H */
