/**
  ******************************************************************************
  * @file    low_level_security.c
  * @author  MCD Application Team
  * @brief   security protection implementation for secure boot on STM32H7RSxx
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
#include <string.h>
#include "boot_hal_cfg.h"
#include "boot_hal_flowcontrol.h"
#include "region_defs.h"
#include "mcuboot_config/mcuboot_config.h"
#include "low_level_security.h"
#include "low_level_obkeys.h"
#if defined(STM32H7S3xx) || defined(STM32H7S7xx)
#include "low_level_mce.h"
#endif /* STM32H7S3xx || STM32H7S7xx */
#include "low_level_ramecc.h"
#ifdef OEMIROT_DEV_MODE
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_INFO
#else
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_OFF
#endif /* OEMIROT_DEV_MODE  */
#include "bootutil/bootutil_log.h"
#include "low_level_rng.h"
#include "target_cfg.h"
#include "bootutil_priv.h"

/* Private typedef -----------------------------------------------------------*/
/** @defgroup OEMIROT_SECURITY_Private_TypeDefs Private Type Definitions
  * @{
  */


/**
  * @brief MPU configuration structure definition
  */
typedef struct
{
  uint8_t  Number;           /*!< Specifies the number of the region to protect. This parameter can be a
                                  value of CORTEX_MPU_Region_Number */
  uint32_t BaseAddress;      /*!< Specifies the base address of the region to protect. */
  uint8_t  Size;             /*!< Specifies the size of the region to protect. */
  uint8_t  SubRegionDisable; /*!< Specifies the sub region field (region is divided in 8 slices) when bit
                                  is 1 region sub region is disabled */
  uint8_t  AccessPermission; /*!< Specifies the region access permission type. This parameter can be a
                                  value of CORTEX_MPU_Region_Permission_Attributes */
  uint8_t  DisableExec;      /*!< Specifies the instruction access status. This parameter can be a value
                                  of  CORTEX_MPU_Instruction_Access */
  uint8_t  Shareable;        /*!< Specifies the shareable value */
  uint8_t  Cacheable;        /*!< Specifies the cacheable value */
  uint8_t  Bufferable;       /*!< Specifies the bufferable value */
  uint8_t  Tex;              /*!< Specifies the tex value */
#ifdef FLOW_CONTROL
  uint32_t flow_step_enable; /*!< Flow control step (stage 1) */
  uint32_t flow_ctrl_enable; /*!< Flow control (stage 1) */
  uint32_t flow_step_check;  /*!< Flow control step (stage 2)  */
  uint32_t flow_ctrl_check;  /*!< Flow control (stage 2) */
#endif /* FLOW_CONTROL */
} OEMIROT_MPU_InitTypeDef;

/**
  * @}
  */

/* Private define ------------------------------------------------------------*/
/** @defgroup OEMIROT_SECURITY_Private_Defines  Private Defines
  * @{
  */
/* DUAL BANK page size */
#define PAGE_SIZE     FLASH_SECTOR_SIZE
#define HDP_PAGE_SIZE FLASH_AREA_HDP_GROUP_SIZE
#define WRP_PAGE_SIZE FLASH_AREA_WRP_GROUP_SIZE

#define PAGE_MAX_NUMBER_IN_BANK 0x7

/* OEMIROT_Boot Vector Address  */
#define OEMIROT_BOOT_VTOR_ADDR ((uint32_t)(BL2_CODE_START))

/**************************
  * Initial configuration *
  *************************/

/* MPU configuration
  ================== */
