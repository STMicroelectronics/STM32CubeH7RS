/**
  ******************************************************************************
  * @file    low_level_obkeys.c
  * @author  MCD Application Team
  * @brief   Low Level Interface module to access OKB area in FLASH
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

/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_hal.h"
#include "low_level_obkeys.h"
#include "low_level_flash.h"
#include "low_level_security.h"
#if  defined(OEMIROT_MCE_PROTECTION)
#include "low_level_mce.h"
#endif /* OEMIROT_MCE_PROTECTION */
#include "boot_hal_flowcontrol.h"
#include "boot_hal_cfg.h"
#include "flash_layout.h"
#include "config-boot.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/

/**
  * Arm Flash device structure.
  */
struct arm_obk_flash_dev_t
{
  OBK_LowLevelDevice_t *dev;
  ARM_FLASH_INFO *data;       /*!< OBK FLASH memory device data */
};

/* Private defines -----------------------------------------------------------*/
#define SBS_EXT_EPOCHSELCR_EPOCH_SEL_S_EPOCH    (1U << SBS_EPOCHSELCR_EPOCH_SEL_Pos )
#define ST_SHA256_TIMEOUT                       (3U)
#define SAES_TIMEOUT                            (100U)

/* config for OBK flash driver */
#define OBK_FLASH0_TOTAL_SIZE                   (0x2000U)
#define OBK_FLASH0_PROG_UNIT                    (FLASH_PROG_UNIT)
#define OBK_FLASH0_ERASED_VAL                   (0xFFU)

#if  defined(OEMUROT_ENABLE)
/* Selector value of hdpl2 data */
#define OBK_HDPL2_DATA_AREA_SELECTOR_RESET      (0xFFU)
#define OBK_HDPL2_DATA_AREA_SELECTOR_A          (0x0U)
#define OBK_HDPL2_DATA_AREA_SELECTOR_B          (0x1U)
#else /* not OEMUROT_ENABLE */
/* Selector value of hdpl1 data */
#define OBK_HDPL1_DATA_AREA_SELECTOR_RESET      (0xFFU)
#define OBK_HDPL1_DATA_AREA_SELECTOR_A          (0x0U)
#define OBK_HDPL1_DATA_AREA_SELECTOR_B          (0x1U)
#endif /* OEMUROT_ENABLE */

/* Number of index used per HDPL to store debug status & DA jump action status*/
#define OBK_HDPL_RESERVED_LAST_INDEX            (1U)

/* Limit of hdpl0 1 2 user OBK area */
#define OBK_HDPL_MAX_INDEX                      (32U - OBK_HDPL_RESERVED_LAST_INDEX)

/* Full available size per level
 * first 8 indexes are write-only
 * Last index is used for the status
 */
#define OBK_HDPL_FULL_USER_AREA_SIZE            ((OBK_HDPL_MAX_INDEX - OBK_HDPL_RW_INDEX)\
                                                 * OBK_HDPL_256_KEY_SIZE)

/* Full available size per level
 * included the 8 first indexes (write-only)
 * Last index is used for the status
 */
#define OBK_HDPL_CHECK_AREA_SIZE           (OBK_HDPL_MAX_INDEX * OBK_HDPL_256_KEY_SIZE)

#if  defined(OEMUROT_ENABLE)
/* Get the offset of the hdpl2 alternate area */
#define GET_OBK_HDPL2_DATA_ALTERNATE_OFFSET()   ((OBK_Hdpl2DataSelector == OBK_HDPL2_DATA_AREA_SELECTOR_A) \
                                                 ? OBK_HDPL2_DATA_AREA_A_OFFSET \
                                                 : OBK_HDPL2_DATA_AREA_B_OFFSET)

/* Get the offset of the hdpl2 current area */
#define GET_OBK_HDPL2_DATA_CURRENT_OFFSET()     ((OBK_Hdpl2DataSelector != OBK_HDPL2_DATA_AREA_SELECTOR_A) \
                                                 ? OBK_HDPL2_DATA_AREA_A_OFFSET \
                                                 : OBK_HDPL2_DATA_AREA_B_OFFSET)

#define OBK_HDPL2_DEBUG_INFO_INDEX              (OBK_HDPL_MAX_INDEX - 1U)
#define OBK_HDPL2_DEBUG_INFO_OFFSET             (OBK_HDPL2_DEBUG_INFO_INDEX * OBK_HDPL_256_KEY_SIZE)
#else /* not OEMUROT_ENABLE */
/* Get the offset of the hdpl1 alternate area */
#define GET_OBK_HDPL1_DATA_ALTERNATE_OFFSET()   ((OBK_Hdpl1DataSelector == OBK_HDPL1_DATA_AREA_SELECTOR_A) \
                                                 ? OBK_HDPL1_DATA_AREA_A_OFFSET \
                                                 : OBK_HDPL1_DATA_AREA_B_OFFSET)

/* Get the offset of the hdpl1 current area */
#define GET_OBK_HDPL1_DATA_CURRENT_OFFSET()     ((OBK_Hdpl1DataSelector != OBK_HDPL1_DATA_AREA_SELECTOR_A) \
                                                 ? OBK_HDPL1_DATA_AREA_A_OFFSET \
                                                 : OBK_HDPL1_DATA_AREA_B_OFFSET)

#define OBK_HDPL1_DEBUG_INFO_INDEX              (OBK_HDPL_MAX_INDEX - 1U)
#define OBK_HDPL1_DEBUG_INFO_OFFSET             (OBK_HDPL1_DEBUG_INFO_INDEX * OBK_HDPL_256_KEY_SIZE)
#endif /* OEMUROT_ENABLE */

#define MEM_CODE_SLOT_ALIGN                     (0x4000U) /* 16 kBytes boundary (mpu constraint) */
#define MEM_DATA_SLOT_ALIGN                     (0x1000U) /* 4 kBytes boundary (mce constraint) */

#define LL_SECU_WRPx(shift)                     (~(1U << shift) & FLASH_WRPSR_WRPS)    /* shift is the number of the protected sector */

/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/
#if  defined(OEMUROT_ENABLE)
OBK_Hdpl2Config_t OBK_Hdpl2Cfg = {0U};
#else /* not OEMUROT_ENABLE */
OBK_Hdpl1Config_t OBK_Hdpl1Cfg = {0U};
#endif /* OEMUROT_ENABLE */

/* Selector of the alternate area (where the new data can be written) */
#if  defined(OEMUROT_ENABLE)
static uint8_t OBK_Hdpl2DataSelector = OBK_HDPL2_DATA_AREA_SELECTOR_RESET;
#else /* not OEMUROT_ENABLE */
static uint8_t OBK_Hdpl1DataSelector = OBK_HDPL1_DATA_AREA_SELECTOR_RESET;
#endif /* OEMUROT_ENABLE */

