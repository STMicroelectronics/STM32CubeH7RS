/**
  ******************************************************************************
  * @file    loader.c
  * @author  MCD Application Team
  * @brief   STiROT-iLoader module.
  *          This file provides set of firmware functions to manage the
  *          STiROT-iLoader functionalities.
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
#include "main.h"
#include "loader.h"
#include "low_level_flash_ext.h"
#include "low_level_shared_mem.h"
#include "boot_hal_iloader.h"
#include "stm32_extmem_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define IMAGE_CODE_MAGIC (0xc1592ee9U) /* Image code magic */
#define SLOTS_PC  (1U) /* Primary code slots */
#define SLOTS_SC  (2U) /* Secondary code slots */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern const LL_SHARED_MEM_Config_t *pSharedRam; /* Shared memory with STiROT */

/* Private function prototypes -----------------------------------------------*/
static STIROT_ILOADER_ErrorStatus LOADER_IsImagePresent(uint32_t SlotStart, uint32_t SlotSize, const uint32_t ImgMagic);
static STIROT_ILOADER_ErrorStatus LOADER_Load(void);
static STIROT_ILOADER_ErrorStatus LOADER_Update(void);
static STIROT_ILOADER_ErrorStatus LOADER_Update_Save_Code(void);
static STIROT_ILOADER_ErrorStatus LOADER_Update_Erase_Secondary_Code(void);
static void LOADER_CopyNvmSlotIntoVmSlot(uint32_t NvmSlotStart, uint32_t VmSlotStart, uint32_t SlotSize);

/**
  * @brief  Check whether there is an image
  * @param  SlotStart slot start address
  * @param  SlotSize slot size
  * @param  ImgMagic image magic to find
  * @retval Loader status
  */
static STIROT_ILOADER_ErrorStatus LOADER_IsImagePresent(uint32_t SlotStart, uint32_t SlotSize, const uint32_t ImgMagic)
{
  /* Check if the expected magic is present in the memory slot */
  if ((uint32_t)ImgMagic == *(uint32_t *) SlotStart)
  {
    LOADER_PRINT("  Image is found\r\n");
    return STIROT_ILOADER_SUCCESS;
  }

  LOADER_PRINT("  No image was found\r\n");
  return STIROT_ILOADER_ERROR;
}

/**
  * @brief  Copy a non-volatile slot into a volatile one
  * @param  NvmSlotStart non-volatile slot start address
  * @param  VmSlotStart volatile slot size
  * @param  SlotSize size to copy
  * @retval None
  */
static void LOADER_CopyNvmSlotIntoVmSlot(uint32_t NvmSlotStart, uint32_t VmSlotStart, uint32_t SlotSize)
{
  __IO uint64_t *nvm_pt = (uint64_t *)NvmSlotStart;
  __IO uint64_t *vm_pt = (uint64_t *)VmSlotStart;
  uint32_t index;
  uint32_t size = SlotSize / 8U;

  for (index = 0U; index < size; index++)
  {
    vm_pt[index] = nvm_pt[index];
  }

  __DSB();
}

/**
  * @brief  Run the operation "Load"
  * @param  None
  * @retval Loader status
  */