const OEMIROT_MPU_InitTypeDef region_cfg_init[] =
{
  /* Region 0: Allows execution of BL2 ("Code") */
  {
    MPU_REGION_NUMBER0,
#if  defined(OEMUROT_ENABLE)
    BL2_RAM_BASE,
    MPU_REGION_SIZE_128KB,            /* BL2_CODE_SIZE */
#else /* OEMUROT_ENABLE */
    FLASH_BASE + FLASH_AREA_BL2_OFFSET,
    MPU_REGION_SIZE_64KB,             /* BL2_CODE_SIZE */
#endif /* OEMUROT_ENABLE */
    0x00U,                            /* All subregions activated */
    MPU_REGION_PRIV_RO,
    MPU_INSTRUCTION_ACCESS_ENABLE,
    MPU_ACCESS_NOT_SHAREABLE,
    MPU_ACCESS_CACHEABLE,
    MPU_ACCESS_NOT_BUFFERABLE,
    MPU_TEX_LEVEL0,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R0,
    FLOW_CTRL_MPU_I_EN_R0,
    FLOW_STEP_MPU_I_CH_R0,
    FLOW_CTRL_MPU_I_CH_R0,
#endif /* FLOW_CONTROL */
  },
#ifndef MCUBOOT_OVERWRITE_ONLY
  /* Region 1: Allows RW access to scratch area ("Data No Cache") */
  {
    MPU_REGION_NUMBER1,
    EXT_FLASH_BASE_ADDRESS + FLASH_AREA_SCRATCH_OFFSET,
    MPU_REGION_SIZE_64KB,             /* FLASH_AREA_SCRATCH_SIZE */
    0x00U,                            /* All subregions activated */
    MPU_REGION_PRIV_RW,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_SHAREABLE,
    MPU_ACCESS_NOT_CACHEABLE,
    MPU_ACCESS_NOT_BUFFERABLE,
    MPU_TEX_LEVEL0,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R1,
    FLOW_CTRL_MPU_I_EN_R1,
    FLOW_STEP_MPU_I_CH_R1,
    FLOW_CTRL_MPU_I_CH_R1,
#endif /* FLOW_CONTROL */
  },
#endif /* MCUBOOT_OVERWRITE_ONLY */
  /* Region 2: Allows RW access to all slots areas ("Data No Cache") */
  {
    MPU_REGION_NUMBER2,
    EXT_FLASH_BASE_ADDRESS + FLASH_AREA_BEGIN_OFFSET,
#if (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM)
    MPU_REGION_SIZE_256KB,            /* FLASH_AREA_END_OFFSET - FLASH_AREA_BEGIN_OFFSET */
    0x00U,                            /* All subregions activated */
#else /* not (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM) */
    MPU_REGION_SIZE_4MB,              /* FLASH_AREA_END_OFFSET - FLASH_AREA_BEGIN_OFFSET */
    0x00U,                            /* All subregions activated */
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM */
    MPU_REGION_PRIV_RW,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_SHAREABLE,
    MPU_ACCESS_NOT_CACHEABLE,
    MPU_ACCESS_NOT_BUFFERABLE,
    MPU_TEX_LEVEL0,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R2,
    FLOW_CTRL_MPU_I_EN_R2,
    FLOW_STEP_MPU_I_CH_R2,
    FLOW_CTRL_MPU_I_CH_R2,
#endif /* FLOW_CONTROL */
  },
#if defined(OEMIROT_MCE_PROTECTION)
  /* Region 5: Allows RW access to the shared DMA buffer */
  {
    MPU_REGION_NUMBER5,
    BL2_DMA_SHARED_MEM_START,
    MPU_REGION_SIZE_256B,             /* BL2_DMA_SHARED_MEM_SIZE */
    0x00U,                            /* All subregions activated */
    MPU_REGION_PRIV_RW,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_NOT_SHAREABLE,
    MPU_ACCESS_NOT_CACHEABLE,
    MPU_ACCESS_BUFFERABLE,
    MPU_TEX_LEVEL1,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R5,
    FLOW_CTRL_MPU_I_EN_R5,
    FLOW_STEP_MPU_I_CH_R5,
    FLOW_CTRL_MPU_I_CH_R5,
#endif /* FLOW_CONTROL */
  },
#endif /* OEMIROT_MCE_PROTECTION */
  /* Region 6: Allows RW access to BL2 SRAM ("Data") */
  {
    MPU_REGION_NUMBER6,
    BL2_SRAM_AREA_BASE,
    MPU_REGION_SIZE_64KB,             /* BL2_SRAM_AREA_END - BL2_SRAM_AREA_BASE + 1 */
    0x00U,                            /* All subregions activated */
    MPU_REGION_PRIV_RW,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_NOT_SHAREABLE,
    MPU_ACCESS_CACHEABLE,
    MPU_ACCESS_BUFFERABLE,
    MPU_TEX_LEVEL1,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R6,
    FLOW_CTRL_MPU_I_EN_R6,
    FLOW_STEP_MPU_I_CH_R6,
    FLOW_CTRL_MPU_I_CH_R6,
#endif /* FLOW_CONTROL */
  },
  /* Region 8: Allows RW access to peripherals ("Device") */
  {
    MPU_REGION_NUMBER8,
    PERIPH_BASE,
    MPU_REGION_SIZE_512MB,
    0x00U,                            /* All subregions activated */
    MPU_REGION_PRIV_RW,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_NOT_SHAREABLE,
    MPU_ACCESS_NOT_CACHEABLE,
    MPU_ACCESS_BUFFERABLE,
    MPU_TEX_LEVEL0,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R8,
    FLOW_CTRL_MPU_I_EN_R8,
    FLOW_STEP_MPU_I_CH_R8,
    FLOW_CTRL_MPU_I_CH_R8,
#endif /* FLOW_CONTROL */
  },
  /* Region 10: Allows read access to System Flash ("Data No Cache") */
  {
    MPU_REGION_NUMBER10,
    SYSTEM_FLASH_BASE,
    MPU_REGION_SIZE_128KB,            /* System Flash */
    0x00U,                            /* All subregions activated */
    MPU_REGION_PRIV_RO,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_NOT_SHAREABLE,
    MPU_ACCESS_NOT_CACHEABLE,
    MPU_ACCESS_NOT_BUFFERABLE,
    MPU_TEX_LEVEL1,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R10,
    FLOW_CTRL_MPU_I_EN_R10,
    FLOW_STEP_MPU_I_CH_R10,
    FLOW_CTRL_MPU_I_CH_R10,
#endif /* FLOW_CONTROL */
  },
  /* Region 11: Allows execution of RSSLIB ("Code") */
  {
    MPU_REGION_NUMBER11,
    0x1FF16000UL,
    MPU_REGION_SIZE_8KB,
    0x03U,                            /*!< 8 Kbytes / 8 * 6 ==> 6 Kbytes */
    MPU_REGION_PRIV_RO,
    MPU_INSTRUCTION_ACCESS_ENABLE,
    MPU_ACCESS_NOT_SHAREABLE,
    MPU_ACCESS_CACHEABLE,
    MPU_ACCESS_NOT_BUFFERABLE,
    MPU_TEX_LEVEL0,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R11,
    FLOW_CTRL_MPU_I_EN_R11,
    FLOW_STEP_MPU_I_CH_R11,
    FLOW_CTRL_MPU_I_CH_R11,
#endif /* FLOW_CONTROL */
  },
  /* Region 12: Disable access to HPDMA */
  {
    MPU_REGION_NUMBER12,
    HPDMA1_BASE,
    MPU_REGION_SIZE_8KB,
    0x00U,                            /* All subregions activated */
    MPU_REGION_NO_ACCESS,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_NOT_SHAREABLE,
    MPU_ACCESS_NOT_CACHEABLE,
    MPU_ACCESS_BUFFERABLE,
    MPU_TEX_LEVEL0,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R12,
    FLOW_CTRL_MPU_I_EN_R12,
    FLOW_STEP_MPU_I_CH_R12,
    FLOW_CTRL_MPU_I_CH_R12,
#endif /* FLOW_CONTROL */
  },
  /* Region 13: Allows RW access to the RAMECC handles,
    * confirmation flag and .noinit ("Data No Cache")
    * NB: The appli does not use .noinit but no smaller MPU region
    * can be applied (no reconfiguration done for appli) */
  {
    MPU_REGION_NUMBER13,
    DATA_NOINIT_START,
    MPU_REGION_SIZE_128B,
    0x00U,                            /* Less than 256B: no subregions */
    MPU_REGION_PRIV_RW,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_NOT_SHAREABLE,
    MPU_ACCESS_NOT_CACHEABLE,
    MPU_ACCESS_NOT_BUFFERABLE,
    MPU_TEX_LEVEL1,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R13,
    FLOW_CTRL_MPU_I_EN_R13,
    FLOW_STEP_MPU_I_CH_R13,
    FLOW_CTRL_MPU_I_CH_R13,
#endif /* FLOW_CONTROL */
  },
#if (OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN)
  /* Region 14: Allows RW access to the "Run" partition */
  {
    MPU_REGION_NUMBER14,
    PRIMARY_RUN_PARTITION_START,
#if (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM)
    MPU_REGION_SIZE_128KB,            /* FLASH_PARTITION_SIZE */
    0x00U,                            /* All subregions activated */
#else /* not (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM) */
    MPU_REGION_SIZE_2MB,              /* FLASH_PARTITION_SIZE */
    0x00U,                            /* All subregions activated */
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM */
    MPU_REGION_PRIV_RW,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_SHAREABLE,
    MPU_ACCESS_NOT_CACHEABLE,
    MPU_ACCESS_NOT_BUFFERABLE,
    MPU_TEX_LEVEL0,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R14,
    FLOW_CTRL_MPU_I_EN_R14,
    FLOW_STEP_MPU_I_CH_R14,
    FLOW_CTRL_MPU_I_CH_R14,
#endif /* FLOW_CONTROL */
  },
#endif /* OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN */
};

const OEMIROT_MPU_InitTypeDef region_cfg_appli[] =
{
  /* First region in this list is configured only at this stage, */
  /* the region will be activated later by NSS jump service. Following regions */
  /*  in this list are configured and activated at this stage. */

  /* Region 14: Allows execution of appli ("Data") */
  {
    MPU_REGION_APP,
#if  (OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN)
    PRIMARY_RUN_PARTITION_START,
#else /* not (OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN) */
    EXT_FLASH_BASE_ADDRESS + IMAGE_PRIMARY_PARTITION_OFFSET,
#endif /* OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN */
#if (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM)
    MPU_REGION_SIZE_128KB,            /* FLASH_PARTITION_SIZE */
    0x00U,                            /* All subregions activated */
#else /* not (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM) */
    MPU_REGION_SIZE_2MB,              /* FLASH_PARTITION_SIZE */
    0x00U,                            /* All subregions activated */
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM */
    MPU_REGION_PRIV_RO,
    MPU_INSTRUCTION_ACCESS_ENABLE,
    MPU_ACCESS_SHAREABLE,
    MPU_ACCESS_CACHEABLE,
    MPU_ACCESS_BUFFERABLE,
    MPU_TEX_LEVEL1,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_A_EN_R14,
    FLOW_CTRL_MPU_A_EN_R14,
    FLOW_STEP_MPU_A_CH_R14,
    FLOW_CTRL_MPU_A_CH_R14,
#endif /* FLOW_CONTROL */
  },
  /* Region 1: Allows RW access to the external flash */
  {
    MPU_REGION_NUMBER1,
    EXT_FLASH_BASE_ADDRESS,
    MPU_REGION_SIZE_128MB,
    0x00U,                            /* All subregions activated */
    MPU_REGION_FULL_ACCESS,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_SHAREABLE,
    MPU_ACCESS_NOT_CACHEABLE,
    MPU_ACCESS_NOT_BUFFERABLE,
    MPU_TEX_LEVEL0,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_A_EN_R1,
    FLOW_CTRL_MPU_A_EN_R1,
    FLOW_STEP_MPU_A_CH_R1,
    FLOW_CTRL_MPU_A_CH_R1,
#endif /* FLOW_CONTROL */
  },
  /* Region 2: Allows RW access to the external ram */
  {
    MPU_REGION_NUMBER2,
    EXT_RAM_BASE_ADDRESS,
    MPU_REGION_SIZE_32MB,
    0x00U,                            /* All subregions activated */
    MPU_REGION_FULL_ACCESS,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_SHAREABLE,
    MPU_ACCESS_NOT_CACHEABLE,
    MPU_ACCESS_NOT_BUFFERABLE,
    MPU_TEX_LEVEL0,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_A_EN_R2,
    FLOW_CTRL_MPU_A_EN_R2,
    FLOW_STEP_MPU_A_CH_R2,
    FLOW_CTRL_MPU_A_CH_R2,
#endif /* FLOW_CONTROL */
  },
  /* Region 5: Allows RW access to appli SRAM ("Data") */
  {
    MPU_REGION_NUMBER5,
    APP_RAM_START,
    MPU_REGION_SIZE_16KB,             /* See APP_RAM_SIZE */
    0x00U,                            /* All subregions activated */
    MPU_REGION_PRIV_RW,
    MPU_INSTRUCTION_ACCESS_DISABLE,
    MPU_ACCESS_NOT_SHAREABLE,
    MPU_ACCESS_CACHEABLE,
    MPU_ACCESS_BUFFERABLE,
    MPU_TEX_LEVEL1,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_A_EN_R5,
    FLOW_CTRL_MPU_A_EN_R5,
    FLOW_STEP_MPU_A_CH_R5,
    FLOW_CTRL_MPU_A_CH_R5,
#endif /* FLOW_CONTROL */
  },
};

