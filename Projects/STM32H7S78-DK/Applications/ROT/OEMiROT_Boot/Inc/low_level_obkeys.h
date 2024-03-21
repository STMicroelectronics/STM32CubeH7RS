/**
  ******************************************************************************
  * @file    low_level_obkeys.h
  * @author  MCD Application Team
  * @brief   Header for low_level_obkeys.c module
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
#ifndef LOW_LEVEL_OBKEYS_H
#define LOW_LEVEL_OBKEYS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_hal.h"
#include "mcuboot_config/mcuboot_config.h"
#include "plat_nv_counters.h"
#include "Driver_Flash.h"
#include "flash_layout.h"

/* Exported constants --------------------------------------------------------*/
#define OBK_HDPL_256_KEY_SIZE     (0x00000020U)

#define FLASH_PROG_UNIT           (OBK_HDPL_256_KEY_SIZE) /* OBkey granularity : 32 */
#define OBK_PROG_UNIT             (FLASH_PROG_UNIT)

#define OBK_MAX_HDPL_AREA_SIZE    (0x400U) /* 32 indexes * 32 bytes */

#define OBK_FEATURE_ENABLE        (0x1U)
#define OBK_FEATURE_DISABLE       (0x0U)

/* Index of the AHK in the OBK to encrypt/decrypt obkeys */
#define OBK_CRYP_KEY_INDEX                      (0U)

#define ALL_OBKEYS                (0x1FFU)             /* Swap all OBkeys */

#define ECDSA_256_PRIV_KEY_LENGTH (70U)
#define ECDSA_256_PUB_KEY_LENGTH  (91U)
#define SHA256_LENGTH             (32U)

#if   (CRYPTO_SCHEME == CRYPTO_SCHEME_EC256)
#define AUTH_PUB_KEY_LENGTH       ECDSA_256_PUB_KEY_LENGTH
#define ENC_PRIV_KEY_LENGTH       ECDSA_256_PRIV_KEY_LENGTH
#else /* not (CRYPTO_SCHEME == CRYPTO_SCHEME_EC256) */
#error "undefined crypto scheme"
#endif /* (CRYPTO_SCHEME == CRYPTO_SCHEME_EC256) */
#define RESERVED1                 ((4U - (AUTH_PUB_KEY_LENGTH % 4U)) % 4U)
#define RESERVED2                 ((4U - (AUTH_PUB_KEY_LENGTH % 4U)) % 4U)
#define RESERVED3                 ((4U - (ENC_PRIV_KEY_LENGTH % 4U)) % 4U)
#define RESERVED4                 ((OBK_FLASH_PROG_UNIT -\
                                    ((AUTH_PUB_KEY_LENGTH + RESERVED1 +\
                                      AUTH_PUB_KEY_LENGTH + RESERVED2 +\
                                      ENC_PRIV_KEY_LENGTH + RESERVED3) % OBK_FLASH_PROG_UNIT))\
                                   % OBK_FLASH_PROG_UNIT)

/* Exported types ------------------------------------------------------------*/

#if  defined(OEMUROT_ENABLE)
/* OEMuROT Hdpl 2 immutable section : to be aligned on 32 bytes
   ============================================================ */
typedef struct
{
  uint8_t  SHA256[SHA256_LENGTH];                                 /* index 8 Mandatory to be first in the structure */
  uint8_t  Hdpl3AuthenticationPubKey[ECDSA_256_PUB_KEY_LENGTH];   /* index 9 (32 Byte) + 10 (32 Bytes) + 11 (27 bytes): 91 Bytes */
  uint8_t  Hdpl3EncryptionPrivKey[ECDSA_256_PRIV_KEY_LENGTH];     /* end of index 11 (5 Bytes) + index 12 (32 bytes) + 13 (32 Bytes) + 14 (1 Bytes): 70 Bytes */
  uint8_t  Reserved[31U];                                         /* Alignment on 32 bytes */
} __attribute__((packed)) OBK_Hdpl2Config_t;

/* OEMuROT Hdpl 2 data section : to be aligned on 32 bytes
   ======================================================= */
typedef struct
{
  uint8_t   SHA256[SHA256_LENGTH];                      /* index 16 Mandatory to be first in the structure */
  uint8_t   CodeSHA256Ref[SHA256_LENGTH];               /* index 17 */
  uint32_t  CodeVersion;                                /* index 18 (8 bytes) */
  uint32_t  CodePrevious;
  uint8_t   Reserved[24U];                              /* Alignment on 32 bytes */
} OBK_Hdpl2Data_t;
#else /* not OEMUROT_ENABLE */
/* OEMiROT Hdpl 1 immutable section : to be aligned on 32 bytes
   ============================================================ */
