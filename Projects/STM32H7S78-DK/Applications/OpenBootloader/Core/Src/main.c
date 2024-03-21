/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   Main program body
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
#include "app_openbootloader.h"
#include "interfaces_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);

/* Private functions ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/

/**
  * @brief  Main program
  * @retval None
  */
int main(void)
{
  /* STM32H7RSxx HAL library initialization:
       - Reset of all peripherals
       - Initializes the Flash interface and the systick
       - Set NVIC Group Priority to 4
       - Low Level Initialization
  */
  HAL_Init();

  /* Configure the System clock to have a frequency of 200 MHz */
  SystemClock_Config();

  /* Initialize the OpenBootloader */
  OpenBootloader_Init();

  while (true)
  {
    OpenBootloader_ProtocolDetection();
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 200000000
  *            HCLK(Hz)                       = 200000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 2
  *            APB4 Prescaler                 = 2
  *            APB5 Prescaler                 = 2
  *            HSI Frequency(Hz)              = 640000
  *            PLL_M                          = 16
  *            PLL_N                          = 100
  *            PLL_P                          = 2
  *            PLL_Q                          = 20
  *            PLL_R                          = 2
  *            Flash Latency(WS)              = 5
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0U};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0U};

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0U};

  /* Enable HSI */
  RCC_OscInitStruct.OscillatorType          = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSIState                = RCC_HSI_ON;
  RCC_OscInitStruct.HSEState                = RCC_HSE_OFF;
  RCC_OscInitStruct.HSIDiv                  = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue     = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI48State              = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL1.PLLState           = RCC_PLL_ON;
  RCC_OscInitStruct.PLL2.PLLState           = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL3.PLLState           = RCC_PLL_ON;
  RCC_OscInitStruct.PLL1.PLLSource          = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL1.PLLM               = 16U;
  RCC_OscInitStruct.PLL1.PLLN               = 100U;
  RCC_OscInitStruct.PLL1.PLLP               = 2U;
  RCC_OscInitStruct.PLL1.PLLQ               = 20U;
  RCC_OscInitStruct.PLL1.PLLR               = 2U;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInitStruct.PeriphClockSelection    = RCC_PERIPHCLK_USBOTGFS;
  PeriphClkInitStruct.UsbOtgFsClockSelection  = RCC_USBOTGFSCLKSOURCE_HSI48;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInitStruct.PeriphClockSelection    = RCC_PERIPHCLK_FDCAN;
  PeriphClkInitStruct.FdcanClockSelection     = RCC_FDCANCLKSOURCE_PLL1Q;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Select HSI as system clock source and configure the HCLK, PCLK1, PCLK2,
     PCLK4 and PCLK5 clocks dividers */
  RCC_ClkInitStruct.ClockType       = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK \
                                       | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 \
                                       | RCC_CLOCKTYPE_PCLK4 | RCC_CLOCKTYPE_PCLK5);
  RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider   = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider   = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider  = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider  = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider  = RCC_APB4_DIV2;
  RCC_ClkInitStruct.APB5CLKDivider  = RCC_APB5_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function is used to de-initialized the clock.
  * @retval None.
  */
void System_DeInit(void)
{
  HAL_RCC_DeInit();

  /* Disable the SysTick timer */
  NVIC_DisableIRQ(SysTick_IRQn);
  NVIC_ClearPendingIRQ(SysTick_IRQn);

  /* Disable the USB voltage level detector */
  HAL_PWREx_DisableUSBVoltageDetector();

  /* Disable interfaces clocks */
  USARTx_CLK_DISABLE();
  I2Cx_CLK_DISABLE();
  I3Cx_CLK_DISABLE();
  SPIx_CLK_DISABLE();
  FDCANx_CLK_DISABLE();
  USB_CLK_DISABLE();

  /* Reset interfaces */
  USARTx_DEINIT();
  SPIx_DEINIT();
  I3Cx_DEINIT();
  I2Cx_DEINIT();
  FDCANx_DEINIT();
  USB_DEINIT();

  /* Disable GPIOx */
  LL_GPIO_DeInit(GPIOB);
  LL_GPIO_DeInit(GPIOE);
  LL_GPIO_DeInit(GPIOD);

  /* Disable USB interrupt */
  HAL_NVIC_DisableIRQ(OTG_FS_IRQn);

  /* Disable SPI interrupt */
  HAL_NVIC_DisableIRQ(SPI4_IRQn);

  /* Disable I3C interrupt */
  HAL_NVIC_DisableIRQ(I3C1_EV_IRQn);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None.
  */
void Error_Handler(void)
{
  /* Handle the error by performing a System Reset.
     The user can add his own error handler.
   */
  NVIC_SystemReset();
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number.
  *         where the assert_param error has occurred.
  * @param  file pointer to the source file name.
  * @param  line assert_param error line source number.
  * @retval None.
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  UNUSED(file);
  UNUSED(line);

  /* Infinite loop */
  while (true)
  {
  }
}
#endif /* USE_FULL_ASSERT */