/* Product state control
   ===================== */
static const uint32_t ProductStatePrioList[] =
{
  OEMIROT_PROD_STATE_OPEN,
  OEMIROT_PROD_STATE_PROVISIONING,
  OEMIROT_PROD_STATE_PROVISIONED,
  OEMIROT_PROD_STATE_LOCKED,
  OEMIROT_PROD_STATE_CLOSED_ECDSA,
  OEMIROT_PROD_STATE_CLOSED_PWD
};

#define NB_PRODUCT_STATE (sizeof(ProductStatePrioList) / sizeof(uint32_t))

/* MPU configuration
   ================= */
#if defined(OEMIROT_JUMP_TO_BL_ENABLE)
static const OEMIROT_MPU_InitTypeDef region_cfg_loader[] =
{
  /* Region 7: Allow access to the bootloader ("Code")  */
  {
    MPU_REGION_BL,
    BOOTLOADER_BASE,
    MPU_REGION_SIZE_32KB,
    0x00U,                            /* All subregions activated */
    MPU_REGION_PRIV_RO,
    MPU_INSTRUCTION_ACCESS_ENABLE,
    MPU_ACCESS_NOT_SHAREABLE,
    MPU_ACCESS_CACHEABLE,
    MPU_ACCESS_NOT_BUFFERABLE,
    MPU_TEX_LEVEL0,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_L_EN_R7,
    FLOW_CTRL_MPU_L_EN_R7,
    FLOW_STEP_MPU_L_CH_R7,
    FLOW_CTRL_MPU_L_CH_R7,
#endif /* FLOW_CONTROL */
  },
};
#endif /* OEMIROT_JUMP_TO_BL_ENABLE */

/**
  * @}
  */
/* Private function prototypes -----------------------------------------------*/
/** @defgroup OEMIROT_SECURITY_Private_Functions  Private Functions
  * @{
  */
#ifdef OEMIROT_MPU_PROTECTION
static void mpu_configure_region(const OEMIROT_MPU_InitTypeDef *MpuRegionCfg, uint8_t Enable);
static HAL_StatusTypeDef mpu_check_region_config(const OEMIROT_MPU_InitTypeDef *MpuRegionCfg, uint8_t Enable);
#endif /* OEMIROT_MPU_PROTECTION */
static void mpu_init_cfg(void);
static void mpu_appli_cfg(void);
#if defined(MCUBOOT_EXT_LOADER)
static void mpu_loader_cfg(void);
#endif /* MCUBOOT_EXT_LOADER */
static void active_tamper(void);
static void mce_oemirot_cfg(void);
#if defined(MCUBOOT_EXT_LOADER)
static void mce_local_loader_cfg(void);
#endif /* MCUBOOT_EXT_LOADER */
static void mce_appli_cfg(void);

/**
  * @}
  */

/** @defgroup OEMIROT_SECURITY_Exported_Functions Exported Functions
  * @{
  */
#if defined(MCUBOOT_EXT_LOADER)
/**
  * @brief  Update the runtime security protections for application start
  *
  * @param  None
  * @retval None
  */
void LL_SECU_UpdateLoaderRunTimeProtections(void)
{
  /* Set MPU to enable execution of secure /non secure  loader */
  mpu_loader_cfg();

  /* Configure MCE*/
  mce_local_loader_cfg();
}
#endif /* MCUBOOT_EXT_LOADER */
/**
  * @brief  Apply the runtime security  protections to
  *
  * @param  None
  * @note   By default, the best security protections are applied
  * @retval None
  */
void LL_SECU_ApplyRunTimeProtections(void)
{
#if defined(OEMIROT_USER_SRAM_ECC)
  /* Init RAMECC peripheral */
  LL_RAMECC_Configure();

#endif /* OEMIROT_USER_SRAM_ECC */
  /* Init MCE peripheral */
#if defined(STM32H7S3xx) || defined(STM32H7S7xx)
  LL_MCE_Configure();
#endif /* STM32H7S3xx || STM32H7S7xx */
  /* Set MPU to forbid execution outside of immutable code  */
  mpu_init_cfg();

  /* Configure MCE*/
  mce_oemirot_cfg();

  /* With OEMIROT_DEV_MODE , active tamper calls Error_Handler */
  active_tamper();

}

/**
  * @brief  Update the runtime security protections for application start
  *
  * @param  None
  * @retval None
  */
void LL_SECU_UpdateRunTimeProtections(void)
{
  /* Update MPU config for application execeution */
  mpu_appli_cfg();

  /* Configure MCE*/
  mce_appli_cfg();
}

/**
  * @brief  Check if the Static security protections are configured.
  *         Those protections are not impacted by a Reset. They are set using the Option Bytes.
  *         When the device is locked, these protections cannot be changed anymore.
  * @param  None
  * @note   By default, the best security protections are applied to the different
  *         flash sections in order to maximize the security level for the specific MCU.
  * @retval None
  */
