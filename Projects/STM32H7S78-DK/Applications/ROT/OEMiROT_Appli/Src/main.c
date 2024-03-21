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

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t *pUserAppId;
const uint8_t UserAppId = 'A';

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

static void SystemClock_Config(void);
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
     the MPU to make it compatible with your application.

     In this application, the MPU configuration set by OEMxROT remains enabled.
     MPU may be reconfigured here, if needed. */

  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();

  /*  set example to const : this const changes in binary without rebuild */
  pUserAppId = (uint8_t *)&UserAppId;

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

  /* Configure the system clock */
  SystemClock_Config();

  /* System clock already configured, simply SystemCoreClock init */
  SystemCoreClockUpdate();

  /* Configure Communication module */
  COM_Init();

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
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL1 (HSI)
  *            SYSCLK(Hz)                     = 380000000 (CPU Clock and AXI Clock)
  *            HCLK(Hz)                       = 190000000 (AHBs Clock)
  *            SYSCLK Prescaler               = 1  (System CPU clock=pll1p_ck)
  *            AHB Prescaler                  = 2  (AXI/AHB System bus clock=System CPU clock/2)
  *            APB1 Prescaler                 = 2  (APB1 bus clock=System bus clock/2)
  *            APB2 Prescaler                 = 2  (APB2 bus clock=System bus clock/2)
  *            APB4 Prescaler                 = 2  (APB4 bus clock=System bus clock/2)
  *            APB5 Prescaler                 = 2  (APB5 bus clock=System bus clock/2)
  *            HSI Frequency(Hz)              = 64000000
  *            HSI Divider                    = 1
  *            PLL1_M                         = 32  (for ref1_ck between 1 to 16MHz)
  *            PLL1_N                         = 380 (64/32 * 380 = 760MHz)
  *            PLL1_P                         = 2  (pll1p_ck = 760/2 = 380MHz)
  *            PLL1_Q                         = 2  (pll1q_ck = 760/2 = 380MHz)
  *            PLL1_R                         = 2  (pll1r_ck = 760/2 = 380MHz)
  *            PLL1_S                         = 2  (pll1s_ck = 760/2 = 380MHz)
  *            PLL1_T                         = 2  (pll1t_ck = 760/2 = 380MHz)
  *            PLL2                           not used
  *            PLL3                           not used
  *            Flash Latency(WS)              = 5
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* Enable voltage range 1 for lowest power (compliant with the use of RAM ECC) */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    /* Initialization error */
    while (1);
  }

  /* Activate PLL1 with HSI as source (HSI is ON at reset) */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL1.PLLM = 32;
  RCC_OscInitStruct.PLL1.PLLN = 380; /* PLL1 VCO = 64/32 * 380 = 760MHz */
  RCC_OscInitStruct.PLL1.PLLP = 2;  /* PLL1 P =380MHz */
  RCC_OscInitStruct.PLL1.PLLQ = 2;  /* PLL1 Q =380MHz */
  RCC_OscInitStruct.PLL1.PLLR = 2;  /* PLL1 R =380MHz */
  RCC_OscInitStruct.PLL1.PLLS = 2;  /* PLL1 S =380MHz */
  RCC_OscInitStruct.PLL1.PLLT = 2;  /* PLL1 T =380MHz */
  RCC_OscInitStruct.PLL1.PLLFractional = 0;
  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization error */
    while (1);
  }

  /* Select PLL1 as system clock source and configure the SYSCLK, HCLK,
     PCLK1, PCLK2, PCLK4 and PCLK5 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK  | \
                                 RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2 | \
                                 RCC_CLOCKTYPE_PCLK4  | RCC_CLOCKTYPE_PCLK5);
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider  = RCC_SYSCLK_DIV1;   /* System CPU clock=pll1p_ck (760MHz) */
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_HCLK_DIV2;     /* AXI/AHB System bus clock=System CPU clock/2 (380MHz) */
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;     /* APB1 bus clock=System bus clock/2 (380MHz) */
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;     /* APB2 bus clock=System bus clock/2 (380MHz) */
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;     /* APB4 bus clock=System bus clock/2 (380MHz) */
  RCC_ClkInitStruct.APB5CLKDivider = RCC_APB5_DIV2;     /* APB5 bus clock=System bus clock/2 (380MHz) */
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization error */
    while (1);
  }
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