static STIROT_ILOADER_ErrorStatus LOADER_Load(void)
{
  uint32_t slots = 0U;

  STIROT_ILOADER_ErrorStatus status = STIROT_ILOADER_ERROR;

  LOADER_PRINT("  Operation Load started\r\n");

  /* Double check the configuration before the operation */
  if ((IS_VM_SHARED_MEM_VALID())
      && (IS_SLOT_IN_VM(pSharedRam->CodeVmStart, pSharedRam->CodeSize))
      && (IS_SLOT_IN_NVM(pSharedRam->CodePrimaryNvmStart, pSharedRam->CodeSize))
      && (IS_SLOT_IN_NVM(pSharedRam->CodeSecondaryNvmStart, pSharedRam->CodeSize))
      && (ARE_SLOTS_SEPARATE(pSharedRam->CodePrimaryNvmStart, pSharedRam->CodeSize,
                             pSharedRam->CodeSecondaryNvmStart, pSharedRam->CodeSize))
      && (pSharedRam->Operation == ILOADER_OPERATION_LOAD))
  {
    /* Handle the code secondary image */
    if (LOADER_IsImagePresent(pSharedRam->CodeSecondaryNvmStart, pSharedRam->CodeSize,
                              IMAGE_CODE_MAGIC) == STIROT_ILOADER_SUCCESS)
    {
      /* Code secondary image present in NVM: copy the image from the NVM into the VM */
      slots |= SLOTS_SC;
      LOADER_PRINT("  Code secondary image loaded in VM\r\n");
      LOADER_CopyNvmSlotIntoVmSlot(pSharedRam->CodeSecondaryNvmStart, pSharedRam->CodeVmStart,
                                   pSharedRam->CodeSize);
    }
    else
    {
      /* No code secondary image in NVM: check the code primary image in NVM */
      if (LOADER_IsImagePresent(pSharedRam->CodePrimaryNvmStart, pSharedRam->CodeSize,
                                IMAGE_CODE_MAGIC) == STIROT_ILOADER_SUCCESS)
      {
        /* Code primary image present in NVM: check the code primary image in VM */
        if (LOADER_IsImagePresent(pSharedRam->CodeVmStart, pSharedRam->CodeSize, IMAGE_CODE_MAGIC)
            == STIROT_ILOADER_ERROR)
        {
          /* Copy the image from the NVM into the VM */
          slots |= SLOTS_PC;
          LOADER_PRINT("  Code primary image loaded in VM\r\n");
          LOADER_CopyNvmSlotIntoVmSlot(pSharedRam->CodePrimaryNvmStart, pSharedRam->CodeVmStart,
                                       pSharedRam->CodeSize);
        }
        else
        {
          /* A code image is already present in VM: do nothing */
        }
      }
    }
    status = STIROT_ILOADER_SUCCESS;
  }

  if (status == STIROT_ILOADER_ERROR)
  {
    /* Set RSSCmd to error (see also ErrorHandler) */
    HAL_SBS_ConfigRSSCommand(ILOADER_RSS_CMD_ERROR);
  }

  LOADER_PRINT("  Operation Load ended\r\n");

  if (slots != 0U)
  {
    if (slots & SLOTS_SC)
    {
      /* Set the load command to secondary code */
      HAL_SBS_ConfigRSSCommand(ILOADER_RSS_CMD_LOAD_SC);
    }
    else if (slots & SLOTS_PC)
    {
      /* Set the load command to primary code */
      HAL_SBS_ConfigRSSCommand(ILOADER_RSS_CMD_LOAD_PC);
    }
    else
    {
      /* Unexpected value: set to error state */
      HAL_SBS_ConfigRSSCommand(ILOADER_RSS_CMD_ERROR);
    }
  }
  else
  {
    /* Set the load command to load empty */
    HAL_SBS_ConfigRSSCommand(ILOADER_RSS_CMD_LOAD_EMPTY);
  }

  return status;
}

/**
  * @brief  Run the operation "Update"
  * @param  None
  * @retval Loader status
  */