void LL_SECU_CheckStaticProtections(void)
{
  static FLASH_OBProgramInitTypeDef flash_option_bytes = {0};
#ifdef OEMIROT_ENABLE_SET_OB
  HAL_StatusTypeDef ret = HAL_ERROR;
#endif  /* OEMIROT_ENABLE_SET_OB  */
#ifdef  OEMIROT_WRP_PROTECT_ENABLE
  uint32_t val;
#endif /* OEMIROT_WRP_PROTECT_ENABLE */
  uint32_t start;
  uint32_t end;
  uint32_t i;
  uint32_t product_state;

  /* Avoid warnings about unused variables */
#if ((!defined(OEMIROT_WRP_PROTECT_ENABLE)) && (!defined(OEMIROT_HDP_PROTECT_ENABLE)))
  (void)start;
  (void)end;
#endif /* (not OEMIROT_WRP_PROTECT_ENABLE) and (not OEMIROT_HDP_PROTECT_ENABLE) */

  /* Get Option Bytes status */
  (void) HAL_FLASHEx_OBGetConfig(&flash_option_bytes);

#ifdef OEMIROT_ENABLE_SET_OB
  /* Clean the option configuration */
  flash_option_bytes.OptionType = 0;
#endif /*   OEMIROT_ENABLE_SET_OB */

#ifdef  OEMIROT_WRP_PROTECT_ENABLE
  /* Check flash write protection */
  start = FLASH_AREA_BL2_OFFSET / WRP_PAGE_SIZE;
  end = (FLASH_AREA_BL2_OFFSET + FLASH_AREA_BL2_SIZE - 1) / WRP_PAGE_SIZE;
  val = 0;
  for (i = (start); i <= (end); i++)
  {
    val |= (1 << i);
  }

  if ((flash_option_bytes.WRPState != OB_WRPSTATE_ENABLE)
      || (flash_option_bytes.WRPSector != val))
  {
    BOOT_LOG_DBG("Flash write protection group 0x%lx: OB 0x%lx",
                 val, flash_option_bytes.WRPSector);
#ifndef OEMIROT_ENABLE_SET_OB
    BOOT_LOG_ERR("Unexpected value for write protection");
    Error_Handler();
#else
    flash_option_bytes.WRPState = OB_WRPSTATE_ENABLE;
    flash_option_bytes.WRPSector = val;

    BOOT_LOG_ERR("Unexpected value for write protection : set wrp1");
    flash_option_bytes.OptionType |= OPTIONBYTE_WRP;
#endif /* OEMIROT_ENABLE_SET_OB */
  }
#endif /* OEMIROT_WRP_PROTECT_ENABLE */

#ifdef  OEMIROT_HDP_PROTECT_ENABLE
  /* Check secure user flash protection (HDP) */
  start = 0;
  end = (FLASH_BL2_HDP_END) / HDP_PAGE_SIZE;

  if ((start != flash_option_bytes.HDPStartPage)
      || (end != flash_option_bytes.HDPEndPage))
  {
    BOOT_LOG_DBG("Hide protection [%ld, %ld] : OB [%ld, %ld]",
                 start,
                 end,
                 flash_option_bytes.HDPStartPage,
                 flash_option_bytes.HDPEndPage);

#ifndef OEMIROT_ENABLE_SET_OB
    BOOT_LOG_ERR("Unexpected value for hide protection");
    Error_Handler();
#else
    BOOT_LOG_ERR("Unexpected value for hide protection : set hdp1");
    flash_option_bytes.HDPStartPage = start;
    flash_option_bytes.HDPEndPage = end;
    flash_option_bytes.OptionType |= OPTIONBYTE_HDP;
#endif  /*  OEMIROT_ENABLE_SET_OB */
  }
#endif /* OEMIROT_HDP_PROTECT_ENABLE */

#ifdef OEMIROT_USER_SRAM_ECC
  /* Check SRAM ECC */
  if ((flash_option_bytes.USERConfig2 & OB_AXISRAM_ECC_ENABLE) != OB_AXISRAM_ECC_ENABLE)
  {
    BOOT_LOG_ERR("Unexpected value for SRAM ECC");
    Error_Handler();
  }
#endif /* OEMIROT_USER_SRAM_ECC */

#ifdef OEMIROT_ENABLE_SET_OB

  /* Configure Options Bytes */
  if (flash_option_bytes.OptionType != 0)
  {
    /* Unlock the Flash to enable the flash control register access */
    HAL_FLASH_Unlock();

    /* Unlock the Options Bytes */
    HAL_FLASH_OB_Unlock();

    if ((flash_option_bytes.OptionType) != 0)
    {
      /* Program the Options Bytes */
      ret = HAL_FLASHEx_OBProgram(&flash_option_bytes);
      if (ret != HAL_OK)
      {
        BOOT_LOG_ERR("Error while setting OB config");
        Error_Handler();
      }
    }

  }
#endif /* OEMIROT_ENABLE_SET_OB */

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

  /* Check Product State : boot if current Product State is greater or equal to selected Product State */
  /* Identify product state mini selected */
  for (i = 0U; i < NB_PRODUCT_STATE; i++)
  {
    if (ProductStatePrioList[i] == OEMIROT_OB_PRODUCT_STATE_VALUE)
    {
      break;
    }
  }
  if (i >= NB_PRODUCT_STATE)
  {
    Error_Handler();
  }
  /* Control if current product state is allowed */
  product_state = LL_SECU_GetProductState(flash_option_bytes.NVState,
                                          flash_option_bytes.ROTConfig & FLASH_ROTSR_OEM_PROVD,
                                          flash_option_bytes.ROTConfig & FLASH_ROTSR_DBG_AUTH);
  for (; i < NB_PRODUCT_STATE; i++)
  {
    if (ProductStatePrioList[i] == product_state)
    {
      break;
    }
  }
  if (i >= NB_PRODUCT_STATE)
  {
    Error_Handler();
  }
}


#ifdef OEMIROT_MPU_PROTECTION
/**
  * @brief  Configure and enable a MPU region
  * @param  MpuRegionCfg Configuration of the region
  * @retval None
  */
static void mpu_configure_region(const OEMIROT_MPU_InitTypeDef *MpuRegionCfg, uint8_t Enable)
{
  uint32_t mpu_rasr = 0UL;

  /* Check the parameters */
  assert_param(IS_MPU_REGION_NUMBER(MpuRegionCfg->Number));
  assert_param(IS_MPU_REGION_ENABLE(Enable));
  assert_param(IS_MPU_INSTRUCTION_ACCESS(MpuRegionCfg->DisableExec));
  assert_param(IS_MPU_REGION_PERMISSION_ATTRIBUTE(MpuRegionCfg->AccessPermission));
  assert_param(IS_MPU_TEX_LEVEL(MpuRegionCfg->Tex));
  assert_param(IS_MPU_ACCESS_SHAREABLE(MpuRegionCfg->Shareable));
  assert_param(IS_MPU_ACCESS_CACHEABLE(MpuRegionCfg->Cacheable));
  assert_param(IS_MPU_ACCESS_BUFFERABLE(MpuRegionCfg->Bufferable));
  assert_param(IS_MPU_SUB_REGION_DISABLE(MpuRegionCfg->SubRegionDisable));
  assert_param(IS_MPU_REGION_SIZE(MpuRegionCfg->Size));

  mpu_rasr = ((uint32_t)MpuRegionCfg->DisableExec      << MPU_RASR_XN_Pos)   |
             ((uint32_t)MpuRegionCfg->AccessPermission << MPU_RASR_AP_Pos)   |
             ((uint32_t)MpuRegionCfg->Tex              << MPU_RASR_TEX_Pos)  |
             ((uint32_t)MpuRegionCfg->Shareable        << MPU_RASR_S_Pos)    |
             ((uint32_t)MpuRegionCfg->Cacheable        << MPU_RASR_C_Pos)    |
             ((uint32_t)MpuRegionCfg->Bufferable       << MPU_RASR_B_Pos)    |
             ((uint32_t)MpuRegionCfg->SubRegionDisable << MPU_RASR_SRD_Pos)  |
             ((uint32_t)MpuRegionCfg->Size             << MPU_RASR_SIZE_Pos);
  if (Enable == MPU_REGION_ENABLE)
  {
    mpu_rasr |= ((uint32_t)Enable << MPU_RASR_ENABLE_Pos);
  }

  MPU->RNR  = MpuRegionCfg->Number;
  MPU->RBAR = MpuRegionCfg->BaseAddress;
  MPU->RASR = mpu_rasr;
}

/**
  * @brief  Check MPU configuration
  * @param  MpuRegionCfg Configuration to be checked
  * @retval HAL_OK if successful, HAL_ERROR otherwise.
  */
static HAL_StatusTypeDef mpu_check_region_config(const OEMIROT_MPU_InitTypeDef *MpuRegionCfg, uint8_t Enable)
{
  uint32_t mpu_rasr = 0UL;

  /* Set the Region number */
  MPU->RNR = MpuRegionCfg->Number;

  mpu_rasr |= (((uint32_t)MpuRegionCfg->DisableExec      << MPU_RASR_XN_Pos) & MPU_RASR_XN_Msk);
  mpu_rasr |= (((uint32_t)MpuRegionCfg->AccessPermission << MPU_RASR_AP_Pos) & MPU_RASR_AP_Msk);
  mpu_rasr |= (((uint32_t)MpuRegionCfg->Tex              << MPU_RASR_TEX_Pos) & MPU_RASR_TEX_Msk);
  mpu_rasr |= (((uint32_t)MpuRegionCfg->Shareable        << MPU_RASR_S_Pos) & MPU_RASR_S_Msk);
  mpu_rasr |= (((uint32_t)MpuRegionCfg->Cacheable        << MPU_RASR_C_Pos) & MPU_RASR_C_Msk);
  mpu_rasr |= (((uint32_t)MpuRegionCfg->Bufferable       << MPU_RASR_B_Pos) & MPU_RASR_B_Msk);
  mpu_rasr |= (((uint32_t)MpuRegionCfg->SubRegionDisable << MPU_RASR_SRD_Pos) & MPU_RASR_SRD_Msk);
  mpu_rasr |= (((uint32_t)MpuRegionCfg->Size             << MPU_RASR_SIZE_Pos) & MPU_RASR_SIZE_Msk);
  if (Enable == MPU_REGION_ENABLE)
  {
    mpu_rasr |= (((uint32_t)Enable << MPU_RASR_ENABLE_Pos) & MPU_RASR_ENABLE_Msk);
  }

  if (((MPU->RBAR & MPU_RBAR_ADDR_Msk) == MpuRegionCfg->BaseAddress) && (MPU->RASR == mpu_rasr))
  {
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;
  }
}
#endif /* OEMIROT_MPU_PROTECTION */

