/**
  ******************************************************************************
  * @file    low_level_mce.c
  * @author  MCD Application Team
  * @brief   Low Level Interface module to use STM32 MCE Ip
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
#if defined(STM32H7S3xx) || defined(STM32H7S7xx)
#include "low_level_mce.h"
#include "low_level_security.h"
#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
#include "low_level_rng.h"
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */
#include "boot_hal_cfg.h"
#include "boot_hal_flowcontrol.h"

/* Private defines -----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/


#define LL_MCE_REGION_DISABLED      (0xFFU)
#define LL_MCE_MASTERKEYS_LENGTH    (16U) /* 128 bits key */
#define LL_MCE_AHK_MCE_AES_INDEX    (1U)
#define LL_MCE_AHK_MCE_NOEKON_INDEX (2U)
#define SAES_DEFAULT_TIMEOUT        (100U)
#define LL_MCE_CODE_PRIMARY_REGION  MCE_REGION1

static MCE_HandleTypeDef hmce_primary;
#if  defined(OEMIROT_MCE_PROTECTION) && (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
static MCE_HandleTypeDef hmce_primary_run;
#endif /* OEMIROT_MCE_PROTECTION && (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM) */

static MCE_RegionConfigTypeDef aRegion[] =
{
  /* Code primary slot (AES) */
  {
    .PrivilegedAccess = MCE_REGION_NPRIV,
    .Mode = MCE_BLOCK_CIPHER,
    .ContextID = MCE_AES_CONTEXT_ID,
    .StartAddress = 0x0,
    .EndAddress = 0x0,
    .AccessMode = MCE_REGION_READONLY,
  },
  /* Code secondary slot only defined (AES) */
  {
    .PrivilegedAccess = MCE_REGION_NPRIV,
    .Mode = MCE_NO_CIPHER,
    .ContextID = MCE_AES_CONTEXT_ID,
    .StartAddress = 0x0,
    .EndAddress = 0x0,
    .AccessMode = MCE_REGION_READONLY,
  },
#if !defined(MCUBOOT_OVERWRITE_ONLY)
  /* Scratch area only defined (AES) */
  {
    .PrivilegedAccess = MCE_REGION_NPRIV,
    .Mode = MCE_BLOCK_CIPHER,
    .ContextID = MCE_AES_CONTEXT_ID,
    .StartAddress = 0x0,
    .EndAddress = 0x0,
    .AccessMode = MCE_REGION_READONLY,
  }
#endif /* not MCUBOOT_OVERWRITE_ONLY */

};

#if  defined(OEMIROT_MCE_PROTECTION) && (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
static MCE_RegionConfigTypeDef aRunRegion[] =
{
  /* Code primary run slot (Noekeon) */
  {
    .PrivilegedAccess = MCE_REGION_NPRIV,
    .Mode = MCE_BLOCK_CIPHER,
    .ContextID = MCE_NO_CONTEXT,
    .StartAddress = 0x0,
    .EndAddress = 0x0,
    .AccessMode = MCE_REGION_READONLY,
  }
};
#endif /* OEMIROT_MCE_PROTECTION && (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM) */

#if  defined(OEMIROT_MCE_PROTECTION)
#if defined(__ICCARM__)
#pragma location=".rodata"
#elif defined(__GNUC__)
__attribute__((section(".rodata")))
#endif /* __ICCARM__ */
static const uint32_t aRegionAccess[LL_MCE_REGION_CFG_END][LL_MCE_REGION_NUMBER] =
{
  /* Install configuration */
  {
    MCE_REGION_READWRITE,
    MCE_REGION_READWRITE,
#if !defined(MCUBOOT_OVERWRITE_ONLY)
    MCE_REGION_READWRITE
#endif /* not MCUBOOT_OVERWRITE_ONLY */
  },
  /* BL configuration */
  {
    LL_MCE_REGION_DISABLED,
    MCE_REGION_READWRITE,
#if !defined(MCUBOOT_OVERWRITE_ONLY)
    LL_MCE_REGION_DISABLED
#endif /* not MCUBOOT_OVERWRITE_ONLY */
  },
  /* Appli configuration */
  {
#if  (OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN)
    LL_MCE_REGION_DISABLED,
#else /* not (OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN) */
    MCE_REGION_READONLY,
#endif /* OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN */
    MCE_REGION_READWRITE,
#if !defined(MCUBOOT_OVERWRITE_ONLY)
    LL_MCE_REGION_DISABLED
#endif /* not MCUBOOT_OVERWRITE_ONLY */
  }
};
#endif /* OEMIROT_MCE_PROTECTION */

#if  defined(OEMIROT_MCE_PROTECTION) && (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
#if defined(__ICCARM__)
#pragma location=".rodata"
#elif defined(__GNUC__)
__attribute__((section(".rodata")))
#endif /* __ICCARM__ */
static const uint32_t aRunRegionAccess[LL_MCE_REGION_CFG_END][LL_MCE_RUN_REGION_NUMBER] =
{
  /* Install configuration */
  {
    MCE_REGION_READWRITE
  },
  /* BL configuration */
  {
    LL_MCE_REGION_DISABLED
  },
  /* Appli configuration */
  {
    MCE_REGION_READONLY
  }
};
#endif /* OEMIROT_MCE_PROTECTION && (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM) */

