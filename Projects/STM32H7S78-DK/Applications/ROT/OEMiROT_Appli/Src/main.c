/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   Main application file.
  *          This application demonstrates Firmware Update, protections
  *          and crypto testing functionalities.
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
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "com.h"
#include "main.h"
/* Avoids the semihosting issue */
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__asm("  .global __ARM_use_no_argv\n");
#endif /* defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) */

/** @defgroup  USER_APP  exported variable
   * @{
  */

/** @addtogroup USER_APP User App Example
  * @{
  */


/** @addtogroup USER_APP_COMMON Common
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define KR_RELOAD                       (uint16_t) 0xAAAA

/* Enable print of boot time (obtained through DWT).
   DWT usage requires product state is not closed/locked.
   OEMxRoT logs must be disabled for relevant boot time. */
/* #define PRINT_BOOT_TIME */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t *pUserAppId;
const uint8_t UserAppId = 'A';
uint64_t time;
uint32_t end;

/* Private function prototypes -----------------------------------------------*/
#if !defined(MCUBOOT_OVERWRITE_ONLY)
static void FW_Valid_AppImage(void);
#endif /* defined(MCUBOOT_OVERWRITE_ONLY) */

const uint32_t MagicTrailerValue[] =
{
  0xf395c277,
  0x7fefd260,
  0x0f505235,
  0x8079b62c,
};

/* Place data in a specific section*/
#if defined(__ICCARM__)
#pragma default_variable_attributes = @ ".flag"
#elif defined(__ARMCC_VERSION)
__attribute__((section(".bss.flag")))
#elif defined(__GNUC__)
__attribute__((section(".flag")))
#endif /* __ICCARM__ */
volatile uint32_t ConfirmationFlag[] =
{
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000
};
/* Stop placing data in specified section*/
#if defined(__ICCARM__)
#pragma default_variable_attributes =
#endif /* __ICCARM__ */

void FW_APP_PrintMainMenu(void);
void FW_APP_Run(void);
void LOADER_Run(void);
void DATA_Display(void);

#if defined(__ICCARM__)
#include <LowLevelIOInterface.h>
#endif /* __ICCARM__ */

#if defined(__ICCARM__)
/* New definition from EWARM V9, compatible with EWARM8 */
int iar_fputc(int ch);
#define PUTCHAR_PROTOTYPE int iar_fputc(int ch)
#elif defined ( __CC_ARM ) || defined(__ARMCC_VERSION)
/* ARM Compiler 5/6*/
int io_putchar(int ch);
#define PUTCHAR_PROTOTYPE int io_putchar(int ch)
#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int32_t uart_putc(int32_t ch)
#endif /* __ICCARM__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  */
PUTCHAR_PROTOTYPE
{
  COM_Transmit((uint8_t *)&ch, 1, TX_TIMEOUT);
  return ch;
}

/* Redirects printf to TFM_DRIVER_STDIO in case of ARMCLANG*/
#if defined(__ARMCC_VERSION)
FILE __stdout;

/* __ARMCC_VERSION is only defined starting from Arm compiler version 6 */
int fputc(int ch, FILE *f)
{
  /* Send byte to USART */
  io_putchar(ch);

  /* Return character written */
  return ch;
}
#elif defined(__GNUC__)
/* Redirects printf to TFM_DRIVER_STDIO in case of GNUARM */
int _write(int fd, char *str, int len)
{
  int i;

  for (i = 0; i < len; i++)
  {
    /* Send byte to USART */
    uart_putc(str[i]);
  }

  /* Return the number of characters written */
  return len;
}
#elif defined(__ICCARM__)
size_t __write(int file, unsigned char const *ptr, size_t len)
{
  size_t idx;
  unsigned char const *pdata = ptr;

  for (idx = 0; idx < len; idx++)
  {
    iar_fputc((int)*pdata);
    pdata++;
  }
  return len;
}
#endif /*  __GNUC__ */


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(int argc, char **argv)
/*int main(void) */
{
  /* !!! To boot in a secure way, OEMxROT has configured and activated the
     Memory Protection Unit (not all resources are allocated).
     In order to keep a secure environment execution, you should reconfigure
     the MPU to fulfill the security requirements of your application.

     In this application, the MPU configuration set by OEMxROT remains enabled.
     MPU may be reconfigured here, if needed. */

  /* Get boot cycles */
  end = DWT->CYCCNT;

  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();

  /*  set example to const : this const changes in binary without rebuild */
  pUserAppId = (uint8_t *)&UserAppId;

  /* Update SystemCoreClock variable according to RCC registers values.

     The system clock reconfiguration by the application is not supported
     when the application runs from an external memory. */
  SystemCoreClockUpdate();

  /* STM32H7RSxx HAL library initialization:
  - Systick timer is configured by default as source of time base, but user
  can eventually implement his proper time base source (a general purpose
  timer for example or other time source), keeping in mind that Time base
  duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
  handled in milliseconds basis.
  - Set NVIC Group Priority to 3
  - Low Level Initialization
  */
  HAL_Init();

  /* Get Boot Time */
  time = ((uint64_t)(end) * 1000U  / SystemCoreClock);

  /* Configure Communication module */
  COM_Init();

#ifdef PRINT_BOOT_TIME
  printf("\r\nBoot time : %u ms at %u MHz", (unsigned int)(time), (unsigned int)(SystemCoreClock/1000000U));
  printf("\r\n");
#endif

  printf("\r\n======================================================================");
  printf("\r\n=              (C) COPYRIGHT 2023 STMicroelectronics                 =");
  printf("\r\n=                                                                    =");
  printf("\r\n=                            User App #%c                             =", *pUserAppId);
#if defined(__ICCARM__)
  printf("\r\n=                        Running at 0x%x                       =", CODE_START);
#else /* __ICCARM__ */
  printf("\r\n=                        Running at 0x%lx                       =", (long unsigned int) CODE_START);
#endif /* __ICCARM__ */
  printf("\r\n======================================================================");
  printf("\r\n\r\n");

  /* User App firmware runs*/
  FW_APP_Run();

  while (1U)
  {}

}

