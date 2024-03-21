/**
  ******************************************************************************
  * @file    boot_hal.c
  * @author  MCD Application Team
  * @brief   This file contains  mcuboot stm32h7rsxx hardware specific implementation
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
#include "boot_hal.h"
#if  defined(MCUBOOT_USE_MCE)
#include "boot_hal_mce.h"
#endif /* MCUBOOT_USE_MCE */
#include "boot_hal_hash_ref.h"
#include "boot_hal_imagevalid.h"
#include "boot_hal_flowcontrol.h"
#include "mcuboot_config/mcuboot_config.h"
#include "uart_stdout.h"
#include "low_level_security.h"
#include "bootutil/boot_record.h"
#include "target_cfg.h"
#include "cmsis.h"
#include "Driver_Flash.h"
#include "region_defs.h"
#include "low_level_rng.h"
#include "low_level_obkeys.h"
#include "low_level_ext_flash.h"
#if (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
#include "low_level_ext_ram.h"
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */
#if  defined(OEMIROT_MCE_PROTECTION)
#include "low_level_mce.h"
#endif /* OEMIROT_MCE_PROTECTION */
#include "low_level_ramecc.h"
#include "bootutil_priv.h"
#include "bootutil/crypto/sha256.h"
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;
extern ARM_DRIVER_FLASH EXT_FLASH_DEV_NAME;


#if defined(MCUBOOT_DOUBLE_SIGN_VERIF)
/* Global variables to memorize images validation status */
#if (MCUBOOT_IMAGE_NUMBER == 1)
uint8_t ImageValidStatus[MCUBOOT_IMAGE_NUMBER] = {IMAGE_INVALID};
#elif (MCUBOOT_IMAGE_NUMBER == 2)
uint8_t ImageValidStatus[MCUBOOT_IMAGE_NUMBER] = {IMAGE_INVALID, IMAGE_INVALID};
#elif (MCUBOOT_IMAGE_NUMBER == 3)
uint8_t ImageValidStatus[MCUBOOT_IMAGE_NUMBER] = {IMAGE_INVALID, IMAGE_INVALID, IMAGE_INVALID};
#elif (MCUBOOT_IMAGE_NUMBER == 4)
uint8_t ImageValidStatus[MCUBOOT_IMAGE_NUMBER] = {IMAGE_INVALID, IMAGE_INVALID, IMAGE_INVALID, IMAGE_INVALID};
#else
#error "MCUBOOT_IMAGE_NUMBER not supported"
#endif /* (MCUBOOT_IMAGE_NUMBER == 1)  */
uint8_t ImageValidIndex = 0;
#endif /* MCUBOOT_DOUBLE_SIGN_VERIF */
#if defined(MCUBOOT_DOUBLE_SIGN_VERIF) || defined(MCUBOOT_USE_HASH_REF)
uint8_t ImageValidEnable = 0;
#endif /* MCUBOOT_DOUBLE_SIGN_VERIF || MCUBOOT_USE_HASH_REF */

#if defined(MCUBOOT_USE_HASH_REF)
#define BL2_HASH_REF_ADDR     (FLASH_HASH_REF_AREA_OFFSET)
uint8_t ImageValidHashUpdate = 0;
uint8_t ImageValidHashRef[MCUBOOT_IMAGE_NUMBER * SHA256_LEN] = {0};
#endif /* MCUBOOT_USE_HASH_REF */

#if defined(FLOW_CONTROL)
/* Global variable for Flow Control state */
volatile uint32_t uFlowProtectValue = FLOW_CTRL_INIT_VALUE;
#endif /* FLOW_CONTROL */
volatile uint32_t uFlowStage = FLOW_STAGE_CFG;

#if !defined(MCUBOOT_OVERWRITE_ONLY)
#define FLAG_SIZE 16
#define CONF_FLAG_ERASE_VALUE 0x00
const uint8_t boot_conf_flag_pattern[FLAG_SIZE] =
{
  0xb7, 0xbb, 0x1f, 0x00, 0x45, 0x64, 0x23, 0x00,
  0x89, 0xa8, 0x23, 0x00, 0x11, 0xd1, 0x23, 0x00
};
/* Place data in a specific section*/
#if defined(__ICCARM__)
#pragma default_variable_attributes = @ ".flagnoinit"
#elif defined(__ARMCC_VERSION)
__attribute__((section(".bss.flagnoinit")))
#elif defined(__GNUC__)
__attribute__((section(".flagnoinit")))
#endif /* __ICCARM__ */
volatile uint8_t boot_conf_flag[FLAG_SIZE];
/* Stop placing data in specified section*/
#if defined(__ICCARM__)
#pragma default_variable_attributes =
#endif /* __ICCARM__ */
#endif /* not MCUBOOT_OVERWRITE_ONLY */