/* Private function prototypes -----------------------------------------------*/
#if defined(OEMIROT_MCE_PROTECTION)
static HAL_StatusTypeDef LL_MCE_Initialize(MCE_HandleTypeDef *hmce, uint32_t SlotAddress);
static void LL_MCE_InitRegion(MCE_RegionConfigTypeDef *pRegionConfig, const uint32_t Offset, const uint32_t RegionSize);
static void LL_MCE_ConfigRegion(MCE_HandleTypeDef *hmce,   MCE_RegionConfigTypeDef *pRegionConfig,
                                const uint32_t RegionType, const uint32_t AccessMode);
static void LL_MCE_ControlRegion(MCE_RegionConfigTypeDef *pRegionConfig, const uint32_t RegionType,
                                 const uint32_t Offset,                  const uint32_t RegionSize);
static HAL_StatusTypeDef LL_MCE_CheckMceConfiguration(MCE_HandleTypeDef *hmce,     MCE_RegionConfigTypeDef *pRegion,
                                                      const uint32_t RegionAccess, const uint8_t RegionType);
static uint32_t LL_MCE_GetRegionStatus(MCE_HandleTypeDef *hmce, uint32_t RegionIndex);
static void LL_MCE_RemoveKey(MCE_HandleTypeDef *hmce);
static void LL_MCE_LockGlobalConfig(MCE_HandleTypeDef *hmce);
#endif /* OEMIROT_MCE_PROTECTION */

/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Configure MCE IP and regions
  * @retval None
  */
void LL_MCE_Configure(void)
{
#if defined(OEMIROT_MCE_PROTECTION)
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    if (LL_MCE_Initialize(&hmce_primary, PRIMARY_PARTITION_START) != HAL_OK)
    {
      Error_Handler();
    }

#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
    if (LL_MCE_Initialize(&hmce_primary_run, PRIMARY_RUN_PARTITION_START) != HAL_OK)
    {
      Error_Handler();
    }

    if (hmce_primary.Instance == hmce_primary_run.Instance)
    {
      Error_Handler();
    }
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

    /* Initialize mce region size*/
    LL_MCE_InitRegion(&aRegion[LL_MCE_REGION_CODE_PRIMARY], PRIMARY_PARTITION_START, PRIMARY_PARTITION_SIZE);
    LL_MCE_InitRegion(&aRegion[LL_MCE_REGION_CODE_SECONDARY], SECONDARY_PARTITION_START, SECONDARY_PARTITION_SIZE);
#if !defined(MCUBOOT_OVERWRITE_ONLY)
    LL_MCE_InitRegion(&aRegion[LL_MCE_REGION_SCRATCH], SCRATCH_START, SCRATCH_SIZE);
#endif /* not MCUBOOT_OVERWRITE_ONLY */

#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
    LL_MCE_InitRegion(&aRunRegion[LL_MCE_REGION_CODE_PRIMARY], PRIMARY_RUN_PARTITION_START, PRIMARY_PARTITION_SIZE);
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

    /* Enable illegal access interrupts */
    __HAL_MCE_ENABLE_IT(&hmce_primary, MCE_IT_ALL);

#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
    /* Enable illegal access interrupts */
    __HAL_MCE_ENABLE_IT(&hmce_primary_run, MCE_IT_ALL);
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

    /* Flow control */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MCE_I_CH1, FLOW_CTRL_MCE_I_CH1);
  }
  else
  {
    if ((((CODE_START & XSPI1_BASE) == XSPI1_BASE) &&
         (hmce_primary.Instance != MCE1)) ||
        (((CODE_START & XSPI2_BASE) == XSPI2_BASE) &&
         (hmce_primary.Instance != MCE2)) ||
        (hmce_primary.State != HAL_MCE_STATE_READY))
    {
      Error_Handler();
    }
    else
    {
      /* Control mce regions configurations */
      LL_MCE_ControlRegion(&aRegion[LL_MCE_REGION_CODE_PRIMARY], LL_MCE_REGION_CODE_PRIMARY,
                           PRIMARY_PARTITION_START, PRIMARY_PARTITION_SIZE);
      LL_MCE_ControlRegion(&aRegion[LL_MCE_REGION_CODE_SECONDARY], LL_MCE_REGION_CODE_SECONDARY,
                           SECONDARY_PARTITION_START, SECONDARY_PARTITION_SIZE);
#if !defined(MCUBOOT_OVERWRITE_ONLY)
      LL_MCE_ControlRegion(&aRegion[LL_MCE_REGION_SCRATCH], LL_MCE_REGION_SCRATCH,
                           SCRATCH_START, SCRATCH_SIZE);
#endif /* not MCUBOOT_OVERWRITE_ONLY */

#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
      LL_MCE_ControlRegion(&aRunRegion[LL_MCE_REGION_CODE_PRIMARY], LL_MCE_REGION_CODE_PRIMARY,
                           PRIMARY_RUN_PARTITION_START, PRIMARY_PARTITION_SIZE);
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

      /* Enable illegal access interrupts */
      __HAL_MCE_ENABLE_IT(&hmce_primary, MCE_IT_ALL);

#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
      /* Enable illegal access interrupts */
      __HAL_MCE_ENABLE_IT(&hmce_primary_run, MCE_IT_ALL);
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

      /* Flow control */
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MCE_I_CH2, FLOW_CTRL_MCE_I_CH2);
    }
  }
