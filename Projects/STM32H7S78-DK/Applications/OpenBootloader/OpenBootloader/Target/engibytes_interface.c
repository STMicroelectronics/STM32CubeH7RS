/**
  ******************************************************************************
  * @file    engibytes_interface.c
  * @author  MCD Application Team
  * @brief   Contains Engi Bytes access functions
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
#include "common_interface.h"

#include "openbl_mem.h"

#include "app_openbootloader.h"
#include "engibytes_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
OPENBL_MemoryTypeDef EB_Descriptor =
{
  EB_MEM_START_ADDRESS,
  EB_MEM_END_ADDRESS,
  EB_MEM_SIZE,
  EB_AREA,
  OPENBL_EB_Read,
  NULL,
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
uint8_t OPENBL_EB_Read(uint32_t address)
{
  return (*(uint8_t *)(address));
}
