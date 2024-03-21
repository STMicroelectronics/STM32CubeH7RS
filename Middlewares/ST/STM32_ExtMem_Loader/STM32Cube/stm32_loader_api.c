/**
 ******************************************************************************
 * @file    stm32_loader_api.c
 * @author  MCD Application Team
 * @brief   This file defines the operations of the external loader
 *          used by STM32CubeProgramer in programming/erasing the device.
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

#if defined(STM32_EXTMEMLOADER_STM32CUBETARGET)
/* Includes ------------------------------------------------------------------*/
#include "stm32_device_info.h"
#include "stm32_loader_api.h"
#include "memory_wrapper.h"
#include <string.h>
#include <stdio.h>

/** @addtogroup STM32_LOADER STM32Cube loader
  * @ingroup EXTMEM_LOADER
  * @{
  */
/** @defgroup STM32_LOADER_Exported_Functions Loarder Entry points
  * @{
  */

/**
  * @}
  */
/* Private Macros ------------------------------------------------------------*/
/** @defgroup STM32_LOADER_Private_Definition Private Definition
  * @{
  */

/** @defgroup STM32_LOADER_Private_Macros Private Macros
  * @{
  */
/**
 * @brief trace macro
 */
#if !defined(STM32_EXTMEMLOADER_TRACE)
#define STM32_EXTMEMLOADER_TRACE(_MSG_)
#endif /* STM32_EXTMEMLOADER_TRACE */

/**
  * @}
  */

/**
  * @}
  */

#if defined(__ICCARM__)
#pragma section = ".bss"
#elif defined(__GNUC__)
extern int __bss_start__, __bss_end__;
extern int __estack_end__;
#endif /* __ICCARM__ */
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
/**
 * @brief not use and only present to avoid compilation issue
 */
 uint32_t __Vectors = 0x0;
#endif /* __CC_ARM || __ARMCC_VERSION */

/** @defgroup STM32_LOADER_Private_Debug Debug
  * @{
  */

/* Private typedefs ----------------------------------------------------------*/
/** @defgroup STM32_LOADER_Private_TypeDefs typedefs
  * @{
  */

/**
 * @brief Debug state machine typedef
 */
typedef enum {
  DEBUG_STATE_WAIT,
  DEBUG_STATE_INIT,
  DEBUG_STATE_WRITE,
  DEBUG_STATE_SECTORERASE,
  DEBUG_STATE_MASSERASE
} DEBUG_STATETypedef;

/**
  * @}
  */


/* Private variables ---------------------------------------------------------*/

/** @defgroup STM32_LOADER_Private_Debug_Variables Variables
  * @{
  */

/**
 * @brief debug state machine execution
 */
volatile DEBUG_STATETypedef exec;

/**
 * @brief debug variable to provide RAM buffer address for write operation
 */
uint8_t *Buff_address;

/**
 * @brief debug variable to provide memory address for write/erase operation
 */
uint32_t Address;

/**
 * @brief debug state to proive size for write/erase operation 
 */
uint32_t Size;

/**
 * @brief debug variable used to avoid compilation warning
 */
