/**
  ******************************************************************************
  * @file    stm32_flash_wrapper.c
  * @author  MCD Application Team
  * @brief   This file defines the services used by the different types
  *          of the external loader available in the module.

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
/**
  * Important note
  * --------------
  * This file is the implementation of functions to wrap our memory API with
  * IAR loader structure based on API published by IAR Systems in flash_loader.h.
  * The whole contents of this file is a creation by STMicroelectronics licensed
  * to you under the License as specified above. We preserve the API definition made
  * by IAR System and update the content of the functions.
  */
//------------------------------------------------------------------------------
//
// Copyright (c) 2008-2015 IAR Systems
//
// Licensed under the Apache License, Version 2.0 (the "License")
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// $Revision: 54713 $
//
//------------------------------------------------------------------------------

#if defined(STM32_EXTMEMLOADER_EWARMTARGET)
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32_extmemloader_conf.h"
/* The flash loader framework API declarations */
#include "flash_loader.h"
#include "flash_loader_extra.h"
#include "memory_wrapper.h"

/** @defgroup EXTMEM_LOADER_EWARM_DEBUG Debug
  * @ingroup EXTMEM_LOADER_EWARM
  * @{
  */

/**
  * @}
  */

/* Private typedefs ----------------------------------------------------------*/
/** @defgroup EXTMEM_LOADER_EWARM_DEBUG_TypeDefs TypeDefs
  * @ingroup EXTMEM_LOADER_EWARM_DEBUG
  * @{
  */

/**
 * @brief Debug state machine typedef
 */
typedef enum
{
  DEBUG_STOP = 0,
  DEBUG_FLASHINIT,
  DEBUG_FLASHWRITE,
  DEBUG_ERASEWRITE,
  DEBUG_FLASHWREAD,
  DEBUG_FLASHERASE
} DEBUG_STATE;

/**
  * @}
  */

/** @defgroup EXTMEM_LOADER_EWARM_WRAPPER_Macros Macros
  * @ingroup EXTMEM_LOADER_EWARM_WRAPPER
  * @{
  */

/**
 * @brief boolean value true
 */
#define STM32_WRAPPER_TRUE 1

/**
 * @brief boolean value false
 */
#define STM32_WRAPPER_FALSE 0

/**
  * @}
  */


/* Private variables ---------------------------------------------------------*/
/** @defgroup EXTMEM_LOADER_EWARM_WRAPPER_Varaibles flash variables
  * @ingroup EXTMEM_LOADER_EWARM_WRAPPER
  * @{
  */

/**
 * @brief status of the mapped mode  MEM_DISABLE or MEM_ENABLE
 */
MEM_MAPSTAT FlashMappedMode = MEM_MAPDISABLE;

/**
 * @brief variable used to avoid multiple initialization
 */
__root const uint8_t FlashInitDone = STM32_WRAPPER_FALSE;

/**
  * @}
  */

/** @defgroup EXTMEM_LOADER_EWARM_DEBUG_TypeDefs TypeDefs
  * @ingroup EXTMEM_LOADER_EWARM_DEBUG
  * @{
  */

/**
 * @brief debug state machine execution
 */
volatile DEBUG_STATE exec_debug;

/**
 * @brief debug variable to provide RAM buffer address for write operation
 */
uint8_t *BufferStart;

/**
 * @brief debug variable to provide RAM buffer address for write operation
 */
uint8_t *BufferEnd;

/**
  * @}
  */

/** @defgroup EXTMEM_LOADER_EWARM_WRAPPER flash wrapper
  * @ingroup EXTMEM_LOADER_EWARM
  * @{
  */
/*************************************************************************
 * Function Name: FlashInit
 * Parameters: Flash Base Address
 *
 * Return: 0: RESULT_OK
 *         1: RESULT_Fail
 *
 * Description: Init OSPI flash driver.
 *************************************************************************/
#if USE_ARGC_ARGV
uint32_t FlashInit(void *base_of_flash, uint32_t image_size,
                   uint32_t link_address, uint32_t flags,
                   int argc, char const *argv[])
#else
uint32_t FlashInit(void *base_of_flash, uint32_t image_size,
                   uint32_t link_address, uint32_t flags)
#endif /*USE_ARGC_ARGV*/
{
  uint32_t retr = RESULT_OK;
  uint32_t AddressFlashInitDone = (uint32_t)&FlashInitDone;

  if (STM32_WRAPPER_FALSE == *((uint8_t *)AddressFlashInitDone))
  {
    /* Initialize the zi memory */
#pragma section = ".bss"
    uint8_t *startadd = __section_begin(".bss");
    uint32_t size = __section_size(".bss");
    memset(startadd, 0, size * sizeof(uint8_t));

    /* System initialisation */
    extmemloader_Init();

    *((uint8_t *)AddressFlashInitDone) = STM32_WRAPPER_TRUE;
  }

  /* Map the memory */
  if (FlashMappedMode == MEM_MAPDISABLE)
  {
    if (memory_mapmode(MEM_MAPENABLE) != MEM_OK)
    {
      retr = RESULT_ERROR;
    }
    FlashMappedMode = MEM_MAPENABLE;
  }

  return retr;
}

/*************************************************************************
 * Function Name: FlashWrite
 * Parameters: block base address, offset in block, data size, ram buffer
 *             pointer
 * Return: 0: RESULT_OK
 *         1: RESULT_Fail
 *
 * Description. Writes data in the memory
 *************************************************************************/