/*
#define ICACHE_MONITOR
*/
#if defined(ICACHE_MONITOR)
#define ICACHE_MONITOR_PRINT() printf("icache monitor - Hit: %x, Miss: %x\r\n", \
                                      HAL_ICACHE_Monitor_GetHitValue(), HAL_ICACHE_Monitor_GetMissValue());
#else
#define ICACHE_MONITOR_PRINT()
#endif /* ICACHE_MONITOR */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup BOOT_HAL_Private_Functions  Private Functions
  * @{
  */
static void boot_jump_to_RSS(uint32_t boot_jump_addr1, uint32_t boot_jump_addr2,
                             uint32_t vector_table, uint32_t MPU_region);
#if defined(OEMIROT_JUMP_TO_BL_ENABLE)
void print_jump_to_bl_warning(void);
#endif /* OEMIROT_JUMP_TO_BL_ENABLE */
void boot_erase_image(void);
void icache_init(void);
#if !defined(MCUBOOT_OVERWRITE_ONLY)
int boot_check_conf_flag(void);
#endif /* not MCUBOOT_OVERWRITE_ONLY */
#if (OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN)
#if defined(MCUBOOT_USE_HASH_REF)
static int boot_compute_run_img_hash(uint32_t FlowStep, uint32_t FlowCtrl);
#endif /* MCUBOOT_USE_HASH_REF */
#endif /* OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN */
/**
  * @}
  */
#if defined(MCUBOOT_EXT_LOADER)
/**
  * @brief This function manage the jump to boot loader application.
  * @note
  * @retval void
  */
void boot_platform_noimage(void)
{
#if defined(OEMIROT_JUMP_TO_BL_ENABLE)
  print_jump_to_bl_warning();
  BOOT_LOG_INF("Jumping to bootloader");
  BOOT_LOG_INF("Disconnect COM port if used by bootloader");
#else /* OEMIROT_JUMP_TO_BL_ENABLE */
  BOOT_LOG_INF("In closed mode, please use DA 'Forced Download' to be able to use bootloader");
  BOOT_LOG_INF("Before doing so, disconnect COM port if used by bootloader");
#endif /* OEMIROT_JUMP_TO_BL_ENABLE */

  /* Check Flow control */
  FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_STAGE_2);
  uFlowStage = FLOW_STAGE_CFG;

  /* Update run time protections for application execution */
  LL_SECU_UpdateLoaderRunTimeProtections();

  /* Erase images (primary only slot) to ensure confidentiality when BL is run */
  boot_erase_image();

  /* Check Flow control */
  FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_STAGE_3_L);
  uFlowStage = FLOW_STAGE_CHK;

  /* Second function call to resist to basic hardware attacks */
  LL_SECU_UpdateLoaderRunTimeProtections();

  /* Second function call to resist to basic hardware attacks */
  boot_erase_image();

#if (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
  if (Ext_Ram_Uninitialize() != HAL_OK)
  {
    BOOT_LOG_ERR("Error while de-initializing External Ram Interface");
    Error_Handler();
  }
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

  /* Check Flow control */
  FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_STAGE_4_L);

#if defined(OEMIROT_JUMP_TO_BL_ENABLE)
  /* Remove clock protection on XSPI memory interface */
  HAL_RCCEx_DisableClockProtection(RCC_CLOCKPROTECT_XSPI);

#if defined(OEMIROT_USER_SRAM_ECC)
  /* Disable RAMECC protection */
  LL_RAMECC_DeInit();
#endif /* OEMIROT_USER_SRAM_ECC */

  /* Jump into BL through RSS */
  boot_jump_to_RSS((uint32_t)&boot_jump_to_RSS, (uint32_t)RSSLIB_PFUNC->JumpHDPLvl3, (uint32_t)BOOTLOADER_BASE,
                   MPU_REGION_BL);

  /* Avoid compiler to pop registers after having changed MSP */
#if !defined(__ICCARM__)
  __builtin_unreachable();
#else
  while (1);
#endif /* defined(__ICCARM__) */
#else /* not OEMIROT_JUMP_TO_BL_ENABLE */
  NVIC_SystemReset();
