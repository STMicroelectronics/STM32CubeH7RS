/**
  ******************************************************************************
  * @file    external_memory_interface.c
  * @author  MCD Application Team
  * @brief   Contains External memory access functions
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
#include "platform.h"
#include "openbl_core.h"

#include "openbl_mem.h"
#include "external_memory_interface.h"
#include "app_openbootloader.h"
#include "common_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define INIT_INDEX                    0x00U
#define READ_INDEX                    0x01U
#define WRITE_INDEX                   0x02U
#define SECTOR_ERASE_INDEX            0x03U
#define MASS_ERASE_INDEX              0x04U
#define EXT_MEM_START_INDEX           0X05U
#define SIZE_INDEX                    0x06U
#define SECTOR_SIZE_INDEX             0x07U
#define SECTOR_MAX_NUMBER_INDEX       0x08U

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t ExtMemStartAddress;
static uint32_t ExtMemSize;
static uint32_t ExtMemSectorSize;
static uint32_t ExtMemMaxSectorsNumber;

/* Private function prototypes -----------------------------------------------*/
static uint32_t OPENBL_ExtMem_IsAddressInRam(uint32_t function_address);

/* Functions pointers needed to interface with external loader function */
static int (*ExtMem_Init)(void) = NULL;
static int (*ExtMem_Read)(uint32_t address, uint32_t data_length, uint8_t *p_data) = NULL;
static int (*ExtMem_Write)(uint32_t address, uint32_t data_length, uint8_t *p_data) = NULL;
static int (*ExtMem_Erase)(uint32_t start_address, uint32_t end_address) = NULL;
static int (*ExtMem_MassErase)(void) = NULL;

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
  * @brief  This function is used for initializing external memory.
  * @param  address The address of external memory functions.
  * @retval Returns SUCCESS if operation was successful, or ERROR if there was an error.
  */
ErrorStatus OPENBL_ExtMem_Init(uint32_t address)
{
  uint32_t *ext_mem_base_address     = (uint32_t *) address;
  static uint8_t ext_mem_initialized = 0U;
  ErrorStatus status                 = ERROR;

  /* The items of this structure will be automatically getting from the external loader */
  OPENBL_MemoryTypeDef external_memory_descriptor = { 0x0 };

  /* Ensure that the external memory structure is registered once in Middleware side */
  if (ext_mem_initialized == 0U)
  {
    /* Functions pointers initialization */
    ExtMem_Init      = (int (*)(void))(*(ext_mem_base_address + INIT_INDEX));
    ExtMem_Read      = (int (*)(uint32_t, uint32_t, uint8_t *))(*(ext_mem_base_address + READ_INDEX));
    ExtMem_Write     = (int (*)(uint32_t, uint32_t, uint8_t *))(*(ext_mem_base_address + WRITE_INDEX));
    ExtMem_Erase     = (int (*)(uint32_t, uint32_t))(*(ext_mem_base_address + SECTOR_ERASE_INDEX));
    ExtMem_MassErase = (int (*)(void))(*(ext_mem_base_address + MASS_ERASE_INDEX));

    /* External memory properties initialization */
    ExtMemStartAddress     = (uint32_t)(*(ext_mem_base_address + EXT_MEM_START_INDEX));
    ExtMemSize             = (uint32_t)(*(ext_mem_base_address + SIZE_INDEX));
    ExtMemSectorSize       = (uint32_t)(*(ext_mem_base_address + SECTOR_SIZE_INDEX));
    ExtMemMaxSectorsNumber = (uint32_t)(*(ext_mem_base_address + SECTOR_MAX_NUMBER_INDEX));

    /* Fill the external memory descriptor structure */
    external_memory_descriptor.StartAddress       = ExtMemStartAddress;
    external_memory_descriptor.EndAddress         = ExtMemStartAddress + ExtMemSize - 1U;
    external_memory_descriptor.Size               = ExtMemSize;
    external_memory_descriptor.Type               = EXTERNAL_MEMORY_AREA;
    external_memory_descriptor.Read               = OPENBL_ExtMem_Read;
    external_memory_descriptor.Write              = OPENBL_ExtMem_Write;
    external_memory_descriptor.SetReadoutProtect  = NULL;
    external_memory_descriptor.SetWriteProtect    = NULL;
    external_memory_descriptor.JumpToAddress      = OPENBL_ExtMem_JumpToAddress;
    external_memory_descriptor.MassErase          = NULL;
    external_memory_descriptor.Erase              = NULL;
    
    /* Register the memory in OpenBL MW */
    (void)OPENBL_MEM_RegisterMemory(&external_memory_descriptor);

    /* The external memory descriptor is registered in OpenBL MW, ensure that this code is not executed a second time */
    ext_mem_initialized = 1U;
  }

  if (ExtMem_Init != NULL)
  {
    if (OPENBL_ExtMem_IsAddressInRam((uint32_t)ExtMem_Init) != 0U)
    {
      if (ExtMem_Init() == 1U)
      {
        status = SUCCESS;
      }
    }
  }

  return status;
}