static STIROT_ILOADER_ErrorStatus LOADER_Update(void)
{
  STIROT_ILOADER_ErrorStatus status = STIROT_ILOADER_ERROR;

  LOADER_PRINT("  Operation Update started\r\n");

#ifdef ILOADER_USE_CACHE
  SCB_CleanDCache();
#endif /* ILOADER_USE_CACHE */

  /* Check the shared ram validity */
  if (((pSharedRam->SubOperations & LOADER_SUBOPERATION_SAVE_CODE)
       != LOADER_SUBOPERATION_SAVE_CODE)
      && ((pSharedRam->SubOperations & LOADER_SUBOPERATION_ERASE_SECONDARY_CODE)
          != LOADER_SUBOPERATION_ERASE_SECONDARY_CODE))
  {
    Error_Handler();
  }

  /* Check if STiROT asked to save primary code slot from volatile memory to non-volatile memory */
  if ((pSharedRam->SubOperations & LOADER_SUBOPERATION_SAVE_CODE) == LOADER_SUBOPERATION_SAVE_CODE)
  {
    status = LOADER_Update_Save_Code();
  }

  if ((((pSharedRam->SubOperations & LOADER_SUBOPERATION_SAVE_CODE) == LOADER_SUBOPERATION_SAVE_CODE) &&
       (status == STIROT_ILOADER_SUCCESS)) ||
      ((pSharedRam->SubOperations & LOADER_SUBOPERATION_ERASE_SECONDARY_CODE) == LOADER_SUBOPERATION_ERASE_SECONDARY_CODE))
  {
    /* Check if STiROT asked to erase secondary code slot from volatile memory */
    if (((pSharedRam->SubOperations & LOADER_SUBOPERATION_ERASE_SECONDARY_CODE)
         == LOADER_SUBOPERATION_ERASE_SECONDARY_CODE)
        && ((pSharedRam->SubOperations & LOADER_SUBOPERATION_SAVE_CODE) == 0U))
    {
      status = LOADER_Update_Erase_Secondary_Code();
    }
  }

  if (status == STIROT_ILOADER_ERROR)
  {
    /* Set RSSCmd to error (see also ErrorHandler) */
    HAL_SBS_ConfigRSSCommand(ILOADER_RSS_CMD_ERROR);
  }

  LOADER_PRINT("  Operation Update ended\r\n");

  return status;
}

/**
  * @brief  Run the suboperation "Save_Code"
  * @param  None
  * @retval Loader status
  */
static STIROT_ILOADER_ErrorStatus LOADER_Update_Save_Code(void)
{
  STIROT_ILOADER_ErrorStatus status = STIROT_ILOADER_ERROR;
  __IO uint32_t *p_vm_data = (uint32_t *) pSharedRam->CodeVmStart;

  LOADER_PRINT("  Operation Save_Code started\r\n");

  /* Double check the configuration before the operation */
  if ((IS_VM_SHARED_MEM_VALID())
      && (IS_SLOT_IN_VM(pSharedRam->CodeVmStart, pSharedRam->CodeSize))
      && (IS_SLOT_IN_NVM(pSharedRam->CodePrimaryNvmStart, pSharedRam->CodeSize))
      && (IS_SLOT_IN_NVM(pSharedRam->CodeSecondaryNvmStart, pSharedRam->CodeSize))
      && (ARE_SLOTS_SEPARATE(pSharedRam->CodePrimaryNvmStart, pSharedRam->CodeSize,
                             pSharedRam->CodeSecondaryNvmStart, pSharedRam->CodeSize))
      && (pSharedRam->Operation == ILOADER_OPERATION_UPDATE)
      && ((pSharedRam->SubOperations & LOADER_SUBOPERATION_SAVE_CODE) == LOADER_SUBOPERATION_SAVE_CODE))
  {
    /* Check if a code image is available thanks to its magic */
    if (LOADER_IsImagePresent(pSharedRam->CodeVmStart, pSharedRam->CodeSize, IMAGE_CODE_MAGIC)
        == STIROT_ILOADER_SUCCESS)
    {
      status = FLASH_EXT_Erase_Size(pSharedRam->CodePrimaryNvmStart, pSharedRam->CodeSize);

      if (status == STIROT_ILOADER_SUCCESS)
      {
        /* Check that the MCE configuration is correctly set */
        if (((MCE1->CR & (MCE_CR_GLOCK | MCE_CR_MKLOCK)) != (MCE_CR_GLOCK | MCE_CR_MKLOCK)) &&
            ((MCE2->CR & (MCE_CR_GLOCK | MCE_CR_MKLOCK)) != (MCE_CR_GLOCK | MCE_CR_MKLOCK)))
        {
          Error_Handler();
        }

        /* Save the volatile code image in the non-volatile memory */
        status = FLASH_EXT_Write(pSharedRam->CodePrimaryNvmStart, (uint8_t *)p_vm_data, pSharedRam->CodeSize);

        if (status == STIROT_ILOADER_SUCCESS)
        {
          /* On success, erase the secondary code slot in the non-volatile memory: the update has been done */
          status = FLASH_EXT_Erase_Size(pSharedRam->CodeSecondaryNvmStart, pSharedRam->CodeSize);
        }
      }
    }
    else
    {
      status = STIROT_ILOADER_SUCCESS;
    }
  }

  LOADER_PRINT("  Operation Save_Code ended\r\n");

  if (status == STIROT_ILOADER_ERROR)
  {
    /* Set RSSCmd to error (see also ErrorHandler) */
    HAL_SBS_ConfigRSSCommand(ILOADER_RSS_CMD_ERROR);
  }

  return status;
}

