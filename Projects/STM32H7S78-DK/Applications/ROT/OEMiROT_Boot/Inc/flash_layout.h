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

#ifndef FLASH_LAYOUT_H
#define FLASH_LAYOUT_H

#ifdef __cplusplus
extern "C" {
#endif

/* This header file is included from linker scatter file as well, where only a
 * limited C constructs are allowed. Therefore it is not possible to include
 * here the platform_retarget.h to access flash related defines. To resolve this
 * some of the values are redefined here with different names, these are marked
 * with comment.
 */

/* Flash layout configuration : begin ****************************************/
/* OEMiROT/STiROT_OEMuROT configuration */
/*#define OEMUROT_ENABLE*/         /* Defined: the project is used for STiROT_OEMuROT boot path
                                      Undefined: the project is used for OEMiROT boot path */

#define NO_LOAD_AND_RUN      (0)                          /*!< OEMiROT_Boot uses "Execute In Place" */
#define LOAD_AND_RUN_INT_RAM (1)                          /*!< OEMiROT_Boot uses "Load & Run" with internal RAM memory */
#define LOAD_AND_RUN_EXT_RAM (2)                          /*!< OEMiROT_Boot uses "Load & Run" with external RAM memory */
#define OEMIROT_LOAD_AND_RUN LOAD_AND_RUN_EXT_RAM /*!< Flag configuring the feature "Load and Run" */

#define MCUBOOT_OVERWRITE_ONLY     /* Defined: the FW installation uses overwrite method.
                                      UnDefined: The FW installation uses swap mode. */

#if defined(STM32H7S3xx) || defined(STM32H7S7xx)
#define MCUBOOT_USE_MCE            /* Defined: The external flash can be protected by MCE.
                                      UnDefined: The MCE isn't supported. */
#endif /* STM32H7S3xx || STM32H7S7xx */

#define MCUBOOT_EXT_LOADER         /* Defined: Use system bootloader (in system flash).
                                      Undefined: Do not use system bootloader. */

#define MCUBOOT_DATA_IMAGE_NUMBER 0     /* 0: No data image (not configurable). */

/* Flash layout configuration : end ******************************************/

/* Use image hash reference to reduce boot time (signature check bypass) */
#define MCUBOOT_USE_HASH_REF

/* control configuration */

/* Total number of images */
#define MCUBOOT_APP_IMAGE_NUMBER 1
#define MCUBOOT_IMAGE_NUMBER (MCUBOOT_APP_IMAGE_NUMBER + MCUBOOT_DATA_IMAGE_NUMBER)
#define MCUBOOT_S_DATA_IMAGE_NUMBER MCUBOOT_DATA_IMAGE_NUMBER

/* The size of a partition. This should be large enough to contain a S or NS
 * sw binary. Each FLASH_AREA_IMAGE contains two partitions. See Flash layout
 * above.
 */
#define LOADER_FLASH_DEV_NAME             Driver_EXT_FLASH0

/* Flash layout info for BL2 bootloader */
#define FLASH_TOTAL_SIZE                (0x10000)             /* Embedded Flash: 64 KBytes */
#define FLASH_BASE_ADDRESS              (0x08000000)          /* Embedded Flash */
#define INT_FLASH_SECTOR_SIZE           (0x2000)              /* Embedded Flash: 8 KB */
#define EXT_FLASH_SECTOR_SIZE           (0x1000)              /* External Flash: 4 KB */
#define EXT_FLASH_TOTAL_SIZE            (0x08000000)          /* External Flash: 128 MBytes */
#if defined(STM32H7S3xx) || defined(STM32H7S7xx)
#define EXT_FLASH_BASE_ADDRESS          (0x90000000)          /* External Flash (XSPI1 + MCE1 - AES) */
#define EXT_RAM_BASE_ADDRESS            (0x70000000)          /* External SRAM (XSPI2 + MCE2 - Noekeon) */
#else
#define EXT_FLASH_BASE_ADDRESS          (0x70000000)          /* External Flash (XSPI2) */
#define EXT_RAM_BASE_ADDRESS            (0x90000000)          /* External SRAM (XSPI1) */
#endif /* STM32H7S3xx || STM32H7S7xx */
#define FLASH_AREA_IMAGE_SECTOR_SIZE    EXT_FLASH_SECTOR_SIZE /* External Flash: 4 KB */



#define FLASH_AREA_WRP_GROUP_SIZE       (0x2000)              /* 8 KB */
#define FLASH_AREA_HDP_GROUP_SIZE       (0x100)               /* 256 bytes */


/* XSPI instances used by the external memories:
 *  - XSPI1 + MCE1 (AES) must be used with the external flash.
 *  - XSPI2 + MCE2 (Noekeon) must be used with the external RAM.
 *
 * On STM32H7S78-DK,
 *  - the external flash is available via XSPI-PHY2 (Port2).
 *  - the external RAM is available via XSPI-PHY1 (Port1).
 * The XSPI I/O Manager (XSPIM) must be used in swapped mode.
 */
#define MCE_AES_CONTEXT                 MCE1_CONTEXT1
#define MCE_AES_CONTEXT_ID              MCE_CONTEXT1
#if defined(STM32H7S3xx) || defined(STM32H7S7xx)
#define EXT_FLASH_XSPI_INSTANCE         XSPI1
#define EXT_RAM_XSPI_INSTANCE           XSPI2
#else
#define EXT_FLASH_XSPI_INSTANCE         XSPI2
#define EXT_RAM_XSPI_INSTANCE           XSPI1
#endif /* STM32H7S3xx || STM32H7S7xx */

/* Size of the DMA shared memory to perform memory-mapped writing with MCE */
#define BL2_DMA_SHARED_MEM_SIZE         (0x100)               /* 256 bytes */

/* Flash device ID */
#define EXT_FLASH_DEVICE_ID             (FLASH_DEVICE_ID + 1) /* External Flash */

/* Flash area IDs */
#define FLASH_AREA_0_ID                 (1)
#if (MCUBOOT_APP_IMAGE_NUMBER == 2)
#define FLASH_AREA_1_ID                 (2)
#endif /* MCUBOOT_APP_IMAGE_NUMBER == 2 */
#define FLASH_AREA_2_ID                 (3)
#if (MCUBOOT_APP_IMAGE_NUMBER == 2)
#define FLASH_AREA_3_ID                 (4)
#endif /* MCUBOOT_APP_IMAGE_NUMBER == 2 */
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
#define FLASH_AREA_4_ID                 (5)
#endif /* MCUBOOT_S_DATA_IMAGE_NUMBER == 1 */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#define FLASH_AREA_5_ID                 (6)
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
#define FLASH_AREA_6_ID                 (7)
#endif /* MCUBOOT_S_DATA_IMAGE_NUMBER == 1 */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#define FLASH_AREA_7_ID                 (8)
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */
#define FLASH_AREA_SCRATCH_ID           (9)

/* Offset and size definitions of the flash partitions that are handled by the
 * bootloader. The image swapping is done between IMAGE_0 and IMAGE_1, SCRATCH
 * is used as a temporary storage during image swapping.
 */

/* scratch area */
#if defined(FLASH_AREA_SCRATCH_ID)
#define FLASH_AREA_SCRATCH_DEVICE_ID    (EXT_FLASH_DEVICE_ID - FLASH_DEVICE_ID)
#if (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM)
#define FLASH_AREA_SCRATCH_OFFSET       (0x00040000)
#else /* not (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM) */
#define FLASH_AREA_SCRATCH_OFFSET       (0x00400000)
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM */
#if defined(MCUBOOT_OVERWRITE_ONLY)
#define FLASH_AREA_SCRATCH_SIZE         (0x0000) /* Not used in MCUBOOT_OVERWRITE_ONLY mode */
#else /* not MCUBOOT_OVERWRITE_ONLY*/
#define FLASH_AREA_SCRATCH_SIZE         (0x10000) /* 64 KB */
#endif /* MCUBOOT_OVERWRITE_ONLY */
/* control scratch area */
#if (FLASH_AREA_SCRATCH_OFFSET % EXT_FLASH_SECTOR_SIZE) != 0
#error "FLASH_AREA_SCRATCH_OFFSET not aligned on EXT_FLASH_SECTOR_SIZE"
#endif /* (FLASH_AREA_SCRATCH_OFFSET % EXT_FLASH_SECTOR_SIZE) != 0*/
#else /* FLASH_AREA_SCRATCH_ID */
#define FLASH_AREA_SCRATCH_SIZE         (0x0)
#endif /* FLASH_AREA_SCRATCH_ID */

#if  !defined(OEMUROT_ENABLE)
/* area for BL2 code protected by hdp */
#define FLASH_AREA_BL2_OFFSET           (0x00000)
#define FLASH_AREA_BL2_SIZE             (0x10000)

/* HDP area end at this address */
#define FLASH_BL2_HDP_END               (FLASH_AREA_BL2_OFFSET+FLASH_AREA_BL2_SIZE-1)
/* control area for BL2 code protected by hdp */
#if ((FLASH_AREA_BL2_OFFSET+FLASH_AREA_BL2_SIZE) % FLASH_AREA_HDP_GROUP_SIZE) != 0
#error "HDP area must be aligned on FLASH_AREA_HDP_GROUP_SIZE"
#endif /* ((FLASH_AREA_BL2_OFFSET+FLASH_AREA_BL2_SIZE) % FLASH_AREA_HDP_GROUP_SIZE) != 0 */

/* control area under WRP group protection */
#if (FLASH_AREA_BL2_OFFSET % FLASH_AREA_WRP_GROUP_SIZE) != 0
#error "FLASH_AREA_BL2_OFFSET not aligned on FLASH_AREA_WRP_GROUP_SIZE"
#endif /* (FLASH_AREA_BL2_OFFSET % FLASH_AREA_WRP_GROUP_SIZE) != 0 */
#if ((FLASH_AREA_BL2_OFFSET+FLASH_AREA_BL2_SIZE) % FLASH_AREA_WRP_GROUP_SIZE) != 0
#error "(FLASH_AREA_BL2_OFFSET+FLASH_AREA_BL2_SIZE) not aligned on FLASH_AREA_WRP_GROUP_SIZE"
#endif /* ((FLASH_AREA_BL2_OFFSET+FLASH_AREA_BL2_SIZE) % FLASH_AREA_WRP_GROUP_SIZE) != 0 */
#endif /* not OEMUROT_ENABLE */

/* BL2 partitions size */
#if (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM)
#define FLASH_PARTITION_SIZE            (0x00020000) /* 128 kBytes */
#else /* not (OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM) */
#define FLASH_PARTITION_SIZE            (0x00200000) /* 2 MBytes */
#endif /* OEMIROT_LOAD_AND_RUN == LOAD_AND_RUN_INT_RAM */

#define FLASH_MAX_DATA_PARTITION_SIZE   (0x0)
#define FLASH_MAX_APP_PARTITION_SIZE    FLASH_PARTITION_SIZE
#define FLASH_MAX_PARTITION_SIZE        ((FLASH_MAX_APP_PARTITION_SIZE >   \
                                          FLASH_MAX_DATA_PARTITION_SIZE) ? \
                                         FLASH_MAX_APP_PARTITION_SIZE : \
                                         FLASH_MAX_DATA_PARTITION_SIZE)