#endif /* OEMIROT_MCE_PROTECTION */
}

/**
  * @brief  Set MCE master keys
  * @param  FlowStep
  * @param  FlowCtrl
  * @retval None
  */
void LL_MCE_SetMasterKey(uint32_t FlowStep, uint32_t FlowCtrl)
{
#if defined(OEMIROT_MCE_PROTECTION)
  uint32_t a_Magic[4U] = {0X8C156679U, 0XD5FEB4AAU, 0X4B0B491BU, 0X2D27A250U};
  uint32_t a_nonce[2U] = {0X9B18FA51U, 0x054F1F29U};
  uint32_t version = 0x00004B11;
  uint32_t a_Key[4U] = { 0U };
  CRYP_HandleTypeDef hcryp = { 0U };
  uint32_t saes_timeout = SAES_DEFAULT_TIMEOUT;
  MCE_AESConfigTypeDef primary_config = { 0U };
#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
  uint32_t a_RunKey[4U] = { 0U };
  size_t length;
  size_t magic_length;
  MCE_NoekeonConfigTypeDef primary_run_config = { 0U };
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

  /* Check MCE cfg is not locked */
  if ((hmce_primary.Instance->CR & (MCE_CR_GLOCK | MCE_CR_MKLOCK)) != 0U)
  {
    Error_Handler();
  }
  if ((MCE_AES_CONTEXT->CCCFGR & MCE_CCCFGR_KEYLOCK) != 0U)
  {
    Error_Handler();
  }

#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
  /* Check MCE run cfg is not locked */
  if ((hmce_primary_run.Instance->CR & (MCE_CR_GLOCK | MCE_CR_MKLOCK)) != 0U)
  {
    Error_Handler();
  }
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

  /* Configure SAES parameters to encrypt the MCE magic, which will form the MCE key */
  /* Set the SAES parameters */
  hcryp.Instance            = SAES;
  hcryp.Init.DataType       = CRYP_NO_SWAP;
  hcryp.Init.Algorithm      = CRYP_AES_ECB;
  hcryp.Init.KeySelect      = CRYP_KEYSEL_AHK;
  hcryp.Init.KeyMode        = CRYP_KEYMODE_NORMAL;
  hcryp.Init.KeySize        = CRYP_KEYSIZE_256B;       /* 256 bits AES Key*/
  LL_SECU_ConfigureSAES(&hcryp, FLASH_KEY_LEVEL_CURRENT, saes_timeout, LL_MCE_AHK_MCE_AES_INDEX);

  if (HAL_CRYP_Encrypt(&hcryp, (uint32_t *)a_Magic, (uint16_t)(LL_MCE_MASTERKEYS_LENGTH / 4U), a_Key,
                       saes_timeout) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_CRYP_DeInit(&hcryp) != HAL_OK)
  {
    Error_Handler();
  }

#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
  /* Generate a random magic */
  length = sizeof(a_Magic);
  RNG_GetBytes((uint8_t *)a_Magic, length, &magic_length);
  if ((magic_length != length) || (magic_length == 0U))
  {
    Error_Handler();
  }

  LL_SECU_ConfigureSAES(&hcryp, FLASH_KEY_LEVEL_CURRENT, saes_timeout, LL_MCE_AHK_MCE_NOEKON_INDEX);

  if (HAL_CRYP_Encrypt(&hcryp, (uint32_t *)a_Magic, (uint16_t)(LL_MCE_MASTERKEYS_LENGTH / 4U), a_RunKey,
                       saes_timeout) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_CRYP_DeInit(&hcryp) != HAL_OK)
  {
    Error_Handler();
  }
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

  /* Generate a diversified nonce */
  primary_config.Nonce[0U] = a_nonce[0U];
  primary_config.Nonce[1U] = a_nonce[1U];
  /* Generate a diversified version (16 meaningful bits) */
  primary_config.Version = version;
  /* Set the key */
  primary_config.pKey = a_Key;

  if (HAL_MCE_ConfigAESContext(&hmce_primary, &primary_config, MCE_AES_CONTEXT_ID) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_MCE_EnableAESContext(&hmce_primary, MCE_AES_CONTEXT_ID) != HAL_OK)
  {
    Error_Handler();
  }

#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
  primary_run_config.KeyType = MCE_USE_MASTERKEYS;
  primary_run_config.pKey = a_RunKey;

  if (HAL_MCE_ConfigNoekeon(&hmce_primary_run, &primary_run_config) != HAL_OK)
  {
    Error_Handler();
  }
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

  FLOW_CONTROL_STEP(uFlowProtectValue, FlowStep, FlowCtrl);
#endif /* OEMIROT_MCE_PROTECTION */
}

