/**
  ******************************************************************************
  * @file    stm32_boot_lrun.c
  * @author  MCD Application Team
  * @brief   this file manages the boot in the mode load and run.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32_boot_lrun.h"

/** @defgroup BOOT
  * @{
  */

/** @defgroup BOOT_LRUN
  * @{
  */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

/* offset of the vector table from the start of the image. Should be set in extmem_conf.h if needed  */
#ifndef EXTMEM_HEADER_OFFSET
#define EXTMEM_HEADER_OFFSET 0
#endif
#if defined(EXTMEM_LRUN_TS_ENABLE_NS) && (!defined(EXTMEM_LRUN_DESTINATION_ADDRESS_NS) \
                                      || !defined(EXTMEM_LRUN_SOURCE_ADDRESS_NS))
#error "ExtMem user configuration incorrect : undefined parameters for Non-Secure image loading"
#endif
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
BOOTStatus_TypeDef MapMemory(void);
BOOTStatus_TypeDef CopyApplication(void);
BOOTStatus_TypeDef JumpToApplication(void);
BOOTStatus_TypeDef GetBaseAddress(uint32_t MemIndex, uint32_t *BaseAddress);

/**
  *  @addtogroup BOOT_LRUN_Exported_Functions Boot LRUN exported functions
  * @{
  */
BOOTStatus_TypeDef BOOT_Application(void)
{
  BOOTStatus_TypeDef retr;

  /* mount the memory */
  retr = MapMemory();
  if (BOOT_OK == retr)
  {
    retr = CopyApplication();
    if (BOOT_OK == retr)
    {
      /* jump on the application */
      retr = JumpToApplication();
    }
  }
  return retr;
}

/**
  * @}
  */

/**
  *  @defgroup BOOT_LRUN_Private_Functions Boot LRUN private functions
  * @{
  */

/**
  * @brief  this function maps the memory
  * @return @ref BOOTStatus_TypeDef
  */
BOOTStatus_TypeDef MapMemory(void)
{
  BOOTStatus_TypeDef retr = BOOT_OK;
  uint32_t BaseAddress = 0;

  /* Map all the memory */
  for (uint8_t index = 0; index < (sizeof(extmem_list_config) / sizeof(EXTMEM_DefinitionTypeDef)); index++)
  {
    switch(EXTMEM_GetMapAddress(index, &BaseAddress))
    {
      case EXTMEM_OK :{
      if (EXTMEM_MemoryMappedMode(index, EXTMEM_ENABLE) != EXTMEM_OK)
      {
        retr = BOOT_ERROR_MAPPEDMODEFAIL;
      }
      break;
      }
      case EXTMEM_ERROR_NOTSUPPORTED :{
        /* the memory doesn't support map mode, nothing to do */
        break;
      }
      default :{
        retr = BOOT_ERROR_NOBASEADDRESS;
      break;
      }
    }
  }
  return retr;
}

/**
  * @brief  This function copy the data from source to destination
  * @return @ref BOOTStatus_TypeDef
  */
BOOTStatus_TypeDef CopyApplication(void)
{
  BOOTStatus_TypeDef retr = BOOT_OK;
  uint8_t *source;
  uint8_t *destination;
  uint32_t MapAddress;
  uint32_t img_size;

#if defined(EXTMEM_LRUN_DESTINATION_INTERNAL)
  /* this case correspond to copy the SW from external memory into internal memory */
  destination = (uint8_t *)EXTMEM_LRUN_DESTINATION_ADDRESS;
#else
  if (EXTMEM_OK != EXTMEM_GetMapAddress(EXTMEM_LRUN_DESTINATION, &MapAddress))
  {
    return BOOT_ERROR_MAPPEDMODEFAIL;
  }
  destination = (uint8_t *)(MapAddress + EXTMEM_LRUN_DESTINATION_ADDRESS);
#endif

  /* get the map address of the source memory */
  switch(EXTMEM_GetMapAddress(EXTMEM_LRUN_SOURCE, &MapAddress)){
  case EXTMEM_OK :{
    /* manage the copy in mapped mode */
    source = (uint8_t*)(MapAddress + EXTMEM_LRUN_SOURCE_ADDRESS);
    img_size = BOOT_GetApplicationSize((uint32_t) source);
    /* copy form source to destination in mapped mode */
    for (uint32_t index=0; index < img_size; index++)
    {
      destination[index] = source[index];
    }
#if defined(EXTMEM_LRUN_TZ_ENABLE_NS)
    source = (uint8_t*)(MapAddress + EXTMEM_LRUN_SOURCE_ADDRESS_NS);
    img_size = BOOT_GetApplicationSize((uint32_t) source);
    destination = (uint8_t *)EXTMEM_LRUN_DESTINATION_ADDRESS_NS;
    /* copy Non-Secure form source to destination in mapped mode */
    for (uint32_t index=0; index < img_size; index++)
    {
      destination[index] = source[index];
    }
#endif
    break;
  }

  case EXTMEM_ERROR_NOTSUPPORTED:{
    img_size = BOOT_GetApplicationSize(EXTMEM_LRUN_SOURCE_ADDRESS);
    /* manage the copy using EXTMEM_Read */
    if (EXTMEM_OK != EXTMEM_Read(EXTMEM_LRUN_SOURCE, EXTMEM_LRUN_SOURCE_ADDRESS, destination, img_size))
    {
      retr = BOOT_ERROR_COPY;
    }
#if defined(EXTMEM_LRUN_TZ_ENABLE_NS)
    img_size = BOOT_GetApplicationSize(EXTMEM_LRUN_SOURCE_ADDRESS_NS);
    destination = (uint8_t *)EXTMEM_LRUN_DESTINATION_ADDRESS_NS;
    /* copy Non-Secure form source to destination in mapped mode */
     if (EXTMEM_OK != EXTMEM_Read(EXTMEM_LRUN_SOURCE, EXTMEM_LRUN_SOURCE_ADDRESS_NS, destination, img_size))
    {
      retr = BOOT_ERROR_COPY;
    }
#endif
    break;
  }

  default :{
    /* return an error */
    retr = BOOT_ERROR_MAPPEDMODEFAIL;
    break;
  }
}
  return retr;
}