/* BL2 flash areas (external flash) */
/* OEMuROT: The slots used by STiROT are after the scratch area.
 *          There is no need for a specific offset. */
#define FLASH_AREA_BEGIN_OFFSET         (0x0000)
#define FLASH_AREAS_DEVICE_ID           (EXT_FLASH_DEVICE_ID - FLASH_DEVICE_ID)

/* App image primary slot */
#if defined(FLASH_AREA_0_ID)
#define FLASH_AREA_0_DEVICE_ID          (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_0_OFFSET             (FLASH_AREA_BEGIN_OFFSET)
#define FLASH_AREA_0_SIZE               (FLASH_PARTITION_SIZE)
/* Control App image primary slot */
#if (FLASH_AREA_0_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_0_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*  (FLASH_AREA_0_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */
#else /* FLASH_AREA_0_ID */
#define FLASH_AREA_0_SIZE               (0x0)
#endif /* FLASH_AREA_0_ID */

/* App image secondary slot */
#if defined(FLASH_AREA_2_ID)
#define FLASH_AREA_2_DEVICE_ID          (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_2_OFFSET             (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_0_SIZE)
#define FLASH_AREA_2_SIZE               (FLASH_PARTITION_SIZE)
/* Control App image secondary slot */
#if (FLASH_AREA_2_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_2_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*   (FLASH_AREA_2_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */
#else /* FLASH_AREA_2_ID */
#define FLASH_AREA_2_SIZE               (0x0)
#endif /* FLASH_AREA_2_ID */