#endif /* OEMIROT_JUMP_TO_BL_ENABLE */
}
#endif /* MCUBOOT_EXT_LOADER */
/**
  * @brief  Jumping to next image using RSS service, after erasing SRAM twice
  * @param  boot_jump_addr1 boot_jump_to_RSS address in order to execute twice SRAM erasing
  * @param  boot_jump_addr2 RSS service address
  * @param  vector table vector table of the next image
  * @param  MPU_region MPU region to be enabled (not applicable if RSS service is jumping to NS image)
  */
__attribute__((naked)) void boot_jump_to_RSS(uint32_t boot_jump_addr1, uint32_t boot_jump_addr2,
                                             uint32_t vector_table, uint32_t MPU_region)
{
  __ASM volatile(
#if !defined(__ICCARM__)
    ".syntax unified                 \n"
#endif /* not __ICCARM__ */
    "mov     r7, r0                  \n"
    "mov     r8, r1                  \n"
    "mov     r9, r2                  \n"
    "mov     r10, r3                 \n"
    "bl      boot_clear_bl2_ram_area \n" /* Clear RAM before jump */
    "movs    r0, #0                  \n" /* Clear registers: R0-R12, */
    "mov     r1, r0                  \n" /* except R7/R8/R9/R10 */
    "mov     r2, r0                  \n"
    "mov     r3, r0                  \n"
    "mov     r4, r0                  \n"
    "mov     r5, r0                  \n"
    "mov     r6, r0                  \n"
    "mov     r11, r0                 \n"
    "mov     r12, r0                 \n"
    "mov     lr,  r0                 \n"
    "mov     r0, r8                  \n"
    "mov     r1, r9                  \n"
    "mov     r2, r10                 \n"
    "mov     r8, r3                  \n"
    "mov     r9, r3                  \n"
    "mov     r10, r3                 \n"
    "bx      r7                      \n" /* Jump to RSS service */
  );
}
/**
  * @brief This function manage the jump to secure application.
  * @note
  * @retval void
  */
void boot_platform_quit(struct boot_arm_vector_table *vector)
{
  static struct boot_arm_vector_table *vt;
#if  (OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN)
  struct image_header *primary_hdr;
  uint32_t size;
  uint32_t tmp_vt;
#endif /* OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN */
#if defined(MCUBOOT_DOUBLE_SIGN_VERIF)
  uint32_t image_index;

  (void)fih_delay();
  /* Check again if images have been validated, to resist to basic hw attacks */
  for (image_index = 0; image_index < MCUBOOT_IMAGE_NUMBER; image_index++)
  {
    if (ImageValidStatus[image_index] != IMAGE_VALID)
    {
      BOOT_LOG_ERR("Error while double controlling images validation");
      Error_Handler();
    }
  }
#endif /* MCUBOOT_DOUBLE_SIGN_VERIF */


#if defined(MCUBOOT_USE_HASH_REF)
  /* Store new hash references in flash for next boot */
  if (ImageValidHashUpdate)
  {
    if (boot_hash_ref_store())
    {
      BOOT_LOG_ERR("Error while storing hash references");
      Error_Handler();
    }
  }
#endif /* MCUBOOT_USE_HASH_REF */

  /* Check Flow control state */
  FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_STAGE_2);
  uFlowStage = FLOW_STAGE_CFG;


#if (OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN)
  /* Get the header structure from primary slot */
  primary_hdr = (struct image_header *) PRIMARY_PARTITION_START;

  /* Hash is computed over image header and image itself.
   * If protected TLVs are present they are also hashed.
   * All these sections need to be copied. */
  size = primary_hdr->ih_hdr_size + primary_hdr->ih_img_size + primary_hdr->ih_protect_tlv_size;

  /* Copy from primary slot to primary run slot (before updating the MPU configuration).
   * The run time MPU configuration removes the access to the external flash.
   * vt should already be up-to-date with the values from the primary partition. */
  memcpy((void *)PRIMARY_RUN_PARTITION_START, (void *)PRIMARY_PARTITION_START, size);

#if defined(MCUBOOT_USE_HASH_REF)
  if (boot_compute_run_img_hash(FLOW_STEP_HASH_A_CH1, FLOW_CTRL_HASH_A_CH1) != 0)
  {
    Error_Handler();
  }
#endif /* MCUBOOT_USE_HASH_REF */
#endif /* OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN */

  RNG_DeInit();

  ICACHE_MONITOR_PRINT()

#ifdef OEMIROT_ICACHE_ENABLE
  /* Invalidate ICache before jumping to application */
  if (HAL_ICACHE_Invalidate() != HAL_OK)
  {
    Error_Handler();
  }
