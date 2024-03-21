/**
  ******************************************************************************
  * @file    flash_interface.c
  * @author  MCD Application Team
  * @brief   Contains FLASH access functions
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
#include "main.h"
#include "platform.h"
#include "common_interface.h"

#include "openbl_mem.h"

#include "app_openbootloader.h"
#include "flash_interface.h"
#include "i2c_interface.h"
#include "optionbytes_interface.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define FLASH_PROG_STEP_SIZE             ((uint8_t)16)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t FlashBusyState           = FLASH_BUSY_STATE_DISABLED;
static FLASH_ProcessTypeDef FlashProcess = {.Lock = HAL_UNLOCKED, \
                                            .ErrorCode = HAL_FLASH_ERROR_NONE, \
                                            .ProcedureOnGoing = 0U, \
                                            .Address = 0U, \
                                            .Sector = 0U, \
                                            .NbSectorsToErase = 0U
                                           };

/* Private function prototypes -----------------------------------------------*/
static void OPENBL_FLASH_Program(uint32_t address, uint32_t data);
ErrorStatus OPENBL_FLASH_SetWriteProtection(FunctionalState state, uint8_t *p_list_of_sectors, uint32_t length);
static ErrorStatus OPENBL_FLASH_DisableWriteProtection(void);

/* Exported variables --------------------------------------------------------*/
OPENBL_MemoryTypeDef FLASH_Descriptor =
{
  FLASH_MEM_START_ADDRESS,
  FLASH_MEM_END_ADDRESS,
  FLASH_MEM_SIZE,
  FLASH_AREA,
  OPENBL_FLASH_Read,
  OPENBL_FLASH_Write,
  NULL,
  OPENBL_FLASH_SetWriteProtection,
  OPENBL_FLASH_JumpToAddress,
  NULL,
  OPENBL_FLASH_Erase
};

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Unlock the FLASH control register access.
  * @retval None.
  */