/* flash areas end offset */
#define FLASH_AREA_END_OFFSET           (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_0_SIZE + \
                                         FLASH_AREA_2_SIZE)
/* Control flash area end */
#if (FLASH_AREA_END_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_END_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*  (FLASH_AREA_END_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */
#if (FLASH_AREA_END_OFFSET > FLASH_AREA_SCRATCH_OFFSET)
#error "FLASH_AREA_END_OFFSET oversteps FLASH_AREA_SCRATCH_OFFSET"
#endif /* (FLASH_AREA_END_OFFSET > FLASH_AREA_SCRATCH_OFFSET) */

/*
 * The maximum number of status entries supported by the bootloader.
 */
#if defined(MCUBOOT_OVERWRITE_ONLY)
#define MCUBOOT_STATUS_MAX_ENTRIES        (0)
#else /* not MCUBOOT_OVERWRITE_ONLY */
#define MCUBOOT_STATUS_MAX_ENTRIES        (((FLASH_MAX_PARTITION_SIZE - 1) / \
                                            FLASH_AREA_SCRATCH_SIZE) + 1)
#endif /* MCUBOOT_OVERWRITE_ONLY */
/* Maximum number of image sectors supported by the bootloader. */
#define MCUBOOT_MAX_IMG_SECTORS           ((FLASH_MAX_PARTITION_SIZE) / \
                                           FLASH_AREA_IMAGE_SECTOR_SIZE)
