/**
  ******************************************************************************
  * @file    mx66uw1g45g.h
  * @modify  MCD Application Team
  * @brief   This file contains all the description of the
  *          MX66UW1G45G XSPI memory.
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
#ifndef MX66UW1G45G_H
#define MX66UW1G45G_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "mx66uw1g45g_conf.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup MX66UW1G45G
  * @{
  */
/** @defgroup MX66UW1G45G_Exported_Constants MX66UW1G45G Exported Constants
  * @{
  */

/**
  * @brief  MX66UW1G45G Size configuration
  */
#define MX66UW1G45G_BLOCK_64K                    (uint32_t)(64 * 1024)         /* 1024 blocks of 64KBytes      */
#define MX66UW1G45G_BLOCK_4K                    (uint32_t)(4  * 1024)         /* 16384 sectors of 4KBytes     */

#define MX66UW1G45G_FLASH_SIZE                   (uint32_t)(1024*1024*1024/8)  /* 1 Gbits => 128MBytes         */
#define MX66UW1G45G_PAGE_SIZE                    (uint32_t)256                 /* 262144 pages of 256 Bytes    */

/**
  * @brief  MX66UW1G45G Timing configuration
  */

#define MX66UW1G45G_BULK_ERASE_MAX_TIME          460000U
#define MX66UW1G45G_BLOCK_ERASE_MAX_TIME         1000U
#define MX66UW1G45G_BLOCK_4K_ERASE_MAX_TIME     400U
#define MX66UW1G45G_WRITE_REG_MAX_TIME           40U

#define MX66UW1G45G_RESET_MAX_TIME               100U                 /* when SWreset during erase operation */

#define MX66UW1G45G_AUTOPOLLING_INTERVAL_TIME    0x10U

/**
  * @brief  MX66UW1G45G Error codes
  */
#define MX66UW1G45G_OK                           (0)
#define MX66UW1G45G_ERROR                        (-1)

/**
  * @brief  re-definition of legacy memory mapped functions
  */
#define MX66UW1G45G_EnableMemoryMappedModeDTR MX66UW1G45G_EnableDTRMemoryMappedMode
#define MX66UW1G45G_EnableMemoryMappedModeSTR MX66UW1G45G_EnableSTRMemoryMappedMode


/******************************************************************************
  * @brief  MX66UW1G45G Commands
  ****************************************************************************/

/*******************************************************************/
/********************************* SPI  ****************************/
/*******************************************************************/

/***** READ/WRITE MEMORY Operations with 3-Byte Address ****************************/
#define MX66UW1G45G_READ_CMD                             0x03U   /*!< Normal Read 3 Byte Address                            */
#define MX66UW1G45G_FAST_READ_CMD                        0x0BU   /*!< Fast Read 3 Byte Address                              */
#define MX66UW1G45G_PAGE_PROG_CMD                        0x02U   /*!< Page Program 3 Byte Address                           */
#define MX66UW1G45G_SECTOR_ERASE_4K_CMD                  0x20U   /*!< SubSector Erase 4KB 3 Byte Address                    */
#define MX66UW1G45G_BLOCK_ERASE_64K_CMD                  0xD8U   /*!< Sector Erase 64KB 3 Byte Address                      */
#define MX66UW1G45G_BULK_ERASE_CMD                       0x60U   /*!< Bulk Erase                                            */

/***** READ/WRITE MEMORY Operations with 4-Byte Address ****************************/
#define MX66UW1G45G_4_BYTE_ADDR_READ_CMD                 0x13U   /*!< Normal Read 4 Byte address                            */
#define MX66UW1G45G_4_BYTE_ADDR_FAST_READ_CMD            0x0CU   /*!< Fast Read 4 Byte address                              */
#define MX66UW1G45G_4_BYTE_PAGE_PROG_CMD                 0x12U   /*!< Page Program 4 Byte Address                           */
#define MX66UW1G45G_4_BYTE_SECTOR_ERASE_4K_CMD           0x21U   /*!< SubSector Erase 4KB 4 Byte Address                    */
#define MX66UW1G45G_4_BYTE_BLOCK_ERASE_64K_CMD           0xDCU   /*!< Sector Erase 64KB 4 Byte Address                      */