/**
  * @brief  Display the TEST Main Menu choices on HyperTerminal
  * @param  None.
  * @retval None.
  */
void FW_APP_PrintMainMenu(void)
{
  printf("\r\n=================== Main Menu ============================\r\n\n");
  printf("  Start BootLoader -------------------------------------- 1\r\n\n");
#if !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_APP_IMAGE_NUMBER == 1)
  printf("  Validate App Image ------------------------------------ 2\r\n\n");
#endif /* (not MCUBOOT_OVERWRITE_ONLY) and (MCUBOOT_APP_IMAGE_NUMBER == 1) */
  printf("  Selection :\r\n\n");
}

/**
  * @brief  Display the TEST Main Menu choices on HyperTerminal
  * @param  None.
  * @retval None.
  */
void FW_APP_Run(void)
{
  uint8_t key = 0U;

  /*##1- Print Main Menu message*/
  FW_APP_PrintMainMenu();

  while (1U)
  {
    /* Reload Independent Watchdog */
    IWDG->KR = KR_RELOAD;

    /* Clean the input path */
    COM_Flush();

    /* Receive key */
    if (COM_Receive(&key, 1U, RX_TIMEOUT) == HAL_OK)
    {
      switch (key)
      {
        case '1' :
          LOADER_Run();
          break;
#if !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_APP_IMAGE_NUMBER == 1)
        case '2':
          FW_Valid_AppImage();
          break;
#endif /* defined(MCUBOOT_OVERWRITE_ONLY) && defined(MCUBOOT_APP_IMAGE_NUMBER == 1) */
        default:
          printf("Invalid Number !\r");
          break;
      }

      /* Print Main Menu message */
      FW_APP_PrintMainMenu();
    }
  }
}


/**
  * @brief  Perform Jump to the BootLoader
  * @retval None.
  */
void LOADER_Run(void)
{
  printf("\r\n  If the product state is CLOSED, please use DA 'Forced Download' to be able to use Bootloader");
  printf("\r\n  Start download with STM32CubeProgrammer through:\r\n");
  printf("\r\n    - USART if the product state is CLOSED (disconnect your UART console before)\r\n");
  printf("\r\n    - ST-LINK if the product state is OPEN\r\n");
  printf("\r\n");

}

#if  !defined(MCUBOOT_OVERWRITE_ONLY)
/**
  * @brief  Write Confirm Flag for  :
  * @brief  - Application image
  * @param  None
  * @retval None
  */
static void FW_Valid_AppImage(void)
{
  const uint32_t FlagPattern[] = { 0x001fbbb7, 0x00236445, 0x0023a889, 0x0023d111 };
  uint32_t i;
  uint32_t flag_size;
  uint32_t result = 0U;

  /* FlagPattern is 32-bit aligned to avoid unaligned accesses with SRAM ECC */
  flag_size = sizeof(FlagPattern) / sizeof(uint32_t);
  for (i = 0U; i < flag_size; i++)
  {
    ConfirmationFlag[i] = FlagPattern[i];
  }
  SCB_CleanDCache();
  for (i = 0U; i < flag_size; i++)
  {
    result |= (FlagPattern[i] ^ ConfirmationFlag[i]);
  }
  if (result == 0U)
  {
#if defined(__ICCARM__)
    printf("  --  Confirm Flag  correctly written %x %x \r\n\n",  ConfirmationFlag[0], FlagPattern[0]);
#else /* __ICCARM__ */
    printf("  --  Confirm Flag  correctly written %lx %lx \r\n\n",
           (long unsigned int) ConfirmationFlag[0],
           (long unsigned int) FlagPattern[0]);
#endif /* __ICCARM__ */


    /* Wait that all instructions & data are done */
    __ISB();
    __DSB();

    NVIC_SystemReset();
  }
  else
  {
    printf("  -- Confirm Flag Not Correctly Written \r\n\n");
  }
}
#endif /* !defined(MCUBOOT_OVERWRITE_ONLY) */

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
  while (1U)
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