/* Internal TMP buffer size used by the bootloader */
#define MCUBOOT_TMPBUF_SZ                 (0x80)

#define IMAGE_MAX_SIZE       FLASH_PARTITION_SIZE


/* Flash device name used by BL2
 * Name is defined in flash driver file: low_level_flash.c
 */
/* Flash Driver Used to Confirm NonSecure App Image or MCUBOOT_APP_IMAGE_NUMBER = 1 */
#define FLASH_PRIMARY_NONSECURE_DEV_NAME           Driver_EXT_FLASH0
#define FLASH_DEV_NAME                             Driver_FLASH0
#define EXT_FLASH_DEV_NAME                         Driver_EXT_FLASH0

/* Flash device names and IDs for BL2 flash areas */
#if defined(FLASH_AREA_0_ID)
#define FLASH_DEV_NAME_0  EXT_FLASH_DEV_NAME
#define FLASH_DEVICE_ID_0 EXT_FLASH_DEVICE_ID
#endif /* FLASH_AREA_0_ID */

#if defined(FLASH_AREA_2_ID)
#define FLASH_DEV_NAME_2  EXT_FLASH_DEV_NAME
#define FLASH_DEVICE_ID_2 EXT_FLASH_DEVICE_ID
#endif /* FLASH_AREA_2_ID */

#if defined(FLASH_AREA_SCRATCH_ID)
#define FLASH_DEV_NAME_SCRATCH  EXT_FLASH_DEV_NAME
#define FLASH_DEVICE_ID_SCRATCH EXT_FLASH_DEVICE_ID
#endif /* FLASH_AREA_SCRATCH_ID */

/* OBK */
#define OBK_INDEX_SIZE                   OBK_PROG_UNIT

/* Start index of RW OBK */
#define OBK_HDPL_RW_INDEX                (8U)

/* offset 0x100 (8 indexes) to use RW OBK */
#define OBK_HDPL1_CFG_OFFSET             (0x00000100U)
#define OBK_HDPL1_CFG_SIZE               (sizeof(OBK_Hdpl1Config_t))

#define OBK_HDPL1_DATA_SIZE              (sizeof(OBK_Hdpl1Data_t))
#define OBK_HDPL1_DATA_AREA_NB           (2U)
#define OBK_HDPL1_DATA_AREA_A_OFFSET     (OBK_HDPL1_CFG_OFFSET + OBK_HDPL1_CFG_SIZE)
#define OBK_HDPL1_DATA_AREA_B_OFFSET     (OBK_HDPL1_DATA_AREA_A_OFFSET + OBK_HDPL1_DATA_SIZE)
#define OBK_HDPL1_DATA_OFFSET            (OBK_HDPL1_DATA_AREA_A_OFFSET)

#if  defined(OEMUROT_ENABLE)
/* Add 8 indexes to be written in RW OBK */
#define OBK_HDPL2_CFG_OFFSET             (0x00000100U)
#define OBK_HDPL2_CFG_SIZE               (sizeof(OBK_Hdpl2Config_t))

#define OBK_HDPL2_DATA_SIZE              (sizeof(OBK_Hdpl2Data_t))
#define OBK_HDPL2_DATA_AREA_NB           (2U)
#define OBK_HDPL2_DATA_AREA_A_OFFSET     (OBK_HDPL2_CFG_OFFSET + OBK_HDPL2_CFG_SIZE)
#define OBK_HDPL2_DATA_AREA_B_OFFSET     (OBK_HDPL2_DATA_AREA_A_OFFSET + OBK_HDPL2_DATA_SIZE)
#define OBK_HDPL2_DATA_OFFSET            (OBK_HDPL2_DATA_AREA_A_OFFSET)
#endif /* OEMUROT_ENABLE */

/* Bootloader description */
#define BOOTLOADER_BASE                     (0x1FF18000UL)

#ifdef __cplusplus
}
#endif

#endif /* FLASH_LAYOUT_H */