/* Private function prototypes -----------------------------------------------*/
#if defined(BL2_HW_ACCEL_ENABLE)
static int32_t OBK_ReadEncrypted(uint32_t Offset, void *pData, uint32_t Length, uint32_t HDPLLevel);
int32_t OBK_WriteEncrypted(uint32_t Offset, const void *pData, uint32_t Length, uint32_t HDPLLevel);
#endif /* BL2_HW_ACCEL_ENABLE */
static int32_t OBK_Read(uint32_t Offset, void *pData, uint32_t Length, uint32_t HDPLLevel);
int32_t OBK_Write(uint32_t Offset, const void *pData, uint32_t Length, uint32_t HDPLLevel);
static HAL_StatusTypeDef Compute_SHA256(uint8_t *pBuffer, uint32_t Length, uint8_t *pSHA256);
static uint32_t MemoryCompare(uint8_t *pAdd1, uint8_t *pAdd2, uint32_t Size);
static bool OBK_IsRangeValid(uint32_t Index, uint32_t Length, uint32_t HDPLLevel, uint32_t SizeLimit);

/* Functions Definition ------------------------------------------------------*/
#if  defined(OEMUROT_ENABLE)
/**
  * @brief  Init the hdpl2 selector of data alternate area
  * @param  None
  * @retval None
  */
void OBK_InitHdpl2DataSel(void)
{
  uint32_t a_bankOffset[OBK_HDPL2_DATA_AREA_NB] = { OBK_HDPL2_DATA_AREA_A_OFFSET, OBK_HDPL2_DATA_AREA_B_OFFSET };
  uint32_t imgVersions[OBK_HDPL2_DATA_AREA_NB] = { 0U };
  uint8_t a_sha256[SHA256_LENGTH] = { 0U };
  OBK_Hdpl2Data_t OBK_hdpl2_data = { 0U };
  uint32_t address = (uint32_t) &OBK_hdpl2_data;
  uint32_t i = 0U;

  if (OBK_Hdpl2DataSelector == OBK_HDPL2_DATA_AREA_SELECTOR_RESET)
  {
    for (i = 0U; i < OBK_HDPL2_DATA_AREA_NB; i++)
    {
      /* Read configuration in OBKeys */
      if (OBK_IsEncryptionEnabled())
      {
#if defined(BL2_HW_ACCEL_ENABLE)
        if (OBK_ReadEncrypted(a_bankOffset[i], &OBK_hdpl2_data, sizeof(OBK_Hdpl2Data_t),
                              FLASH_KEY_LEVEL_CURRENT) != ARM_DRIVER_OK)
        {
          Error_Handler();
        }
#else /* not BL2_HW_ACCEL_ENABLE */
        Error_Handler();
#endif /* BL2_HW_ACCEL_ENABLE */
      }
      else
      {
        if (OBK_Read(a_bankOffset[i], &OBK_hdpl2_data, sizeof(OBK_Hdpl2Data_t), FLASH_KEY_LEVEL_CURRENT) != ARM_DRIVER_OK)
        {
          Error_Handler();
        }
      }

      /* Verif SHA256 on the whole Hdpl2 data except first 32 bytes of SHA256 */
      if (Compute_SHA256((uint8_t *)(address + SHA256_LENGTH), sizeof(OBK_Hdpl2Data_t) - SHA256_LENGTH, a_sha256) != HAL_OK)
      {
        Error_Handler();
      }

      if (MemoryCompare((uint8_t *)address, a_sha256, SHA256_LENGTH) != 0U)
      {
        /* The alternate area is found*/
        OBK_Hdpl2DataSelector = (a_bankOffset[i] == OBK_HDPL2_DATA_AREA_A_OFFSET) ?
                                OBK_HDPL2_DATA_AREA_SELECTOR_A : OBK_HDPL2_DATA_AREA_SELECTOR_B;
        break;
      }
      else
      {
        /* store the code version */
        imgVersions[i] = OBK_hdpl2_data.CodeVersion;
      }
    }

    /* All OBK areas are valid */
    if (OBK_Hdpl2DataSelector == OBK_HDPL2_DATA_AREA_SELECTOR_RESET)
    {
      /* the unvalidation of the previous current area failed */
      /* the oldest version is the alternate sector (lowest version number) */
      /* In the case that version numbers are equal, area A is selected */
      OBK_Hdpl2DataSelector = (imgVersions[OBK_HDPL2_DATA_AREA_SELECTOR_A] <= imgVersions[OBK_HDPL2_DATA_AREA_SELECTOR_B]) ?
                              OBK_HDPL2_DATA_AREA_SELECTOR_A : OBK_HDPL2_DATA_AREA_SELECTOR_B;
    }

    /* Check that an hdpl2 data area is selected */
    if ((OBK_Hdpl2DataSelector != OBK_HDPL2_DATA_AREA_SELECTOR_RESET) &&
        ((OBK_Hdpl2DataSelector == OBK_HDPL2_DATA_AREA_SELECTOR_A) ||
         (OBK_Hdpl2DataSelector == OBK_HDPL2_DATA_AREA_SELECTOR_B)))
    {
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_OBK_DATA_SEL_CH1, FLOW_CTRL_OBK_DATA_SEL_CH1);
    }
    else
    {
      Error_Handler();
    }
  }
}
#else /* not OEMUROT_ENABLE */
/**
  * @brief  Init the hdpl1 selector of data alternate area
  * @param  None
  * @retval None
  */
void OBK_InitHdpl1DataSel(void)
{
  uint32_t a_bankOffset[OBK_HDPL1_DATA_AREA_NB] = { OBK_HDPL1_DATA_AREA_A_OFFSET, OBK_HDPL1_DATA_AREA_B_OFFSET };
  uint32_t imgVersions[OBK_HDPL1_DATA_AREA_NB] = { 0U };
  uint8_t a_sha256[SHA256_LENGTH] = { 0U };
  OBK_Hdpl1Data_t OBK_hdpl1_data = { 0U };
  uint32_t address = (uint32_t) &OBK_hdpl1_data;
  uint32_t i = 0U;

  if (OBK_Hdpl1DataSelector == OBK_HDPL1_DATA_AREA_SELECTOR_RESET)
  {
    for (i = 0U; i < OBK_HDPL1_DATA_AREA_NB; i++)
    {
      /* Read configuration in OBKeys */
      if (OBK_IsEncryptionEnabled())
      {
#if defined(BL2_HW_ACCEL_ENABLE)
        if (OBK_ReadEncrypted(a_bankOffset[i], &OBK_hdpl1_data, sizeof(OBK_Hdpl1Data_t),
                              FLASH_KEY_LEVEL_CURRENT) != ARM_DRIVER_OK)
        {
          Error_Handler();
        }
#else /* not BL2_HW_ACCEL_ENABLE */
        Error_Handler();
#endif /* BL2_HW_ACCEL_ENABLE */
      }
      else
      {
        if (OBK_Read(a_bankOffset[i], &OBK_hdpl1_data, sizeof(OBK_Hdpl1Data_t), FLASH_KEY_LEVEL_CURRENT) != ARM_DRIVER_OK)
        {
          Error_Handler();
        }
      }

      /* Verif SHA256 on the whole Hdpl1 data except first 32 bytes of SHA256 */
      if (Compute_SHA256((uint8_t *)(address + SHA256_LENGTH), sizeof(OBK_Hdpl1Data_t) - SHA256_LENGTH, a_sha256) != HAL_OK)
      {
        Error_Handler();
      }

      if (MemoryCompare((uint8_t *)address, a_sha256, SHA256_LENGTH) != 0U)
      {
        /* The alternate area is found*/
        OBK_Hdpl1DataSelector = (a_bankOffset[i] == OBK_HDPL1_DATA_AREA_A_OFFSET) ?
                                OBK_HDPL1_DATA_AREA_SELECTOR_A : OBK_HDPL1_DATA_AREA_SELECTOR_B;
        break;
      }
      else
      {
        /* store the code version */
        imgVersions[i] = OBK_hdpl1_data.CodeVersion;
      }
    }

    /* All OBK areas are valid */
    if (OBK_Hdpl1DataSelector == OBK_HDPL1_DATA_AREA_SELECTOR_RESET)
    {
      /* the unvalidation of the previous current area failed */
      /* the oldest version is the alternate sector (lowest version number) */
      /* In the case that version numbers are equal, area A is selected */
      OBK_Hdpl1DataSelector = (imgVersions[OBK_HDPL1_DATA_AREA_SELECTOR_A] <= imgVersions[OBK_HDPL1_DATA_AREA_SELECTOR_B]) ?
                              OBK_HDPL1_DATA_AREA_SELECTOR_A : OBK_HDPL1_DATA_AREA_SELECTOR_B;
    }

    /* Check that an hdpl1 data area is selected */
    if ((OBK_Hdpl1DataSelector != OBK_HDPL1_DATA_AREA_SELECTOR_RESET) &&
        ((OBK_Hdpl1DataSelector == OBK_HDPL1_DATA_AREA_SELECTOR_A) ||
         (OBK_Hdpl1DataSelector == OBK_HDPL1_DATA_AREA_SELECTOR_B)))
    {
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_OBK_DATA_SEL_CH1, FLOW_CTRL_OBK_DATA_SEL_CH1);
    }
    else
    {
      Error_Handler();
    }
  }
}
#endif /* OEMUROT_ENABLE */

