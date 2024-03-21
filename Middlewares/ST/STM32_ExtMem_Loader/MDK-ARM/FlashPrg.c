 /**
  ******************************************************************************
  * @file    FlashPrg.c
  * @author  MCD Application Team
  * @brief   This file is inherit from MDK-ARM.
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
  * Arm Limited loader structure based on API published by IAR Systems in flashOS.h.
  * The whole contents of this file is a creation by STMicroelectronics licensed
  * to you under the License as specified above. We preserve the API definition made
  * by Arm Limited and update the content of the functions.
  */
/*
 * Copyright (c) 2010-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if defined(STM32_EXTMEMLOADER_MDKARMTARGET)
/* Includes ------------------------------------------------------------------*/
#include "FlashOS.h"
#include "memory_wrapper.h"
#include "string.h"
#include "stdlib.h"

/** @addtogroup MDKARM_LOADER MDK-ARM loader
  * @ingroup EXTMEM_LOADER
  * @{
  */

/**
  * @}
  */

/** @defgroup MDKARM_LOADER_Private_Debug Debug
  * @ingroup MDKARM_LOADER
  * @{
  */

/**
  * @}
  */

/* Private macros ---------------------------------------------------------*/
/** @addtogroup MDKARM_LOADER_Private_Debug_Macros Macros
  * @ingroup MDKARM_LOADER_Private_Debug
  * @{
  */

/**
 * @brief debug state machine
 */
  typedef enum {
        DEBUG_STOP = 0,
        DEBUG_INIT,
        DEBUG_WRITE,
        DEBUG_ERASESECTOR,
        DEBUG_ERASECHIP,
        DEBUG_READ
  } DEBUG_STATE;

/**
 * @brief not use and only present to avoid compilation issue
 */