#endif /* ICACHE_ENABLED */

  SCB_DisableICache();
  SCB_DisableDCache();

#if  (OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN)
  /* Use a temporary variable to avoid cast issues during operations */
  tmp_vt = (uint32_t)vector ;
  tmp_vt = tmp_vt - PRIMARY_PARTITION_START ;    /* code offset in primary partition */
  tmp_vt = tmp_vt + PRIMARY_RUN_PARTITION_START; /* address in primary run partition */
  vt = (struct boot_arm_vector_table *)tmp_vt;

#if defined(MCUBOOT_USE_HASH_REF)
  if (boot_compute_run_img_hash(FLOW_STEP_HASH_A_CH2, FLOW_CTRL_HASH_A_CH2) != 0)
  {
    Error_Handler();
  }
#endif /* MCUBOOT_USE_HASH_REF */
#else /* not (OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN) */
  vt = (struct boot_arm_vector_table *)vector;
#endif /* OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN */

  /* Update run time protections for application execution */
  LL_SECU_UpdateRunTimeProtections();

  /* Check Flow control state */
  FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_STAGE_3_A);
  uFlowStage = FLOW_STAGE_CHK;

  /* Double the update of run time protections, to resist to basic hardware attacks */
  LL_SECU_UpdateRunTimeProtections();

  /* Check Flow control for dynamic protections */
  FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_STAGE_4_A);

#if  (OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN)
  if (EXT_FLASH_DEV_NAME.Uninitialize() != ARM_DRIVER_OK)
  {
    BOOT_LOG_ERR("Error while uninitializing External Flash Interface");
    Error_Handler();
  }
#endif /* OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN */

  boot_jump_to_RSS((uint32_t)&boot_jump_to_RSS, (uint32_t)RSSLIB_PFUNC->JumpHDPLvl3, (uint32_t)vt, MPU_REGION_APP);
  /* Avoid compiler to pop registers after having changed MSP */
#if !defined(__ICCARM__)
  __builtin_unreachable();
#else
  while (1);
#endif /* defined(__ICCARM__) */
}


/**
  * @brief This function is called to clear all RAM area before jumping in
  * in Secure application .
  * @note
  * @retval void
  */
void boot_clear_bl2_ram_area(void)
{
  __IO uint32_t *pt = (uint32_t *)BL2_DATA_START;
  uint32_t index;

  for (index = 0; index < (BL2_DATA_SIZE / 4); index++)
  {
    pt[index] = 0;
  }

#if  defined(OEMIROT_MCE_PROTECTION)
  pt = (uint32_t *)DMA_SHARED_MEM_START;
  for (index = 0; index < (DMA_SHARED_MEM_SIZE / 4); index++)
  {
    pt[index] = 0;
  }
#endif /* OEMIROT_MCE_PROTECTION */
}


#if defined(MCUBOOT_USE_HASH_REF)
/**
  * @brief This function store all hash references in flash
  * @return 0 on success; nonzero on failure.
  */
int boot_hash_ref_store(void)
{
#if  defined(OEMUROT_ENABLE)
  OBK_Hdpl2Data_t OBK_hdpl2_data = { 0U };
#else /* not OEMUROT_ENABLE */
  OBK_Hdpl1Data_t OBK_hdpl1_data = { 0U };
#endif /* OEMUROT_ENABLE */
  /* Read all OBK hdpl1/2 data & control SHA256 */
#if  defined(OEMUROT_ENABLE)
  if (OBK_ReadHdpl2Data(&OBK_hdpl2_data) !=  HAL_OK)
#else /* not OEMUROT_ENABLE */
  if (OBK_ReadHdpl1Data(&OBK_hdpl1_data) !=  HAL_OK)
#endif /* OEMUROT_ENABLE */
  {
    return BOOT_EFLASH;
  }

  /* update hash references */
#if  defined(OEMUROT_ENABLE)
  memcpy(&OBK_hdpl2_data.CodeSHA256Ref[0], ImageValidHashRef, (SHA256_LENGTH * MCUBOOT_IMAGE_NUMBER));
#else /* not OEMUROT_ENABLE */
  memcpy(&OBK_hdpl1_data.CodeSHA256Ref[0], ImageValidHashRef, (SHA256_LENGTH * MCUBOOT_IMAGE_NUMBER));
#endif /* OEMUROT_ENABLE */

  /* Update all OBK hdpl1 data with associated hash references */
#if  defined(OEMUROT_ENABLE)
  if (OBK_UpdateHdpl2Data(&OBK_hdpl2_data) != HAL_OK)
#else /* not OEMUROT_ENABLE */
  if (OBK_UpdateHdpl1Data(&OBK_hdpl1_data) != HAL_OK)
#endif /* OEMUROT_ENABLE */
  {
    return BOOT_EFLASH;
  }

  return 0;
}

