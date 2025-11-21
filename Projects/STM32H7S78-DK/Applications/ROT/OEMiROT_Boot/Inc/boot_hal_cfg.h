/**
  ******************************************************************************
  * @file    boot_hal_cfg.h
  * @author  MCD Application Team
  * @brief   File fixing configuration flag specific for STM32H7RSxx platform
  *
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
#ifndef BOOT_HAL_CFG_H
#define BOOT_HAL_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_hal.h"
#include "flash_layout.h"
/* IWDG */
#define KR_RELOAD          (uint16_t) 0xAAAA
/* RTC clock */
#define RTC_CLOCK_SOURCE_LSI
#ifdef RTC_CLOCK_SOURCE_LSI
#define RTC_ASYNCH_PREDIV  0x7F
#define RTC_SYNCH_PREDIV   0x00F9
#endif /* RTC_CLOCK_SOURCE_LSI */
#ifdef RTC_CLOCK_SOURCE_LSE
#define RTC_ASYNCH_PREDIV  0x7F
#define RTC_SYNCH_PREDIV   0x00FF
#endif /* RTC_CLOCK_SOURCE_LSE */
#define RCC_CSI_TIMEOUT_VALUE 1U  /* 1 ms */

/* Static protections */
#if  !defined(OEMUROT_ENABLE)
#define OEMIROT_WRP_PROTECT_ENABLE /*!< Write Protection  */
#define OEMIROT_HDP_PROTECT_ENABLE /*!< HDP protection   */
#endif /* not OEMUROT_ENABLE */
#define OEMIROT_USER_SRAM_ECC /*!< SRAM ECC */

#define OEMIROT_NV_STATE_POS                (16U) /*!< NV state position */
#define OEMIROT_NV_STATE_OPEN               (OB_NVSTATE_OPEN << OEMIROT_NV_STATE_POS) /*!< NV state: Open */
#define OEMIROT_NV_STATE_CLOSE              (OB_NVSTATE_CLOSE << OEMIROT_NV_STATE_POS) /*!< NV state: Close */
#define OEMIROT_PROD_STATE_NOT_SET           0xFB03B154U /*!< Product state: Not set */
#define OEMIROT_PROD_STATE_OPEN             OEMIROT_NV_STATE_OPEN /*!< Product state: Open */
#define OEMIROT_PROD_STATE_PROVISIONING     (OEMIROT_NV_STATE_CLOSE | \
                                             OB_OEM_PROVD_DEFAULT | \
                                             OB_DBG_AUTH_DEFAULT) /*!< Product state: Provisioning */
#define OEMIROT_PROD_STATE_PROVISIONED      (OEMIROT_NV_STATE_CLOSE | \
                                             OB_OEM_PROVD_ENABLE | \
                                             OB_DBG_AUTH_DEFAULT) /*!< Product state: Provisioned */
#define OEMIROT_PROD_STATE_LOCKED           (OEMIROT_NV_STATE_CLOSE | \
                                             OB_OEM_PROVD_ENABLE | \
                                             OB_DBG_AUTH_LOCKED) /*!< Product state: Locked */
#define OEMIROT_PROD_STATE_CLOSED_ECDSA (OEMIROT_NV_STATE_CLOSE | \
                                         OB_OEM_PROVD_ENABLE | \
                                         OB_DBG_AUTH_ECDSA_SIGN) /*!< Product state: Closed (ECDSA sign) */
#define OEMIROT_PROD_STATE_CLOSED_PWD       (OEMIROT_NV_STATE_CLOSE | \
                                             OB_OEM_PROVD_ENABLE | \
                                             OB_DBG_AUTH_PASSWORD) /*!< Product state: Closed (Password) */

#ifdef OEMIROT_DEV_MODE
#define OEMIROT_OB_PRODUCT_STATE_VALUE OEMIROT_PROD_STATE_OPEN   /*!< Product State */
/*#define OEMIROT_JUMP_TO_BL_ENABLE */ /*!< Jump into BootLoader to download new images */
#else
#define OEMIROT_OB_PRODUCT_STATE_VALUE OEMIROT_PROD_STATE_CLOSED_ECDSA /*!< Product State */
#endif /* OEMIROT_DEV_MODE */


#define NO_TAMPER            (0)                /*!< No tamper activated */
#define INTERNAL_TAMPER_ONLY (1)                /*!< Only Internal tamper activated */
#define ALL_TAMPER           (2)                /*!< Internal and External tamper activated */
#define OEMIROT_TAMPER_ENABLE INTERNAL_TAMPER_ONLY            /*!< TAMPER configuration flag  */

#ifdef OEMIROT_DEV_MODE
#define OEMIROT_ERROR_HANDLER_STOP_EXEC /*!< Error handler stops execution (else it resets) */
#endif /* OEMIROT_DEV_MODE */

/* Run time protections */
#define OEMIROT_MPU_PROTECTION    /*!< OEMiROT_Boot uses MPU to prevent execution outside of OEMiROT_Boot code  */
#if defined(STM32H7S3xx) || defined(STM32H7S7xx)
#define OEMIROT_MCE_PROTECTION    /*!< OEMiROT_Boot uses MCE to prevent accesses to not encrypted code in external flash  */
#endif /* STM32H7S3xx || STM32H7S7xx */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  OEMIROT_SUCCESS = 0U,
  OEMIROT_FAILED
} OEMIROT_ErrorStatus;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Error_Handler(void) __NO_RETURN;

#ifdef __cplusplus
}
#endif

#endif /* BOOT_HAL_CFG_H */