void LL_SECU_DisableCleanMpu(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;
  uint32_t i;

  SCB_CleanDCache();

  HAL_MPU_Disable();

  memset(&MPU_InitStruct, 0, sizeof(MPU_InitStruct));

  for (i = MPU_REGION_NUMBER0; i <= MPU_REGION_NUMBER15; i++)
  {
    MPU_InitStruct.Enable = MPU_REGION_DISABLE;
    MPU_InitStruct.BaseAddress = 0;
    MPU_InitStruct.Number = i;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);
  }
}

/**
  * @brief  mpu init
  * @param  None
  * @retval None
  */
static void mpu_init_cfg(void)
{
#ifdef OEMIROT_MPU_PROTECTION
  int32_t i;

  /* configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* Clean D-Cache before configuring MPU areas */
    SCB_CleanDCache();

    /*  ITCM address are not mapped by MPU :
     *  Since an ITCM access can allow a read to protected area
     *  The ITCM is disabled */
    SCB->ITCMCR &= ~SCB_ITCMCR_EN_Msk;

    /* configure secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(region_cfg_init); i++)
    {
      mpu_configure_region(&region_cfg_init[i], MPU_REGION_ENABLE);
      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, region_cfg_init[i].flow_step_enable,
                        region_cfg_init[i].flow_ctrl_enable);
    }

    /* Enables the MPU */
    HAL_MPU_Enable(MPU_HARDFAULT_NMI);
#if defined(SCB_SHCSR_MEMFAULTENA_Msk)
    /* Enables memory fault exception */
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
#endif /* SCB_SHCSR_MEMFAULTENA_Msk */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MPU_I_EN, FLOW_CTRL_MPU_I_EN);
  }
  /* verification stage */
  else
  {
    /* check secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(region_cfg_init); i++)
    {
      if (mpu_check_region_config(&region_cfg_init[i], MPU_REGION_ENABLE) != HAL_OK)
      {
        Error_Handler();
      }
      else
      {
        /* Execution stopped if flow control failed */
        FLOW_CONTROL_STEP(uFlowProtectValue, region_cfg_init[i].flow_step_check,
                          region_cfg_init[i].flow_ctrl_check);
      }
    }

    /* check secure MPU */
    if (MPU->CTRL != (MPU_HARDFAULT_NMI | MPU_CTRL_ENABLE_Msk))
    {
      Error_Handler();
    }
#if defined(SCB_SHCSR_MEMFAULTENA_Msk)
    else if ((SCB->SHCSR & SCB_SHCSR_MEMFAULTENA_Msk) != SCB_SHCSR_MEMFAULTENA_Msk)
    {
      Error_Handler();
    }
#endif /* SCB_SHCSR_MEMFAULTENA_Msk */
    else
    {
      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MPU_I_CH, FLOW_CTRL_MPU_I_CH);
    }
  }
#endif /* OEMIROT_MPU_PROTECTION */
}


static void mpu_appli_cfg(void)
{
#ifdef OEMIROT_MPU_PROTECTION
  int32_t i;
  HAL_StatusTypeDef status;

  /* configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* Clean D-Cache before configuring MPU areas */
    SCB_CleanDCache();

    /*  ITCM address are not mapped by MPU :
     *  Since an ITCM access can allow a read to protected area
     *  The ITCM is disabled */
    SCB->ITCMCR &= ~SCB_ITCMCR_EN_Msk;

    /* configure secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(region_cfg_appli); i++)
    {
      /* First region configured should be activated by NSS JUMP service,
         execution rights given to primary code slot */
      if (i == 0)
      {
        if (region_cfg_appli[i].Number != MPU_REGION_APP)
        {
          Error_Handler();
        }
        mpu_configure_region(&region_cfg_appli[i], MPU_REGION_DISABLE);
      }
      else
      {
        mpu_configure_region(&region_cfg_appli[i], MPU_REGION_ENABLE);
      }

      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, region_cfg_appli[i].flow_step_enable,
                        region_cfg_appli[i].flow_ctrl_enable);
    }
  }
  else
  {
    /* check secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(region_cfg_appli); i++)
    {
      /* First region configured should be activated by NSS JUMP service,
         execution rights given to primary code slot */
      if (i == 0)
      {
        if (region_cfg_appli[i].Number != MPU_REGION_APP)
        {
          Error_Handler();
        }
        status = mpu_check_region_config(&region_cfg_appli[i], MPU_REGION_DISABLE);
      }
      else
      {
        status = mpu_check_region_config(&region_cfg_appli[i], MPU_REGION_ENABLE);
      }

      if (status != HAL_OK)
      {
        Error_Handler();
      }
      else
      {
        /* Execution stopped if flow control failed */
        FLOW_CONTROL_STEP(uFlowProtectValue, region_cfg_appli[i].flow_step_check,
                          region_cfg_appli[i].flow_ctrl_check);
      }
    }
  }
#endif /* OEMIROT_MPU_PROTECTION */
}

/**
  * @brief  configure mce before jumping into application
  * @param  None
  * @retval None
  */
static void mce_appli_cfg(void)
{
#ifdef OEMIROT_MCE_PROTECTION
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* Depending on the configuration, a master key may be removed. */
    LL_MCE_SetAppliMasterKey(FLOW_STEP_MCE_A_EN, FLOW_CTRL_MCE_A_EN);

    /* Disable the write access on the primary slot */
    LL_MCE_SetRegion(LL_MCE_REGION_CFG_APPLI, LL_MCE_REGION_CODE_PRIMARY,
                     FLOW_STEP_MCE_A_EN_R0,   FLOW_CTRL_MCE_A_EN_R0);
    LL_MCE_SetRegion(LL_MCE_REGION_CFG_APPLI, LL_MCE_REGION_CODE_SECONDARY,
                     FLOW_STEP_MCE_A_EN_R1,   FLOW_CTRL_MCE_A_EN_R1);
#if !defined(MCUBOOT_OVERWRITE_ONLY)
    LL_MCE_SetRegion(LL_MCE_REGION_CFG_APPLI, LL_MCE_REGION_SCRATCH,
                     FLOW_STEP_MCE_A_EN_R2,   FLOW_CTRL_MCE_A_EN_R2);
#endif /* not MCUBOOT_OVERWRITE_ONLY */
#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
    LL_MCE_SetRunRegion(LL_MCE_REGION_CFG_APPLI, LL_MCE_REGION_CODE_PRIMARY,
                        FLOW_STEP_MCE_A_EN_R3,   FLOW_CTRL_MCE_A_EN_R3);
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

    /* Lock the regions */
    LL_MCE_LockAppliRegions(FLOW_STEP_MCE_A_EN_RLCK, FLOW_CTRL_MCE_A_EN_RLCK);

  }
  else
  {
    LL_MCE_CheckAppliMasterKey(FLOW_STEP_MCE_A_CH, FLOW_CTRL_MCE_A_CH);

    /* check MCE regions */
    (void)LL_MCE_CheckRegion(LL_MCE_REGION_CFG_APPLI, LL_MCE_REGION_CODE_PRIMARY,
                             FLOW_STEP_MCE_A_CH_R0,   FLOW_CTRL_MCE_A_CH_R0);
    (void)LL_MCE_CheckRegion(LL_MCE_REGION_CFG_APPLI, LL_MCE_REGION_CODE_SECONDARY,
                             FLOW_STEP_MCE_A_CH_R1,   FLOW_CTRL_MCE_A_CH_R1);
#if !defined(MCUBOOT_OVERWRITE_ONLY)
    (void)LL_MCE_CheckRegion(LL_MCE_REGION_CFG_APPLI, LL_MCE_REGION_SCRATCH,
                             FLOW_STEP_MCE_A_CH_R2,   FLOW_CTRL_MCE_A_CH_R2);
#endif /* not MCUBOOT_OVERWRITE_ONLY */
#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
    (void)LL_MCE_CheckRunRegion(LL_MCE_REGION_CFG_APPLI, LL_MCE_REGION_CODE_PRIMARY,
                                FLOW_STEP_MCE_A_CH_R3,   FLOW_CTRL_MCE_A_CH_R3);
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

    /* Lock the regions */
    LL_MCE_CheckLockAppliRegions(FLOW_STEP_MCE_A_CH_RLCK, FLOW_CTRL_MCE_A_CH_RLCK);
  }