/***** Setting commands ************************************************************/
#define MX66UW1G45G_WRITE_ENABLE_CMD                     0x06U   /*!< Write Enable                                          */
#define MX66UW1G45G_WRITE_DISABLE_CMD                    0x04U   /*!< Write Disable                                         */
#define MX66UW1G45G_PROG_ERASE_SUSPEND_CMD               0xB0U   /*!< Program/Erase suspend                                 */
#define MX66UW1G45G_PROG_ERASE_RESUME_CMD                0x30U   /*!< Program/Erase resume                                  */
#define MX66UW1G45G_ENTER_DEEP_POWER_DOWN_CMD            0xB9U   /*!< Enter deep power down                                 */
#define MX66UW1G45G_RELEASE_DEEP_POWER_DOWN_CMD          0xABU   /*!< Release from deep power down                          */
#define MX66UW1G45G_SET_BURST_LENGTH_CMD                 0xC0U   /*!< Set burst length                                      */
#define MX66UW1G45G_ENTER_SECURED_OTP_CMD                0xB1U   /*!< Enter secured OTP)                                    */
#define MX66UW1G45G_EXIT_SECURED_OTP_CMD                 0xC1U   /*!< Exit secured OTP)                                     */

/***** RESET commands ************************************************************/
#define MX66UW1G45G_NOP_CMD                              0x00U   /*!< No operation                                          */
#define MX66UW1G45G_RESET_ENABLE_CMD                     0x66U   /*!< Reset Enable                                          */
#define MX66UW1G45G_RESET_MEMORY_CMD                     0x99U   /*!< Reset Memory                                          */

/***** Register Commands (SPI) ****************************************************/
#define MX66UW1G45G_READ_ID_CMD                          0x9FU   /*!< Read IDentification                                   */
#define MX66UW1G45G_READ_SERIAL_FLASH_DISCO_PARAM_CMD    0x5AU   /*!< Read Serial Flash Discoverable Parameter              */
#define MX66UW1G45G_READ_STATUS_REG_CMD                  0x05U   /*!< Read Status Register                                  */
#define MX66UW1G45G_READ_CFG_REG_CMD                     0x15U   /*!< Read configuration Register                           */
#define MX66UW1G45G_WRITE_STATUS_REG_CMD                 0x01U   /*!< Write Status Register                                 */
#define MX66UW1G45G_READ_CFG_REG2_CMD                    0x71U   /*!< Read configuration Register2                          */
#define MX66UW1G45G_WRITE_CFG_REG2_CMD                   0x72U   /*!< Write configuration Register2                                */
#define MX66UW1G45G_READ_FAST_BOOT_REG_CMD               0x16U   /*!< Read fast boot Register                               */
#define MX66UW1G45G_WRITE_FAST_BOOT_REG_CMD              0x17U   /*!< Write fast boot Register                              */
#define MX66UW1G45G_ERASE_FAST_BOOT_REG_CMD              0x18U   /*!< Erase fast boot Register                              */
#define MX66UW1G45G_READ_SECURITY_REG_CMD                0x2BU   /*!< Read security Register                                */
#define MX66UW1G45G_WRITE_SECURITY_REG_CMD               0x2FU   /*!< Write security Register                               */
#define MX66UW1G45G_READ_LOCK_REG_CMD                    0x2DU   /*!< Read lock Register                                    */
#define MX66UW1G45G_WRITE_LOCK_REG_CMD                   0x2CU   /*!< Write lock Register                                   */

