/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   Main application file.
  *          This application demonstrates Secure STiROT uses case
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
#include "com.h"
#include "main.h"
#include "test_versions.h"

/* Avoids the semihosting issue */
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__asm("  .global __ARM_use_no_argv\n");
#endif /* defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) */

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
/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define KR_RELOAD                       (uint16_t) 0xAAAA
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t *pUserAppId;
const uint8_t UserAppId = 'A';

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
void FW_APP_PrintMainMenu(void);
void FW_APP_Run(void);
void RNG_Init(void);
void Function_A_Run(void);
void Function_B_Run(void);

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


/* Private user code ---------------------------------------------------------*/


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  pUserAppId = (uint8_t *)&UserAppId;

  /* MCU Configuration--------------------------------------------------------*/

  /* !!! To boot in a secure way, STiROT has configured and activated the
     Memory Protection Unit (not all resources are allocated).
     In order to keep a secure environment execution, you should reconfigure
     the MPU to make it compatible with your application.

     In this application, the MPU configuration set by STiROT remains enabled.
     MPU may be reconfigured here, if needed. */

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  SystemClock_Config();

  /* Start HW randomization */
  RNG_Init();

  /* Configure Communication module */
  COM_Init();

  printf("\r\n================================================================");
  printf("\r\n=            (C) COPYRIGHT 2023 STMicroelectronics             =");
  printf("\r\n=                                                              =");
  printf("\r\n=                          User App #%c                         =", *pUserAppId);
  printf("\r\n================================================================");
  printf("\r\n");

  /* Print versions of appli code image and appli data image */
  TEST_VERSIONS_PrintVersions();

  /* User App firmware runs*/
  FW_APP_Run();

  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief  Display the TEST Main Menu choices on HyperTerminal
  * @param  None.
  * @retval None.
  */
void FW_APP_PrintMainMenu(void)
{
  printf("\r\n=========================== Main Menu ==========================\r\n\n");
  printf("  Start < Function A > --------------------------------------- 1\r\n\n");
  printf("  Start < Function B > --------------------------------------- 2\r\n\n");
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
          Function_A_Run();
          break;
        case '2' :
          Function_B_Run();
          break;
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
  * @brief  This function initialize the RNG ip .
  * @retval None
  */
void RNG_Init(void)
{
  RNG_HandleTypeDef handle;
  /* Select RNG clock source */
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;

  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* RNG Peripheral clock enable */
  __HAL_RCC_RNG_CLK_ENABLE();

  /* Initialize RNG instance */
  handle.Instance = RNG;
  handle.State = HAL_RNG_STATE_RESET;
  handle.Lock = HAL_UNLOCKED;

  if (HAL_RNG_Init(&handle) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* Infinite loop */
  while (1)
  {
  }
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
  * @brief  This function is executed in case 1.
  * @retval None
  */
void Function_A_Run(void)
{
  printf("  Function A  \r\n");
  printf("  To be implemented by the OEM \r\n\n");
}

/**
  * @brief  This function is executed in case 2.
  * @retval None
  */
void Function_B_Run(void)
{
  printf("  Function B  \r\n");
  printf("  To be implemented by the OEM \r\n\n");
}