#endif /* OEMIROT_MCE_PROTECTION */
}

/**
  * @brief  configure MCE for OEMiROT
  * @param  None
  * @retval None
  */
static void mce_oemirot_cfg(void)
{
#ifdef OEMIROT_MCE_PROTECTION
  /* Check the parameters */
  assert_param(IS_VALID_Loader_op(Loader_op));

  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* set Decryption keys for the OEMiROT */
    LL_MCE_SetMasterKey(FLOW_STEP_MCE_O_EN, FLOW_CTRL_MCE_O_EN);

    LL_MCE_SetRegion(LL_MCE_REGION_CFG_INSTALL, LL_MCE_REGION_CODE_PRIMARY,
                     FLOW_STEP_MCE_O_EN_R0,     FLOW_CTRL_MCE_O_EN_R0);
    LL_MCE_SetRegion(LL_MCE_REGION_CFG_INSTALL, LL_MCE_REGION_CODE_SECONDARY,
                     FLOW_STEP_MCE_O_EN_R1,     FLOW_CTRL_MCE_O_EN_R1);
#if !defined(MCUBOOT_OVERWRITE_ONLY)
    LL_MCE_SetRegion(LL_MCE_REGION_CFG_INSTALL, LL_MCE_REGION_SCRATCH,
                     FLOW_STEP_MCE_O_EN_R2,     FLOW_CTRL_MCE_O_EN_R2);
#endif /* not MCUBOOT_OVERWRITE_ONLY */
#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
    LL_MCE_SetRunRegion(LL_MCE_REGION_CFG_INSTALL, LL_MCE_REGION_CODE_PRIMARY,
                        FLOW_STEP_MCE_O_EN_R3,     FLOW_CTRL_MCE_O_EN_R3);
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */
  }
  else
  {
    LL_MCE_CheckMasterKey(FLOW_STEP_MCE_O_CH, FLOW_CTRL_MCE_O_CH);

    /* check MCE regions */
    (void)LL_MCE_CheckRegion(LL_MCE_REGION_CFG_INSTALL, LL_MCE_REGION_CODE_PRIMARY,
                             FLOW_STEP_MCE_O_CH_R0,     FLOW_CTRL_MCE_O_CH_R0);
    (void)LL_MCE_CheckRegion(LL_MCE_REGION_CFG_INSTALL, LL_MCE_REGION_CODE_SECONDARY,
                             FLOW_STEP_MCE_O_CH_R1,     FLOW_CTRL_MCE_O_CH_R1);
#if !defined(MCUBOOT_OVERWRITE_ONLY)
    (void)LL_MCE_CheckRegion(LL_MCE_REGION_CFG_INSTALL, LL_MCE_REGION_SCRATCH,
                             FLOW_STEP_MCE_O_CH_R2,     FLOW_CTRL_MCE_O_CH_R2);
#endif /* not MCUBOOT_OVERWRITE_ONLY */
#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
    (void)LL_MCE_CheckRunRegion(LL_MCE_REGION_CFG_INSTALL, LL_MCE_REGION_CODE_PRIMARY,
                                FLOW_STEP_MCE_O_CH_R3,     FLOW_CTRL_MCE_O_CH_R3);
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

    /* As the keys may be de-configured during a following step, no key lock is done now. */
  }
#endif /* OEMIROT_MCE_PROTECTION */
}

/**
  * @brief  configure mce before jumping into Loader
  * @param  None
  * @retval None
  */
#if defined(MCUBOOT_EXT_LOADER)
static void mce_local_loader_cfg(void)
{
#ifdef OEMIROT_MCE_PROTECTION
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* The MCE master keys are not written for the BL
     * in order to disable the read access of the code primary slot.
     * The following function clears the master keys.
     */
    LL_MCE_SetLoaderMasterKey(FLOW_STEP_MCE_L_EN, FLOW_CTRL_MCE_L_EN);

    /* Disable the access on the primary slot */
    LL_MCE_SetRegion(LL_MCE_REGION_CFG_BL,  LL_MCE_REGION_CODE_PRIMARY,
                     FLOW_STEP_MCE_L_EN_R0, FLOW_CTRL_MCE_L_EN_R0);
    LL_MCE_SetRegion(LL_MCE_REGION_CFG_BL,  LL_MCE_REGION_CODE_SECONDARY,
                     FLOW_STEP_MCE_L_EN_R1, FLOW_CTRL_MCE_L_EN_R1);
#if !defined(MCUBOOT_OVERWRITE_ONLY)
    LL_MCE_SetRegion(LL_MCE_REGION_CFG_BL,  LL_MCE_REGION_SCRATCH,
                     FLOW_STEP_MCE_L_EN_R2, FLOW_CTRL_MCE_L_EN_R2);
#endif /* not MCUBOOT_OVERWRITE_ONLY */
#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
    LL_MCE_SetRunRegion(LL_MCE_REGION_CFG_BL,  LL_MCE_REGION_CODE_PRIMARY,
                        FLOW_STEP_MCE_L_EN_R3, FLOW_CTRL_MCE_L_EN_R3);
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

    /* Lock the regions */
    LL_MCE_LockLoaderRegions(FLOW_STEP_MCE_L_EN_RLCK, FLOW_CTRL_MCE_L_EN_RLCK);
  }
  else
  {
    LL_MCE_CheckLoaderMasterKey(FLOW_STEP_MCE_L_CH, FLOW_CTRL_MCE_L_CH);

    /* check MCE regions */
    (void)LL_MCE_CheckRegion(LL_MCE_REGION_CFG_BL,  LL_MCE_REGION_CODE_PRIMARY,
                             FLOW_STEP_MCE_L_CH_R0, FLOW_CTRL_MCE_L_CH_R0);
    (void)LL_MCE_CheckRegion(LL_MCE_REGION_CFG_BL,  LL_MCE_REGION_CODE_SECONDARY,
                             FLOW_STEP_MCE_L_CH_R1, FLOW_CTRL_MCE_L_CH_R1);
#if !defined(MCUBOOT_OVERWRITE_ONLY)
    (void)LL_MCE_CheckRegion(LL_MCE_REGION_CFG_BL,  LL_MCE_REGION_SCRATCH,
                             FLOW_STEP_MCE_L_CH_R2, FLOW_CTRL_MCE_L_CH_R2);
#endif /* not MCUBOOT_OVERWRITE_ONLY */
#if  (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
    (void)LL_MCE_CheckRunRegion(LL_MCE_REGION_CFG_BL,  LL_MCE_REGION_CODE_PRIMARY,
                                FLOW_STEP_MCE_L_CH_R3, FLOW_CTRL_MCE_L_CH_R3);
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

    /* Lock the regions */
    LL_MCE_CheckLockLoaderRegions(FLOW_STEP_MCE_L_CH_RLCK, FLOW_CTRL_MCE_L_CH_RLCK);
  }