/**
  * @brief This function load all hash references from flash
  * @return 0 on success; nonzero on failure.
  */
int boot_hash_ref_load(void)
{
#if  defined(OEMUROT_ENABLE)
  OBK_Hdpl2Data_t OBK_hdpl2_data = { 0U };
#else /* not OEMUROT_ENABLE */
  OBK_Hdpl1Data_t OBK_hdpl1_data = { 0U };
#endif /* OEMUROT_ENABLE */

  /* Read all OBK hdpl1 data & control SHA256 */
#if  defined(OEMUROT_ENABLE)
  if (OBK_ReadHdpl2Data(&OBK_hdpl2_data) != HAL_OK)
#else /* not OEMUROT_ENABLE */
  if (OBK_ReadHdpl1Data(&OBK_hdpl1_data) != HAL_OK)
#endif /* OEMUROT_ENABLE */
  {
    return BOOT_EFLASH;
  }

  /* update hash references */
#if  defined(OEMUROT_ENABLE)
  memcpy(ImageValidHashRef, &OBK_hdpl2_data.CodeSHA256Ref[0], (SHA256_LENGTH * MCUBOOT_IMAGE_NUMBER));
#else /* not OEMUROT_ENABLE */
  memcpy(ImageValidHashRef, &OBK_hdpl1_data.CodeSHA256Ref[0], (SHA256_LENGTH * MCUBOOT_IMAGE_NUMBER));
#endif /* OEMUROT_ENABLE */

  return 0;
}

/**
  * @brief This function set one hash reference in ram
  * @param hash_ref hash reference to update
  * @param size size of the hash references
  * @param image_index index of image corresponding to hash reference
  * @return 0 on success; nonzero on failure.
  */
int boot_hash_ref_set(uint8_t *hash_ref, uint8_t size, uint8_t image_index)
{
  /* Check size */
  if (size != SHA256_LEN)
  {
    return BOOT_EFLASH;
  }

  /* Check image index */
  if (image_index >= MCUBOOT_IMAGE_NUMBER)
  {
    return BOOT_EFLASH;
  }

  /* Set hash reference */
  memcpy(ImageValidHashRef + (image_index * SHA256_LEN), hash_ref, SHA256_LEN);

  /* Memorize that hash references will have to be updated in flash (later) */
  ImageValidHashUpdate++;

  return 0;
}

/**
  * @brief This function get one hash reference from ram
  * @param hash_ref hash reference to get
  * @param size size of the hash reference
  * @param image_index index of image corresponding to hash reference
  * @return 0 on success; nonzero on failure.
  */
int boot_hash_ref_get(uint8_t *hash_ref, uint8_t size, uint8_t image_index)
{
  /* Check size */
  if (size != SHA256_LEN)
  {
    return BOOT_EFLASH;
  }

  /* Check image index */
  if (image_index >= MCUBOOT_IMAGE_NUMBER)
  {
    return BOOT_EFLASH;
  }

  /* Get hash reference */
  memcpy(hash_ref, ImageValidHashRef + (image_index * SHA256_LEN), SHA256_LEN);

  return 0;
}
#endif /* MCUBOOT_USE_HASH_REF */

#ifdef MCUBOOT_USE_MCE
/**
  * @brief This function check if a buffer is in primary code
  * @param fa_id identifier of the flash area
  * @param offset_in_flash offset of the buffer in the flash area
  * @param len size of the buffer
  * @return true if the buffer is in primary code; false otherwise.
  */
bool boot_is_in_primary(uint8_t fa_id, uint32_t offset_in_flash, size_t len)
{
  uint32_t buffer_in_flash;
  uint32_t primary_start = EXT_FLASH_BASE_ADDRESS + FLASH_AREA_0_OFFSET;
  uint32_t primary_end = EXT_FLASH_BASE_ADDRESS + FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE - 1;

  /* Check the fa_id */
  if (fa_id == FLASH_AREA_0_ID)
  {
    buffer_in_flash = EXT_FLASH_BASE_ADDRESS + offset_in_flash;
  }
  else
  {
    return false;
  }

  /* Check the address */
  if ((buffer_in_flash >= primary_start) && (buffer_in_flash <= primary_end))
  {
    return true;
  }
  if (((buffer_in_flash + len) >= primary_start) && ((buffer_in_flash + len) <= primary_end))
  {
    return true;
  }
  return false;
}