/**
  * @brief  This function jumps to the application through its vector table
  * @return @ref BOOTStatus_TypeDef
  */
BOOTStatus_TypeDef JumpToApplication(void)
{
  uint32_t primask_bit;
  typedef  void (*pFunction)(void);
  static pFunction JumpToApp;
  uint32_t Application_vector;
  /* Suspend SysTick */
  HAL_SuspendTick();

#if defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1U)
  /* if I-Cache is enabled, disable I-Cache-----------------------------------*/
  if (SCB->CCR & SCB_CCR_IC_Msk)
  {
    SCB_DisableICache();
  }
#endif /* defined(ICACHE_PRESENT) && (ICACHE_PRESENT == 1U) */

#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
  /* if D-Cache is enabled, disable D-Cache-----------------------------------*/
  if (SCB->CCR & SCB_CCR_DC_Msk)
  {
    SCB_DisableDCache();
  }
#endif /* defined(DCACHE_PRESENT) && (DCACHE_PRESENT == 1U) */

  /* Initialize user application's Stack Pointer & Jump to user application  */
  primask_bit = __get_PRIMASK();
  __disable_irq();

  Application_vector = BOOT_GetApplicationVectorTable();

  SCB->VTOR = (uint32_t)Application_vector;
  JumpToApp = (pFunction) (*(__IO uint32_t *)(Application_vector + 4));

#if ((defined (__ARM_ARCH_8M_MAIN__ ) && (__ARM_ARCH_8M_MAIN__ == 1)) || \
     (defined (__ARM_ARCH_8_1M_MAIN__ ) && (__ARM_ARCH_8_1M_MAIN__ == 1)) || \
     (defined (__ARM_ARCH_8M_BASE__ ) && (__ARM_ARCH_8M_BASE__ == 1))    )
  /* on ARM v8m, set MSPLIM before setting MSP to avoid unwanted stack overflow faults */
  __set_MSPLIM(0x00000000);
#endif  /* __ARM_ARCH_8M_MAIN__ or __ARM_ARCH_8M_BASE__ */

  __set_MSP(*(__IO uint32_t*)Application_vector);

  /* Re-enable the interrupts */
  __set_PRIMASK(primask_bit);

  JumpToApp();
  return BOOT_OK;
}


 __weak uint32_t BOOT_GetApplicationSize(uint32_t img_addr)
{
  UNUSED(img_addr);
  return EXTMEM_LRUN_SOURCE_SIZE;
}

__weak uint32_t BOOT_GetApplicationVectorTable(void)
{
  uint32_t vector_table;
#if defined(EXTMEM_LRUN_DESTINATION_INTERNAL)
  vector_table = EXTMEM_LRUN_DESTINATION_ADDRESS;
#else
  if (EXTMEM_OK != EXTMEM_GetMapAddress(EXTMEM_LRUN_DESTINATION, &vector_table))
  {
    return 0xffffffff;
  }
  vector_table += EXTMEM_LRUN_DESTINATION_ADDRESS;
#endif
  vector_table += EXTMEM_HEADER_OFFSET;
  return vector_table;
}
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