/**
  * @brief  configure the MCE keys for the application
  * @param  FlowStep
  * @param  FlowCtrl
  * @retval None
  */
void LL_MCE_SetAppliMasterKey(uint32_t FlowStep, uint32_t FlowCtrl)
{
#if defined(OEMIROT_MCE_PROTECTION)

#if  (OEMIROT_LOAD_AND_RUN == NO_LOAD_AND_RUN)
  if (HAL_MCE_LockAESContextConfig(&hmce_primary, MCE_AES_CONTEXT_ID) != HAL_OK)
  {
    Error_Handler();
  }
#else /* OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN */
  /* Remove the master key for the primary slot */
  LL_MCE_RemoveKey(&hmce_primary);
#endif /* OEMIROT_LOAD_AND_RUN == NO_LOAD_AND_RUN */

#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
  if (HAL_MCE_LockNoekeonMasterKeys(&hmce_primary_run) != HAL_OK)
  {
    Error_Handler();
  }
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

  FLOW_CONTROL_STEP(uFlowProtectValue, FlowStep, FlowCtrl);
#endif /* OEMIROT_MCE_PROTECTION */
}

/**
  * @brief  configure the MCE keys for the loader
  * @param  FlowStep
  * @param  FlowCtrl
  * @retval None
  */
void LL_MCE_SetLoaderMasterKey(uint32_t FlowStep, uint32_t FlowCtrl)
{
#if defined(OEMIROT_MCE_PROTECTION)
  /* Remove the master key for the primary slot */
  LL_MCE_RemoveKey(&hmce_primary);
#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
  /* Disable MCE as there is no image to protect */
  LL_MCE_DeInit(&hmce_primary_run);
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

  FLOW_CONTROL_STEP(uFlowProtectValue, FlowStep, FlowCtrl);
#endif /* OEMIROT_MCE_PROTECTION */
}

/**
  * @brief  Check the configuration of the MCE master key
  * @param  FlowStep
  * @param  FlowCtrl
  * @retval None
  */
void LL_MCE_CheckMasterKey(uint32_t FlowStep, uint32_t FlowCtrl)
{
#if defined(OEMIROT_MCE_PROTECTION)
#if  (OEMIROT_LOAD_AND_RUN == NO_LOAD_AND_RUN)
  /* The key is configured */
  if ((MCE_AES_CONTEXT->CCCFGR & MCE_CCCFGR_KEYCRC) == 0U)
  {
    Error_Handler();
  }
#endif /* OEMIROT_LOAD_AND_RUN == NO_LOAD_AND_RUN */

#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
  /* The key is configured */
  if ((hmce_primary_run.Instance->SR & MCE_SR_MKVALID) != MCE_SR_MKVALID)
  {
    Error_Handler();
  }
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

  FLOW_CONTROL_STEP(uFlowProtectValue, FlowStep, FlowCtrl);
#endif /* OEMIROT_MCE_PROTECTION */
}

/**
  * @brief  Check the configuration of the "appli" MCE master key
  * @param  FlowStep
  * @param  FlowCtrl
  * @retval None
  */
void LL_MCE_CheckAppliMasterKey(uint32_t FlowStep, uint32_t FlowCtrl)
{
#if defined(OEMIROT_MCE_PROTECTION)
#if  (OEMIROT_LOAD_AND_RUN == NO_LOAD_AND_RUN)
  /* The key is configured */
  if ((MCE_AES_CONTEXT->CCCFGR & MCE_CCCFGR_KEYCRC) == 0U)
  {
    Error_Handler();
  }

  /* The key is locked */
  if ((MCE_AES_CONTEXT->CCCFGR & MCE_CCCFGR_KEYLOCK) != MCE_CCCFGR_KEYLOCK)
  {
    Error_Handler();
  }
#else /* OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN */
  /* The key isn't configured anymore */
  if ((MCE_AES_CONTEXT->CCCFGR & MCE_CCCFGR_KEYCRC) != 0U)
  {
    Error_Handler();
  }
#endif /* OEMIROT_LOAD_AND_RUN == NO_LOAD_AND_RUN */

#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
  /* The key is configured */
  if ((hmce_primary_run.Instance->CR & MCE_CR_MKLOCK) != MCE_CR_MKLOCK)
  {
    Error_Handler();
  }
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

  FLOW_CONTROL_STEP(uFlowProtectValue, FlowStep, FlowCtrl);
#endif /* OEMIROT_MCE_PROTECTION */
}

/**
  * @brief  Check the configuration of the "loader" MCE master key
  * @param  FlowStep
  * @param  FlowCtrl
  * @retval None
  */