/**
  * @brief This function check if a buffer is erased, taking MCE into account
  * @param fa_id identifier of the flash area
  * @param offset_in_flash offset of the buffer in the flash area
  * @param len size of the buffer
  * @param erased_val value found in the erased flash
  * @return true if the buffer is erased; false otherwise.
  */
bool boot_is_in_primary_and_erased(uint8_t fa_id, uint32_t offset_in_flash, size_t len, uint8_t erased_val)
{
  size_t i;
  uint8_t *buffer_in_flash;
  uint8_t *u8b;
#if  defined(OEMIROT_MCE_PROTECTION)
  bool crypted_content_erased = true;
#endif /* OEMIROT_MCE_PROTECTION */
  bool decrypted_content_erased = true;

  /* Check the location of the buffer */
  if (boot_is_in_primary(fa_id, offset_in_flash, len) == false)
  {
    return false;
  }

  /* Get the address */
  buffer_in_flash = (uint8_t *)(EXT_FLASH_BASE_ADDRESS + offset_in_flash);

  /* Check the buffer without checking MCE status:
   *   - MCE enabled: the buffer can have been forced to "erased_val"
   *   - MCE disabled: the external memory can be truly erased */
  for (i = 0, u8b = buffer_in_flash; i < len; i++)
  {
    if (u8b[i] != erased_val)
    {
      decrypted_content_erased = false;
    }
  }

#if  defined(OEMIROT_MCE_PROTECTION)
  /* Check the buffer is in the encrypted area */
  if (LL_MCE_IsBufferInCodePrimaryRegion(buffer_in_flash, len) == false)
  {
    return decrypted_content_erased;
  }

  /* Check whether the buffer is encrypted */
  if (LL_MCE_IsCodePrimaryRegionEnabled() == false)
  {
    return decrypted_content_erased;
  }

  /* Check the content of the external flash without MCE */
  if (LL_MCE_DisableCodePrimaryRegion() != HAL_OK)
  {
    Error_Handler();
  }
  for (i = 0, u8b = buffer_in_flash; i < len; i++)
  {
    if (u8b[i] != erased_val)
    {
      crypted_content_erased = false;
    }
  }
  if (LL_MCE_EnableCodePrimaryRegion() != HAL_OK)
  {
    Error_Handler();
  }

  /* Return the result */
  if ((decrypted_content_erased == true) || (crypted_content_erased == true))
  {
    return true;
  }
  return false;
#else /* OEMIROT_MCE_PROTECTION */
  return decrypted_content_erased;
#endif /* OEMIROT_MCE_PROTECTION */
}
#endif /* MCUBOOT_USE_MCE */

/**
  * @brief Erase image (code + data)
  * @note
  * @retval execution_status
  */
void boot_erase_image(void)
{
  uint32_t sector;

  /* Erase code image */
  for (sector = FLASH_AREA_0_OFFSET;
       sector < (FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE);
       sector += FLASH_AREA_IMAGE_SECTOR_SIZE)
  {
    if (EXT_FLASH_DEV_NAME.EraseSector(sector) != ARM_DRIVER_OK)
    {
      Error_Handler();
    }
  }

}
/**
  * @brief This function configures and enables the ICache.
  * @note
  * @retval execution_status
  */