#define MX66UW1G45G_READ_DPB_REG_CMD                     0xE0U   /*!< Read DPB register                                     */
#define MX66UW1G45G_WRITE_DPB_REG_CMD                    0xE1U   /*!< Write DPB register                                    */
#define MX66UW1G45G_READ_SPB_STATUS_CMD                  0xE2U   /*!< Read SPB status                                       */
#define MX66UW1G45G_WRITE_SPB_BIT_CMD                    0xE3U   /*!< SPB bit program                                       */
#define MX66UW1G45G_ERASE_ALL_SPB_CMD                    0xE4U   /*!< Erase all SPB bit                                     */
#define MX66UW1G45G_WRITE_PROTECT_SEL_CMD                0x68U   /*!< Write Protect selection                               */
#define MX66UW1G45G_GANG_BLOCK_LOCK_CMD                  0x7EU   /*!< Gang block lock: whole chip write protect             */
#define MX66UW1G45G_GANG_BLOCK_UNLOCK_CMD                0x98U   /*!< Gang block unlock: whole chip write unprotect         */
#define MX66UW1G45G_READ_PASSWORD_REGISTER_CMD           0x27U   /*!< Read Password                                         */
#define MX66UW1G45G_WRITE_PASSWORD_REGISTER_CMD          0x28U   /*!< Write Password                                        */
#define MX66UW1G45G_PASSWORD_UNLOCK_CMD                  0x29U   /*!< Unlock Password                                       */


/*******************************************************************/
/********************************* OPI  ****************************/
/*******************************************************************/

/***** READ/WRITE MEMORY Operations  ****************************/
#define MX66UW1G45G_OCTA_READ_CMD                             0xEC13U  /*!< Octa IO Read                                     */
#define MX66UW1G45G_OCTA_READ_DTR_CMD                         0xEE11U  /*!< Octa IO Read DTR                                 */
#define MX66UW1G45G_OCTA_PAGE_PROG_CMD                        0x12EDU  /*!< Octa Page Program                                */
#define MX66UW1G45G_OCTA_SECTOR_ERASE_4K_CMD                  0x21DEU  /*!< Octa SubSector Erase 4KB                         */
#define MX66UW1G45G_OCTA_BLOCK_ERASE_64K_CMD                  0xDC23U  /*!< Octa Sector Erase 64KB 3                         */
#define MX66UW1G45G_OCTA_BULK_ERASE_CMD                       0x609FU  /*!< Octa Bulk Erase                                  */

/***** Setting commands ************************************************************/
#define MX66UW1G45G_OCTA_WRITE_ENABLE_CMD                     0x06F9U   /*!< Octa Write Enable                               */
#define MX66UW1G45G_OCTA_WRITE_DISABLE_CMD                    0x04FBU   /*!< Octa Write Disable                              */
#define MX66UW1G45G_OCTA_PROG_ERASE_SUSPEND_CMD               0xB04FU   /*!< Octa Program/Erase suspend                      */
#define MX66UW1G45G_OCTA_PROG_ERASE_RESUME_CMD                0x30CFU   /*!< Octa Program/Erase resume                       */
#define MX66UW1G45G_OCTA_ENTER_DEEP_POWER_DOWN_CMD            0xB946U   /*!< Octa Enter deep power down                      */
#define MX66UW1G45G_OCTA_RELEASE_DEEP_POWER_DOWN_CMD          0xAB54U   /*!< Octa Release from deep power down               */
#define MX66UW1G45G_OCTA_SET_BURST_LENGTH_CMD                 0xC03FU   /*!< Octa Set burst length                           */
#define MX66UW1G45G_OCTA_ENTER_SECURED_OTP_CMD                0xB14EU   /*!< Octa Enter secured OTP)                         */
#define MX66UW1G45G_OCTA_EXIT_SECURED_OTP_CMD                 0xC13EU   /*!< Octa Exit secured OTP)                          */

