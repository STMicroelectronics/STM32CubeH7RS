/**
  ******************************************************************************
  * @file    low_level_flash_ext.c
  * @author  MCD Application Team
  * @brief   External Flash Low Level Interface module
  *          This file provides set of firmware functions to manage external
  *          flash low level interface.
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
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "low_level_flash_ext.h"
#include "boot_hal_iloader.h"
#include "stm32_extmem_conf.h"
#include "stm32_sfdp_driver_api.h"
#include "stm32h7rsxx.h"

XSPI_HandleTypeDef hxspi;
DMA_HandleTypeDef hdmatx;

/* Private defines -----------------------------------------------------------*/
#define REGION_EXTERNAL_MEMORY_DMA_NUMBER   (MPU_REGION_NUMBER10)

#define REGION_EXTERNAL_MEMORY_NUMBER       (MPU_REGION_NUMBER11)
#define REGION_EXTERNAL_MEMORY_SIZE         (MPU_REGION_SIZE_256MB)

/* Private macros ------------------------------------------------------------*/
#define REGION_MIN_SIZE(_A_,_B_) (((_A_) > (_B_)) ? (_B_) : (_A_))
/* Private function prototypes -----------------------------------------------*/
static STIROT_ILOADER_ErrorStatus FLASH_EXT_XSPI_Init(uint32_t Address);
static void FLASH_EXT_MPU_EnableRegion(uint8_t RegionID, uint32_t RegionSize);
static int32_t FLASH_EXT_SetHslv(void);

void EXTMEM_MemCopy(uint32_t *destination_Address, const uint8_t *ptrData, uint32_t DataSize);

/* Functions Definition ------------------------------------------------------*/

/**
  * @brief XSPI2 Initialization Function
  * @param Address XSPI address
  * @retval STIROT_ILOADER_SUCCESS if successful, STIROT_ILOADER_ERROR otherwise.
  */