void LL_MCE_CheckLoaderMasterKey(uint32_t FlowStep, uint32_t FlowCtrl)
{
#if defined(OEMIROT_MCE_PROTECTION)
  /* The key isn't configured anymore */
  if ((MCE_AES_CONTEXT->CCCFGR & MCE_CCCFGR_KEYCRC) != 0U)
  {
    Error_Handler();
  }

#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
  /* Noekeon context is no more configured */
  if ((hmce_primary_run.Instance->SR & MCE_SR_MKVALID) == MCE_SR_MKVALID)
  {
    Error_Handler();
  }
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

  FLOW_CONTROL_STEP(uFlowProtectValue, FlowStep, FlowCtrl);
#endif /* OEMIROT_MCE_PROTECTION */
}

/**
  * @brief  Check MCE regions
  * @param  Config MCE configuration to set
  * @param  RegionType region type (also used as index)
  * @param  FlowStep
  * @param  FlowCtrl
  * @retval HAL_OK if successful, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LL_MCE_CheckRegion(const uint8_t Config, const uint32_t RegionType, const uint32_t FlowStep,
                                     const uint32_t FlowCtrl)
{
#if defined(OEMIROT_MCE_PROTECTION)
  HAL_StatusTypeDef ret = HAL_ERROR;

  /* Check the parameter */
  assert_param(RegionType < ARRAY_SIZE(aRegion));

  if (LL_MCE_CheckMceConfiguration(&hmce_primary, aRegion, aRegionAccess[Config][RegionType], RegionType) != HAL_OK)
  {
    Error_Handler();
  }

  ret = HAL_OK;

  FLOW_CONTROL_STEP(uFlowProtectValue, FlowStep, FlowCtrl);

  return ret;
#else /* not OEMIROT_MCE_PROTECTION */
  return HAL_OK;
#endif /* OEMIROT_MCE_PROTECTION */
}

#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
/**
  * @brief  Check MCE run regions
  * @param  Config MCE configuration to set
  * @param  RegionType region type (also used as index)
  * @param  FlowStep
  * @param  FlowCtrl
  * @retval HAL_OK if successful, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LL_MCE_CheckRunRegion(const uint8_t Config, const uint32_t RegionType, const uint32_t FlowStep,
                                        const uint32_t FlowCtrl)
{
#if defined(OEMIROT_MCE_PROTECTION)
  HAL_StatusTypeDef ret = HAL_ERROR;

  /* Check the parameter */
  assert_param(RegionType < ARRAY_SIZE(aRunRegion));

  if (LL_MCE_CheckMceConfiguration(&hmce_primary_run, aRunRegion, aRunRegionAccess[Config][RegionType],
                                   RegionType) != HAL_OK)
  {
    Error_Handler();
  }

  ret = HAL_OK;

  FLOW_CONTROL_STEP(uFlowProtectValue, FlowStep, FlowCtrl);

  return ret;
#else /* not OEMIROT_MCE_PROTECTION */
  return HAL_OK;
#endif /* OEMIROT_MCE_PROTECTION */
}
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

/**
  * @brief  Check the status of the code primary region
  * @retval true if enabled, false otherwise.
  */