uint32_t __Vectors = 0x0;

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup MDKARM_LOADER_Private_Variables Private Variables
  * @ingroup MDKARM_LOADER
  * @{
  */

/**
 * @brief status of the mapped mode MEM_DISABLE or MEM_ENABLE
 */
MEM_MAPSTAT mapmode = MEM_MAPDISABLE;

/**
  * @}
  */

/** @defgroup MDKARM_LOADER_Private_Debug_Variables Variables
  * @ingroup MDKARM_LOADER_Private_Debug
  * @{
  */

/**
 * @brief variable used to avoid multiple initialisation
 */
__attribute__((used)) const int initdone = 0xDEADDEAD;

/**
 * @brief variable used to avoid multiple initialisation
 */
__attribute__((used)) volatile int* Ptrinitdone = 0x0;

/**
  * @}
  */

/** @defgroup MDKARM_LOADER_Private_Debug_Variables Variables
  * @ingroup MDKARM_LOADER_Private_Debug
  * @{
  */

/**
 * @brief debug state machine execution
 */
volatile DEBUG_STATE exec_debug = 0;

/**
 * @brief debug variable to provide RAM buffer address for write operation
 */
uint8_t* BufferStart;

/**
 * @brief debug variable to provide memory address for write/erase operation
 */
uint32_t Address;

/**
 * @brief debug state to provide size for write/erase operation
 */
uint32_t Size;

/**
  * @}
  */


/** @defgroup MDKARM_LOADER_Exported_Debug_Functions functions
  * @ingroup MDKARM_LOADER_Private_Debug
  * @{
  */
int main(void)
{

  /* The debug is based on the usage of global variables which help to simulate
     the loader behavior in the IDE context
       - theFlashParams : structure used has operation parameters
       - exec : variable used to launch a test
       - FlashBufferStart-FlashBufferEnd : RAM  buffer space used to store
         read/write/erase operations.
  */
  exec_debug = DEBUG_INIT;

  do
  {
    /* Wait an update of exec_debug to launch a debug operation */
    while(exec_debug == DEBUG_STOP)
    {
      HAL_GetTick();
    };

    /* Execute the requested action */
    switch(exec_debug)
    {
    case DEBUG_INIT :
      Init(0, 0, 0);
      exec_debug = DEBUG_STOP;
      break;

    case DEBUG_WRITE : {
      ProgramPage (Address, Size, BufferStart);
      exec_debug = DEBUG_INIT;
      break;
    }

    case DEBUG_READ : {
      /* This implementation could be used to confirm that the write operation
      is conform with the request DEBUG_FLASHWRITE without any change on
      the content of theFlashParams.
      the check is using map mode to control the data
      */

      exec_debug = DEBUG_INIT;
      break;
    }

    case DEBUG_ERASESECTOR :
      EraseSector(Address);
      exec_debug = DEBUG_INIT;
      break;

    case DEBUG_ERASECHIP:
      EraseChip();
      exec_debug = DEBUG_INIT;
      break;
    default:
      exec_debug = DEBUG_STOP;
      break;
    }

  } while(1);

  return 0;
}

/**
  * @}
  */

/** @defgroup MDKARM_LOADER_Exported_Functions Loarder Entry points
  * @ingroup MDKARM_LOADER
  * @{
  */

/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */
int Init (unsigned long adr, unsigned long clk, unsigned long fnc)
{

  int retr = 0;

  Ptrinitdone = (int *)&initdone;

	if (*Ptrinitdone == 0xDEADDEAD)  /* the init should be done only once to improve the performance */
	{

	  /* get the ZI data location */
	  extern uint32_t Image$$PrgData$$ZI$$Base;
    extern uint32_t Image$$PrgData$$ZI$$Limit;

    uint8_t *start_address = (uint8_t *)&Image$$PrgData$$ZI$$Base;
    uint32_t size  = (uint32_t)&Image$$PrgData$$ZI$$Limit - (uint32_t)start_address;

    memset(start_address, 0, ((size) * sizeof(uint8_t)));

    *Ptrinitdone = 1;

    /* set the ZI data to zero */
    memset(start_address, 0, size);

    /* mark initialisation done */
    *Ptrinitdone = 1;

    /* System initialisation */
    extmemloader_Init();

    /* memory initialisation */
    if (memory_init() != MEM_OK)
    {
      retr = 1;
    }
  }

  if (retr == 0)
  {
    /* enable memory mapped mode if disabled */
    if (mapmode == MEM_MAPDISABLE)
    {
      if (memory_mapmode(MEM_MAPENABLE) == MEM_OK)
      {
         mapmode = MEM_MAPENABLE;
      }
      else
      {
        retr = 1;
      }
    }
  }
  return retr;
}

/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int UnInit(unsigned long fnc)
{
  return (0);
}

/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */
int EraseChip(void)
{
  int retr = 0;

  /* disable memory mapped mode if enabled */
  if (mapmode == MEM_MAPENABLE)
  {
    if (memory_mapmode(MEM_MAPDISABLE) == MEM_OK)
    {
      mapmode = MEM_MAPDISABLE;
    }
    else
    {
      retr = 1;
    }
  }
  /* Execute the masserase operation */
  if(retr == 0)
  {
    if(memory_masserase() != MEM_OK)
    {
      retr = 1;
    }
  }
  return retr;
}

/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */
int EraseSector(unsigned long adr)
{
  int retr = 0;

  /* disable memory mapped mode if enabled */
  if (mapmode == MEM_MAPENABLE)
  {
    if (memory_mapmode(MEM_MAPDISABLE) == MEM_OK)
    {
      mapmode = MEM_MAPDISABLE;
    }
    else
    {
      retr = 1;
    }
  }
  /* Execute the sectorerase operation */
  if (retr == 0)
  {
    if (memory_sectorerase(adr, adr, STM32EXTLOADER_DEVICE_SECTOR_SIZE) != MEM_OK)
    {
      retr = 1;
    }
  }
  /* enable memory mapped mode if disabled */
  if (mapmode == MEM_MAPDISABLE)
  {
    if (memory_mapmode(MEM_MAPENABLE) == MEM_OK)
    {
      mapmode = MEM_MAPENABLE;
    }
    else
    {

      retr = 1;
    }
  }

  return retr;
}

/*
 *  Program Page in Flash Memory
 *    Parameter: adr:  Page Start Address
 *               sz:   Page Size
 *               buf:  Page Data
 *  Return Value:   0 - OK,  1 - Failed
 */
int ProgramPage(unsigned long block_start, unsigned long size, unsigned char *buffer)
{
  int retr = 0;

 /* disable memory mapped mode if enabled */
  if (mapmode == MEM_MAPENABLE)
  {
    if (memory_mapmode(MEM_MAPDISABLE) == MEM_OK)
    {
      mapmode = MEM_MAPDISABLE;
    }
    else
    {
      retr = 1;
    }
  }
	/* Execute the write operation */
  if (retr == 0)
  {
    if (memory_write(block_start, size, buffer) != MEM_OK)
    {
      retr = 1;
    }
  }

  return retr;
}

/*
 *    verify Flash Memory
 *    Parameter: adr:  Page Start Address
 *               sz:   Page Size
 *               buf:  Page Data
 *  Return Value:   0 - OK,  adr - Failed
 */
unsigned long Verify(unsigned long adr, unsigned long sz, unsigned char *buf)
{
  int retr = 0;

  while (sz-- > 0)
  {
    if (*(char *)adr++ != *((char *)buf++))
      return (adr);
  }

  return adr;
}

/**
  * @}
  */

#endif /* STM32_EXTMEMLOADER_MDKARMTARGET */