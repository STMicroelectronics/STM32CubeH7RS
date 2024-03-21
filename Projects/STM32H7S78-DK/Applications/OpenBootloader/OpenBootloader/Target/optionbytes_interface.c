/**
  ******************************************************************************
  * @file    optionbytes_interface.c
  * @author  MCD Application Team
  * @brief   Contains Option Bytes access functions
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
#include "optionbytes_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
OPENBL_MemoryTypeDef OB_Descriptor =
{
  OB_MEM_START_ADDRESS,
  OB_MEM_END_ADDRESS,
  OB_MEM_SIZE,
  OB_AREA,
  OPENBL_OB_Read,
  OPENBL_OB_Write,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  This function is used to read data from a given address.
  * @param  address The address to be read.
  * @retval Returns the read value.
  */
uint8_t OPENBL_OB_Read(uint32_t address)
{
  return (*(uint8_t *)(address));
}

/**
  * @brief  Launch the option byte loading.
  * @retval None.
  */
void OPENBL_OB_Launch(void)
{
  /* There is no need for option byte launch in this product */
}

/**
  * @brief  This function is used to write data in Option bytes.
  * @param  address The address where that data will be written.
  * @param  p_data The data to be written.
  * @param  data_length The length of the data to be written.
  * @retval None.
  */
void OPENBL_OB_Write(uint32_t address, uint8_t *p_data, uint32_t data_length)
{
  HAL_StatusTypeDef status;

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  /* Wait for last operation to be completed */
  status = OPENBL_FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);

  /* Unlock the FLASH & Option Bytes Registers access */
  if (HAL_FLASH_Unlock() != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_FLASH_OB_Unlock() != HAL_OK)
  {
    Error_Handler();
  }

  if ((address == OB_MEM_START_ADDRESS) && (status == HAL_OK))
  {
    /* Enable FLASH programming bit */
    SET_BIT(FLASH->OPTCR, FLASH_OPTCR_PG_OPT);

    /* Write FLASH security status register programming */
    if (data_length > 4U)
    {
      WRITE_REG(FLASH->NVSRP, (*(uint32_t *)(uint32_t)&p_data[4]));
    }

    /* Write FLASH RoT status register programming */
    if (data_length > 12U)
    {
      WRITE_REG(FLASH->ROTSRP, (*(uint32_t *)(uint32_t)&p_data[12]));
    }

    /* Write FLASH OTP lock status programming register */
    if (data_length > 20U)
    {
      WRITE_REG(FLASH->OTPLSRP, (*(uint32_t *)(uint32_t)&p_data[20]));
    }

    /* Write FLASH write protection status register programming */
    if (data_length > 28U)
    {
      WRITE_REG(FLASH->WRPSRP, (*(uint32_t *)(uint32_t)&p_data[28]));
    }

    /* Write FLASH hide protection status register programming */
    if (data_length > 52U)
    {
      WRITE_REG(FLASH->HDPSRP, (*(uint32_t *)(uint32_t)&p_data[52]));
    }

    /* Write the option byte word 1 */
    if (data_length > 100U)
    {
      WRITE_REG(FLASH->OBW1SRP, (*(uint32_t *)(uint32_t)&p_data[100]));
    }

    /* Wait for last operation to be completed */
    status = OPENBL_FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);

    if (status == HAL_OK)
    {
      /* Set FLASH options control register Bit */
      CLEAR_BIT(FLASH->OPTCR, FLASH_OPTCR_PG_OPT);
    }
  }
}