volatile const int condition = 1;

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup STM32_LOADER_Private_Definition
  * @{
  */

/** @defgroup STM32_LOADER_Private_Variables Private Variables
  * @{
  */

/**
 * @brief status of the mapped mode  MEM_DISABLE or MEM_ENABLE
 */
MEM_MAPSTAT MemoryMappedMode;

/**
  * @}
  */

/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @defgroup STM32_LOADER_Private_Functions Private Functions
  * @{
  */
static uint32_t CheckSum(uint32_t StartAddress, uint32_t Size, uint32_t InitVal);
/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup STM32_LOADER_Private_Debug
  * @{
  */

/** @defgroup STM32_LOADER_Exported_Debug_Functions Debug Functions
  * @{
  */

/**
 * @brief entry point definition for debug
 */
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
/*nothing to be done*/
#elif defined(__GNUC__)
uint32_t g_pfnVectors[] __attribute__((section(".isr_vector"))) = {
};
void __attribute__((used,optimize("Os"))) Reset_Handler(void)
{

  asm(
      "ldr r0, =_estack\n"
      "nop\n"
      "mov     sp, r0\n");

  asm(
      "ldr r0, =main\n"
      "nop\n"
      "mov     pc, r0\n");
}
#elif defined(__ICCARM__)
extern const uint32_t __ICFEDIT_region_RAM_end__;

void Reset_Handler(void);

uint32_t __vector_table[] __attribute__((section(".vectors"))) = {
    (uint32_t)(&__ICFEDIT_region_RAM_end__), /* Stack pointer */
    (uint32_t)&Reset_Handler                 /* Reset handler */
};

void Reset_Handler(void)
{
  main();
}
#endif /* STM32_EXTFLASHLOADER_DEBUG_NA */


/**
 * @brief  main function used for debug purpose
  * @retval the function always returns 0
 */
int main(void)
{
  exec = DEBUG_STATE_INIT;
  do
  {
    switch (exec)
    {
    case DEBUG_STATE_WAIT:{
      /* nothing to do just loop to wait an exec update */
      break;
    }
    case DEBUG_STATE_INIT :{
      Init();
      exec = DEBUG_STATE_WAIT;
      break;
    }
    case DEBUG_STATE_WRITE : {
      Write (Address, Size, (uint8_t *)Buff_address);
      exec = DEBUG_STATE_INIT;
      break;
    }
    case DEBUG_STATE_MASSERASE :{
      MassErase(0);
      exec = DEBUG_STATE_INIT;
      break;
    }

    case DEBUG_STATE_SECTORERASE :{
      SectorErase(Address,Size);
      exec = DEBUG_STATE_INIT;
      break;
    }
    }

  } while (condition);

  return 0;
}

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup STM32_LOADER_Exported_Functions
  * @{
  */

/**
 * @brief  System initialization.
  * @retval  1: Operation succeeded or 0: Operation failed
 */
uint32_t Init(void)
{
  uint32_t retr = 1;
  uint8_t *startadd;
  uint32_t size;

  /*  get ZI Init variables to zero */
#if defined(__ICCARM__)
  startadd = __section_begin(".bss");
  size = __section_size(".bss");
#elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
  extern uint32_t Image$$PrgData$$ZI$$Base;
  extern uint32_t Image$$PrgData$$ZI$$Limit;

  startadd = (uint8_t *)&Image$$PrgData$$ZI$$Base;
  size     = (uint32_t)&Image$$PrgData$$ZI$$Limit - (uint32_t)startadd;
#elif defined ( __GNUC__ )
  startadd = (uint8_t*)& __bss_start__;
  size     = (uint8_t*)& __bss_end__ - (uint8_t*)& __bss_start__;
#else
#error "the compiler is not yet supported"
#endif /* __ICCARM__ */

  /*  Init variables to zero */
  memset(startadd, 0, size * sizeof(uint8_t));

  extmemloader_Init();

  /* Enable default memory mapped mode */
  if (memory_mapmode(MEM_MAPENABLE)  != MEM_OK)
  {
    retr = 0;
  }

  MemoryMappedMode = MEM_MAPENABLE;

  return retr;
}

/**
 * @brief   Program memory.
 * @param   Address: page address
 * @param   Size   : size of data
 * @param   buffer : pointer to data buffer
 * @retval  1      : Operation succeeded
 * @retval  0      : Operation failed
 */
KeepInCompilation uint32_t Write(uint32_t Address, uint32_t Size, uint8_t *buffer)
{
  STM32_EXTMEMLOADER_TRACE("\n\nCall Function Write\n");

  /* Disable memory mapped mode if enabled */
  if (MemoryMappedMode == MEM_MAPENABLE)
  {
    if (memory_mapmode(MEM_MAPDISABLE)  != MEM_OK)
    {
      return 0;
    }
    MemoryMappedMode = MEM_MAPDISABLE;
  }

  if (memory_write(Address, Size, buffer) != MEM_OK)
  {
    return 0;
  }

  return 1;
}

/**
 * @brief 	 Full erase of the device
 * @param 	 Parallelism : 0
 * @retval  1           : Operation succeeded
 * @retval  0           : Operation failed
 */
KeepInCompilation uint32_t MassErase(uint32_t Parallelism)
{
  STM32_EXTMEMLOADER_TRACE("\n\nCall Function MassErase\n");
  /* Disable memory mapped mode if enabled */
  if (MemoryMappedMode == MEM_MAPENABLE)
  {
    if (memory_mapmode(MEM_MAPDISABLE)  != MEM_OK)
    {
      return 0;
    }
    MemoryMappedMode = MEM_MAPDISABLE;
  }

  if (memory_masserase() != MEM_OK)
  {
    return 0;
  }

  return 1;
}

/**
 * @brief   Sector erase.
 * @param   EraseStartAddress :  erase start address
 * @param   EraseEndAddress   :  erase end address
 * @retval  None
 */
KeepInCompilation uint32_t SectorErase(uint32_t EraseStartAddress, uint32_t EraseEndAddress)
{
  STM32_EXTMEMLOADER_TRACE("\n\nCall Function SectorErase\n");
  /* Disable memory mapped mode if enabled */
  if (MemoryMappedMode == MEM_MAPENABLE)
  {
    if (memory_mapmode(MEM_MAPDISABLE)  != MEM_OK)
    {
      return 0;
    }
    MemoryMappedMode = MEM_MAPDISABLE;
  }

  if (memory_sectorerase(EraseStartAddress, EraseEndAddress, STM32EXTLOADER_DEVICE_SECTOR_SIZE) != MEM_OK)
  {
    return 0;
  }

  return 1;
}

/**
  * Description :
  * Verify flash memory with RAM buffer and calculates checksum value of
  * the programmed memory
  * Inputs    :
  *      FlashAddr     : Flash address
  *      RAMBufferAddr : RAM buffer address
  *      Size          : Size (in WORD)
  *      InitVal       : Initial CRC value
  * outputs   :
  *     R0             : Operation failed (address of failure)
  *     R1             : Checksum value
  * Note: Optional for all types of device
  */
KeepInCompilation uint64_t Verify (uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size, uint32_t Missalignement)
{
  uint32_t verified_data = 0, init_val = 0;
  uint32_t memory_addr = MemoryAddr;
  uint32_t ram_addr = RAMBufferAddr;
  uint32_t size = Size*4;
  uint64_t checksum;

  STM32_EXTMEMLOADER_TRACE("\n\nCall Function Verify\n");

  /* Enable memory mapped mode if disabled */
  if (MemoryMappedMode == MEM_MAPDISABLE)
  {
    if (memory_mapmode(MEM_MAPENABLE) != MEM_OK)
    {
      return 0;
    }
    MemoryMappedMode = MEM_MAPENABLE;
  }

  checksum = CheckSum(memory_addr + (Missalignement & 0xFU), size - ((Missalignement >> 16U) & 0xFU), init_val);
  while (size>verified_data)
  {
    if ( *(uint8_t*)memory_addr++ != *((uint8_t*)ram_addr + verified_data))
      return ((checksum<<32U) + (memory_addr + verified_data));

    verified_data++;
  }

  return (checksum<<32U);
}

/**
  * @}
  */

/** @addtogroup STM32_LOADER_Private_Functions Private Functions
  * @{
  */

/**
 * Description :
 * Calculates checksum value of the memory zone
 * Inputs    :
 *      StartAddress  : Flash start address
 *      Size          : Size (in WORD)
 *      InitVal       : Initial CRC value
 * outputs   :
 *     R0             : Checksum value
 * Note: Optional for all types of device
 */
static uint32_t CheckSum(uint32_t StartAddress, uint32_t Size, uint32_t InitVal)
{
  uint8_t missalignement_address = StartAddress % 4U;
  uint8_t missalignement_size = Size;
  uint32_t cnt;
  uint32_t init_val = InitVal;
  uint32_t start_addr;
  uint32_t val;

  STM32_EXTMEMLOADER_TRACE("\n\nCall Function CheckSum\n");

  start_addr = StartAddress - StartAddress % 4U;
  Size += (Size % 4U == 0) ? 0U : 4U - (Size % 4U);

  for (cnt = 0; cnt < Size; cnt += 4)
  {
    val = *(uint32_t *)start_addr;
    if (missalignement_address)
    {
      switch (missalignement_address)
      {
      case 1:
        init_val += (uint8_t)(val >> 8U & 0xFFU);
        init_val += (uint8_t)(val >> 16U & 0xFFU);
        init_val += (uint8_t)(val >> 24U & 0xFFU);
        missalignement_address -= 1;
        break;
      case 2:
        init_val += (uint8_t)(val >> 16U & 0xFFU);
        init_val += (uint8_t)(val >> 24U & 0xFFU);
        missalignement_address -= 2;
        break;
      case 3:
        init_val += (uint8_t)(val >> 24U & 0xFFU);
        missalignement_address -= 3;
        break;
      }
    }
    else if ((Size - missalignement_size) % 4 && (Size - cnt) <= 4U)
    {
      switch (Size - missalignement_size)
      {
      case 1U:
        init_val += (uint8_t)val;
        init_val += (uint8_t)(val >> 8U & 0xFFU);
        init_val += (uint8_t)(val >> 16U & 0xFFU);
        missalignement_size -= 1;
        break;
      case 2U:
        init_val += (uint8_t)val;
        init_val += (uint8_t)(val >> 8U & 0xFFU);
        missalignement_size -= 2;
        break;
      case 3U:
        init_val += (uint8_t)val;
        missalignement_size -= 3U;
        break;
      }
    }
    else
    {
      init_val += (uint8_t)val;
      init_val += (uint8_t)(val >> 8U & 0xFFU);
      init_val += (uint8_t)(val >> 16U & 0xFFU);
      init_val += (uint8_t)(val >> 24U & 0xFFU);
    }
    start_addr += 4U;
  }

  return (init_val);
}

/**
  * @}
  */

/**
  * @}
 */

#endif /* STM32_EXTMEMLOADER_CUBEPROGRAMMERTARGET */