/***** RESET commands ************************************************************/
#define MX66UW1G45G_OCTA_NOP_CMD                              0x00FFU   /*!< Octa No operation                               */
#define MX66UW1G45G_OCTA_RESET_ENABLE_CMD                     0x6699U   /*!< Octa Reset Enable                               */
#define MX66UW1G45G_OCTA_RESET_MEMORY_CMD                     0x9966U   /*!< Octa Reset Memory                               */

/***** Register Commands (OPI) ****************************************************/
#define MX66UW1G45G_OCTA_READ_ID_CMD                          0x9F60U   /*!< Octa Read IDentification                        */
#define MX66UW1G45G_OCTA_READ_SERIAL_FLASH_DISCO_PARAM_CMD    0x5AA5U   /*!< Octa Read Serial Flash Discoverable Parameter   */
#define MX66UW1G45G_OCTA_READ_STATUS_REG_CMD                  0x05FAU   /*!< Octa Read Status Register                       */
#define MX66UW1G45G_OCTA_READ_CFG_REG_CMD                     0x15EAU   /*!< Octa Read configuration Register                */
#define MX66UW1G45G_OCTA_WRITE_STATUS_REG_CMD                 0x01FEU   /*!< Octa Write Status Register                      */
#define MX66UW1G45G_OCTA_READ_CFG_REG2_CMD                    0x718EU   /*!< Octa Read configuration Register2               */
#define MX66UW1G45G_OCTA_WRITE_CFG_REG2_CMD                   0x728DU   /*!< Octa Write configuration Register2                     */
#define MX66UW1G45G_OCTA_READ_FAST_BOOT_REG_CMD               0x16E9U   /*!< Octa Read fast boot Register                    */
#define MX66UW1G45G_OCTA_WRITE_FAST_BOOT_REG_CMD              0x17E8U   /*!< Octa Write fast boot Register                   */
#define MX66UW1G45G_OCTA_ERASE_FAST_BOOT_REG_CMD              0x18E7U   /*!< Octa Erase fast boot Register                   */
#define MX66UW1G45G_OCTA_READ_SECURITY_REG_CMD                0x2BD4U   /*!< Octa Read security Register                     */
#define MX66UW1G45G_OCTA_WRITE_SECURITY_REG_CMD               0x2FD0U   /*!< Octa Write security Register                    */
#define MX66UW1G45G_OCTA_READ_LOCK_REG_CMD                    0x2DD2U   /*!< Octa Read lock Register                         */
#define MX66UW1G45G_OCTA_WRITE_LOCK_REG_CMD                   0x2CD3U   /*!< Octa Write lock Register                        */
#define MX66UW1G45G_OCTA_READ_DPB_REG_CMD                     0xE01FU   /*!< Octa Read DPB register                          */
#define MX66UW1G45G_OCTA_WRITE_DPB_REG_CMD                    0xE11EU   /*!< Octa Write DPB register                         */
#define MX66UW1G45G_OCTA_READ_SPB_STATUS_CMD                  0xE21DU   /*!< Octa Read SPB status                            */
#define MX66UW1G45G_OCTA_WRITE_SPB_BIT_CMD                    0xE31CU   /*!< Octa SPB bit program                            */
#define MX66UW1G45G_OCTA_ERASE_ALL_SPB_CMD                    0xE41BU   /*!< Octa Erase all SPB bit                          */
#define MX66UW1G45G_OCTA_WRITE_PROTECT_SEL_CMD                0x6897U   /*!< Octa Write Protect selection                    */
#define MX66UW1G45G_OCTA_GANG_BLOCK_LOCK_CMD                  0x7E81U   /*!< Octa Gang block lock: whole chip write protect  */
#define MX66UW1G45G_OCTA_GANG_BLOCK_UNLOCK_CMD                0x9867U   /*!< Octa Gang block unlock: whole chip write unprote*/
#define MX66UW1G45G_OCTA_READ_PASSWORD_REGISTER_CMD           0x27D8U   /*!< Octa Read Password                              */
#define MX66UW1G45G_OCTA_WRITE_PASSWORD_REGISTER_CMD          0x28D7U   /*!< Octa Write Password                             */
#define MX66UW1G45G_OCTA_PASSWORD_UNLOCK_CMD                  0x29D6U   /*!< Octa Unlock Password                            */