static STIROT_ILOADER_ErrorStatus FLASH_EXT_XSPI_Init(uint32_t Address)
{
  XSPIM_CfgTypeDef sXspiManagerCfg = { 0U };

  if (Address == XSPI1_BASE)
  {
    hxspi.Instance = XSPI1;
  }
  else if (Address == XSPI2_BASE)
  {
    hxspi.Instance = XSPI2;
  }
  else
  {
    /* FMC not currently supported by the STM32_ExtMem_Manager middleware */
    Error_Handler();
  }

  /* XSPI initialization */
  /* ClockPrescaler set to 3, so initial XSPI clock = 200MHz / (1+3) = 50MHz */
  hxspi.Init.FifoThresholdByte       = 4U;
  hxspi.Init.MemoryMode              = HAL_XSPI_SINGLE_MEM;
  hxspi.Init.MemoryType              = HAL_XSPI_MEMTYPE_MACRONIX;
  hxspi.Init.MemorySize              = HAL_XSPI_SIZE_32GB;
  hxspi.Init.ChipSelectHighTimeCycle = 2U;
  hxspi.Init.FreeRunningClock        = HAL_XSPI_FREERUNCLK_DISABLE;
  hxspi.Init.ClockMode               = HAL_XSPI_CLOCK_MODE_0;
  hxspi.Init.WrapSize                = HAL_XSPI_WRAP_NOT_SUPPORTED;
  hxspi.Init.ClockPrescaler          = 3U;
  hxspi.Init.SampleShifting          = HAL_XSPI_SAMPLE_SHIFT_NONE;
  hxspi.Init.DelayHoldQuarterCycle   = HAL_XSPI_DHQC_ENABLE;
  hxspi.Init.ChipSelectBoundary      = HAL_XSPI_BONDARYOF_NONE;
  hxspi.Init.MaxTran                 = 0U;
  hxspi.Init.Refresh                 = 0U;
  hxspi.Init.MemorySelect            = HAL_XSPI_CSSEL_NCS1;

  if (HAL_XSPI_Init(&hxspi) != HAL_OK)
  {
    return STIROT_ILOADER_ERROR;
  }

  /*
   * Map the XSPI 1 to the external flash (port 2) on the STM32H7S78-DK board:
   *  XSPIM shall be used in swapped mode
   * Map the XSPI 2 to the external flash (port 2) on the STM32H7S78-DK board:
   *  XSPIM default configuration, nothing to do
   */
  if (hxspi.Instance == XSPI1)
  {
    /* Configure the output port 2 for XSPI */
    /* Connect Instance1(XSPI1/MCE1) to port2(NOR Flash)*/
    sXspiManagerCfg.IOPort         = HAL_XSPIM_IOPORT_2;
    sXspiManagerCfg.nCSOverride    = HAL_XSPI_CSSEL_OVR_NCS1;
    sXspiManagerCfg.Req2AckTime    = 10U;

    if (HAL_XSPIM_Config(&hxspi, &sXspiManagerCfg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      Error_Handler();
    }
  }

  return STIROT_ILOADER_SUCCESS;
}

/**
  * @brief  This function is used to write data with DMA.
  * @param  None
  * @retval None
  */
static void FLASH_EXT_XSPI_DMA_Init(void)
{
  /* Clear the handler of DMA */
  memset((void *)&hdmatx, 0U, sizeof(DMA_HandleTypeDef));

  HAL_NVIC_SetPriority(HPDMA1_Channel12_IRQn, 0x0FU, 0U);
  HAL_NVIC_EnableIRQ(HPDMA1_Channel12_IRQn);

  /* Configure the XSPI DMA transmit */
  hdmatx.Init.Request               = DMA_REQUEST_SW;
  hdmatx.Init.Direction             = DMA_MEMORY_TO_MEMORY;
  hdmatx.Init.SrcInc                = DMA_SINC_INCREMENTED;
  hdmatx.Init.DestInc               = DMA_DINC_INCREMENTED;
  hdmatx.Init.Priority              = DMA_HIGH_PRIORITY;
  hdmatx.Init.SrcBurstLength        = 16U;
  hdmatx.Init.DestBurstLength       = 16U;
  hdmatx.Init.TransferEventMode     = DMA_TCEM_BLOCK_TRANSFER;
  hdmatx.Init.SrcDataWidth          = DMA_SRC_DATAWIDTH_WORD;
  hdmatx.Init.DestDataWidth         = DMA_DEST_DATAWIDTH_WORD;
  hdmatx.Init.BlkHWRequest          = DMA_BREQ_SINGLE_BURST;
  hdmatx.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;

  hdmatx.Instance                   = HPDMA1_Channel12;

  /* Enable the GPDMA clock */
  __HAL_RCC_HPDMA1_CLK_ENABLE();

  /* Initialize the DMA channel */
  HAL_DMA_Init(&hdmatx);
}

/**
  * @brief  This function sets up a MPU region
  * @param region ID
  * @param region size
  * @retval None
  */
static void FLASH_EXT_MPU_EnableRegion(uint8_t RegionID, uint32_t RegionSize)
{
  uint32_t primask_bit;
  uint32_t address = 0U;
  MPU_Region_InitTypeDef region = {0U};

  switch (RegionID)
  {
    case REGION_EXTERNAL_MEMORY_DMA_NUMBER:
    {
      /* Enter critical section to lock the system and avoid any issue around MPU mechanism */
      primask_bit = __get_PRIMASK();
      __disable_irq();

      /* Disable the DMA region, to allow its use */
      region.Number = RegionID;
      region.Enable = MPU_REGION_DISABLE;
      HAL_MPU_ConfigRegion(&region);

      /* Exit critical section to lock the system and avoid any issue around MPU mechanism */
      __set_PRIMASK(primask_bit);
    }
    break;

    case REGION_EXTERNAL_MEMORY_NUMBER:
    {
      /* Enable the access to the external memory */
      address = (hxspi.Instance == XSPI1) ? XSPI1_BASE :
                (hxspi.Instance == XSPI2) ? XSPI2_BASE :
                0xFFFFFFFFU;

      region.Enable = MPU_REGION_ENABLE;
      region.Number = RegionID;
      region.BaseAddress = address;
      region.Size = RegionSize;
      region.SubRegionDisable = 0x0;
      region.TypeExtField = MPU_TEX_LEVEL0;
      region.AccessPermission = MPU_REGION_PRIV_RW;
      region.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
      region.IsShareable = MPU_ACCESS_SHAREABLE;
      region.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
      region.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
      HAL_MPU_ConfigRegion(&region);
    }
    break;

    default:
    {
      /* Nothing to unexpected region identifier */
    }
    break;
  }
}

/**
  * @brief  Configure "HSLV" for the external flash support
  * @param  None
  * @retval ARM_DRIVER error status
  */
static int32_t FLASH_EXT_SetHslv(void)
{
  FLASH_OBProgramInitTypeDef flash_option_bytes = {0};
  uint32_t user_config = OB_XSPI1_HSLV_ENABLE | OB_XSPI2_HSLV_ENABLE;

  /* Set the OB needed for "XSPI High-speed at low voltage" */
  /* Both XSPI must be configured when XSPIM is used */
  HAL_FLASHEx_OBGetConfig(&flash_option_bytes);

  if ((flash_option_bytes.USERConfig1 & user_config) != user_config)
  {
    if (HAL_FLASH_Unlock() != HAL_OK)
    {
      return HAL_ERROR;
    }
    if (HAL_FLASH_OB_Unlock() != HAL_OK)
    {
      return HAL_ERROR;
    }
    flash_option_bytes.OptionType = OPTIONBYTE_USER;
    flash_option_bytes.USERType = OB_USER_XSPI1_HSLV | OB_USER_XSPI2_HSLV;
    flash_option_bytes.USERConfig1 = user_config;
    if (HAL_FLASHEx_OBProgram(&flash_option_bytes) != HAL_OK)
    {
      return HAL_ERROR;
    }
    if (HAL_FLASH_OB_Lock() != HAL_OK)
    {
      return HAL_ERROR;
    }
    if (HAL_FLASH_Lock() != HAL_OK)
    {
      return HAL_ERROR;
    }
  }

  /* Configure "high speed low voltage" */
  /* Both XSPI must be configured when XSPIM is used */
  HAL_SBS_EnableIOSpeedOptimize(SBS_IO_XSPI1_HSLV);
  HAL_SBS_EnableIOSpeedOptimize(SBS_IO_XSPI2_HSLV);

  return HAL_OK;
}

/**
  * @brief  This function initializes OSPI interface
  * @param  Address XSPI address
  * @retval STIROT_ILOADER_ErrorStatus STIROT_ILOADER_SUCCESS if successful, STIROT_ILOADER_ERROR otherwise.
  */
STIROT_ILOADER_ErrorStatus FLASH_EXT_Init(uint32_t Address)
{
  EXTMEM_DRIVER_NOR_SFDP_InfoTypeDef FlashInfo;
  __IO uint32_t xspiAddress = 0U;
  __IO uint32_t xspiRcc = 0U;

  xspiAddress = Address & 0xF0000000U;

  if (FLASH_EXT_SetHslv() != HAL_OK)
  {
    return STIROT_ILOADER_ERROR;
  }

  /* Initialize the external memory */
  if (FLASH_EXT_XSPI_Init(xspiAddress) != STIROT_ILOADER_SUCCESS)
  {
    return STIROT_ILOADER_ERROR;
  }

  if (xspiAddress == XSPI1_BASE)
  {
    xspiRcc = RCC_PERIPHCLK_XSPI1;
  }
  else if (xspiAddress == XSPI2_BASE)
  {
    xspiRcc = RCC_PERIPHCLK_XSPI2;
  }
  else
  {
    Error_Handler();
  }

  /* Enable the access to the external memory */
  FLASH_EXT_MPU_EnableRegion(REGION_EXTERNAL_MEMORY_NUMBER, REGION_EXTERNAL_MEMORY_SIZE);

  /* Enable the dma */
  FLASH_EXT_MPU_EnableRegion(REGION_EXTERNAL_MEMORY_DMA_NUMBER, 0U);

  /* Initialize the EXTMEM */
  if (EXTMEM_Init(MEMORY_SERIAL_0, HAL_RCCEx_GetPeriphCLKFreq(xspiRcc)) != EXTMEM_OK)
  {
    return STIROT_ILOADER_ERROR;
  }

  /* Enable default memory mapped mode */
  if (EXTMEM_MemoryMappedMode(MEMORY_SERIAL_0, EXTMEM_ENABLE) != EXTMEM_OK)
  {
    return STIROT_ILOADER_ERROR;
  }

  /* Initialize DMA for the external memory */
  FLASH_EXT_XSPI_DMA_Init();

  /* Set clock protection on XSPI memory interface */
  HAL_RCCEx_EnableClockProtection(RCC_CLOCKPROTECT_XSPI);

  /* Get flash information */
  EXTMEM_GetInfo(MEMORY_SERIAL_0, &FlashInfo);

  return STIROT_ILOADER_SUCCESS;
}

/**
  * @brief  This function does an erase of n (depends on Length) pages in external OSPI flash
  * @param  Address: flash address to be erased
  * @param  Length: number of bytes
  * @retval STIROT_ILOADER_ErrorStatus STIROT_ILOADER_SUCCESS if successful, STIROT_ILOADER_ERROR otherwise.
  */
STIROT_ILOADER_ErrorStatus FLASH_EXT_Erase_Size(uint32_t Address, uint32_t Length)
{
  STIROT_ILOADER_ErrorStatus e_ret_status = STIROT_ILOADER_SUCCESS;
  uint32_t block_index = 0U;
  uint32_t start_address = 0U;

  /* Disable memory mapped mode */
  if (EXTMEM_MemoryMappedMode(MEMORY_SERIAL_0, EXTMEM_DISABLE) != EXTMEM_OK)
  {
    return STIROT_ILOADER_ERROR;
  }

  /* flash address to erase is the offset from begin of external flash */
  start_address = Address & 0x0FFFFFFFU;

  /* Loop on 64KBytes block */
  for (block_index = 0U; block_index < ((Length - 1U) / ERASE_BLOC_SIZE) + 1U; block_index++)
  {
    if (EXTMEM_EraseSector(MEMORY_SERIAL_0, start_address, ERASE_BLOC_SIZE) != EXTMEM_OK)
    {
      e_ret_status = STIROT_ILOADER_ERROR;
    }

    /* next 64KBytes block */
    start_address += ERASE_BLOC_SIZE;
  }

  /* Enable back memory mapped mode (even in case of writing error) */
  if (EXTMEM_MemoryMappedMode(MEMORY_SERIAL_0, EXTMEM_ENABLE) != EXTMEM_OK)
  {
    e_ret_status = STIROT_ILOADER_ERROR;
  }

  return e_ret_status;
}

/**
  * @brief  This function writes a data buffer in external QSPI flash.
  * @param  WriteAddress: flash address to write
  * @param  pData: pointer on buffer with data to write
  * @param  Length: number of bytes
  * @retval STIROT_ILOADER_ErrorStatus STIROT_ILOADER_SUCCESS if successful, STIROT_ILOADER_ERROR otherwise.
  */
STIROT_ILOADER_ErrorStatus FLASH_EXT_Write(uint32_t WriteAddress, const uint8_t *pData, uint32_t Length)
{
  STIROT_ILOADER_ErrorStatus e_ret_status = STIROT_ILOADER_SUCCESS;
  EXTMEM_DRIVER_NOR_SFDP_InfoTypeDef FlashInfo;

  /* Do nothing if Length equal to 0 */
  if (Length == 0U)
  {
    return STIROT_ILOADER_SUCCESS;
  }

  /* Disable memory mapped mode */
  if (EXTMEM_MemoryMappedMode(MEMORY_SERIAL_0, EXTMEM_DISABLE) != EXTMEM_OK)
  {
    /* Return here as following step cannot be run */
    return STIROT_ILOADER_ERROR;
  }

  /* Get flash information */
  EXTMEM_GetInfo(MEMORY_SERIAL_0, &FlashInfo);

  /* Manage the write of the application in mapped mode */
  if (EXTMEM_WriteInMappedMode(MEMORY_SERIAL_0, WriteAddress, (uint8_t *) pData, Length) != EXTMEM_OK)
  {
    e_ret_status = STIROT_ILOADER_ERROR;
  }

  /* Enable back memory mapped mode (even in case of writing error) */
  if (EXTMEM_MemoryMappedMode(MEMORY_SERIAL_0, EXTMEM_ENABLE) != EXTMEM_OK)
  {
    e_ret_status = STIROT_ILOADER_ERROR;
  }

#if defined(CONTROL_FLASH_WRITEOPERATION)
  {
    uint32_t index = 0U;
    uint8_t *address = (uint8_t *)WriteAddress;

    /* check the SW write operation */
    for (index = 0U; index < Length; index++)
    {
      if (*address != pData[index])
      {
        Error_Handler();
      }
      address = address + 1U;
    }
  }
#endif /* CONTROL_FLASH_WRITEOPERATION */

  return e_ret_status;
}

/**
  * @brief  This function deinitialize OSPI interface
  * @param  none
  * @retval STIROT_ILOADER_ErrorStatus STIROT_ILOADER_SUCCESS if successful, STIROT_ILOADER_ERROR otherwise.
  */
STIROT_ILOADER_ErrorStatus FLASH_EXT_DeInit(void)
{
  HAL_RCCEx_DisableClockProtection(RCC_CLOCKPROTECT_XSPI);

  if (HAL_XSPI_DeInit(&hxspi) != HAL_OK)
  {
    return STIROT_ILOADER_ERROR;
  }

  __HAL_RCC_XSPI2_FORCE_RESET();
  __HAL_RCC_XSPI2_RELEASE_RESET();

  return STIROT_ILOADER_SUCCESS;
}

/**
  * @brief  This function does a memory copy with DMA on XSPI
  * @param  destination_Address destination address
  * @param  ptrData source address
  * @param  DataSize data size to copy
  * @retval None.
  */
void EXTMEM_MemCopy(uint32_t *destination_Address, const uint8_t *ptrData, uint32_t DataSize)
{
  /* Ensure memory access are done before the use of DMA */
  __DMB();

  HAL_DMA_Start_IT(&hdmatx, (uint32_t)ptrData, (uint32_t)destination_Address, DataSize);
  HAL_DMA_PollForTransfer(&hdmatx, HAL_DMA_FULL_TRANSFER, 1000U);
}