/**
  * @brief  Run the suboperation "Erase_Secondary_Code"
  * @param  None
  * @retval Loader status
  */
static STIROT_ILOADER_ErrorStatus LOADER_Update_Erase_Secondary_Code(void)
{
  STIROT_ILOADER_ErrorStatus status = STIROT_ILOADER_ERROR;

  LOADER_PRINT("  Operation Erase_Secondary_Code started\r\n");

  /* Double check the configuration before the operation */
  if ((IS_VM_SHARED_MEM_VALID())
      && (IS_SLOT_IN_NVM(pSharedRam->CodeSecondaryNvmStart, pSharedRam->CodeSize))
      && (ARE_SLOTS_SEPARATE(pSharedRam->CodePrimaryNvmStart, pSharedRam->CodeSize,
                             pSharedRam->CodeSecondaryNvmStart, pSharedRam->CodeSize))
      && (pSharedRam->Operation == ILOADER_OPERATION_UPDATE)
      && ((pSharedRam->SubOperations & LOADER_SUBOPERATION_ERASE_SECONDARY_CODE)
          == LOADER_SUBOPERATION_ERASE_SECONDARY_CODE))
  {
    /* Erase the code secondary slot in the non-volatile memory: the image is invalid */
    if (FLASH_EXT_Erase_Size(pSharedRam->CodeSecondaryNvmStart, pSharedRam->CodeSize) == STIROT_ILOADER_SUCCESS)
    {
      status = STIROT_ILOADER_SUCCESS;
    }
  }

  LOADER_PRINT("  Operation Erase_Secondary_Code ended\r\n");

  if (status == STIROT_ILOADER_ERROR)
  {
    /* Set RSSCmd to error (see also ErrorHandler) */
    HAL_SBS_ConfigRSSCommand(ILOADER_RSS_CMD_ERROR);
  }

  return status;
}

/**
  * @brief  Run the STiROT-iLoader functionalities
  * @param  None
  * @retval Loader status
  */
STIROT_ILOADER_ErrorStatus LOADER_Run(void)
{
  STIROT_ILOADER_ErrorStatus status = STIROT_ILOADER_ERROR;

  LOADER_PRINT("  STiROT-iLoader started\r\n");

  LOADER_PRINT("\r\n");

  switch (pSharedRam->Operation)
  {
    case ILOADER_OPERATION_LOAD:
      /* Load slots from the non-volatile memory to the volatile memory */
      status = LOADER_Load();
      break;
    case ILOADER_OPERATION_UPDATE:
      /* Update slots from the volatile memory to the non-volatile memory */
      status = LOADER_Update();
      break;
    default:
      status = STIROT_ILOADER_ERROR;
      /* Set RSSCmd to error (see also ErrorHandler) */
      HAL_SBS_ConfigRSSCommand(ILOADER_RSS_CMD_ERROR);
      break;
  }

  LOADER_PRINT("  STiROT-iLoader ended with status = 0x%x\r\n", status);
  LOADER_PRINT("  STiROT-iLoader ended with RSSCmd = 0x%x\r\n", HAL_SBS_GetRSSCommand());

  return status;
}