/******************************************************************************
  * @brief  MX66UW1G45G Registers
  ****************************************************************************/
/* Status Register */
#define MX66UW1G45G_SR_WIP                               0x01U   /*!< Write in progress                                       */
#define MX66UW1G45G_SR_WEL                               0x02U   /*!< Write enable latch                                      */
#define MX66UW1G45G_SR_PB                                0x3CU   /*!< Block protected against program and erase operations    */

/* Configuration Register 1 */
#define MX66UW1G45G_CR1_ODS                              0x07U   /*!< Output driver strength                                  */
#define MX66UW1G45G_CR1_TB                               0x08U   /*!< Top / bottom  selected                                  */
#define MX66UW1G45G_CR1_PBE                              0x10U   /*!< Preamble bit enable                                     */

/* Configuration Register 2 */
/* Address : 0x00000000 */
#define MX66UW1G45G_CR2_REG1_ADDR                        0x00000000U  /*!< CR2 register address 0x00000000                    */
#define MX66UW1G45G_CR2_SOPI                             0x01U        /*!< STR OPI Enable                                     */
#define MX66UW1G45G_CR2_DOPI                             0x02U        /*!< DTR OPI Enable                                     */
/* Address : 0x00000200 */
#define MX66UW1G45G_CR2_REG2_ADDR                        0x00000200U  /*!< CR2 register address 0x00000200                    */
#define MX66UW1G45G_CR2_DQSPRC                           0x01U        /*!< DTR DQS pre-cycle                                  */
#define MX66UW1G45G_CR2_DOS                              0x02U        /*!< DQS on STR mode                                    */
/* Address : 0x00000300 */
#define MX66UW1G45G_CR2_REG3_ADDR                        0x00000300U  /*!< CR2 register address 0x00000300                    */
#define MX66UW1G45G_CR2_DC                               0x07U        /*!< Dummy cycle                                        */
#define MX66UW1G45G_CR2_DC_20_CYCLES                     0x00U        /*!< 20 Dummy cycles                                    */
#define MX66UW1G45G_CR2_DC_18_CYCLES                     0x01U        /*!< 18 Dummy cycles                                    */
#define MX66UW1G45G_CR2_DC_16_CYCLES                     0x02U        /*!< 16 Dummy cycles                                    */
#define MX66UW1G45G_CR2_DC_14_CYCLES                     0x03U        /*!< 14 Dummy cycles                                    */
#define MX66UW1G45G_CR2_DC_12_CYCLES                     0x04U        /*!< 12 Dummy cycles                                    */
#define MX66UW1G45G_CR2_DC_10_CYCLES                     0x05U        /*!< 10 Dummy cycles                                    */
#define MX66UW1G45G_CR2_DC_8_CYCLES                      0x06U        /*!< 8 Dummy cycles                                     */
#define MX66UW1G45G_CR2_DC_6_CYCLES                      0x07U        /*!< 6 Dummy cycles                                     */
/* Address : 0x00000500 */
#define MX66UW1G45G_CR2_REG4_ADDR                        0x00000500U  /*!< CR2 register address 0x00000500                    */
#define MX66UW1G45G_CR2_PPTSEL                           0x01U        /*!< Preamble pattern selection                         */
/* Address : 0x40000000 */
#define MX66UW1G45G_CR2_REG5_ADDR                        0x40000000U  /*!< CR2 register address 0x40000000                    */
#define MX66UW1G45G_CR2_DEFSOPI                          0x01U        /*!< Enable SOPI after power on reset                   */
#define MX66UW1G45G_CR2_DEFDOPI                          0x02U        /*!< Enable DOPI after power on reset                   */