void OPENBL_FLASH_Unlock(void)
{
  if (HAL_FLASH_Unlock() != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Lock the FLASH control register access.
  * @retval None.
  */
void OPENBL_FLASH_Lock(void)
{
  if (HAL_FLASH_Lock() != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Unlock the FLASH Option Bytes Registers access.
  * @retval None.
  */
void OPENBL_FLASH_OB_Unlock(void)
{
  if (HAL_FLASH_Unlock() != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_FLASH_OB_Unlock() != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function is used to read data from a given address.
  * @param  address The address to be read.
  * @retval Returns the read value.
  */
uint8_t OPENBL_FLASH_Read(uint32_t address)
{
  return (*(uint8_t *)(address));
}

/**
  * @brief  This function is used to write data in FLASH memory.
  * @param  address The address where that data will be written.
  * @param  p_data The data to be written.
  * @param  data_length The length of the data to be written.
  * @retval None.
  */
void OPENBL_FLASH_Write(uint32_t address, uint8_t *p_data, uint32_t data_length)
{
  uint32_t index;
  __ALIGNED(4) uint8_t data[FLASH_PROG_STEP_SIZE] = {0x0U};
  uint8_t remaining;

  if ((p_data != NULL) && (data_length != 0U))
  {
    /* Unlock the flash memory for write operation */
    OPENBL_FLASH_Unlock();

    /* Clear error programming flags */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    /* Program double-word by double-word (16 bytes) */
    while ((data_length >> 4U) > 0U)
    {
      for (index = 0U; index < FLASH_PROG_STEP_SIZE; index++)
      {
        data[index] = *(p_data + index);
      }

      OPENBL_FLASH_Program(address, (uint32_t)data);

      address     += FLASH_PROG_STEP_SIZE;
      p_data      += FLASH_PROG_STEP_SIZE;
      data_length -= FLASH_PROG_STEP_SIZE;
    }

    /* If remaining count, go back to fill the rest with 0xFF */
    if (data_length > 0U)
    {
      remaining = FLASH_PROG_STEP_SIZE - data_length;

      /* Copy the remaining bytes */
      for (index = 0U; index < data_length; index++)
      {
        data[index] = *(p_data + index);
      }

      /* Fill the upper bytes with 0xFF */
      for (index = 0U; index < remaining; index++)
      {
        data[index + data_length] = 0xFFU;
      }

      /* FLASH word program */
      OPENBL_FLASH_Program(address, (uint32_t)data);
    }

    /* Lock the Flash to disable the flash control register access */
    OPENBL_FLASH_Lock();
  }
}

/**
  * @brief  This function is used to jump to a given address.
  * @param  address The address where the function will jump.
  * @retval None.
  */
void OPENBL_FLASH_JumpToAddress(uint32_t address)
{
  Function_Pointer jump_to_address;

  /* De-initialize all HW resources used by the Open Bootloader to their reset values */
  OPENBL_DeInit();

  /* Enable IRQ */
  Common_EnableIrq();

  jump_to_address = (Function_Pointer)(*(__IO uint32_t *)(address + 4U));

  /* Initialize user application's stack pointer */
  Common_SetMsp(*(__IO uint32_t *) address);

  jump_to_address();
}

/**
  * @brief  This function is used to get the product state.
  * @retval The return value can be one of the following values:
  *         @arg @ref OB_NVSTATE_OPEN  : product state open.
  *         @arg @ref OB_NVSTATE_CLOSE : product state closed.
  */
uint32_t OPENBL_FLASH_GetProductState(void)
{
  FLASH_OBProgramInitTypeDef flash_ob;

  /* Get the Option bytes configuration */
  HAL_FLASHEx_OBGetConfig(&flash_ob);

  return flash_ob.NVState;
}

/**
  * @brief  This function is used to enable or disable write protection of the specified FLASH areas.
  * @param  state Can be one of these values:
  *         @arg DISABLE: Disable FLASH write protection
  *         @arg ENABLE: Enable FLASH write protection
  * @param  p_sectors Contains the sector to be protected.
  * @param  -length The length of the sector to be protected.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Enable or disable of the write protection is done.
  *          - ERROR:   Enable or disable of the write protection is not done.
  */
ErrorStatus OPENBL_FLASH_SetWriteProtection(FunctionalState state, uint8_t *p_sectors, uint32_t length)
{
  ErrorStatus status;

  if (state == ENABLE)
  {
    status = OPENBL_FLASH_EnableWriteProtection(p_sectors, length);
  }
  else if (state == DISABLE)
  {
    status = OPENBL_FLASH_DisableWriteProtection();
  }
  else
  {
    status = ERROR;
  }

  if (status == SUCCESS)
  {
    /* Register system reset callback */
    Common_SetPostProcessingCallback(OPENBL_OB_Launch);
  }

  return status;
}

/**
  * @brief  This function is used to start FLASH mass erase operation.
  * @param  p_data Pointer to the buffer that contains mass erase operation options.
  * @param  data_length Size of the Data buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Mass erase operation done.
  *          - ERROR:   Mass erase operation failed or the value of one parameter is not OK.
  */
ErrorStatus OPENBL_FLASH_MassErase(uint8_t *p_data, uint32_t data_length)
{
  uint32_t sector_error;
  uint16_t bank_option;
  ErrorStatus status = SUCCESS;
  FLASH_EraseInitTypeDef erase_init_struct;

  /* Unlock the flash memory for erase operation */
  OPENBL_FLASH_Unlock();

  erase_init_struct.TypeErase = FLASH_TYPEERASE_MASSERASE;

  if (data_length >= 2U)
  {
    bank_option  = (uint16_t)p_data[1] << 8U;
    bank_option |= (uint16_t)p_data[0];

    if (bank_option == FLASH_MASS_ERASE)
    {
      erase_init_struct.Sector = 0U;
    }
    else
    {
      status = ERROR;
    }

    if (status == SUCCESS)
    {
      if (OPENBL_FLASH_ExtendedErase(&erase_init_struct, &sector_error) != HAL_OK)
      {
        status = ERROR;
      }
      else
      {
        status = SUCCESS;
      }
    }
  }
  else
  {
    status = ERROR;
  }

  /* Lock the Flash to disable the flash control register access */
  OPENBL_FLASH_Lock();

  return status;
}

/**
  * @brief  This function is used to erase the specified FLASH pages.
  * @param  p_data Pointer to the buffer that contains erase operation options.
  * @param  data_length Size of the Data buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Erase operation done.
  *          - ERROR:   Erase operation failed or the value of one parameter is not OK.
  */
ErrorStatus OPENBL_FLASH_Erase(uint8_t *p_data, uint32_t data_length)
{
  uint32_t counter;
  uint32_t sectors_number;
  uint32_t sector_error    = 0U;
  uint32_t errors          = 0U;
  ErrorStatus status       = SUCCESS;
  FLASH_EraseInitTypeDef erase_init_struct;

  /* Unlock the flash memory for erase operation */
  OPENBL_FLASH_Unlock();

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  sectors_number = (uint32_t)(*(uint16_t *)(p_data));

  /* The sector number size is 2 bytes */
  p_data += 2U;

  erase_init_struct.TypeErase = FLASH_TYPEERASE_SECTORS;
  erase_init_struct.NbSectors = 1U;

  for (counter = 0U; ((counter < sectors_number) && (counter < (data_length / 2U))) ; counter ++)
  {
    erase_init_struct.Sector = ((uint32_t)(*(uint16_t *)(p_data)));

    if (status != ERROR)
    {
      if (OPENBL_FLASH_ExtendedErase(&erase_init_struct, &sector_error) != HAL_OK)
      {
        errors++;
      }
    }
    else
    {
      /* Reset the status for next erase operation */
      status = SUCCESS;
    }

    /* The sector number size is 2 bytes */
    p_data += 2U;
  }

  /* Lock the Flash to disable the flash control register access */
  OPENBL_FLASH_Lock();

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
  * @brief  This function is used to Set Flash busy state variable to activate busy state sending
  *         during flash operations.
  * @retval None.
  */
void OPENBL_Enable_BusyState_Flag(void)
{
  /* Enable Flash busy state sending */
  FlashBusyState = FLASH_BUSY_STATE_ENABLED;
}

/**
  * @brief  This function is used to disable the send of busy state in I2C non stretch mode.
  * @retval None.
  */
void OPENBL_Disable_BusyState_Flag(void)
{
  /* Disable Flash busy state sending */
  FlashBusyState = FLASH_BUSY_STATE_DISABLED;
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Program double word at a specified FLASH address.
  * @param  address specifies the address to be programmed.
  * @param  data specifies the data to be programmed.
  * @retval None.
  */
static void OPENBL_FLASH_Program(uint32_t address, uint32_t data)
{
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  (void)HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, address, data);
}

/**
  * @brief  This function is used to enable write protection of the specified FLASH areas.
  * @param  p_list_of_sectors Contains the list of sectors to be protected.
  * @param  length The length of the list of sectors to be protected.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Enable or disable of the write protection is done.
  *          - ERROR:   Enable or disable of the write protection is not done.
  */
ErrorStatus OPENBL_FLASH_EnableWriteProtection(uint8_t *p_list_of_sectors, uint32_t length)
{
  uint8_t index;
  uint8_t sectors;
  ErrorStatus status = SUCCESS;
  FLASH_OBProgramInitTypeDef flash_ob;

  /* Unlock the FLASH registers & Option Bytes registers access */
  OPENBL_FLASH_OB_Unlock();

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  /* Enable FLASH_WRP_SECTORS write protection */
  flash_ob.OptionType = OPTIONBYTE_WRP;
  flash_ob.WRPState   = OB_WRPSTATE_ENABLE;
  flash_ob.WRPSector  = 0U;

  for (index = 0U; index < length; index++)
  {
    sectors = p_list_of_sectors[index];

    if (sectors < FLASH_SECTOR_NB)
    {
      flash_ob.WRPSector |= (uint32_t)1U << (sectors);
    }
  }

  if (flash_ob.WRPSector != 0U)
  {
    status = (HAL_FLASHEx_OBProgram(&flash_ob) != HAL_OK) ? ERROR : SUCCESS;
  }

  return status;
}

/**
  * @brief  This function is used to disable write protection.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Enable or disable of the write protection is done.
  *          - ERROR:   Enable or disable of the write protection is not done.
  */
static ErrorStatus OPENBL_FLASH_DisableWriteProtection(void)
{
  ErrorStatus status;
  FLASH_OBProgramInitTypeDef flash_ob;

  /* Unlock the FLASH registers & Option Bytes registers access */
  OPENBL_FLASH_OB_Unlock();

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  /* Disable FLASH_WRP_SECTORS write protection */
  flash_ob.OptionType = OPTIONBYTE_WRP;
  flash_ob.WRPState   = OB_WRPSTATE_DISABLE;

  status = (HAL_FLASHEx_OBProgram(&flash_ob) != HAL_OK) ? ERROR : SUCCESS;

  return status;
}


/**
  * @brief  Wait for a FLASH operation to complete.
  * @param  timeout Maximum FLASH operation timeout.
  * @retval Returns the status of the FLASH operation.
  */
#if defined (__ICCARM__)
__ramfunc HAL_StatusTypeDef OPENBL_FLASH_WaitForLastOperation(uint32_t timeout)
#else
__attribute__((section(".ramfunc"))) HAL_StatusTypeDef OPENBL_FLASH_WaitForLastOperation(uint32_t timeout)
#endif /* (__ICCARM__) */
{
  uint32_t error;
  uint32_t counter         = 0U;
  HAL_StatusTypeDef status = HAL_OK;

  /* While the FLASH is in busy state, send busy byte to the host */
  while (__HAL_FLASH_GET_FLAG(FLASH_SR_BUSY) != 0U)
  {
    /* Check if we need to send a busy byte
       NOTE: this can be removed if I2C protocol is not used */
    if (FlashBusyState == FLASH_BUSY_STATE_ENABLED)
    {
      OPENBL_I2C_SendBusyByte();
    }

    counter++;

    if (counter > timeout)
    {
      status = HAL_TIMEOUT;
      break;
    }
  }

  /* Check FLASH operation error flags */
  error = (FLASH->ISR & FLASH_FLAG_ALL_ERRORS);

  if (error != 0U)
  {
    /* Save the error code */
    FlashProcess.ErrorCode |= error;

    /* Clear error programming flags */
    FLASH->ICR = FLASH_FLAG_ALL_ERRORS;

    status = HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Perform a mass erase or erase the specified FLASH memory sectors.
  * @param[in] p_erase_init pointer to an FLASH_EraseInitTypeDef structure that
  *         contains the configuration information for the erasing.
  * @param[out] p_sector_error pointer to variable that contains the configuration
  *         information on faulty sector in case of error (0xFFFFFFFF means that all
  *         the sectors have been correctly erased).
  * @retval Returns the status of the FLASH operation.
  */
#if defined (__ICCARM__)
__ramfunc HAL_StatusTypeDef OPENBL_FLASH_ExtendedErase(FLASH_EraseInitTypeDef *p_erase_init,
                                                       uint32_t *p_sector_error)
#else
__attribute__((section(".ramfunc"))) HAL_StatusTypeDef OPENBL_FLASH_ExtendedErase(
  FLASH_EraseInitTypeDef *p_erase_init, uint32_t *p_sector_error)
#endif /* (__ICCARM__) */
{
  HAL_StatusTypeDef status;

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  /* Process Locked */
  __HAL_LOCK(&pFlash);

  /* Reset error code */
  pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;

  /* Wait for last operation to be completed */
  status = OPENBL_FLASH_WaitForLastOperation(PROGRAM_TIMEOUT);

  if (status == HAL_OK)
  {
    if (p_erase_init->TypeErase == FLASH_TYPEERASE_MASSERASE)
    {
      /* Set the Mass Erase Bit and proceed to erase */
      SET_BIT(FLASH->CR, (FLASH_CR_BER | FLASH_CR_START));

      status = OPENBL_FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
    }
    else
    {
      /* Initialization of p_sector_error variable */
      *p_sector_error = 0xFFFFFFFFU;

      /* Proceed to erase the sectors */
      MODIFY_REG(FLASH->CR, (FLASH_CR_SSN | FLASH_CR_SER | FLASH_CR_START),
                 ((p_erase_init->Sector << FLASH_CR_SSN_Pos) | FLASH_CR_SER | FLASH_CR_START));

      /* Wait for last operation to be completed */
      status = OPENBL_FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);

      if (status != HAL_OK)
      {
        /* In case of error, stop erase procedure and return the faulty sector */
        *p_sector_error = p_erase_init->Sector;
      }

      /* If the erase operation is completed, disable the PER Bit */
      CLEAR_BIT(FLASH->CR, FLASH_CR_SER);
    }
  }

  /* Process Unlocked */
  __HAL_UNLOCK(&pFlash);

  return status;
}
