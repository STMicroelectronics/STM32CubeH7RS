/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   Main program body through the HAL API
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
#include "com.h"
#include "loader.h"
#include "low_level_flash_ext.h"
#include "low_level_shared_mem.h"
#include "boot_hal_iloader.h"
#if defined(__ICCARM__)
#include <LowLevelIOInterface.h>
#endif /* __ICCARM__ */

/** @addtogroup STM32H7RSxx_HAL_Examples
  * @{
  */

/** @addtogroup STiROT_iLoader
  * @{
  */

/* Private typedefs ----------------------------------------------------------*/
#define KR_RELOAD                       (uint16_t) 0xAAAA
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/**
  * @brief SRAM address of STiROT-iLoader memory configuration
  */
extern const LL_SHARED_MEM_Config_t *pSharedRam;

/* Private function prototypes -----------------------------------------------*/
#if defined(__ICCARM__)
/* New definition from EWARM V9, compatible with EWARM8 */
int iar_fputc(int ch);
#define PUTCHAR_PROTOTYPE int iar_fputc(int ch)
#elif defined(__ARMCC_VERSION)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif /* __ICCARM__ */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  STIROT_ILOADER_ErrorStatus status = STIROT_ILOADER_ERROR;

  /* Reload Independent Watchdog for STiROT iLoader */
  IWDG->KR = KR_RELOAD;

#ifdef ILOADER_USE_CACHE
  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();
#endif /* ILOADER_USE_CACHE */

  /* MCU Configuration--------------------------------------------------------*/

  /* STM32H7RSxx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

#if defined (STIROT_ILOADER_DEBUG)
  /* Initialize the Communication module */
  COM_Init();
#endif /* STIROT_ILOADER_DEBUG */

  /* Clock configuration is already set by the previous stage */

  /* Initialize the external flash module */
  if (FLASH_EXT_Init(pSharedRam->CodePrimaryNvmStart) != STIROT_ILOADER_SUCCESS)
  {
    /* Initialization error */
    Error_Handler();
  }

  /* Check the configuration in the shared VM */
  if ((IS_VM_SHARED_MEM_VALID())
      && (IS_SLOT_SIZE_VALID(pSharedRam->CodeSize))
      && (IS_SLOT_IN_VM(pSharedRam->CodeVmStart, pSharedRam->CodeSize))
      && (IS_SLOT_IN_NVM(pSharedRam->CodePrimaryNvmStart, pSharedRam->CodeSize))
      && (IS_SLOT_IN_NVM(pSharedRam->CodeSecondaryNvmStart, pSharedRam->CodeSize))
      && (ARE_SLOTS_SEPARATE(pSharedRam->CodePrimaryNvmStart, pSharedRam->CodeSize,
                             pSharedRam->CodeSecondaryNvmStart, pSharedRam->CodeSize))
      && (IS_ILOADER_OPERATION(pSharedRam->Operation))
      && (IS_ILOADER_SUBOPERATION(pSharedRam->Operation, pSharedRam->SubOperations)))
  {
    if (IS_ILOADER_RSS_CMD(HAL_SBS_GetRSSCommand()))
    {
      status = STIROT_ILOADER_SUCCESS;
    }
    else
    {
      LOADER_PRINT("RSSCMD is invalid\r\n\n");
      Error_Handler();
    }
  }
  else
  {
    LOADER_PRINT("Shared memory area is invalid\r\n\n");
    Error_Handler();
  }

  /* STiROT-iLoader firmware runs */
  if (status == STIROT_ILOADER_SUCCESS)
  {
    LOADER_Run();
  }

#if defined (STIROT_ILOADER_DEBUG)
  /* Deinitialize the Communication module */
  COM_DeInit();
#endif /* STIROT_ILOADER_DEBUG */

  /* Deinitialize the external flash module */
  if (FLASH_EXT_DeInit() != STIROT_ILOADER_SUCCESS)
  {
    /* Deinitialization error */
    Error_Handler();
  }

  /* Reset the system */
#ifdef ILOADER_USE_CACHE
  SCB_CleanDCache();
#endif /* ILOADER_USE_CACHE */
  HAL_NVIC_SystemReset();

  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief  Retargets the C library __write function to the IAR function iar_fputc.
  * @param  file: file descriptor.
  * @param  ptr: pointer to the buffer where the data is stored.
  * @param  len: length of the data to write in bytes.
  * @retval length of the written data in bytes.
  */
#if defined(__ICCARM__)
size_t __write(int file, unsigned char const *ptr, size_t len)
{
  size_t idx;
  unsigned char const *pdata = ptr;

  for (idx = 0U; idx < len; idx++)
  {
    iar_fputc((int)*pdata);
    pdata++;
  }
  return len;
}
#endif /* __ICCARM__ */

/**
  * @brief  Retargets the C library printf function to SFU UART.
  * @param  None
  * @retval ch
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  (void) COM_Transmit((uint8_t *)&ch, 1U, TX_TIMEOUT);

  return ch;
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */
