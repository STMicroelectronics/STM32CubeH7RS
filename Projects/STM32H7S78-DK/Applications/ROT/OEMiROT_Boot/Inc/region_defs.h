/*
 * Copyright (c) 2017-2018 ARM Limited
 * Copyright (c) 2023 STMicroelectronics
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

#ifndef REGION_DEFS_H
#define REGION_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flash_layout.h"

#define BL2_HEAP_SIZE                       0x00005F0
#define BL2_MSP_STACK_SIZE                  0x0001100

/* Base addresses */
#define DEF_DTCM_BASE                       (0x20000000) /*!< DTCM (64 kB used) */
#define DEF_SRAM1_AXI_BASE                  (0x24000000) /*!< AXI SRAM1 (128 kB) */
#define DEF_SRAM2_AHB_BASE                  (0x30004000) /*!< AHB SRAM2 (16 kB) base address */
#define DEF_FLASH_BASE                      (0x08000000) /*!< FLASH (64 KB) base address */
#if  defined(OEMUROT_ENABLE)
#define BL2_RAM_BASE                        (DEF_SRAM1_AXI_BASE)
#endif /* OEMUROT_ENABLE */
#if (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM)
#define IMAGE_PRIMARY_RUN_PARTITION_BASE    (EXT_RAM_BASE_ADDRESS)
#elif (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM)
#define IMAGE_PRIMARY_RUN_PARTITION_BASE    (SRAM3_AXI_BASE)
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_EXT_RAM */

/* Sizes */
#define DEF_DTCM_SIZE                       (0x10000) /*!< DTCM 64 KB */
#define DEF_SRAM1_AXI_SIZE                  (0x20000)
#define DEF_SRAM2_AHB_SIZE                  (0x4000)
#if  defined(OEMUROT_ENABLE)
#define BL2_RAM_SIZE                        (DEF_SRAM1_AXI_SIZE)
#endif /* OEMUROT_ENABLE */

#define TOTAL_ROM_SIZE                      FLASH_TOTAL_SIZE

/*  This area in SRAM 2 is updated BL2 and can be lock to avoid any changes */
#define BOOT_SHARED_DATA_SIZE               0
#define BOOT_SHARED_DATA_BASE               0

/*
 * Boot partition structure if MCUBoot is used:
 * 0x0_0000 Bootloader header
 * 0x0_0400 Image area
 * 0xz_zzzz Trailer
 */
/* IMAGE_CODE_SIZE is the space available for the software binary image.
 * It is less than the FLASH_PARTITION_SIZE because we reserve space
 * for the image header and trailer introduced by the bootloader.
 */

#define BL2_HEADER_SIZE                     (0x400) /*!< Appli image header size */
#define BL2_DATA_HEADER_SIZE                (0x20)  /*!< Data image header size */
#define BL2_TRAILER_SIZE                    (0x2000)
#ifdef BL2
#define IMAGE_PRIMARY_PARTITION_OFFSET      (FLASH_AREA_0_OFFSET)
#define IMAGE_SECONDARY_PARTITION_OFFSET    (FLASH_AREA_2_OFFSET)
#define SCRATCH_PARTITION_OFFSET            (FLASH_AREA_SCRATCH_OFFSET)
#if (OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN)
#define IMAGE_PRIMARY_RUN_PARTITION_OFFSET  (0x00000000)
#endif /* OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN */
#else
#error "Config without BL2 not supported"
#endif /* BL2 */


#define IMAGE_CODE_SIZE \
  (FLASH_PARTITION_SIZE - BL2_HEADER_SIZE - BL2_TRAILER_SIZE)

/* Alias definitions */
#if  defined(OEMUROT_ENABLE)
#define BL2_RAM_ALIAS(x)                    ((BL2_RAM_BASE) + (x))
#else /* not OEMUROT_ENABLE */
#define BL2_ROM_ALIAS(x)                    ((DEF_FLASH_BASE) + (x))
#endif /* OEMUROT_ENABLE */

/* External Flash Base */
#define ROM_ALIAS(x)                        ((EXT_FLASH_BASE_ADDRESS) + (x))

/* Primary partition for MCE configuration */
#define PRIMARY_PARTITION_START             (ROM_ALIAS(IMAGE_PRIMARY_PARTITION_OFFSET))
#define PRIMARY_PARTITION_SIZE              (FLASH_AREA_0_SIZE)

/* Primary run partition for Load & Run configuration */
#if (OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN)
#define PRIMARY_RUN_PARTITION_START         (IMAGE_PRIMARY_RUN_PARTITION_BASE + IMAGE_PRIMARY_RUN_PARTITION_OFFSET)
#endif /* OEMIROT_LOAD_AND_RUN != NO_LOAD_AND_RUN */

/* Non-secure regions
 *
 * DTCM is more secure (not accessible to GPDMA) and is used for BL2_DATA.
 *
 * The configuration of RAMECC handles and of the confirmation flag
 * is common with the user application. These mappings shall be kept
 * aligned.
 *
 */