/* Security Register */
#define MX66UW1G45G_SECR_SOI                             0x01U        /*!< Secured OTP indicator                              */
#define MX66UW1G45G_SECR_LDSO                            0x02U        /*!< Lock-down secured OTP                              */
#define MX66UW1G45G_SECR_PSB                             0x04U        /*!< Program suspend bit                                */
#define MX66UW1G45G_SECR_ESB                             0x08U        /*!< Erase suspend bit                                  */
#define MX66UW1G45G_SECR_P_FAIL                          0x20U        /*!< Program fail flag                                  */
#define MX66UW1G45G_SECR_E_FAIL                          0x40U        /*!< Erase fail flag                                    */
#define MX66UW1G45G_SECR_WPSEL                           0x80U        /*!< Write protection selection                         */

#define XSPI_ALTERNATE_BYTE_PATTERN                      0x00U

/**
  * @}
  */

/** @defgroup MX66UW1G45G_Exported_Types MX66UW1G45G Exported Types
  * @{
  */
typedef struct
{
  uint32_t FlashSize;                        /*!< Size of the flash                             */
  uint32_t EraseSectorSize;                  /*!< Size of sectors for the erase operation       */
  uint32_t EraseSectorsNumber;               /*!< Number of sectors for the erase operation     */
  uint32_t EraseSubSectorSize;               /*!< Size of subsector for the erase operation     */
  uint32_t EraseSubSectorNumber;             /*!< Number of subsector for the erase operation   */
  uint32_t EraseSubSector1Size;              /*!< Size of subsector 1 for the erase operation   */
  uint32_t EraseSubSector1Number;            /*!< Number of subsector 1 for the erase operation */
  uint32_t ProgPageSize;                     /*!< Size of pages for the program operation       */
  uint32_t ProgPagesNumber;                  /*!< Number of pages for the program operation     */
} MX66UW1G45G_Info_t;

typedef enum
{
  MX66UW1G45G_SPI_MODE = 0,                 /*!< 1-1-1 commands, Power on H/W default setting  */
  MX66UW1G45G_OPI_MODE                      /*!< 8-8-8 commands                                */
} MX66UW1G45G_Interface_t;

typedef enum
{
  MX66UW1G45G_STR_TRANSFER = 0,             /*!< Single Transfer Rate                          */
  MX66UW1G45G_DTR_TRANSFER                  /*!< Double Transfer Rate                          */
} MX66UW1G45G_Transfer_t;

typedef enum
{
  MX66UW1G45G_ERASE_4K = 0,                 /*!< 4K size Sector erase                          */
  MX66UW1G45G_ERASE_64K,                    /*!< 64K size Block erase                          */
  MX66UW1G45G_ERASE_BULK                    /*!< Whole bulk erase                              */
} MX66UW1G45G_Erase_t;

typedef enum
{
  MX66UW1G45G_3BYTES_SIZE = 0,              /*!< 3 Bytes address mode                           */
  MX66UW1G45G_4BYTES_SIZE                   /*!< 4 Bytes address mode                           */
} MX66UW1G45G_AddressWidth_t;

/**
  * @}
  */

/** @defgroup MX66UW1G45G_Exported_Functions MX66UW1G45G Exported Functions
  * @{
  */
/* Function by commands combined */
int32_t MX66UW1G45G_GetFlashInfo(MX66UW1G45G_Info_t *pInfo);
int32_t MX66UW1G45G_AutoPollingMemReady(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode,
                                         MX66UW1G45G_Transfer_t Rate);

/* Read/Write Array Commands **************************************************/
int32_t MX66UW1G45G_ReadSTR(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode,
                             MX66UW1G45G_AddressWidth_t AddressWidth, uint8_t *pData, uint32_t ReadAddr, uint32_t Size);