uint32_t FlashWrite(void *block_start,
                    uint32_t offset_into_block,
                    uint32_t count,
                    char const *buffer)
{
  uint8_t *Src;
  uint32_t Dest;
  uint32_t size;
  uint32_t retr = RESULT_OK;

  /* exit the map mode if not already done */
  if (FlashMappedMode == MEM_MAPENABLE)
  {
    if (memory_mapmode(MEM_MAPDISABLE) != MEM_OK)
    {
      retr = RESULT_ERROR;
    }
    FlashMappedMode = MEM_MAPDISABLE;
  }

  if (retr == RESULT_OK)
  {
    /* Set destination address */
    Dest = (uint32_t)block_start + offset_into_block;
    /* Set source address */
    Src = (uint8_t *)(buffer);
    /* Set the operation size */
    size = count;
    /* Execute the write operation */
    if (memory_write(Dest, size, Src) != MEM_OK)
    {
      retr = RESULT_ERROR;
    }
  }

  return retr;
}

/*************************************************************************
 * Function Name: FlashErase
 * Parameters:  Block Address, Block Size
 *
 * Return: 0:RESULT_OK
 *         1:RESULT_Fail
 *
 * Description: Erase block
 *************************************************************************/
uint32_t FlashErase(void *block_start,
                    uint32_t block_size)
{
  uint32_t result = RESULT_OK;

  /* exit the map mode if not already done */
  if (FlashMappedMode == MEM_MAPENABLE)
  {
    if (memory_mapmode(MEM_MAPDISABLE) != MEM_OK)
    {
      result = RESULT_ERROR;
      goto error;
    }
    FlashMappedMode = MEM_MAPDISABLE;
  }

  if (MEM_OK != memory_sectorerase((uint32_t)block_start & 0xffffffff, ((uint32_t)block_start & 0xffffffff) + block_size, block_size))
  {
    result = RESULT_ERROR;
  }
error:
  return result;
}

/*************************************************************************
 * Function Name: FlashSignoff
 * Parameters: none
 *
 * Return: 0: RESULT_OK
 *         1: RESULT_ERROR
 *
 * Description:
 *************************************************************************/
#if USE_ARGC_ARGV
OPTIONAL_SIGNOFF
uint32_t FlashSignoff(void)
{
  uint32_t result = RESULT_OK;
  /* exit the map mode if not already done */
  if (FlashMappedMode == MEM_ENABLE)
  {
    /* Disable memory mapped mode if enabled */
    if (memory_mapmode(MEM_DISABLE) != MEM_OK)
    {
      result = RESULT_ERROR;
    }
    else
    {
      FlashMappedMode = MEM_DISABLE;
    }
  }
  return result;
}
#endif

/*************************************************************************
 * Function Name: FlashChecksum
 * Parameters: none
 *
 * Return: 0:RESULT_OK
 *         wrong address
 *
 * Description:
 *************************************************************************/
OPTIONAL_CHECKSUM
uint32_t FlashChecksum(void const *begin, uint32_t count)
{

  /* enter the map mode if not already done */
  if (FlashMappedMode == MEM_MAPDISABLE)
  {

    /* enable memory mapped mode if disabled */
    if (memory_mapmode(MEM_MAPENABLE) != MEM_OK)
    {
      return RESULT_ERROR;
    }
    else
    {
      FlashMappedMode = MEM_MAPENABLE;
    }
  }
  return Crc16((uint8_t const *)begin, count);
}

/**
  * @}
  */

/*************************************************************************
 * Function Name: main
 * Parameters: none
 *
 * Return: none
 *
 * Description: this function can be used for debug purpose, it reproduces the
 * behavior of the loader in EWARM context
 *************************************************************************/

/** @defgroup EXTMEM_LOADER_EWARM_DEBUG_FUNCTION function
  * @ingroup EXTMEM_LOADER_EWARM_DEBUG
  * @{
  */
/**
  * @brief   main function used for debug.
  */
void main(void)
{

  /* The debug is based on the usage of global variables which help to simulate
     the loader behavior in the IDE context
       - theFlashParams : structure used has operation parameters
       - exec : variable used to launch a test
       - FlashBufferStart-FlashBufferEnd : RAM  buffer space used to store
         read/write/erase operations.
  */
  exec_debug = DEBUG_FLASHINIT;
  do
  {
    /* Execute the requested action */
    switch (exec_debug)
    {
    case DEBUG_FLASHINIT:
      FlashInit((void *)theFlashParams.base_ptr,
                theFlashParams.block_size,        // Image size
                theFlashParams.offset_into_block, // link adr
                theFlashParams.count);
      exec_debug = DEBUG_STOP;
      break;

    case DEBUG_FLASHWRITE:
    {
      theFlashParams.count = FlashWrite((void *)theFlashParams.base_ptr,
                                        theFlashParams.offset_into_block,
                                        theFlashParams.count,
                                        theFlashParams.buffer);
      exec_debug = DEBUG_FLASHINIT;
      break;
    }

    case DEBUG_FLASHWREAD:
    {
      /* This implementation could be used to confirm that the write operation
      is conform with the request DEBUG_FLASHWRITE without any change on
      the content of theFlashParams.
      the check is using map mode to control the data
      */
      exec_debug = DEBUG_FLASHINIT;
      break;
    }

    case DEBUG_ERASEWRITE:
      break;

    case DEBUG_FLASHERASE:
      theFlashParams.count = FlashErase((void *)theFlashParams.base_ptr,
                                        theFlashParams.block_size);
      exec_debug = DEBUG_FLASHINIT;
      break;

    default:
      exec_debug = DEBUG_STOP;
      /* Wait an update of exec_debug to launch a debug operation */
      HAL_GetTick();
      break;
    }

  } while (1);
}

/**
  * @brief   Reset Handler used for debug
  */
void Reset_Handler(void)
{
  main();
}

/**
  * @}
  */
#endif /* STM32_EXTMEMLOADER_EWARMTARGET */