void icache_init(void)
{
#ifdef ICACHE_MONITOR
  if (HAL_ICACHE_Monitor_Reset(ICACHE_MONITOR_HIT_MISS) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Monitor_Start(ICACHE_MONITOR_HIT_MISS) != HAL_OK)
  {
    Error_Handler();
  }
#endif /* ICACHE_MONITOR */
  ICACHE_MONITOR_PRINT()

  /* Enable ICache */
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief This function prints a warning regarding the jump to BL.
  * @note
  * @retval void
  */
#if defined(OEMIROT_JUMP_TO_BL_ENABLE)
void print_jump_to_bl_warning(void)
{
  BOOT_LOG_WRN("!!! Jumping into BL from OEMIROT is not recommended !!!");
  BOOT_LOG_WRN("!!! This feature should only be used in development mode !!!");
  BOOT_LOG_WRN("!!! The secure method is using DA 'Force Download' !!!");
}
#endif /* OEMIROT_JUMP_TO_BL_ENABLE */

#if !defined(MCUBOOT_OVERWRITE_ONLY)
/**
  * Check the confirmation flag in RAM.
  *
  * @return                  0 on success; nonzero on failure.
  */
int boot_check_conf_flag(void)
{
  int rc = 0U;
  uint32_t i;

  for (i = 0U; i < sizeof(boot_conf_flag_pattern); i++)
  {
    rc |= (boot_conf_flag_pattern[i] ^ boot_conf_flag[i]);
  }

  return rc;
}
#endif /* not MCUBOOT_OVERWRITE_ONLY */

#if (OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN)
#if defined(MCUBOOT_USE_HASH_REF)
/**
  * Compute SHA256 over the run image.
  *
  * @param  FlowStep Flow step
  * @param  FlowCtrl Flow control
  * @return 0 on success; nonzero on failure.
  */
static int boot_compute_run_img_hash(uint32_t FlowStep, uint32_t FlowCtrl)
{
  uint32_t size;
  bootutil_sha256_context sha256_ctx;
  struct image_header *hdr;
  uint8_t hash_ref[32];
  uint8_t hash_run[32];
  fih_int fih_rc;

  /* Get the reference hash */
  if (boot_hash_ref_get(hash_ref, sizeof(hash_ref), 0) != 0)
  {
    return BOOT_EBADSTATUS;
  }

  /* set Hash on area */
  bootutil_sha256_init(&sha256_ctx);

  /* Get the header structure from primary run slot */
  hdr = (struct image_header *) PRIMARY_RUN_PARTITION_START;

  /* Hash is computed over image header and image itself. */
  /* If protected TLVs are present they are also hashed. */
  size = hdr->ih_hdr_size + hdr->ih_img_size + hdr->ih_protect_tlv_size;

  /* compute write protect area set as excuteable */
  bootutil_sha256_update(&sha256_ctx,
                         (void *)PRIMARY_RUN_PARTITION_START,
                         size);

  bootutil_sha256_finish(&sha256_ctx, hash_run);

  FIH_CALL(boot_fih_memequal, fih_rc, hash_ref, hash_run, sizeof(hash_ref));
  if (fih_not_eq(fih_rc, FIH_SUCCESS))
  {
    return BOOT_EBADIMAGE;
  }

  /* Flow control */
  FLOW_CONTROL_STEP(uFlowProtectValue, FlowStep, FlowCtrl);

  return 0;
}
#endif /* OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN */
#endif /* MCUBOOT_USE_HASH_REF */

/* exported variables --------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  Platform init
  * @param  None
  * @retval status
  */
int32_t boot_platform_init(void)
{
  /* Take the changes from SetSysClock into account */
  SystemCoreClockUpdate();

  /* STM32H7RSxx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Set NVIC Group Priority to 3
       - Low Level Initialization
     */
  HAL_Init();
#ifdef OEMIROT_DEV_MODE
  /* Init for log */
  stdio_init();
#endif /*  OEMIROT_DEV_MODE */

  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();

#ifdef OEMIROT_ICACHE_ENABLE
  /* Configure and enable ICache */
  icache_init();
#endif /* ICACHE_ENABLED */

  /* Reload Independent Watchdog */
  IWDG->KR = KR_RELOAD;

  /* Start HW randomization */
  RNG_Init();

  (void)fih_delay_init();

  /* disable and clean previous mpu config */
  LL_SECU_DisableCleanMpu();

#if defined(OEMIROT_JUMP_TO_BL_ENABLE)
  print_jump_to_bl_warning();
#endif /* OEMIROT_JUMP_TO_BL_ENABLE */

#if  defined(OEMUROT_ENABLE)
  /* Initialize the HDPL2 data area selector */
  OBK_InitHdpl2DataSel();

  /* Control the HDPL2 data area selector */
  OBK_ControlHdpl2DataSel();

  /* Control Flash Layout configuration */
  OBK_ControlHdpl2Data();
#else /* not OEMUROT_ENABLE */
  /* Initialize the HDPL1 data area selector */
  OBK_InitHdpl1DataSel();

  /* Control the HDPL1 data area selector */
  OBK_ControlHdpl1DataSel();

  /* Control Flash Layout configuration */
  OBK_ControlHdpl1Data();
#endif /* OEMUROT_ENABLE */

  /* Apply Run time Protection */
  LL_SECU_ApplyRunTimeProtections();
  /* Check static protections */
  LL_SECU_CheckStaticProtections();

  if (OBK_IsEncryptionEnabled())
  {
    BOOT_LOG_DBG("OBK encryption is supported");
  }
  else
  {
    BOOT_LOG_DBG("OBK encryption is not supported");
  }

  /* Configure keys */
#if  defined(OEMUROT_ENABLE)
  OBK_ReadHdpl2Config(&OBK_Hdpl2Cfg);
#else /* not OEMUROT_ENABLE */
  OBK_ReadHdpl1Config(&OBK_Hdpl1Cfg);
#endif /* OEMUROT_ENABLE */

  /* Check Flow control state */
  FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_STAGE_1);
  uFlowStage = FLOW_STAGE_CHK;
  /* Double protections apply / check to resist to basic fault injections */
  /* Apply Run time Protection */
  (void)fih_delay();
  LL_SECU_ApplyRunTimeProtections();
  /* Check static protections */
  LL_SECU_CheckStaticProtections();

  /* Control Hdpl1/2 config */
#if  defined(OEMUROT_ENABLE)
  OBK_VerifyHdpl2Config(&OBK_Hdpl2Cfg);
#else /* not OEMUROT_ENABLE */
  OBK_VerifyHdpl1Config(&OBK_Hdpl1Cfg);
#endif /* OEMUROT_ENABLE */

  if (FLASH_DEV_NAME.Initialize(NULL) != ARM_DRIVER_OK)
  {
    BOOT_LOG_ERR("Error while initializing Flash Interface");
    Error_Handler();
  }

#if defined(MCUBOOT_USE_HASH_REF)
  /* Load all images hash references (for mcuboot) */
  if (boot_hash_ref_load())
  {
    BOOT_LOG_ERR("Error while loading Hash references from FLash");
    Error_Handler();
  }
#endif /* MCUBOOT_USE_HASH_REF */

  if (EXT_FLASH_DEV_NAME.Initialize(NULL) != ARM_DRIVER_OK)
  {
    BOOT_LOG_ERR("Error while initializing External Flash Interface");
    Error_Handler();
  }

#if (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
  if (Ext_Ram_Initialize() != HAL_OK)
  {
    BOOT_LOG_ERR("Error while initializing External Ram Interface");
    Error_Handler();
  }
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

#if !defined(MCUBOOT_OVERWRITE_ONLY)
  if (boot_check_conf_flag())
  {
    BOOT_LOG_DBG("No confirmation required");
  }
  else
  {
    if (boot_set_confirmed() == 0U)
    {
      BOOT_LOG_DBG("Confirm Flag correctly set");
    }
    else
    {
      BOOT_LOG_ERR("Confirm Flag Not Correctly Written");
      Error_Handler();
    }
  }
  memset((void *)boot_conf_flag, CONF_FLAG_ERASE_VALUE, sizeof(boot_conf_flag));
#ifndef OEMIROT_MPU_PROTECTION
  SCB_CleanDCache();
#endif /* not OEMIROT_MPU_PROTECTION */
#endif /* not MCUBOOT_OVERWRITE_ONLY */

  return 0;
}

