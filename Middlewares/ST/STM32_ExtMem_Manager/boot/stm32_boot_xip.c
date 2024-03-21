/**
  ******************************************************************************
  * @file    stm32_boot_xip.c
  * @author  MCD Application Team
  * @brief   this file manages the boot in the mode execute in place.
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
#include "stm32_boot_xip.h"
#include "stm32_extmem_conf.h"

/** @addtogroup BOOT
  * @{
  */

/** @addtogroup BOOT_XIP
  * @{
  */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

/* offset of the image from the boot memory base */
#ifndef EXTMEM_XIP_IMAGE_OFFSET
#define EXTMEM_XIP_IMAGE_OFFSET 0
#endif

/* offset of the vector table from the start of the image */
#ifndef EXTMEM_HEADER_OFFSET
#define EXTMEM_HEADER_OFFSET 0
#endif

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
BOOTStatus_TypeDef JumpToApplication(void);
BOOTStatus_TypeDef MapMemory(void);
BOOTStatus_TypeDef GetBaseAddress(uint32_t MemIndex, uint32_t *BaseAddress);

/**
  *  @addtogroup BOOT_XIP_Exported_Functions Boot XIP exported functions
  * @{
  */

BOOTStatus_TypeDef BOOT_Application(void)
{
  BOOTStatus_TypeDef retr;

  /* mount the memory */
  retr = MapMemory();
  if (BOOT_OK == retr)
  {
    /* jump on the application */
    retr = JumpToApplication();
  }
  return retr;
}

/**
  * @}
  */

/**
  *  @defgroup BOOT_XIP_Private_Functions Boot XIP private functions
  * @{
  */

/**
  * @brief  this function maps the memory
  * @return @ref BOOTStatus_TypeDef
  */
BOOTStatus_TypeDef MapMemory(void)
{
  BOOTStatus_TypeDef retr = BOOT_OK;

  /* Map all the memory */
  for (uint8_t index = 0; index < (sizeof(extmem_list_config) / sizeof(EXTMEM_DefinitionTypeDef)); index++)
  {
    switch(EXTMEM_MemoryMappedMode(index, EXTMEM_ENABLE))
    {
      case EXTMEM_ERROR_NOTSUPPORTED :
           if (EXTMEM_MEMORY_BOOTXIP ==  index)
           {
             retr = BOOT_ERROR_INCOMPATIBLEMEMORY;
           }
           else
           {
            /* We considers the memory will be not used any more */
            EXTMEM_DeInit(index);
           }
      case EXTMEM_OK:
      break;
      default :
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
  pFunction JumpToApp;
  uint32_t Application_vector;

  if (EXTMEM_OK != EXTMEM_GetMapAddress(EXTMEM_MEMORY_BOOTXIP, &Application_vector))
  {
      return BOOT_ERROR_INCOMPATIBLEMEMORY;
  }

  /* Suspend SysTick */
  HAL_SuspendTick();

  /* Disable I-Cache---------------------------------------------------------*/
  SCB_DisableICache();

  /* Disable D-Cache---------------------------------------------------------*/
  SCB_DisableDCache();

  /* Initialize user application's Stack Pointer & Jump to user application  */
  primask_bit = __get_PRIMASK();
  __disable_irq();

  /* Apply offsets for image location and vector table offset */
  Application_vector += EXTMEM_XIP_IMAGE_OFFSET + EXTMEM_HEADER_OFFSET;

  SCB->VTOR = (uint32_t)Application_vector;
  JumpToApp = (pFunction) (*(__IO uint32_t *)(Application_vector + 4u));
  __set_MSP(*(__IO uint32_t*) Application_vector);

  /* Re-enable the interrupts */
  __set_PRIMASK(primask_bit);

  JumpToApp();
  return BOOT_OK;
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