int32_t MX66UW1G45G_ReadDTR(XSPI_HandleTypeDef *Ctx, uint8_t *pData, uint32_t ReadAddr, uint32_t Size);
int32_t MX66UW1G45G_PageProgram(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode,
                                 MX66UW1G45G_AddressWidth_t AddressWidth, uint8_t *pData, uint32_t WriteAddr,
                                 uint32_t Size);
int32_t MX66UW1G45G_PageProgramDTR(XSPI_HandleTypeDef *Ctx, uint8_t *pData, uint32_t WriteAddr, uint32_t Size);
int32_t MX66UW1G45G_BlockErase(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode, MX66UW1G45G_Transfer_t Rate,
                                MX66UW1G45G_AddressWidth_t AddressWidth, uint32_t BlockAddress,
                                MX66UW1G45G_Erase_t BlockSize);
int32_t MX66UW1G45G_ChipErase(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode, MX66UW1G45G_Transfer_t Rate);
int32_t MX66UW1G45G_EnableMemoryMappedModeSTR(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode,
                                               MX66UW1G45G_AddressWidth_t AddressWidth);
int32_t MX66UW1G45G_EnableMemoryMappedModeDTR(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode);
int32_t MX66UW1G45G_Suspend(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode, MX66UW1G45G_Transfer_t Rate);
int32_t MX66UW1G45G_Resume(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode, MX66UW1G45G_Transfer_t Rate);

/* Register/Setting Commands **************************************************/
int32_t MX66UW1G45G_WriteEnable(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode, MX66UW1G45G_Transfer_t Rate);
int32_t MX66UW1G45G_WriteDisable(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode, MX66UW1G45G_Transfer_t Rate);
int32_t MX66UW1G45G_ReadStatusRegister(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode,
                                        MX66UW1G45G_Transfer_t Rate, uint8_t *Value);
int32_t MX66UW1G45G_WriteStatusRegister(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode,
                                         MX66UW1G45G_Transfer_t Rate, uint8_t Value);
int32_t MX66UW1G45G_WriteCfgRegister(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode,
                                      MX66UW1G45G_Transfer_t Rate, uint8_t Value);
int32_t MX66UW1G45G_ReadCfgRegister(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode,
                                     MX66UW1G45G_Transfer_t Rate, uint8_t *Value);
int32_t MX66UW1G45G_WriteCfg2Register(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode,
                                       MX66UW1G45G_Transfer_t Rate, uint32_t WriteAddr, uint8_t Value);
int32_t MX66UW1G45G_ReadCfg2Register(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode,
                                      MX66UW1G45G_Transfer_t Rate, uint32_t ReadAddr, uint8_t *Value);
int32_t MX66UW1G45G_WriteSecurityRegister(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode,
                                           MX66UW1G45G_Transfer_t Rate, uint8_t Value);
int32_t MX66UW1G45G_ReadSecurityRegister(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode,
                                          MX66UW1G45G_Transfer_t Rate, uint8_t *Value);

/* ID/Security Commands *******************************************************/
int32_t MX66UW1G45G_ReadID(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode, MX66UW1G45G_Transfer_t Rate,
                            uint8_t *ID);

/* Reset Commands *************************************************************/
int32_t MX66UW1G45G_ResetEnable(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode, MX66UW1G45G_Transfer_t Rate);
int32_t MX66UW1G45G_ResetMemory(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode, MX66UW1G45G_Transfer_t Rate);
int32_t MX66UW1G45G_NoOperation(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode, MX66UW1G45G_Transfer_t Rate);
int32_t MX66UW1G45G_EnterPowerDown(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode,
                                    MX66UW1G45G_Transfer_t Rate);
int32_t MX66UW1G45G_ReleasePowerDown(XSPI_HandleTypeDef *Ctx, MX66UW1G45G_Interface_t Mode,
                                     MX66UW1G45G_Transfer_t Rate);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* MX66UW1G45G_H */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
