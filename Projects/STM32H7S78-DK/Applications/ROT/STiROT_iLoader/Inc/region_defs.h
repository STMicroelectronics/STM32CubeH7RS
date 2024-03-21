/**
  ******************************************************************************
  * @file    region_defs.h
  * @author  MCD Application Team
  * @brief   This file contains the definitions related to the memory slots.
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef REGION_DEFS_H
#define REGION_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Address of the external flash */
#define EXTERNAL_FLASH_SLOT_START_1  (XSPI1_BASE)
#define EXTERNAL_FLASH_SLOT_START_2  (XSPI2_BASE)
#define EXTERNAL_FLASH_SLOT_SIZE   (0x10000000U)

/* Shared Volatile Memory */
#if defined (__CC_ARM) || defined(__ARMCC_VERSION)
extern uint32_t Image$$SHARED_MEM_region$$Base;

#define __region_SHARED_MEM_start__ ( (uint32_t) & Image$$SHARED_MEM_region$$Base)
#define __region_SHARED_MEM_end__   ( ((uint32_t) & Image$$SHARED_MEM_region$$Base) + \
                                      (0x40U - 1U) )

#define SHARED_RAM_START (__region_SHARED_MEM_start__)
#define SHARED_RAM_END (__region_SHARED_MEM_end__)

#elif defined(__ICCARM__) || defined(__GNUC__)
extern uint32_t __region_SHARED_MEM_start__;
extern uint32_t __region_SHARED_MEM_end__;
#define SHARED_RAM_START ((uint32_t) &__region_SHARED_MEM_start__)
#define SHARED_RAM_END ((uint32_t) &__region_SHARED_MEM_end__)
#endif /* __ICCARM__ || __GNUC__ */

/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/** @brief  Check the configuration of the VM shared memory
  * @retval TRUE or FALSE
  */
#define IS_VM_SHARED_MEM_VALID() ((SHARED_RAM_START < SHARED_RAM_END) \
                                  && ((SHARED_RAM_END - SHARED_RAM_START + 1) >= sizeof(LL_SHARED_MEM_Config_t)) \
                                  && (SHARED_RAM_START >= DTCM_BASE) \
                                  && (SHARED_RAM_END < (DTCM_BASE + DTCM_SIZE)))

/** @brief  Check that the size of the slot is valid
  * @param  __SIZE__  Size of the slot
  * @retval TRUE or FALSE
  */
#define IS_SLOT_SIZE_VALID(__SIZE__) ((__SIZE__ != 0U) \
                                      && ((__SIZE__ % ERASE_BLOC_SIZE) == 0U))

/** @brief  Check that the slot is in volatile memory
  * @param  __START__ Start address of the slot
  * @param  __SIZE__  Size of the slot
  * @retval TRUE or FALSE
  */
#define IS_SLOT_IN_VM(__START__, __SIZE__) ((__START__ >= (uint32_t)SRAM1_AXI_BASE) \
                                            && ((__START__ + __SIZE__ - 1U) <= \
                                                ((uint32_t)SRAM4_AXI_BASE + (uint32_t)SRAM4_AXI_SIZE -1U)))

/** @brief  Check that the slot is in non volatile memory
  * @param  __START__ Start address of the slot
  * @param  __SIZE__  Size of the slot
  * @retval TRUE or FALSE
  */
#define IS_SLOT_IN_NVM(__START__, __SIZE__) (((__SIZE__ % ERASE_BLOC_SIZE) == 0U) \
                                             && ((__START__ % ERASE_BLOC_SIZE) == 0U) \
                                             && (IS_SLOT_IN_NVM_AREA(__START__, __SIZE__, EXTERNAL_FLASH_SLOT_START_1) || \
                                                 IS_SLOT_IN_NVM_AREA(__START__, __SIZE__, EXTERNAL_FLASH_SLOT_START_2)))


/** @brief  Check that the slot is in non volatile area
  * @param  __START__ Start address of the slot
  * @param  __SIZE__  Size of the slot
  * @param  __AREA__  Area address
  * @retval TRUE or FALSE
  */
#define IS_SLOT_IN_NVM_AREA(__START__, __SIZE__, __AREA__) ((__START__ >= __AREA__) \
                                                            && (__START__ + __SIZE__ <= \
                                                                (__AREA__ + EXTERNAL_FLASH_SLOT_SIZE)))

/** @brief  Check that the slots are not overlapping
  * @param  __START_1__ Start address of the first slot
  * @param  __SIZE_1__  Size of the first slot
  * @param  __START_2__ Start address of the second slot
  * @param  __SIZE_2__  Size of the second slot
  * @retval TRUE or FALSE
  */
#define ARE_SLOTS_SEPARATE(__START_1__, __SIZE_1__, __START_2__, __SIZE_2__) \
  (((__START_1__ + __SIZE_1__ - 1) < __START_2__) \
   || ((__START_2__ + __SIZE_2__ - 1) < __START_1__))

/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* REGION_DEFS_H */
