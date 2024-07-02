/**
  ******************************************************************************
  * @file    low_level_mce.h
  * @author  MCD Application Team
  * @brief   Header for low_level_mce.c module
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
#ifndef LOW_LEVEL_MCE_H
#define LOW_LEVEL_MCE_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(STM32H7S3xx) || defined(STM32H7S7xx)
/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_hal.h"
#include "boot_hal_cfg.h"
#include "low_level_obkeys.h"


/* Types of MCE regions (also used as indexes) */
#define LL_MCE_REGION_CODE_PRIMARY                    (0U)
#define LL_MCE_REGION_CODE_SECONDARY                  (LL_MCE_REGION_CODE_PRIMARY + 1U)
#if defined(MCUBOOT_OVERWRITE_ONLY)
#define LL_MCE_REGION_NUMBER                          (LL_MCE_REGION_CODE_SECONDARY + 1U)
#else /* not MCUBOOT_OVERWRITE_ONLY */
#define LL_MCE_REGION_SCRATCH                         (LL_MCE_REGION_CODE_SECONDARY + 1U)
#define LL_MCE_REGION_NUMBER                          (LL_MCE_REGION_SCRATCH + 1U)
#endif /* MCUBOOT_OVERWRITE_ONLY */

#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
#define LL_MCE_RUN_REGION_NUMBER                      (LL_MCE_REGION_CODE_PRIMARY + 1U)
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

#define LL_MCE_REGION_CFG_INSTALL                     (0U)                             /* 0x0 cfg for install mode */
#define LL_MCE_REGION_CFG_BL                          (LL_MCE_REGION_CFG_INSTALL + 1U) /* 0x1 cfg for BL */
#define LL_MCE_REGION_CFG_APPLI                       (LL_MCE_REGION_CFG_BL + 1U)      /* 0x2 cfg for Appli */
#define LL_MCE_REGION_CFG_END                         (LL_MCE_REGION_CFG_APPLI + 1U)

/* Exported functions ------------------------------------------------------- */
void LL_MCE_DeInit(MCE_HandleTypeDef *hmce);
void LL_MCE_Configure(void);
void LL_MCE_SetRegion(uint8_t Config, uint8_t Index, uint32_t FlowStep, uint32_t FlowCtrl);
#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
void LL_MCE_SetRunRegion(uint8_t Config, uint8_t Index, uint32_t FlowStep, uint32_t FlowCtrl);
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */
void LL_MCE_SetMasterKey(uint32_t FlowStep, uint32_t FlowCtrl);
void LL_MCE_SetAppliMasterKey(uint32_t FlowStep, uint32_t FlowCtrl);
void LL_MCE_SetLoaderMasterKey(uint32_t FlowStep, uint32_t FlowCtrl);
void LL_MCE_LockAppliRegions(const uint32_t FlowStep, const uint32_t FlowCtrl);
void LL_MCE_LockLoaderRegions(const uint32_t FlowStep, const uint32_t FlowCtrl);
void LL_MCE_CheckMasterKey(uint32_t FlowStep, uint32_t FlowCtrl);
void LL_MCE_CheckAppliMasterKey(uint32_t FlowStep, uint32_t FlowCtrl);
void LL_MCE_CheckLoaderMasterKey(uint32_t FlowStep, uint32_t FlowCtrl);
void LL_MCE_CheckAppliMasterKey(uint32_t FlowStep, uint32_t FlowCtrl);
void LL_MCE_CheckLockAppliRegions(const uint32_t FlowStep, const uint32_t FlowCtrl);
void LL_MCE_CheckLockLoaderRegions(const uint32_t FlowStep, const uint32_t FlowCtrl);
HAL_StatusTypeDef LL_MCE_CheckRegion(const uint8_t Config, const uint32_t RegionType, const uint32_t FlowStep,
                                     const uint32_t FlowCtrl);
#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
HAL_StatusTypeDef LL_MCE_CheckRunRegion(const uint8_t Config, const uint32_t RegionType, const uint32_t FlowStep,
                                        const uint32_t FlowCtrl);
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */
bool LL_MCE_IsCodePrimaryRegionEnabled(void);
HAL_StatusTypeDef LL_MCE_DisableCodePrimaryRegion(void);
HAL_StatusTypeDef LL_MCE_EnableCodePrimaryRegion(void);
bool LL_MCE_IsBufferInCodePrimaryRegion(const uint8_t *pBuffer, size_t Length);

#endif /* STM32H7S3xx || STM32H7S7xx */

#ifdef __cplusplus
}
#endif

#endif /* LOW_LEVEL_MCE_H */