#if  defined(OEMUROT_ENABLE)
/**
  * @brief  Init the hdpl2 selector of data alternate area
  * @param  None
  * @retval None
  */
void OBK_ControlHdpl2DataSel(void)
{
  if ((OBK_Hdpl2DataSelector != OBK_HDPL2_DATA_AREA_SELECTOR_A) &&
      (OBK_Hdpl2DataSelector != OBK_HDPL2_DATA_AREA_SELECTOR_B))
  {
    Error_Handler();
  }
  else
  {
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_OBK_DATA_SEL_CH2, FLOW_CTRL_OBK_DATA_SEL_CH2);
  }
}
#else /* not OEMUROT_ENABLE */
/**
  * @brief  Init the hdpl1 selector of data alternate area
  * @param  None
  * @retval None
  */
void OBK_ControlHdpl1DataSel(void)
{
  if ((OBK_Hdpl1DataSelector != OBK_HDPL1_DATA_AREA_SELECTOR_A) &&
      (OBK_Hdpl1DataSelector != OBK_HDPL1_DATA_AREA_SELECTOR_B))
  {
    Error_Handler();
  }
  else
  {
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_OBK_DATA_SEL_CH2, FLOW_CTRL_OBK_DATA_SEL_CH2);
  }
}
#endif /* OEMUROT_ENABLE */

/**
  * @brief  Check whether the OBKeys are encrypted
  * @param  None
  * @retval bool: true if the OBKeys are encrypted, false otherwise
  */
bool OBK_IsEncryptionEnabled(void)
{
#if defined(BL2_HW_ACCEL_ENABLE)
  static FLASH_OBProgramInitTypeDef flash_option_bytes = {0};
  uint32_t product_state;

  /* Get Option Bytes status */
  (void) HAL_FLASHEx_OBGetConfig(&flash_option_bytes);

  /* Check Non-Volatile State */
  if (!IS_OB_NVSTATE(flash_option_bytes.NVState))
  {
    Error_Handler();
  }

  /* Check Root-Of-Trust OEM Provisioned state */
  if (!IS_OB_OEM_PROVD(flash_option_bytes.ROTConfig & FLASH_ROTSR_OEM_PROVD))
  {
    Error_Handler();
  }

  /* Check Root-Of-Trust Debug authentication method */
  if (!IS_OB_DBG_AUTH(flash_option_bytes.ROTConfig & FLASH_ROTSR_DBG_AUTH))
  {
    Error_Handler();
  }

  /* Get the product state */
  product_state = LL_SECU_GetProductState(flash_option_bytes.NVState,
                                          flash_option_bytes.ROTConfig & FLASH_ROTSR_OEM_PROVD,
                                          flash_option_bytes.ROTConfig & FLASH_ROTSR_DBG_AUTH);

  /* Return the result */
  if ((product_state == OEMIROT_PROD_STATE_LOCKED) || (product_state == OEMIROT_PROD_STATE_CLOSED_ECDSA)
      || (product_state == OEMIROT_PROD_STATE_CLOSED_PWD))
  {
    return true;
  }
  else
  {
    return false;
  }
#else /* not BL2_HW_ACCEL_ENABLE */
  return false;
#endif /* BL2_HW_ACCEL_ENABLE */
}

/**
  * @brief  Get the index number in obkey from an address
  * @param  Address : address to get the index
  * @retval uint32_t : the corresponding index
  */
static uint32_t OBK_CalculateKeyIndex(const uint32_t Address)
{
  if (Address % OBK_PROG_UNIT != 0)
  {
    Error_Handler();
  }

  return (Address / OBK_PROG_UNIT);
}

/**
  * @brief  Read encrypted OBkeys
  * @param  Offset Offset in the OBKeys area (aligned on 32 bytes)
  * @param  pData Data buffer to be filled (aligned on 32 bytes)
  * @param  Length Length of the data to read
  * @param  HDPLLevel level of the key to write (FLASH_KEY_LEVEL_CURRENT: HDPL1 / FLASH_KEY_LEVEL_NEXT: HDPL2)
  * @retval ARM_DRIVER error status
  */