#define IMAGE_PRIMARY_AREA_OFFSET           (IMAGE_PRIMARY_PARTITION_OFFSET + BL2_HEADER_SIZE)
#define CODE_START                          (ROM_ALIAS(IMAGE_PRIMARY_AREA_OFFSET))
#define CODE_SIZE                           (IMAGE_CODE_SIZE)
#define DATA_START                          (DEF_DTCM_BASE)
#define DATA_SIZE                           (DEF_DTCM_SIZE)
#define SCRATCH_START                       (ROM_ALIAS(SCRATCH_PARTITION_OFFSET))
#define SCRATCH_SIZE                        (FLASH_AREA_SCRATCH_SIZE)
/* The RAMECC handles and the confirmation flag are at the end of the RAM (shared with the user application).
 * The user application does not use the data noinit area.
 */
#define APP_RAM_START                       (DEF_SRAM2_AHB_BASE)
#define APP_RAM_DATA_SIZE                   (DEF_SRAM2_AHB_SIZE)
#define RAMECC_HANDLE_SIZE                  (0x40)
#define RAMECC_HANDLE_START                 (APP_RAM_START + APP_RAM_DATA_SIZE - RAMECC_HANDLE_SIZE)
#define CONF_FLAG_SIZE                      (0x20)
#define CONF_FLAG_START                     (RAMECC_HANDLE_START - CONF_FLAG_SIZE)
#define DATA_NOINIT_SIZE                    (0x20)
#define DATA_NOINIT_START                   (CONF_FLAG_START - DATA_NOINIT_SIZE)
/* The DMA shared memory is kept accessible to GPDMA in AHB SRAM2 */
#define DMA_SHARED_MEM_SIZE                 (BL2_DMA_SHARED_MEM_SIZE)
#define DMA_SHARED_MEM_MPU_ALIGNMENT        (DMA_SHARED_MEM_SIZE - RAMECC_HANDLE_SIZE - CONF_FLAG_SIZE - DATA_NOINIT_SIZE)
#define DMA_SHARED_MEM_START                (DATA_NOINIT_START - DMA_SHARED_MEM_MPU_ALIGNMENT - DMA_SHARED_MEM_SIZE)

/* Secondary partition for new images/ in case of firmware upgrade */
#define SECONDARY_PARTITION_START           (ROM_ALIAS(IMAGE_SECONDARY_PARTITION_OFFSET))
#define SECONDARY_PARTITION_SIZE            (FLASH_AREA_2_SIZE)

#ifdef BL2
#if  defined(OEMUROT_ENABLE)
#define BL2_CODE_START                      (BL2_RAM_ALIAS(BL2_HEADER_SIZE))
#define BL2_CODE_SIZE                       (BL2_RAM_SIZE)
#else /* not OEMUROT_ENABLE */
/* Bootloader region protected by hdp */
#define BL2_CODE_START                      (BL2_ROM_ALIAS(FLASH_AREA_BL2_OFFSET))
#define BL2_CODE_SIZE                       (FLASH_AREA_BL2_SIZE)
#endif /* OEMUROT_ENABLE */
#define BL2_CODE_LIMIT                      (BL2_CODE_START + BL2_CODE_SIZE - 1)

/* Bootloader boot address */
#define BL2_BOOT_VTOR_ADDR                  (BL2_CODE_START)

/*  keep 256 bytes unused to place while(1) for non secure to enable */
/*  regression from local tool with non secure attachment
 *  This avoid blocking board in case of hardening error */
#define BL2_DATA_START                      (DATA_START)
#define BL2_DATA_SIZE                       (DATA_SIZE)
#define BL2_DATA_LIMIT                      (BL2_DATA_START + BL2_DATA_SIZE - 1)

#define BL2_RAMECC_HANDLE_START             (RAMECC_HANDLE_START)
#define BL2_RAMECC_HANDLE_SIZE              (RAMECC_HANDLE_SIZE)
#define BL2_RAMECC_HANDLE_LIMIT             (BL2_RAMECC_HANDLE_START + BL2_RAMECC_HANDLE_SIZE - 1)

#define BL2_CONF_FLAG_START                 (CONF_FLAG_START)
#define BL2_CONF_FLAG_SIZE                  (CONF_FLAG_SIZE)
#define BL2_CONF_FLAG_LIMIT                 (BL2_CONF_FLAG_START + BL2_CONF_FLAG_SIZE - 1)

#define BL2_DATA_NOINIT_START               (DATA_NOINIT_START)
#define BL2_DATA_NOINIT_SIZE                (DATA_NOINIT_SIZE)
#define BL2_DATA_NOINIT_LIMIT               (BL2_DATA_NOINIT_START + BL2_DATA_NOINIT_SIZE - 1)

#define BL2_DMA_SHARED_MEM_START            (DMA_SHARED_MEM_START)

/* Define BL2 MPU SRAM protection to remove execution capability */
/* Area is covering the complete SRAM memory space alias */
#define BL2_SRAM_AREA_BASE                  (DATA_START)
#define BL2_SRAM_AREA_END                   (DATA_START + DATA_SIZE - 1)
#endif /* BL2 */


/* Additional Check to detect flash download slot overlap or overflow */
#define FLASH_AREA_END_OFFSET_MAX (EXT_FLASH_TOTAL_SIZE)
#if FLASH_AREA_END_OFFSET > FLASH_AREA_END_OFFSET_MAX
#error "Flash memory overflow"
#endif /* FLASH_AREA_END_OFFSET > FLASH_AREA_END_OFFSET_MAX */

#ifdef __cplusplus
}
#endif

#endif /* REGION_DEFS_H */