/**
  * @brief  This function is used to read data from a given address.
  * @param  address The address to be read.
  * @retval Returns the read value.
  */
uint8_t OPENBL_ExtMem_Read(uint32_t address)
{
  uint8_t data[1];

  if (ExtMem_Read != NULL)
  {
    /* Read one byte of data from external memory */
    if (OPENBL_ExtMem_IsAddressInRam((uint32_t)ExtMem_Read) != 0U)
    {
      ExtMem_Read(address, 1U, data);
    }

    /* Return the read data */
    return (data[0]);
  }
  else
  {
    /* Return the read value */
    return (*(uint8_t *)(address));
  }
}

/**
  * @brief  This function is used to write data in external memory.
  * @param  address The address where that data will be written.
  * @param  p_data The pointer of data to be written.
  * @param  data_length The length of the data to be written.
  * @retval None.
  */
void OPENBL_ExtMem_Write(uint32_t address, uint8_t *p_data, uint32_t data_length)
{
  if (ExtMem_Write != NULL)
  {
    if (OPENBL_ExtMem_IsAddressInRam((uint32_t)ExtMem_Write) != 0U)
    {
      ExtMem_Write(address, data_length, p_data);
    }
  }
}

/**
  * @brief  This function is used to jump to a given address.
  * @param  address The address where the function will jump.
  * @retval None.
  */
void OPENBL_ExtMem_JumpToAddress(uint32_t address)
{
  Function_Pointer jump_to_address;

  /* Deinitialize all HW resources used by the Bootloader to their reset values */
  OPENBL_DeInit();

  /* Enable IRQ */
  Common_EnableIrq();

  jump_to_address = (Function_Pointer)(*(__IO uint32_t *)(address + 4U));

  /* Initialize user application's stack pointer */
  Common_SetMsp(*(__IO uint32_t *) address);

  jump_to_address();
}

/**
  * @brief  This function is used to erase the specified external FLASH pages.
  * @param  p_special_cmd Pointer to the OPENBL_SpecialCmdTypeDef structure.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Erase operation done
  *          - ERROR:   Erase operation failed or the value of one parameter is not ok.
  */
ErrorStatus OPENBL_ExtMem_Erase(OPENBL_SpecialCmdTypeDef *p_special_cmd)
{
  uint32_t start_address;
  uint32_t end_address;
  uint32_t sectors_id;
  uint32_t errors         = 0U;
  uint32_t counter        = 0U;
  ErrorStatus status      = ERROR;
  uint16_t sectors_number = (p_special_cmd->SizeBuffer1) / 4U;

  /* Get 1st sector to erase */
  sectors_id = (uint32_t)(*(uint32_t *)p_special_cmd->Buffer1);

  /* Check if it is a ExtMem_MassErase or not*/
  if (sectors_id == 0xFFFFFFFFU)
  {
    if (ExtMem_MassErase != NULL)
    {
      if (OPENBL_ExtMem_IsAddressInRam((uint32_t)ExtMem_MassErase) != 0U)
      {
        if (ExtMem_MassErase() == 0U)
        {
          errors++;
        }
      }
      else
      {
        errors++;
      }
    }
  }
  else
  {
    if (ExtMem_Erase != NULL)
    {
      if (OPENBL_ExtMem_IsAddressInRam((uint32_t)ExtMem_Erase) != 0U)
      {
        for (counter = 0U; counter < sectors_number; counter++)
        {
          sectors_id = (uint32_t)(*(uint32_t *)(p_special_cmd->Buffer1 + (counter * 4)));

          if (sectors_id < ExtMemMaxSectorsNumber)
          {
            start_address = (sectors_id * ExtMemSectorSize);
            end_address   = start_address;

            if (ExtMem_Erase(start_address, end_address) == 0U)
            {
              errors++;
            }
          }
        }
      }
    }
    else
    {
      errors++;
    }
  }

  if (errors > 0U)
  {
    status = ERROR;
  }
  else
  {
    status = SUCCESS;
  }

  return status;
}

/**
  * @brief  Check if the function address is located in RAM.
  * @param  address the address to be check.
  * @retval Returns true if the function address is in RAM, false otherwise.
  */
static uint32_t OPENBL_ExtMem_IsAddressInRam(uint32_t address)
{
  return ((address > RAM_MEM_START_ADDRESS) && (address < RAM_MEM_END_ADDRESS));
}