#endif /* OEMIROT_MCE_PROTECTION */
}
#endif /* MCUBOOT_EXT_LOADER */
#if defined(MCUBOOT_EXT_LOADER)
static void mpu_loader_cfg(void)
{
#if defined(OEMIROT_MPU_PROTECTION) && defined(OEMIROT_JUMP_TO_BL_ENABLE)
  uint32_t i = 0U;
  volatile uint8_t mpu_status = MPU_REGION_ENABLE;

  /* configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* Clean D-Cache before configuring MPU areas */
    SCB_CleanDCache();

    /*  ITCM address are not mapped by MPU :
     *  Since an ITCM access can allow a read to protected area
     *  The ITCM is disabled */
    SCB->ITCMCR &= ~SCB_ITCMCR_EN_Msk;

    /* configure additional MPU region */
    for (i = 0U; i < ARRAY_SIZE(region_cfg_loader); i++)
    {
      mpu_status = MPU_REGION_ENABLE;
      if (region_cfg_loader[i].Number == MPU_REGION_BL)
      {
        mpu_status = MPU_REGION_DISABLE;
      }
      mpu_configure_region(&region_cfg_loader[i], mpu_status);

      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, region_cfg_loader[i].flow_step_enable,
                        region_cfg_loader[i].flow_ctrl_enable);
    }
  }
  /* verification stage */
  else
  {
    /* check secure MPU regions */
    for (i = 0U; i < ARRAY_SIZE(region_cfg_loader); i++)
    {
      mpu_status = MPU_REGION_ENABLE;
      if (region_cfg_loader[i].Number == MPU_REGION_BL)
      {
        mpu_status = MPU_REGION_DISABLE;
      }
      if (mpu_check_region_config(&region_cfg_loader[i], mpu_status) != HAL_OK)
      {
        Error_Handler();
      }
      else
      {
        /* Execution stopped if flow control failed */
        FLOW_CONTROL_STEP(uFlowProtectValue, region_cfg_loader[i].flow_step_check,
                          region_cfg_loader[i].flow_ctrl_check);
      }
    }
  }
#endif /* OEMIROT_MPU_PROTECTION and OEMIROT_JUMP_TO_BL_ENABLE */
}
#endif /* MCUBOOT_EXT_LOADER */


#if (OEMIROT_TAMPER_ENABLE != NO_TAMPER)
const RTC_PrivilegeStateTypeDef TamperPrivConf =
{
  .rtcPrivilegeFull = RTC_PRIVILEGE_FULL_NO,
  .rtcPrivilegeFeatures = RTC_PRIVILEGE_FEATURE_NONE,
  .tampPrivilegeFull = TAMP_PRIVILEGE_FULL_YES,
  .MonotonicCounterPrivilege = TAMP_MONOTONIC_CNT_PRIVILEGE_NO,
  .backupRegisterStartZone2 = 0,
  .backupRegisterStartZone3 = 0
};
const RTC_InternalTamperTypeDef InternalTamperConf =
{
  .IntTamper = RTC_INT_TAMPER_9 | RTC_INT_TAMPER_15,
  .TimeStampOnTamperDetection = RTC_TIMESTAMPONTAMPERDETECTION_DISABLE,
  .NoErase                  = RTC_TAMPER_ERASE_BACKUP_ENABLE
};
void TAMP_IRQHandler(void)
{
  NVIC_SystemReset();
}
#ifdef OEMIROT_DEV_MODE
extern volatile uint32_t TamperEventCleared;
#endif /* OEMIROT_DEV_MODE */
#endif /* (OEMIROT_TAMPER_ENABLE != NO_TAMPER) */
RTC_HandleTypeDef RTCHandle;

static void active_tamper(void)
{
#if (OEMIROT_TAMPER_ENABLE == ALL_TAMPER)
  RTC_ActiveTampersTypeDef sAllTamper;
  /*  use random generator to feed  */
  uint32_t Seed[4] = {0, 0, 0, 0};
  uint32_t len = 0;
  uint32_t j;
#endif /* (OEMIROT_TAMPER_ENABLE == ALL_TAMPER) */
#if (OEMIROT_TAMPER_ENABLE != NO_TAMPER)
  fih_int fih_rc = FIH_FAILURE;
  RTC_PrivilegeStateTypeDef TamperPrivConfGet;
#endif /* OEMIROT_TAMPER_ENABLE != NO_TAMPER) */
  /* configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
#if defined(OEMIROT_DEV_MODE) && (OEMIROT_TAMPER_ENABLE != NO_TAMPER)
    if (TamperEventCleared == 1)
    {
      BOOT_LOG_INF("Boot with TAMPER Event Active");
#if (OEMIROT_TAMPER_ENABLE == ALL_TAMPER)
      /* avoid several re-boot in DEV_MODE with Tamper active */
      BOOT_LOG_INF("Plug the tamper cable, and reboot");
      BOOT_LOG_INF("Or");
#endif /* (OEMIROT_TAMPER_ENABLE == ALL_TAMPER) */
      BOOT_LOG_INF("Build and Flash with flag #define OEMIROT_TAMPER_ENABLE NO_TAMPER\n");
      Error_Handler();
    }
#endif /*  OEMIROT_DEV_MODE && (OEMIROT_TAMPER_ENABLE != NO_TAMPER) */

    /* RTC Init */
    RTCHandle.Instance = RTC;
    RTCHandle.Init.HourFormat     = RTC_HOURFORMAT_12;
    RTCHandle.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV;
    RTCHandle.Init.SynchPrediv    = RTC_SYNCH_PREDIV;
    RTCHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
    RTCHandle.Init.OutPutRemap    = RTC_OUTPUT_REMAP_NONE;
    RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RTCHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_PUSHPULL;
    RTCHandle.Init.OutPutPullUp   = RTC_OUTPUT_PULLUP_NONE;

    if (HAL_RTC_Init(&RTCHandle) != HAL_OK)
    {
      Error_Handler();
    }
#if (OEMIROT_TAMPER_ENABLE == ALL_TAMPER)
    /* generate random seed */
    RNG_GetBytes((unsigned char *)Seed, sizeof(Seed), (size_t *)&len);
    if ((len != sizeof(Seed)) || (len == 0))
    {
      Error_Handler();
    }
    BOOT_LOG_DBG("TAMPER SEED [0x%lx,0x%lx,0x%lx,0x%lx]", Seed[0], Seed[1], Seed[2], Seed[3]);
    /* Configure active tamper common parameters  */
    sAllTamper.ActiveFilter = RTC_ATAMP_FILTER_ENABLE;
    sAllTamper.ActiveAsyncPrescaler = RTC_ATAMP_ASYNCPRES_RTCCLK_32;
    sAllTamper.TimeStampOnTamperDetection = RTC_TIMESTAMPONTAMPERDETECTION_ENABLE;
    sAllTamper.ActiveOutputChangePeriod = 4;
    sAllTamper.Seed[0] = Seed[0];
    sAllTamper.Seed[1] = Seed[1];
    sAllTamper.Seed[2] = Seed[2];
    sAllTamper.Seed[3] = Seed[3];

    /* Disable all Active Tampers */
    /* No active tamper */
    for (j = 0; j < RTC_TAMP_NB; j++)
    {
      sAllTamper.TampInput[j].Enable = RTC_ATAMP_DISABLE;
    }
    sAllTamper.TampInput[1].Enable = RTC_ATAMP_ENABLE;
    sAllTamper.TampInput[1].Output = 7;
    sAllTamper.TampInput[1].NoErase =  RTC_TAMPER_ERASE_BACKUP_ENABLE;
    sAllTamper.TampInput[1].MaskFlag = RTC_TAMPERMASK_FLAG_DISABLE;
    sAllTamper.TampInput[1].Interrupt = RTC_ATAMP_INTERRUPT_ENABLE;
    /* Set active tampers */
    if (HAL_RTCEx_SetActiveTampers(&RTCHandle, &sAllTamper) != HAL_OK)
    {
      Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_ACT_EN, FLOW_CTRL_TAMP_ACT_EN);
#else
    HAL_RTCEx_DeactivateTamper(&RTCHandle, RTC_TAMPER_ALL);