/**
  * @brief  This function is executed in case of error occurrence.
  *         This function does not return.
  * @param  None
  * @retval None
  */
#ifdef OEMIROT_ERROR_HANDLER_STOP_EXEC
/* Place code in a specific section */
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_Error_Code"
#else /* not __ICCARM__ */
__attribute__((section(".BL2_Error_Code")))
#endif /* __ICCARM__ */
void Error_Handler(void)
{
  while (1);
}
#else /* OEMIROT_ERROR_HANDLER_STOP_EXEC */
/* Place code in a specific section */
#if defined(__ICCARM__)
#pragma default_function_attributes = @ ".BL2_Error_Code"
__NO_RETURN void Error_Handler(void)
#else /* not __ICCARM__ */
__attribute__((section(".BL2_Error_Code")))
void Error_Handler(void)
#endif /* __ICCARM__ */
{
  /* It is customizeable */
  NVIC_SystemReset();
#if !defined(__ICCARM__)
  /* Avoid bx lr instruction (for fault injection) */
  __builtin_unreachable();
#endif /* defined(__ICCARM__) */
}
#endif /* OEMIROT_ERROR_HANDLER_STOP_EXEC */

/* Stop placing data in specified section */
#if defined(__ICCARM__)
#pragma default_function_attributes =
#endif /* __ICCARM__ */

#if defined(__ARMCC_VERSION)
/* reimplement the function to reach Error Handler */
void __aeabi_assert(const char *expr, const char *file, int line)
{
#ifdef OEMIROT_DEV_MODE
  printf("assertion \" %s \" failed: file %s %d\n", expr, file, line);
#endif /*  OEMIROT_DEV_MODE  */
  Error_Handler();
}
#endif  /*  __ARMCC_VERSION */
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  Error_Handler();
}
#endif /* USE_FULL_ASSERT */