typedef struct
{
  uint8_t  SHA256[SHA256_LENGTH];                                 /* index 8 Mandatory to be first in the structure */
  uint8_t  Hdpl3AuthenticationPubKey[ECDSA_256_PUB_KEY_LENGTH];   /* index 9 (32 Byte) + 10 (32 Bytes) + 11 (27 bytes): 91 Bytes */
  uint8_t  Hdpl3EncryptionPrivKey[ECDSA_256_PRIV_KEY_LENGTH];     /* end of index 11 (5 Bytes) + index 12 (32 bytes) + 13 (32 Bytes) + 14 (1 Bytes): 70 Bytes */
  uint8_t  Reserved[31U];                                         /* Alignment on 32 bytes */
} __attribute__((packed)) OBK_Hdpl1Config_t;

/* OEMiROT Hdpl 1 data section : to be aligned on 32 bytes
   ======================================================= */
typedef struct
{
  uint8_t   SHA256[SHA256_LENGTH];                      /* index 16 Mandatory to be first in the structure */
  uint8_t   CodeSHA256Ref[SHA256_LENGTH];               /* index 17 */
  uint32_t  CodeVersion;                                /* index 18 (8 bytes) */
  uint32_t  CodePrevious;
  uint8_t   Reserved[24U];                               /* Alignment on 32 bytes */
} OBK_Hdpl1Data_t;
#endif /* OEMUROT_ENABLE */
/* Driver configuration
   ==================== */
typedef struct
{
  uint32_t base;
  uint32_t limit;
} OBK_FlashRange_t;

typedef struct
{
  uint32_t nb;
  OBK_FlashRange_t *range;
} OBK_FlashVect_t;

typedef struct
{
  OBK_FlashVect_t erase;
  OBK_FlashVect_t write;
  uint32_t read_error;
} OBK_LowLevelDevice_t;

/* External variables --------------------------------------------------------*/
extern OBK_LowLevelDevice_t OBK_FLASH0_DEV;
extern ARM_DRIVER_FLASH OBK_Driver_FLASH0;
#if  defined(OEMUROT_ENABLE)
extern OBK_Hdpl2Config_t OBK_Hdpl2Cfg;
#else /* not OEMUROT_ENABLE */
extern OBK_Hdpl1Config_t OBK_Hdpl1Cfg;
#endif /* OEMUROT_ENABLE */

/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
#if  defined(OEMUROT_ENABLE)
void OBK_InitHdpl2DataSel(void);
void OBK_ControlHdpl2DataSel(void);
void OBK_ControlHdpl2Data(void);
void OBK_ReadHdpl2Config(OBK_Hdpl2Config_t *pOBK_Hdpl2Cfg);
HAL_StatusTypeDef OBK_ReadHdpl2Data(OBK_Hdpl2Data_t *pOBK_Hdpl2Data);
HAL_StatusTypeDef OBK_UpdateHdpl2Data(OBK_Hdpl2Data_t *pOBK_Hdpl2Data);
void OBK_VerifyHdpl2Config(OBK_Hdpl2Config_t *pOBK_Hdpl2Cfg);
#else /* not OEMUROT_ENABLE */
void OBK_InitHdpl1DataSel(void);
void OBK_ControlHdpl1DataSel(void);
void OBK_ControlHdpl1Data(void);
void OBK_ReadHdpl1Config(OBK_Hdpl1Config_t *pOBK_Hdpl1Cfg);
HAL_StatusTypeDef OBK_ReadHdpl1Data(OBK_Hdpl1Data_t *pOBK_Hdpl1Data);
HAL_StatusTypeDef OBK_UpdateHdpl1Data(OBK_Hdpl1Data_t *pOBK_Hdpl1Data);
void OBK_VerifyHdpl1Config(OBK_Hdpl1Config_t *pOBK_Hdpl1Cfg);
#endif /* OEMUROT_ENABLE */

bool OBK_IsEncryptionEnabled(void);

HAL_StatusTypeDef OBK_UpdateNVCounter(enum nv_counter_t CounterId, uint32_t Counter);
HAL_StatusTypeDef OBK_GetNVCounter(enum nv_counter_t CounterId, uint32_t *pCounter);

#ifdef __cplusplus
}
#endif

#endif /* LOW_LEVEL_OBKEYS_H */