bool LL_MCE_IsCodePrimaryRegionEnabled(void)
{
  MCE_Region_TypeDef *p_region;
  __IO uint32_t address;

  address = (__IO uint32_t)((uint32_t)hmce_primary.Instance + 0x40U + (0x10U * LL_MCE_CODE_PRIMARY_REGION));
  p_region = (MCE_Region_TypeDef *)address;

  if ((p_region->REGCR & MCE_REGCR_BREN) == MCE_REGCR_BREN)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/**
  * @brief  Disable the MCE protection of the code primary region
  * @retval HAL_OK if successful, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LL_MCE_DisableCodePrimaryRegion(void)
{
  return HAL_MCE_DisableRegion(&hmce_primary, LL_MCE_CODE_PRIMARY_REGION);
}

/**
  * @brief  Enable the MCE protection of the code primary region
  * @retval HAL_OK if successful, HAL_ERROR otherwise.
  */
HAL_StatusTypeDef LL_MCE_EnableCodePrimaryRegion(void)
{
  return HAL_MCE_EnableRegion(&hmce_primary, LL_MCE_CODE_PRIMARY_REGION);
}

/**
  * @brief  Check whether the buffer is in the code primary region
  * @param  pBuffer address of the buffer
  * @param  Length length of the buffer
  * @retval 1 if enabled, 0 otherwise.
  */
bool LL_MCE_IsBufferInCodePrimaryRegion(const uint8_t *pBuffer, size_t Length)
{
  if ((((uint32_t)pBuffer) >= aRegion[LL_MCE_CODE_PRIMARY_REGION].StartAddress)
      && (((uint32_t)pBuffer) <= aRegion[LL_MCE_CODE_PRIMARY_REGION].EndAddress))
  {
    return true;
  }
  if ((((uint32_t)(pBuffer + Length)) >= aRegion[LL_MCE_CODE_PRIMARY_REGION].StartAddress)
      && (((uint32_t)(pBuffer + Length)) <= aRegion[LL_MCE_CODE_PRIMARY_REGION].EndAddress))
  {
    return true;
  }
  return false;
}

/**
  * @brief  Set MCE region
  * @param  Config MCE configuration to set
  * @param  Index region index
  * @param  FlowStep
  * @param  FlowCtrl
  * @retval None
  */
void LL_MCE_SetRegion(const uint8_t Config, const uint8_t Index, const uint32_t FlowStep, const uint32_t FlowCtrl)
{
#if defined(OEMIROT_MCE_PROTECTION)
  /* Check the parameters */
  assert_param(Index < ARRAY_SIZE(aRegion));
  assert_param(Config < LL_MCE_REGION_CFG_END);

  LL_MCE_ConfigRegion(&hmce_primary, &aRegion[Index], Index, aRegionAccess[Config][Index]);

  FLOW_CONTROL_STEP(uFlowProtectValue, FlowStep, FlowCtrl);
#endif /* OEMIROT_MCE_PROTECTION */
}

#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
/**
  * @brief  Set MCE run region
  * @param  Config MCE configuration to set
  * @param  Index region index
  * @param  FlowStep
  * @param  FlowCtrl
  * @retval None
  */
void LL_MCE_SetRunRegion(const uint8_t Config, const uint8_t Index, const uint32_t FlowStep, const uint32_t FlowCtrl)
{
#if defined(OEMIROT_MCE_PROTECTION)
  /* Check the parameters */
  assert_param(Index < ARRAY_SIZE(aRunRegion));
  assert_param(Config < LL_MCE_REGION_CFG_END);

  LL_MCE_ConfigRegion(&hmce_primary_run, &aRunRegion[Index], Index, aRunRegionAccess[Config][Index]);

  FLOW_CONTROL_STEP(uFlowProtectValue, FlowStep, FlowCtrl);
#endif /* OEMIROT_MCE_PROTECTION */
}
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

/**
  * @brief  Lock MCE regions (application configuration)
  * @param  FlowStep
  * @param  FlowCtrl
  * @retval None
  */
void LL_MCE_LockAppliRegions(const uint32_t FlowStep, const uint32_t FlowCtrl)
{
#if defined(OEMIROT_MCE_PROTECTION)
  LL_MCE_LockGlobalConfig(&hmce_primary);

#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
  LL_MCE_LockGlobalConfig(&hmce_primary_run);
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

  FLOW_CONTROL_STEP(uFlowProtectValue, FlowStep, FlowCtrl);
#endif /* OEMIROT_MCE_PROTECTION */
}

/**
  * @brief  Lock MCE regions (loader configuration)
  * @param  FlowStep
  * @param  FlowCtrl
  * @retval None
  */
void LL_MCE_LockLoaderRegions(const uint32_t FlowStep, const uint32_t FlowCtrl)
{
#if defined(OEMIROT_MCE_PROTECTION)
  LL_MCE_LockGlobalConfig(&hmce_primary);

  FLOW_CONTROL_STEP(uFlowProtectValue, FlowStep, FlowCtrl);
#endif /* OEMIROT_MCE_PROTECTION */
}

/**
  * @brief  Check the lock of MCE regions (application configuration)
  * @param  FlowStep
  * @param  FlowCtrl
  * @retval None
  */
void LL_MCE_CheckLockAppliRegions(const uint32_t FlowStep, const uint32_t FlowCtrl)
{
#if defined(OEMIROT_MCE_PROTECTION)
  LL_MCE_LockGlobalConfig(&hmce_primary);
#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
  LL_MCE_LockGlobalConfig(&hmce_primary_run);
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

  FLOW_CONTROL_STEP(uFlowProtectValue, FlowStep, FlowCtrl);
#endif /* OEMIROT_MCE_PROTECTION */
}

/**
  * @brief  Check the lock of MCE regions (loader configuration)
  * @param  FlowStep
  * @param  FlowCtrl
  * @retval None
  */
void LL_MCE_CheckLockLoaderRegions(const uint32_t FlowStep, const uint32_t FlowCtrl)
{
#if defined(OEMIROT_MCE_PROTECTION)
  LL_MCE_LockGlobalConfig(&hmce_primary);

  FLOW_CONTROL_STEP(uFlowProtectValue, FlowStep, FlowCtrl);
#endif /* OEMIROT_MCE_PROTECTION */
}

/**
  * @brief  DeInit MCE IP
  * @param  hmce pointer on mce handle
  * @retval None
  */
void LL_MCE_DeInit(MCE_HandleTypeDef *hmce)
{
#if defined(OEMIROT_MCE_PROTECTION)
  /* Remove the key */
  LL_MCE_RemoveKey(hmce);

  /* Disable the MCE peripheral */
  (void) HAL_MCE_DeInit(hmce);
#endif /* OEMIROT_MCE_PROTECTION */
}

#if defined(OEMIROT_MCE_PROTECTION)
/**
  * @brief  Remove the MCE key
  * @param  hmce pointer on mce handle
  * @retval None
  */
void LL_MCE_RemoveKey(MCE_HandleTypeDef *hmce)
{
  /* Remove the keys */
  if (hmce->Instance == MCE1)
  {
    MCE_AES_CONTEXT->CCKEYR3 = 0U;
  }
  else if (hmce->Instance == MCE2)
  {
    hmce->Instance->MKEYR3 = 0U;
  }
  else
  {
    /* Do nothing */
  }
}

/**
  * @brief  Initialize the right MCE instance depending on the slot address
  * @param  hmce pointer on mce handle
  * @param  SlotAddress address of the slot
  * @retval HAL status
  */
static HAL_StatusTypeDef LL_MCE_Initialize(MCE_HandleTypeDef *hmce, uint32_t SlotAddress)
{
  HAL_StatusTypeDef status = HAL_ERROR;

  /* Reset the instance */
  hmce->Instance = NULL;

  /* Initialize MCE instance */
  /* MCE1 is mapped on XSPI1 */
  /* MCE2 is mapped on XSPI2 */
  if ((SlotAddress & XSPI1_BASE) == XSPI1_BASE)
  {
    /* MCE Peripheral clock enable */
    __HAL_RCC_XSPI1_CLK_ENABLE();

    /* Reset the XSPI memory interface */
    __HAL_RCC_XSPI1_FORCE_RESET();
    __HAL_RCC_XSPI1_RELEASE_RESET();

    hmce->Instance = MCE1;
  }
  else if ((SlotAddress & XSPI2_BASE) == XSPI2_BASE)
  {
    /* MCE Peripheral clock enable */
    __HAL_RCC_XSPI2_CLK_ENABLE();

    /* Reset the XSPI memory interface */
    __HAL_RCC_XSPI2_FORCE_RESET();
    __HAL_RCC_XSPI2_RELEASE_RESET();

    hmce->Instance = MCE2;
  }
  else
  {
    /* Do nothing */
  }

  if ((hmce->Instance == MCE1) || (hmce->Instance == MCE2))
  {
    __HAL_MCE_RESET_HANDLE_STATE(hmce);
    status = HAL_MCE_Init(hmce);
  }

  return status;
}

/**
  * @brief  Initialize a MCE region
  * @param  pRegionConfig region configuration to apply
  * @param  Offset start address of the region
  * @param  RegionSize size of the region
  * @retval None
  */
static void LL_MCE_InitRegion(MCE_RegionConfigTypeDef *pRegionConfig, const uint32_t Offset, const uint32_t RegionSize)
{
  pRegionConfig->StartAddress = Offset;
  pRegionConfig->EndAddress = Offset + RegionSize - 1U;
}

/**
  * @brief  Configure a MCE region
  * @param  hmce MCE handler
  * @param  pRegionConfig region configuration to apply
  * @param  RegionType region type (also used as index)
  * @param  AccessMode MCE access mode
  * @retval None
  */
static void LL_MCE_ConfigRegion(MCE_HandleTypeDef *hmce,   MCE_RegionConfigTypeDef *pRegionConfig,
                                const uint32_t RegionType, const uint32_t AccessMode)
{
  if ((RegionType != LL_MCE_REGION_CODE_SECONDARY) && (pRegionConfig->Mode != MCE_BLOCK_CIPHER))
  {
    Error_Handler();
  }

  /* NB: RegionType is used as index */
  if (AccessMode != LL_MCE_REGION_DISABLED)
  {
    pRegionConfig->AccessMode = AccessMode;
    if (HAL_MCE_ConfigRegion(hmce, RegionType, pRegionConfig) != HAL_OK)
    {
      Error_Handler();
    }
    /* XSPI1 + MCE1 (AES) must be used with the external flash */
    if (hmce->Instance == MCE1)
    {
      if (HAL_MCE_SetRegionAESContext(hmce, MCE_AES_CONTEXT_ID, RegionType) != HAL_OK)
      {
        Error_Handler();
      }
    }
  }
  else if (LL_MCE_GetRegionStatus(hmce, RegionType) == MCE_REGCR_BREN)
  {
    (void)HAL_MCE_DisableRegion(hmce, RegionType);
  }
  else
  {
    /* Region is already disabled */
  }
}

/**
  * @brief  Control a MCE region
  * @param  pRegionConfig region configuration
  * @param  RegionType region type (also used as index)
  * @param  Offset start address of the region
  * @param  RegionSize size of the region
  * @retval None
  */
static void LL_MCE_ControlRegion(MCE_RegionConfigTypeDef *pRegionConfig, const uint32_t RegionType,
                                 const uint32_t Offset,                  const uint32_t RegionSize)
{
  if ((RegionType != LL_MCE_REGION_CODE_SECONDARY) && (pRegionConfig->Mode != MCE_BLOCK_CIPHER))
  {
    Error_Handler();
  }

  if ((pRegionConfig->StartAddress != Offset) ||
      (pRegionConfig->EndAddress != (Offset + RegionSize - 1U)))
  {
    Error_Handler();
  }
}

/**
  * @brief  Check if a region is enabled or not
  * @param  hmce pointer on mce handle
  * @param  pRegion pointer on mce configurations
  * @param  pRegionRw pointer on mce rw accesses
  * @param  RegionType region type (also used as index)
  * @retval HAL status
  */
static HAL_StatusTypeDef LL_MCE_CheckMceConfiguration(MCE_HandleTypeDef *hmce,     MCE_RegionConfigTypeDef *pRegion,
                                                      const uint32_t RegionAccess, const uint8_t RegionType)
{
  MCE_RegionConfigTypeDef mceReadConfig = { 0U };
  uint32_t ext_mem_base = 0U;
  __IO uint32_t address;
  MCE_Region_TypeDef *p_region;
  HAL_StatusTypeDef status = HAL_ERROR;

  if ((RegionType != LL_MCE_REGION_CODE_SECONDARY) && (pRegion->Mode != MCE_BLOCK_CIPHER))
  {
    Error_Handler();
  }

  /* Check the address */
  if ((pRegion[RegionType].StartAddress & XSPI1_BASE) == XSPI1_BASE)
  {
    ext_mem_base = XSPI1_BASE;
  }
  else if ((pRegion[RegionType].StartAddress & XSPI2_BASE) == XSPI2_BASE)
  {
    ext_mem_base = XSPI2_BASE;
  }
  else
  {
    /* Do nothing */
  }

  if (ext_mem_base != 0U)
  {
    if (((hmce->Instance == MCE1) && (ext_mem_base == XSPI1_BASE))
        || ((hmce->Instance == MCE2) && (ext_mem_base == XSPI2_BASE)))
    {
      /* The region is enabled*/
      if ((LL_MCE_GetRegionStatus(hmce, RegionType) == MCE_REGCR_BREN) &&
          (RegionAccess != LL_MCE_REGION_DISABLED))
      {
        /* Take Lock */
        __HAL_LOCK(hmce);

        address = (__IO uint32_t)((uint32_t)hmce->Instance + 0x40U + (0x10U * RegionType));
        p_region = (MCE_Region_TypeDef *)address;

        mceReadConfig.PrivilegedAccess = READ_BIT(p_region->REGCR, MCE_REGCR_PRIV);
        mceReadConfig.Mode = READ_BIT(p_region->REGCR, MCE_REGCR_ENC);
        mceReadConfig.ContextID = READ_BIT(p_region->REGCR, MCE_REGCR_CTXID);
        mceReadConfig.AccessMode = READ_BIT(p_region->ATTR, MCE_ATTR_WREN);

        mceReadConfig.StartAddress = READ_REG(p_region->SADDR);
        mceReadConfig.EndAddress   = READ_REG(p_region->EADDR);

        /* Release Lock */
        __HAL_UNLOCK(hmce);

        /* Check the configuration */
        if ((mceReadConfig.AccessMode == pRegion[RegionType].AccessMode) &&
            ((mceReadConfig.EndAddress) == (pRegion[RegionType].EndAddress - ext_mem_base)) &&
            ((mceReadConfig.StartAddress) == (pRegion[RegionType].StartAddress - ext_mem_base)) &&
            (mceReadConfig.ContextID == pRegion[RegionType].ContextID) &&
            (mceReadConfig.Mode == pRegion[RegionType].Mode) &&
            (mceReadConfig.PrivilegedAccess == pRegion[RegionType].PrivilegedAccess))
        {
          status = HAL_OK;
        }
      }
      /* The region is disabled as expected */
      else if ((LL_MCE_GetRegionStatus(hmce, RegionType) == LL_MCE_REGION_DISABLED) &&
               (RegionAccess == LL_MCE_REGION_DISABLED))
      {
        status = HAL_OK;
      }
      else
      {
        /* Nothing to do */
      }
    }
  }

  return status;
}

/**
  * @brief  Check if a region is enabled or not
  * @param  hmce pointer on mce handle
  * @param  RegionIndex index of the region
  * @retval MCE_REGCR_BREN if the region is enabled, else LL_MCE_REGION_DISABLED
  */
static uint32_t LL_MCE_GetRegionStatus(MCE_HandleTypeDef *hmce, uint32_t RegionIndex)
{
  uint8_t ret = 0U;
  uint32_t address = 0U;
  MCE_Region_TypeDef *pRegion = 0U;

  address  = (__IO uint32_t)((uint32_t)hmce->Instance + 0x40U + (0x10U * RegionIndex));
  pRegion = (MCE_Region_TypeDef *)address ;

  /* Take Lock */
  __HAL_LOCK(hmce);

  /* Check region is enabled, else error */
  if ((pRegion->REGCR & MCE_REGCR_BREN) == MCE_REGCR_BREN)
  {
    ret = MCE_REGCR_BREN;
  }
  else
  {
    ret = LL_MCE_REGION_DISABLED;
  }

  /* Release Lock */
  __HAL_UNLOCK(hmce);

  return ret;
}

/**
  * @brief  Lock MCE global configuration
  * @param  hmce pointer on mce handle
  * @retval None
  */
static void LL_MCE_LockGlobalConfig(MCE_HandleTypeDef *hmce)
{
  if ((HAL_MCE_LockGlobalConfig(hmce) != HAL_OK) &&
      ((hmce->Instance->CR & MCE_CR_GLOCK) != MCE_CR_GLOCK))
  {
    Error_Handler();
  }
}
#endif /* OEMIROT_MCE_PROTECTION */
#endif /* STM32H7S3xx || STM32H7S7xx */