#if defined(BL2_HW_ACCEL_ENABLE)
static int32_t OBK_ReadEncrypted(uint32_t Offset, void *pData, uint32_t Length, uint32_t HDPLLevel)
{
  CRYP_HandleTypeDef hcryp = { 0U };
  uint32_t a_aes_iv[4U] = {0x8001D1CEU, 0xD1CED1CEU, 0xD1CE8001U, 0xCED1CED1U};
  uint32_t SaesTimeout = SAES_TIMEOUT;
  FLASH_KeyConfigTypeDef keyConfig = { 0U };
  static uint8_t a_ReadEncrypted[OBK_MAX_HDPL_AREA_SIZE];
  uint32_t destination = (uint32_t) a_ReadEncrypted;
  uint32_t index = OBK_CalculateKeyIndex(Offset);
  uint32_t lg = Length;

  memset(a_ReadEncrypted, 0U, ARRAY_SIZE(a_ReadEncrypted));

  /* Check OBKeys boundaries in Hal function */
  if (OBK_IsRangeValid(index, lg, HDPLLevel, ARRAY_SIZE(a_ReadEncrypted)) == false)
  {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  /* Read configuration in OBKeys */
  keyConfig.HDPLLevel = HDPLLevel;

  (void) HAL_FLASH_Unlock();
  (void) HAL_FLASH_OB_Unlock();

  while (destination < ((uint32_t)a_ReadEncrypted + lg))
  {
    if (index >= OBK_HDPL_MAX_INDEX)
    {
      return ARM_DRIVER_ERROR_SPECIFIC;
    }
    keyConfig.Index = index;
    keyConfig.Size = FLASH_KEY_256_BITS;

    if (HAL_FLASHEx_GetKey(&keyConfig, (uint32_t *)destination) != HAL_OK)
    {
      return ARM_DRIVER_ERROR_SPECIFIC;
    }
    index++;
    destination += OBK_PROG_UNIT;
  }
  (void) HAL_FLASH_OB_Lock();
  (void) HAL_FLASH_Lock();

  /* Configure SAES parameters */
  /* Configure SAES to use 256-Byte AHK */
  hcryp.Instance = SAES;
  hcryp.Init.DataType  = CRYP_NO_SWAP;
  hcryp.Init.KeySelect = CRYP_KEYSEL_AHK;  /* Hardware unique key (256-bits) */
  hcryp.Init.Algorithm = CRYP_AES_CBC;
  hcryp.Init.KeyMode = CRYP_KEYMODE_NORMAL;
  hcryp.Init.KeySize = CRYP_KEYSIZE_256B; /* 256 bits AES Key*/
  hcryp.Init.pInitVect = a_aes_iv;
  LL_SECU_ConfigureSAES(&hcryp, HDPLLevel, SaesTimeout, OBK_CRYP_KEY_INDEX);

  /*Size is n words*/
  if (HAL_CRYP_Decrypt(&hcryp, (uint32_t *)a_ReadEncrypted, (uint16_t)(Length / 4U), (uint32_t *)pData,
                       SaesTimeout) != HAL_OK)
  {
    return ARM_DRIVER_ERROR_SPECIFIC;
  }
  if (HAL_CRYP_DeInit(&hcryp) != HAL_OK)
  {
    return ARM_DRIVER_ERROR_SPECIFIC;
  }

  return ARM_DRIVER_OK;
}

/**
  * @brief  Write encrypted OBkeys
  * @param  Offset Offset in the OBKeys area (aligned on 32 bytes)
  * @param  pData Data buffer to be programmed encrypted (aligned on 32 bytes)
  * @param  Length length of the input buffer in bytes (aligned on 32 bytes)
  * @param  HDPLLevel: level of the key to write (FLASH_KEY_LEVEL_CURRENT: HDPL1 / FLASH_KEY_LEVEL_NEXT: HDPL2)
  * @retval ARM_DRIVER error status
  */
int32_t OBK_WriteEncrypted(uint32_t Offset, const void *pData, uint32_t Length, uint32_t HDPLLevel)
{
  uint32_t SaesTimeout = SAES_TIMEOUT;
  CRYP_HandleTypeDef hcryp = { 0U };
  uint32_t a_aes_iv[4U] = {0x8001D1CEU, 0xD1CED1CEU, 0xD1CE8001U, 0xCED1CED1U};
  static uint8_t a_DataEncrypted[OBK_MAX_HDPL_AREA_SIZE] = { 0U };
  uint32_t destination = (uint32_t)a_DataEncrypted;
  FLASH_KeyConfigTypeDef keyConfig = { 0U };
  uint32_t lg = 0U;
  uint32_t index = OBK_CalculateKeyIndex(Offset);
  uint32_t a_readKey[8U] = { 0U };

  /* Align data block on OBK key size of 32 bytes if it is not done */
  lg = (Length % OBK_PROG_UNIT != 0U) ?
       (Length / OBK_PROG_UNIT + 1) * OBK_PROG_UNIT :
       Length;

  /* Check OBKeys boundaries in Hal function */
  if (OBK_IsRangeValid(index, lg, HDPLLevel, ARRAY_SIZE(a_DataEncrypted)) == false)
  {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  memset(a_DataEncrypted, 0U, ARRAY_SIZE(a_DataEncrypted));

  /* Configure SAES parameters */
  hcryp.Instance = SAES;
  hcryp.Init.DataType  = CRYP_NO_SWAP;
  hcryp.Init.KeySelect = CRYP_KEYSEL_AHK;        /* Hardware key : derived hardware unique key (DHUK 256-bit) */
  hcryp.Init.Algorithm = CRYP_AES_CBC;
  hcryp.Init.KeyMode = CRYP_KEYMODE_NORMAL;
  hcryp.Init.KeySize = CRYP_KEYSIZE_256B;       /* 256 bits AES Key*/
  hcryp.Init.pInitVect = a_aes_iv;
  LL_SECU_ConfigureSAES(&hcryp, HDPLLevel, SaesTimeout, OBK_CRYP_KEY_INDEX);

  /*Size is n words*/
  if (HAL_CRYP_Encrypt(&hcryp, (uint32_t *)pData, (uint16_t)(lg / 4U), (uint32_t *)a_DataEncrypted,
                       SaesTimeout) != HAL_OK)
  {
    return ARM_DRIVER_ERROR_SPECIFIC;
  }
  if (HAL_CRYP_DeInit(&hcryp) != HAL_OK)
  {
    return ARM_DRIVER_ERROR_SPECIFIC;
  }

  /* Unlock Flash area */
  (void) HAL_FLASH_Unlock();
  (void) HAL_FLASH_OB_Unlock();

  /* Program OBKeys */
  keyConfig.HDPLLevel = HDPLLevel;
  keyConfig.Size = FLASH_KEY_256_BITS;

  while (destination < ((uint32_t)a_DataEncrypted + lg))
  {
    if (index >= OBK_HDPL_MAX_INDEX)
    {
      return ARM_DRIVER_ERROR_SPECIFIC;
    }

    keyConfig.Index = index;
    if (HAL_FLASHEx_KeyConfig(&keyConfig, (uint32_t *) destination) != HAL_OK)
    {
      return ARM_DRIVER_ERROR_SPECIFIC;
    }

    if (HAL_FLASHEx_GetKey(&keyConfig, a_readKey) != HAL_OK)
    {
      return ARM_DRIVER_ERROR_SPECIFIC;
    }

    if (MemoryCompare((uint8_t *)a_readKey, (uint8_t *)destination, OBK_PROG_UNIT) != 0U)
    {
      Error_Handler();
    }

    index++;
    destination += OBK_PROG_UNIT;
  }

  /* Lock the User Flash area */
  (void) HAL_FLASH_OB_Lock();
  (void) HAL_FLASH_Lock();

  return ARM_DRIVER_OK;
}
#endif /* BL2_HW_ACCEL_ENABLE */

/**
  * @brief  Check input parameters of read/write OBK functions
  * @param  Index start index
  * @param  Length length of the input buffer in bytes (aligned on 32 bytes)
  * @param  HDPLLevel level of the key to write (FLASH_KEY_LEVEL_CURRENT: HDPL1 / FLASH_KEY_LEVEL_NEXT: HDPL2)
  * @param  SizeLimit buffer size to not overflow
  * @retval bool true if parameters are valid, else false
  */
static bool OBK_IsRangeValid(uint32_t Index, uint32_t Length, uint32_t HDPLLevel, uint32_t SizeLimit)
{
  /* Check OBKeys boundaries in Hal function */
  return ((Index >= OBK_HDPL_RW_INDEX) &&
          (Index < OBK_HDPL_MAX_INDEX) &&
          (IS_KEY_HDPL_LEVEL(HDPLLevel)) &&
          (HDPLLevel != FLASH_KEY_LEVEL_PLUS_TWO) &&
          (Length <= SizeLimit) &&
          (Length != 0) &&
          (Length % OBK_PROG_UNIT == 0));
}

/**
  * @brief  Read non-encrypted OBkeys
  * @param  Offset Offset in the OBKeys area (aligned on 32 bytes)
  * @param  pData Data buffer to be filled (aligned on 32 bytes)
  * @param  Length Length of the data to read
  * @param  HDPLLevel level of the key to write (FLASH_KEY_LEVEL_CURRENT: HDPL1 / FLASH_KEY_LEVEL_NEXT: HDPL2)
  * @retval ARM_DRIVER error status
  */
static int32_t OBK_Read(uint32_t Offset, void *pData, uint32_t Length, uint32_t HDPLLevel)
{
  FLASH_KeyConfigTypeDef keyConfig = { 0U };
  uint32_t destination = (uint32_t) pData;
  uint32_t index = OBK_CalculateKeyIndex(Offset);

  /* Check OBKeys boundaries in Hal function */
  if (OBK_IsRangeValid(index, Length, HDPLLevel, OBK_MAX_HDPL_AREA_SIZE) == false)
  {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  /* Read configuration in OBKeys */
  keyConfig.HDPLLevel = HDPLLevel;

  (void) HAL_FLASH_Unlock();
  (void) HAL_FLASH_OB_Unlock();

  while (destination < ((uint32_t)pData + Length))
  {
    if (index >= OBK_HDPL_MAX_INDEX)
    {
      return ARM_DRIVER_ERROR_SPECIFIC;
    }
    keyConfig.Index = index;
    keyConfig.Size = FLASH_KEY_256_BITS;

    if (HAL_FLASHEx_GetKey(&keyConfig, (uint32_t *)destination) != HAL_OK)
    {
      return ARM_DRIVER_ERROR_SPECIFIC;
    }
    index++;
    destination += OBK_PROG_UNIT;
  }
  (void) HAL_FLASH_OB_Lock();
  (void) HAL_FLASH_Lock();

  return ARM_DRIVER_OK;
}

/**
  * @brief  Write non-encrypted OBkeys
  * @param  Offset Offset in the OBKeys area (aligned on 32 bytes)
  * @param  pData Data buffer to be programmed encrypted (aligned on 32 bytes)
  * @param  Length length of the input buffer in bytes (aligned on 32 bytes)
  * @param  HDPLLevel: level of the key to write (FLASH_KEY_LEVEL_CURRENT: HDPL1 / FLASH_KEY_LEVEL_NEXT: HDPL2)
  * @retval ARM_DRIVER error status
  */
int32_t OBK_Write(uint32_t Offset, const void *pData, uint32_t Length, uint32_t HDPLLevel)
{
  uint32_t destination = (uint32_t)pData;
  FLASH_KeyConfigTypeDef keyConfig = { 0U };
  uint32_t lg = 0U;
  uint32_t index = OBK_CalculateKeyIndex(Offset);
  uint32_t a_readKey[8U] = { 0U };

  /* Align data block on OBK key size of 32 bytes if it is not done */
  lg = (Length % OBK_PROG_UNIT != 0U) ?
       (Length / OBK_PROG_UNIT + 1) * OBK_PROG_UNIT :
       Length;

  /* Check OBKeys boundaries in Hal function */
  if (OBK_IsRangeValid(index, lg, HDPLLevel, OBK_MAX_HDPL_AREA_SIZE) == false)
  {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  /* Unlock Flash area */
  (void) HAL_FLASH_Unlock();
  (void) HAL_FLASH_OB_Unlock();

  /* Program OBKeys */
  keyConfig.HDPLLevel = HDPLLevel;
  keyConfig.Size = FLASH_KEY_256_BITS;

  while (destination < ((uint32_t)pData + lg))
  {
    if (index >= OBK_HDPL_MAX_INDEX)
    {
      return ARM_DRIVER_ERROR_SPECIFIC;
    }

    keyConfig.Index = index;
    if (HAL_FLASHEx_KeyConfig(&keyConfig, (uint32_t *) destination) != HAL_OK)
    {
      return ARM_DRIVER_ERROR_SPECIFIC;
    }

    __ISB();
    __DSB();

    if (HAL_FLASHEx_GetKey(&keyConfig, a_readKey) != HAL_OK)
    {
      return ARM_DRIVER_ERROR_SPECIFIC;
    }

    if (MemoryCompare((uint8_t *)a_readKey, (uint8_t *)destination, OBK_PROG_UNIT) != 0U)
    {
      Error_Handler();
    }

    index++;
    destination += OBK_PROG_UNIT;
  }

  /* Lock the User Flash area */
  (void) HAL_FLASH_OB_Lock();
  (void) HAL_FLASH_Lock();

  return ARM_DRIVER_OK;
}

/**
  * @brief  Memory compare with constant time execution.
  * @note   Objective is to avoid basic attacks based on time execution
  * @param  pAdd1 Address of the first buffer to compare
  * @param  pAdd2 Address of the second buffer to compare
  * @param  Size Size of the comparison
  * @retval SFU_ SUCCESS if equal, a SFU_error otherwise.
  */
static uint32_t MemoryCompare(uint8_t *pAdd1, uint8_t *pAdd2, uint32_t Size)
{
  uint8_t result = 0x00U;
  uint32_t i = 0U;

  for (i = 0U; i < Size; i++)
  {
    result |= pAdd1[i] ^ pAdd2[i];
  }
  return result;
}

/**
  * @brief  Compute SHA256
  * @param  pBuffer pointer to the input buffer to be hashed
  * @param  Length length of the input buffer in bytes
  * @param  pSHA256 pointer to the compuyed digest
  * @retval None
  */
static HAL_StatusTypeDef Compute_SHA256(uint8_t *pBuffer, uint32_t Length, uint8_t *pSHA256)
{
  static HASH_HandleTypeDef hhash;

  /* Enable HASH clock */
  __HAL_RCC_HASH_CLK_ENABLE();

  hhash.Instance = HASH;
  /* HASH Configuration */
  if (HAL_HASH_DeInit(&hhash) != HAL_OK)
  {
    return HAL_ERROR;
  }
  hhash.Init.DataType = HASH_BYTE_SWAP;
  hhash.Init.Algorithm = HASH_ALGOSELECTION_SHA256;
  if (HAL_HASH_Init(&hhash) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* HASH computation */
  if (HAL_HASH_Start(&hhash, pBuffer, Length, pSHA256, ST_SHA256_TIMEOUT) != HAL_OK)
  {
    return HAL_ERROR;
  }
  return HAL_OK;
}

#if  defined(OEMUROT_ENABLE)
/**
  * @brief  Read data in OBkeys Hdpl2
  * @param  pOBK_Hdpl2Data pointer on Hdpl2 data
  * @retval None
  */
HAL_StatusTypeDef OBK_ReadHdpl2Data(OBK_Hdpl2Data_t *pOBK_Hdpl2Data)
{
  uint8_t a_sha256[SHA256_LENGTH] = { 0U };
  uint32_t address = (uint32_t) pOBK_Hdpl2Data;
  uint32_t offset = GET_OBK_HDPL2_DATA_CURRENT_OFFSET();

  /* Read configuration in OBKeys */
  if (OBK_IsEncryptionEnabled())
  {
#if defined(BL2_HW_ACCEL_ENABLE)
    if (OBK_ReadEncrypted(offset, (void *) pOBK_Hdpl2Data, sizeof(OBK_Hdpl2Data_t),
                          FLASH_KEY_LEVEL_CURRENT) != ARM_DRIVER_OK)
    {
      return HAL_ERROR;
    }
#else /* not BL2_HW_ACCEL_ENABLE */
    Error_Handler();
#endif /* BL2_HW_ACCEL_ENABLE */
  }
  else
  {
    if (OBK_Read(offset, (void *) pOBK_Hdpl2Data, sizeof(OBK_Hdpl2Data_t), FLASH_KEY_LEVEL_CURRENT) != ARM_DRIVER_OK)
    {
      return HAL_ERROR;
    }
  }

  /* Verif SHA256 on the whole Hdpl2 data except first 32 bytes of SHA256 */
  if (Compute_SHA256((uint8_t *)(address + SHA256_LENGTH), sizeof(OBK_Hdpl2Data_t) - SHA256_LENGTH, a_sha256) != HAL_OK)
  {
    return HAL_ERROR;
  }
  if (MemoryCompare(pOBK_Hdpl2Data->SHA256, a_sha256, SHA256_LENGTH) != 0U)
  {
    BOOT_LOG_ERR("Wrong OBK HDPL2 data");
    return HAL_ERROR;
  }

  return HAL_OK;
}
#else /* not OEMUROT_ENABLE */
/**
  * @brief  Read data in OBkeys Hdpl1
  * @param  pOBK_Hdpl1Data pointer on Hdpl1 data
  * @retval None
  */
HAL_StatusTypeDef OBK_ReadHdpl1Data(OBK_Hdpl1Data_t *pOBK_Hdpl1Data)
{
  uint8_t a_sha256[SHA256_LENGTH] = { 0U };
  uint32_t address = (uint32_t) pOBK_Hdpl1Data;
  uint32_t offset = GET_OBK_HDPL1_DATA_CURRENT_OFFSET();

  /* Read configuration in OBKeys */
  if (OBK_IsEncryptionEnabled())
  {
#if defined(BL2_HW_ACCEL_ENABLE)
    if (OBK_ReadEncrypted(offset, (void *) pOBK_Hdpl1Data, sizeof(OBK_Hdpl1Data_t),
                          FLASH_KEY_LEVEL_CURRENT) != ARM_DRIVER_OK)
    {
      return HAL_ERROR;
    }
#else /* not BL2_HW_ACCEL_ENABLE */
    Error_Handler();
#endif /* BL2_HW_ACCEL_ENABLE */
  }
  else
  {
    if (OBK_Read(offset, (void *) pOBK_Hdpl1Data, sizeof(OBK_Hdpl1Data_t), FLASH_KEY_LEVEL_CURRENT) != ARM_DRIVER_OK)
    {
      return HAL_ERROR;
    }
  }

  /* Verif SHA256 on the whole Hdpl1 data except first 32 bytes of SHA256 */
  if (Compute_SHA256((uint8_t *)(address + SHA256_LENGTH), sizeof(OBK_Hdpl1Data_t) - SHA256_LENGTH, a_sha256) != HAL_OK)
  {
    return HAL_ERROR;
  }
  if (MemoryCompare(pOBK_Hdpl1Data->SHA256, a_sha256, SHA256_LENGTH) != 0U)
  {
    BOOT_LOG_ERR("Wrong OBK HDPL1 data");
    return HAL_ERROR;
  }

  return HAL_OK;
}
#endif /* OEMUROT_ENABLE */

#if  defined(OEMUROT_ENABLE)
/**
  * @brief  Update data in OBkeys Hdpl2
  * @param  pOBK_Hdpl2Data pointer on Hdpl2 data
  * @retval ARM_DRIVER error status
  */
HAL_StatusTypeDef OBK_UpdateHdpl2Data(OBK_Hdpl2Data_t *pOBK_Hdpl2Data)
{
  uint8_t a_sha256[SHA256_LENGTH] = { 0U };
  uint32_t address = (uint32_t) pOBK_Hdpl2Data;
  FLASH_KeyConfigTypeDef keyConfig = { 0U };

  if (OBK_Hdpl2DataSelector == OBK_HDPL2_DATA_AREA_SELECTOR_RESET)
  {
    return HAL_ERROR;
  }

  /* Verif SHA256 on the whole hdpl1 data except first 32 bytes of SHA256 */
  if (Compute_SHA256((uint8_t *)(address + SHA256_LENGTH), sizeof(OBK_Hdpl2Data_t) - SHA256_LENGTH, a_sha256) != HAL_OK)
  {
    BOOT_LOG_ERR("Wrong OBK HDPL2 data");
    return HAL_ERROR;
  }
  (void) memcpy(&pOBK_Hdpl2Data->SHA256[0U], a_sha256, SHA256_LENGTH);

  /* Write configuration in alternate hdpl2 data OBKeys area */
  keyConfig.Index = GET_OBK_HDPL2_DATA_ALTERNATE_OFFSET();
  if (OBK_IsEncryptionEnabled())
  {
#if defined(BL2_HW_ACCEL_ENABLE)
    if (OBK_WriteEncrypted(keyConfig.Index, (void *) pOBK_Hdpl2Data, sizeof(OBK_Hdpl2Data_t),
                           FLASH_KEY_LEVEL_CURRENT) != ARM_DRIVER_OK)
    {
      return HAL_ERROR;
    }
#else /* not BL2_HW_ACCEL_ENABLE */
    Error_Handler();
#endif /* BL2_HW_ACCEL_ENABLE */
  }
  else
  {
    if (OBK_Write(keyConfig.Index, (void *) pOBK_Hdpl2Data, sizeof(OBK_Hdpl2Data_t),
                  FLASH_KEY_LEVEL_CURRENT) != ARM_DRIVER_OK)
    {
      return HAL_ERROR;
    }
  }

  /* write invalid SHA256 in the current area without encryption to invalidate it */
  keyConfig.HDPLLevel = FLASH_KEY_LEVEL_CURRENT;
  keyConfig.Size = FLASH_KEY_256_BITS;
  keyConfig.Index = GET_OBK_HDPL2_DATA_CURRENT_OFFSET() / OBK_PROG_UNIT;
  a_sha256[0U] = 0U;

  /* Unlock  Flash area */
  (void) HAL_FLASH_Unlock();
  (void) HAL_FLASH_OB_Unlock();

  if (HAL_FLASHEx_KeyConfig(&keyConfig, (uint32_t *) a_sha256) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Lock the User Flash area */
  (void) HAL_FLASH_OB_Lock();
  (void) HAL_FLASH_Lock();

  /* Update the alternate area selector*/
  OBK_Hdpl2DataSelector = (OBK_Hdpl2DataSelector == OBK_HDPL2_DATA_AREA_SELECTOR_A) ?
                          OBK_HDPL2_DATA_AREA_SELECTOR_B :
                          OBK_HDPL2_DATA_AREA_SELECTOR_A;

  return HAL_OK;
}
#else /* not OEMUROT_ENABLE */
/**
  * @brief  Update data in OBkeys Hdpl1
  * @param  pOBK_Hdpl1Data pointer on Hdpl1 data
  * @retval ARM_DRIVER error status
  */
HAL_StatusTypeDef OBK_UpdateHdpl1Data(OBK_Hdpl1Data_t *pOBK_Hdpl1Data)
{
  uint8_t a_sha256[SHA256_LENGTH] = { 0U };
  uint32_t address = (uint32_t) pOBK_Hdpl1Data;
  FLASH_KeyConfigTypeDef keyConfig = { 0U };

  if (OBK_Hdpl1DataSelector == OBK_HDPL1_DATA_AREA_SELECTOR_RESET)
  {
    return HAL_ERROR;
  }

  /* Verif SHA256 on the whole hdpl1 data except first 32 bytes of SHA256 */
  if (Compute_SHA256((uint8_t *)(address + SHA256_LENGTH), sizeof(OBK_Hdpl1Data_t) - SHA256_LENGTH, a_sha256) != HAL_OK)
  {
    BOOT_LOG_ERR("Wrong OBK HDPL1 data");
    return HAL_ERROR;
  }
  (void) memcpy(&pOBK_Hdpl1Data->SHA256[0U], a_sha256, SHA256_LENGTH);

  /* Write configuration in alternate hdpl1 data OBKeys area */
  keyConfig.Index = GET_OBK_HDPL1_DATA_ALTERNATE_OFFSET();
  if (OBK_IsEncryptionEnabled())
  {
#if defined(BL2_HW_ACCEL_ENABLE)
    if (OBK_WriteEncrypted(keyConfig.Index, (void *) pOBK_Hdpl1Data, sizeof(OBK_Hdpl1Data_t),
                           FLASH_KEY_LEVEL_CURRENT) != ARM_DRIVER_OK)
    {
      return HAL_ERROR;
    }
#else /* not BL2_HW_ACCEL_ENABLE */
    Error_Handler();
#endif /* BL2_HW_ACCEL_ENABLE */
  }
  else
  {
    if (OBK_Write(keyConfig.Index, (void *) pOBK_Hdpl1Data, sizeof(OBK_Hdpl1Data_t),
                  FLASH_KEY_LEVEL_CURRENT) != ARM_DRIVER_OK)
    {
      return HAL_ERROR;
    }
  }

  /* write invalid SHA256 in the current area without encryption to invalidate it */
  keyConfig.HDPLLevel = FLASH_KEY_LEVEL_CURRENT;
  keyConfig.Size = FLASH_KEY_256_BITS;
  keyConfig.Index = GET_OBK_HDPL1_DATA_CURRENT_OFFSET() / OBK_PROG_UNIT;
  a_sha256[0U] = 0U;

  /* Unlock  Flash area */
  (void) HAL_FLASH_Unlock();
  (void) HAL_FLASH_OB_Unlock();

  if (HAL_FLASHEx_KeyConfig(&keyConfig, (uint32_t *) a_sha256) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Lock the User Flash area */
  (void) HAL_FLASH_OB_Lock();
  (void) HAL_FLASH_Lock();

  /* Update the alternate area selector*/
  OBK_Hdpl1DataSelector = (OBK_Hdpl1DataSelector == OBK_HDPL1_DATA_AREA_SELECTOR_A) ?
                          OBK_HDPL1_DATA_AREA_SELECTOR_B :
                          OBK_HDPL1_DATA_AREA_SELECTOR_A;

  return HAL_OK;
}
#endif /* OEMUROT_ENABLE */

/* Public Functions Definition ------------------------------------------------------*/


#if  defined(OEMUROT_ENABLE)
/**
  * @brief  Read configuration in OBkeys Hdpl2
  * @param  pOBK_Hdpl1Cfg : pointer on Hdpl2 configuration
  * @retval None
  */
void OBK_ReadHdpl2Config(OBK_Hdpl2Config_t *pOBK_Hdpl2Cfg)
{
  uint8_t a_sha256[SHA256_LENGTH] = { 0U };
  uint32_t address = (uint32_t) pOBK_Hdpl2Cfg;

  /* Read configuration in OBKeys */
  if (OBK_IsEncryptionEnabled())
  {
#if defined(BL2_HW_ACCEL_ENABLE)
    if (OBK_ReadEncrypted(OBK_HDPL2_CFG_OFFSET, (void *) pOBK_Hdpl2Cfg, sizeof(OBK_Hdpl2Config_t),
                          FLASH_KEY_LEVEL_CURRENT) != ARM_DRIVER_OK)
    {
      Error_Handler();
    }
#else /* not BL2_HW_ACCEL_ENABLE */
    Error_Handler();
#endif /* BL2_HW_ACCEL_ENABLE */
  }
  else
  {
    if (OBK_Read(OBK_HDPL2_CFG_OFFSET, (void *) pOBK_Hdpl2Cfg, sizeof(OBK_Hdpl2Config_t),
                 FLASH_KEY_LEVEL_CURRENT) != ARM_DRIVER_OK)
    {
      Error_Handler();
    }
  }

  /* Verif SHA256 on the whole Hdpl2 config except first 32 bytes of SHA256 */
  if (Compute_SHA256((uint8_t *)(address + SHA256_LENGTH), sizeof(OBK_Hdpl2Config_t) - SHA256_LENGTH, a_sha256) != HAL_OK)
  {
    Error_Handler();
  }
  if (MemoryCompare(&pOBK_Hdpl2Cfg->SHA256[0U], a_sha256, SHA256_LENGTH) != 0U)
  {
    BOOT_LOG_ERR("read: Wrong OBK HDPL2 cfg");
    Error_Handler();
  }
}
#else /* not OEMUROT_ENABLE */
/**
  * @brief  Read configuration in OBkeys Hdpl1
  * @param  pOBK_Hdpl1Cfg : pointer on Hdpl1 configuration
  * @retval None
  */
void OBK_ReadHdpl1Config(OBK_Hdpl1Config_t *pOBK_Hdpl1Cfg)
{
  uint8_t a_sha256[SHA256_LENGTH] = { 0U };
  uint32_t address = (uint32_t) pOBK_Hdpl1Cfg;

  /* Read configuration in OBKeys */
  if (OBK_IsEncryptionEnabled())
  {
#if defined(BL2_HW_ACCEL_ENABLE)
    if (OBK_ReadEncrypted(OBK_HDPL1_CFG_OFFSET, (void *) pOBK_Hdpl1Cfg, sizeof(OBK_Hdpl1Config_t),
                          FLASH_KEY_LEVEL_CURRENT) != ARM_DRIVER_OK)
    {
      Error_Handler();
    }
#else /* not BL2_HW_ACCEL_ENABLE */
    Error_Handler();
#endif /* BL2_HW_ACCEL_ENABLE */
  }
  else
  {
    if (OBK_Read(OBK_HDPL1_CFG_OFFSET, (void *) pOBK_Hdpl1Cfg, sizeof(OBK_Hdpl1Config_t),
                 FLASH_KEY_LEVEL_CURRENT) != ARM_DRIVER_OK)
    {
      Error_Handler();
    }
  }

  /* Verif SHA256 on the whole Hdpl1 config except first 32 bytes of SHA256 */
  if (Compute_SHA256((uint8_t *)(address + SHA256_LENGTH), sizeof(OBK_Hdpl1Config_t) - SHA256_LENGTH, a_sha256) != HAL_OK)
  {
    Error_Handler();
  }
  if (MemoryCompare(&pOBK_Hdpl1Cfg->SHA256[0U], a_sha256, SHA256_LENGTH) != 0U)
  {
    BOOT_LOG_ERR("read: Wrong OBK HDPL1 cfg");
    Error_Handler();
  }
}
#endif /* OEMUROT_ENABLE */

#if  defined(OEMUROT_ENABLE)
/**
  * @brief  Verify configuration in OBkeys Hdpl2
  * @param  pOBK_Hdpl1Cfg pointer on Hdpl2 configuration
  * @retval None
  */
void OBK_VerifyHdpl2Config(OBK_Hdpl2Config_t *pOBK_Hdpl2Cfg)
{
  uint8_t a_sha256[SHA256_LENGTH] = { 0U };
  uint32_t Address = (uint32_t) pOBK_Hdpl2Cfg;

  /* Verif SHA256 on the whole Hdpl2 config except first 32 bytes of SHA256 */
  if (Compute_SHA256((uint8_t *)(Address + SHA256_LENGTH), sizeof(OBK_Hdpl2Config_t) - SHA256_LENGTH, a_sha256) != HAL_OK)
  {
    Error_Handler();
  }
  if (MemoryCompare(&pOBK_Hdpl2Cfg->SHA256[0U], a_sha256, SHA256_LENGTH) != 0U)
  {
    BOOT_LOG_ERR("verify: Wrong OBK HDPL2 cfg");
    Error_Handler();
  }
}
#else /* not OEMUROT_ENABLE */
/**
  * @brief  Verify configuration in OBkeys Hdpl1
  * @param  pOBK_Hdpl1Cfg pointer on Hdpl1 configuration
  * @retval None
  */
void OBK_VerifyHdpl1Config(OBK_Hdpl1Config_t *pOBK_Hdpl1Cfg)
{
  uint8_t a_sha256[SHA256_LENGTH] = { 0U };
  uint32_t Address = (uint32_t) pOBK_Hdpl1Cfg;

  /* Verif SHA256 on the whole Hdpl1 config except first 32 bytes of SHA256 */
  if (Compute_SHA256((uint8_t *)(Address + SHA256_LENGTH), sizeof(OBK_Hdpl1Config_t) - SHA256_LENGTH, a_sha256) != HAL_OK)
  {
    Error_Handler();
  }
  if (MemoryCompare(&pOBK_Hdpl1Cfg->SHA256[0U], a_sha256, SHA256_LENGTH) != 0U)
  {
    BOOT_LOG_ERR("verify: Wrong OBK HDPL1 cfg");
    Error_Handler();
  }
}
#endif /* OEMUROT_ENABLE */

#if  defined(OEMUROT_ENABLE)
/**
  * @brief  Control data in OBkeys Hdpl2
  * @param  none
  * @retval None
  */
void OBK_ControlHdpl2Data(void)
{
  OBK_Hdpl2Data_t OBK_hdpl2_data = { 0U };

  /* Read data in OBKeys HDPL2 */
  if (OBK_ReadHdpl2Data(&OBK_hdpl2_data) == HAL_OK)
  {
    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_OBK_DATA_CH1, FLOW_CTRL_OBK_DATA_CH1);
  }
  else
  {
    Error_Handler();
  }
}
#else /* not OEMUROT_ENABLE */
/**
  * @brief  Control data in OBkeys Hdpl1
  * @param  none
  * @retval None
  */
void OBK_ControlHdpl1Data(void)
{
  OBK_Hdpl1Data_t OBK_hdpl1_data = { 0U };

  /* Read data in OBKeys HDPL1 */
  if (OBK_ReadHdpl1Data(&OBK_hdpl1_data) == HAL_OK)
  {
    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_OBK_DATA_CH1, FLOW_CTRL_OBK_DATA_CH1);
  }
  else
  {
    Error_Handler();
  }
}
#endif /* OEMUROT_ENABLE */

/**
  * @brief  Get counter
  * @param  pCounter pointer on current Fw version
  * @retval None
  */
HAL_StatusTypeDef OBK_GetNVCounter(enum nv_counter_t CounterId, uint32_t *pCounter)
{
#if  defined(OEMUROT_ENABLE)
  OBK_Hdpl2Data_t OBK_data = { 0U };
#else /* not OEMUROT_ENABLE */
  OBK_Hdpl1Data_t OBK_data = { 0U };
#endif /* OEMUROT_ENABLE */

#if  defined(OEMUROT_ENABLE)
  /* Read all OBK hdpl2 data & control SHA256 */
  if (OBK_ReadHdpl2Data(&OBK_data) != HAL_OK)
#else /* not OEMUROT_ENABLE */
  /* Read all OBK hdpl1 data & control SHA256 */
  if (OBK_ReadHdpl1Data(&OBK_data) != HAL_OK)
#endif /* OEMUROT_ENABLE */
  {
    return HAL_ERROR;
  }
  *pCounter = 0xFFFFFFFFU;
  switch (CounterId)
  {
    case PLAT_NV_COUNTER_3 :
      if (OBK_data.CodeVersion >= OBK_data.CodePrevious)
      {
        *pCounter = OBK_data.CodeVersion;
        return HAL_OK;
      }
      break;
    default :
      break;
  }
  return HAL_ERROR;
}

/**
  * @brief  Update counter in OBKeys
  * @param  Counter Fw version
  * @retval None
  */
HAL_StatusTypeDef OBK_UpdateNVCounter(enum nv_counter_t CounterId, uint32_t Counter)
{
#if  defined(OEMUROT_ENABLE)
  OBK_Hdpl2Data_t OBK_data = { 0U };
#else /* not OEMUROT_ENABLE */
  OBK_Hdpl1Data_t OBK_data = { 0U };
#endif /* OEMUROT_ENABLE */

#if  defined(OEMUROT_ENABLE)
  /* Read all OBK hdpl2 data & control SHA256 */
  if (OBK_ReadHdpl2Data(&OBK_data) != HAL_OK)
#else /* not OEMUROT_ENABLE */
  /* Read all OBK hdpl1 data & control SHA256 */
  if (OBK_ReadHdpl1Data(&OBK_data) != HAL_OK)
#endif /* OEMUROT_ENABLE */
  {
    return HAL_ERROR;
  }

  switch (CounterId)
  {
    case PLAT_NV_COUNTER_3 :
      OBK_data.CodePrevious = OBK_data.CodeVersion;
      OBK_data.CodeVersion = Counter;
      break;
    default :
      return HAL_ERROR;
      break;
  }

#if  defined(OEMUROT_ENABLE)
  /* Update all OBK hdpl2 data with associated SHA256 */
  if (OBK_UpdateHdpl2Data(&OBK_data) != HAL_OK)
#else /* not OEMUROT_ENABLE */
  /* Update all OBK hdpl1 data with associated SHA256 */
  if (OBK_UpdateHdpl1Data(&OBK_data) != HAL_OK)
#endif /* OEMUROT_ENABLE */
  {
    return HAL_ERROR;
  }
  else
  {
    return HAL_OK;
  }
}