#endif  /* (OEMIROT_TAMPER_ENABLE == ALL_TAMPER) */
#if (OEMIROT_TAMPER_ENABLE != NO_TAMPER)
    /*  Internal Tamper activation  */
    /*  Enable Cryptographic IPs fault (tamp_itamp9), System fault detection (tamp_itamp15)*/
    if (HAL_RTCEx_SetInternalTamper(&RTCHandle, (RTC_InternalTamperTypeDef *)&InternalTamperConf) != HAL_OK)
    {
      Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_INT_EN, FLOW_CTRL_TAMP_INT_EN);

    /*  Set tamper configuration privileged only   */
    if (HAL_RTCEx_PrivilegeModeSet(&RTCHandle, (RTC_PrivilegeStateTypeDef *)&TamperPrivConf) != HAL_OK)
    {
      Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_PRIV_EN, FLOW_CTRL_TAMP_PRIV_EN);

    /*  Activate Secret Erase */
    HAL_RTCEx_Erase_SecretDev_Conf(&RTCHandle, (uint32_t)TAMP_SECRETDEVICE_ERASE_BKP_SRAM);
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_CFG_EN, FLOW_CTRL_TAMP_CFG_EN);
    BOOT_LOG_DBG("TAMPER Activated");
#else
    HAL_RTCEx_DeactivateInternalTamper(&RTCHandle, RTC_INT_TAMPER_ALL);
#endif /* (OEMIROT_TAMPER_ENABLE != NO_TAMPER) */
  }
#if (OEMIROT_TAMPER_ENABLE != NO_TAMPER)
  /* verification stage */
  else
  {
#if (OEMIROT_TAMPER_ENABLE == ALL_TAMPER)
    /* Check active tampers */
    if ((READ_BIT(TAMP->ATOR, TAMP_ATOR_INITS) == 0U) ||
        (READ_BIT(TAMP->IER, (TAMP_IER_TAMP1IE << 1)) != (TAMP_IER_TAMP1IE << 1)) ||
        (READ_REG(TAMP->ATCR1) != 0xC4050c02U) ||
        (READ_REG(TAMP->ATCR2) != TAMP_ATCR2_ATOSEL2) ||
        (READ_REG(TAMP->CR1) != (0x41000000U | (TAMP_CR1_TAMP1E << 1))) ||
        (READ_REG(TAMP->CR2) != 0x00000000U))
    {
      Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_ACT_CH, FLOW_CTRL_TAMP_ACT_CH);
#endif  /* (OEMIROT_TAMPER_ENABLE == ALL_TAMPER) */
    /*  Check Internal Tamper activation */
    if ((READ_BIT(RTC->CR, RTC_CR_TAMPTS) != InternalTamperConf.TimeStampOnTamperDetection) ||
#if (OEMIROT_TAMPER_ENABLE == ALL_TAMPER)
        (READ_REG(TAMP->CR1) != (0x41000000U | (TAMP_CR1_TAMP1E << 1))) ||
#else
        (READ_REG(TAMP->CR1) != 0x41000000U) ||
#endif /* (OEMIROT_TAMPER_ENABLE == ALL_TAMPER) */
        (READ_REG(TAMP->CR3) != 0x00000000U))
    {
      Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_INT_CH, FLOW_CTRL_TAMP_INT_CH);

    /*  Check tamper configuration privileged only   */
    if (HAL_RTCEx_PrivilegeModeGet(&RTCHandle, (RTC_PrivilegeStateTypeDef *)&TamperPrivConfGet) != HAL_OK)
    {
      Error_Handler();
    }
    FIH_CALL(boot_fih_memequal, fih_rc, (void *)&TamperPrivConf, (void *)&TamperPrivConfGet, sizeof(TamperPrivConf));
    if (fih_not_eq(fih_rc, FIH_SUCCESS))
    {
      Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_PRIV_CH, FLOW_CTRL_TAMP_PRIV_CH);

  }
#endif /*  OEMIROT_TAMPER_ENABLE != NO_TAMPER */
}


/**
  * @brief  Translate the Non-volatile state, the OEM Provisioned state and
  *         the Root-Of-Trust Debug authentication method into a unique
  *         product state.
  * @param  NvState       Non-volatile state
  * @param  OemProvdState OEM Provisioned state
  * @param  DbgAuth       Root-Of-Trust Debug authentication method
  * @retval Translated product state
  */
uint32_t LL_SECU_GetProductState(uint32_t NvState, uint32_t OemProvdState, uint32_t DbgAuth)
{
  uint32_t product_state = OEMIROT_PROD_STATE_NOT_SET;

  if (NvState == OB_NVSTATE_OPEN)
  {
    product_state = OEMIROT_PROD_STATE_OPEN;
  }

  if (NvState == OB_NVSTATE_CLOSE)
  {
    if (OemProvdState == OB_OEM_PROVD_ENABLE)
    {
      switch (DbgAuth)
      {
        case OB_DBG_AUTH_DEFAULT:
          product_state = OEMIROT_PROD_STATE_PROVISIONED;
          break;
        case OB_DBG_AUTH_LOCKED:
          product_state = OEMIROT_PROD_STATE_LOCKED;
          break;
        case OB_DBG_AUTH_ECDSA_SIGN:
          product_state = OEMIROT_PROD_STATE_CLOSED_ECDSA;
          break;
        case OB_DBG_AUTH_PASSWORD:
          product_state = OEMIROT_PROD_STATE_CLOSED_PWD;
          break;
        default:
          /* Unexpected use case: keep product_state to "NONE". */
          break;
      }
    }
    else if (DbgAuth == OB_DBG_AUTH_DEFAULT)
    {
      product_state = OEMIROT_PROD_STATE_PROVISIONING;
    }
    else
    {
      /* Unexpected use case: keep product_state to "NONE". */
    }
  }
  return product_state;
}

/**
  * @brief  Configure SAES peripheral with AHK
  * @param  hCryp SAES handle reference
  * @param  HDPLLevel HDP level key to encrypt/decrypt
  * @param  SaesTimeout timeout of SAES
  * @retval None
  */
#if defined(STM32H7S3xx) || defined(STM32H7S7xx)
void LL_SECU_ConfigureSAES(CRYP_HandleTypeDef *hCryp, uint32_t HDPLLevel, uint32_t SaesTimeout,
                           uint32_t AHKIndex)
{
  uint32_t timeout = 0U;
  HAL_StatusTypeDef status = HAL_ERROR;
  FLASH_KeyConfigTypeDef keyCfg = { 0U };

  /* enable SBS clock */
  __HAL_RCC_SBS_CLK_ENABLE();
  /* enable HSI 48 MHz */
  __HAL_RCC_HSI48_ENABLE();
  /* enable RNG clock */
  __HAL_RCC_RNG_CLK_ENABLE();
  /* enable SAES clock */
  __HAL_RCC_SAES_CLK_ENABLE();

  /* force the SAES Peripheral Clock Reset */
  __HAL_RCC_SAES_FORCE_RESET();
  /* release the SAES Peripheral Clock Reset */
  __HAL_RCC_SAES_RELEASE_RESET();

  if (HAL_CRYP_DeInit(hCryp) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure SAES to use 256-Byte AHK */
  if (HAL_CRYP_Init(hCryp) != HAL_OK)
  {
    Error_Handler();
  }

  __DSB();

  (void) HAL_FLASH_Unlock();
  (void) HAL_FLASH_OB_Unlock();

  keyCfg.HDPLLevel = HDPLLevel;
  keyCfg.Index = AHKIndex;

  status = HAL_FLASHEx_GetKey(&keyCfg, NULL);

  if (status != HAL_OK)
  {
    Error_Handler();
  }

  /* Wait the key transfer is completed*/
  timeout = HAL_GetTick() + timeout;
  while (((SAES->SR & SAES_SR_BUSY) != 0U) && ((SAES->SR & SAES_SR_KEYVALID) == 0U))
  {
    if (timeout != HAL_MAX_DELAY)
    {
      if (HAL_GetTick() >= SaesTimeout)
      {
        Error_Handler();
      }
    }
  }

  (void) HAL_FLASH_OB_Lock();
  (void) HAL_FLASH_Lock();

}
#endif /* STM32H7S3xx || STM32H7S7xx */
void IWDG_IRQHandler(void)
{
  NVIC_SystemReset();
}

void RAMECC_IRQHandler(void)
{
  NVIC_SystemReset();
}
/**
  * @}
  */
