/**
  ******************************************************************************
  * @file    stm32h7rsxx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
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
#include "boot_hal_cfg.h"
#include <string.h>



/** @addtogroup STM32H7RSxx_HAL_Examples
  * @{
  */

/** @defgroup RTC_ActiveTamper
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define RCC_LSI_TIMEOUT_VALUE           ((uint32_t)2U)    /* 2 ms (minimum Tick + 1) */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t RccXspimClkEnabled = 0;
/* Private constant ----------------------------------------------------------*/
/** @defgroup HAL_MSP_Private_Constant
  * @{
  */
#define LSI_TIMEOUT_VALUE         1U        /* 1 ms */
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
HAL_StatusTypeDef RCC_OscConfig(RCC_OscInitTypeDef  *pOscInitStruct);
HAL_StatusTypeDef RCCEx_PeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *pPeriphClkInit);
uint32_t RCCEx_GetXspiCLKFreq(uint32_t PeriphClk);
void XSPIM_InitPort1(void);
void XSPIM_InitPort2(void);
void XSPIM_DeInitPort1(void);
void XSPIM_DeInitPort2(void);
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief  Initialize the RCC Oscillators according to the specified parameters in the
  *         RCC_OscInitTypeDef.
  * @param  RCC_OscInitStruct  pointer to an RCC_OscInitTypeDef structure that
  *         contains the configuration information for the RCC Oscillators.
  * @retval HAL status
  */
HAL_StatusTypeDef RCC_OscConfig(RCC_OscInitTypeDef  *pOscInitStruct)
{
  uint32_t tickstart = 0;

  /* Check Null pointer */
  if (pOscInitStruct == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_RCC_OSCILLATORTYPE(pOscInitStruct->OscillatorType));

  /*----------------------------- CSI Configuration --------------------------*/
  if (((pOscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_CSI) == RCC_OSCILLATORTYPE_CSI)
  {
    /* Check the parameters */
    assert_param(IS_RCC_CSI(pOscInitStruct->CSIState));

    /* When the CSI is used as system clock it will not disabled */
    if ((__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_CSI) ||
        (((RCC->CR & (RCC_CR_PLL1RDY | RCC_CR_PLL2RDY | RCC_CR_PLL3RDY)) != 0U) &&
         (__HAL_RCC_GET_PLL_OSCSOURCE() == RCC_PLLSOURCE_CSI)))
    {
      /* When CSI is used as system clock it will not disabled */
      if (pOscInitStruct->CSIState == RCC_CSI_OFF)
      {
        return HAL_ERROR;
      }
    }
    else
    {
      /* Check the CSI State */
      if ((pOscInitStruct->CSIState) != RCC_CSI_OFF)
      {
        /* Enable the Internal High Speed oscillator (CSI). */
        __HAL_RCC_CSI_ENABLE();

        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till CSI is ready */
        while (READ_BIT(RCC->CR, RCC_CR_CSIRDY) == 0U)
        {
          if ((HAL_GetTick() - tickstart) > RCC_CSI_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
      else
      {
        /* Disable the Internal High Speed oscillator (CSI). */
        __HAL_RCC_CSI_DISABLE();

        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till CSI is disabled */
        while (READ_BIT(RCC->CR, RCC_CR_CSIRDY) != 0U)
        {
          if ((HAL_GetTick() - tickstart) > RCC_CSI_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
    }
  }

#if defined (RTC_CLOCK_SOURCE_LSI)
  /*------------------------------ LSI Configuration -------------------------*/
  if (((pOscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_LSI) == RCC_OSCILLATORTYPE_LSI)
  {

    /* Check the parameters */
    assert_param(IS_RCC_LSI(pOscInitStruct->LSIState));

    /* Update LSI configuration in Backup Domain control register    */

    /* Check the LSI State */
    if (pOscInitStruct->LSIState != RCC_LSI_OFF)
    {
      /* Enable the Internal Low Speed oscillator (LSI). */
      __HAL_RCC_LSI_ENABLE();

      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait till LSI is ready */
      while (READ_BIT(RCC->CSR, RCC_CSR_LSIRDY) == 0U)
      {
        if ((HAL_GetTick() - tickstart) > LSI_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
    else
    {
      /* Disable the Internal Low Speed oscillator (LSI). */
      __HAL_RCC_LSI_DISABLE();

      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait till LSI is disabled */
      while (READ_BIT(RCC->CSR, RCC_CSR_LSIRDY) != 0U)
      {
        if ((HAL_GetTick() - tickstart) > LSI_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
  }
#endif /* RTC_CLOCK_SOURCE_LSI */

#if defined(RTC_CLOCK_SOURCE_LSE)
  /*------------------------------ LSE Configuration -------------------------*/
  if (((pOscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_LSE) == RCC_OSCILLATORTYPE_LSE)
  {
    /* Check the parameters */
    assert_param(IS_RCC_LSE(pOscInitStruct->LSEState));

    /* Enable write access to Backup domain */
    PWR->CR1 |= PWR_CR1_DBP;

    /* Set the new LSE configuration -----------------------------------------*/
    __HAL_RCC_LSE_CONFIG(pOscInitStruct->LSEState);

    /* Check the LSE State */
    if (pOscInitStruct->LSEState != RCC_LSE_OFF)
    {
      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait till LSE is ready */
      while (READ_BIT(RCC->BDCR, RCC_BDCR_LSERDY) == 0U)
      {
        if ((HAL_GetTick() - tickstart) > RCC_LSE_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
    else
    {
      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait till LSE is disabled */
      while (READ_BIT(RCC->BDCR, RCC_BDCR_LSERDY) != 0U)
      {
        if ((HAL_GetTick() - tickstart) > RCC_LSE_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
  }
#endif /* RTC_CLOCK_SOURCE_LSE */

  return HAL_OK;
}

/**
  * @brief  Initialize the RCC extended peripherals clocks according to the specified
  *         parameters in the RCC_PeriphCLKInitTypeDef.
  * @param  pPeriphClkInit  pointer to an RCC_PeriphCLKInitTypeDef structure that
  *         contains a field PeriphClockSelection which can be one of the following values:
  *            @arg @ref RCC_PERIPHCLK_RTC RTC peripheral clock
  *
  * @note   Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select
  *         the RTC clock source: in this case the access to Backup domain is enabled.
  *
  * @retval HAL status
  */
HAL_StatusTypeDef RCCEx_PeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *pPeriphClkInit)
{
  uint32_t tmpregister;
  uint32_t tickstart;
  HAL_StatusTypeDef ret = HAL_OK;      /* Intermediate status */
  HAL_StatusTypeDef status = HAL_OK;   /* Final status */

  /* Check the parameters */
  assert_param(IS_RCC_PERIPHCLOCK(pPeriphClkInit->PeriphClockSelection));

  /*-------------------------- RTC clock source configuration ----------------------*/
  if (((pPeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_RTC) == RCC_PERIPHCLK_RTC)
  {

    /* Check for RTC Parameters used to output RTCCLK */
    assert_param(IS_RCC_RTCCLKSOURCE(pPeriphClkInit->RTCClockSelection));

    /* As the RTC clock source selection can be changed only if the Backup Domain is reset */
    /* reset the Backup domain only if the RTC Clock source selection is modified from default reset value */
    tmpregister = READ_BIT(RCC->BDCR, RCC_BDCR_RTCSEL);

    if ((tmpregister != RCC_RTCCLKSOURCE_DISABLE) && (tmpregister != (pPeriphClkInit->RTCClockSelection & RCC_BDCR_RTCSEL)))
    {
      /* Enable write access to Backup domain */
      SET_BIT(PWR->CR1, PWR_CR1_DBP);

      /* Read back to check Backup domain enabled */
      if (READ_BIT(PWR->CR1, PWR_CR1_DBP) == 0U)
      {
        ret = HAL_ERROR;
      }
      else
      {
        /* Store the content of BDCR register before the reset of Backup Domain */
        /* excepted the RTC clock source selection that will be changed */
        tmpregister = READ_BIT(RCC->BDCR, ~(RCC_BDCR_RTCSEL));
        __HAL_RCC_BACKUPRESET_FORCE();
        __HAL_RCC_BACKUPRESET_RELEASE();
        /* Restore the content of BDCR register */
        WRITE_REG(RCC->BDCR, tmpregister);
      }
    }

    if (ret == HAL_OK)
    {
      /* If LSE is selected as RTC clock source (and enabled prior to Backup Domain reset), wait for LSE reactivation */
      if (pPeriphClkInit->RTCClockSelection == RCC_RTCCLKSOURCE_LSE)
      {
        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till LSE is ready */
        while (READ_BIT(RCC->BDCR, RCC_BDCR_LSERDY) == 0U)
        {
          if ((HAL_GetTick() - tickstart) > RCC_LSE_TIMEOUT_VALUE)
          {
            ret = HAL_TIMEOUT;
            break;
          }
        }
      }

      if (ret == HAL_OK)
      {
        /* Apply new RTC clock source selection */
        __HAL_RCC_RTC_CONFIG(pPeriphClkInit->RTCClockSelection);
      }
      else
      {
        /* set overall return value */
        status = ret;
      }
    }
    else
    {
      /* set overall return value */
      status = ret;
    }
  }
  return status;
}

/**
  * @brief  Return the peripheral clock frequency for a given peripheral
  * @note   Return 0 if peripheral clock identifier not managed by this API or
  *         if the selected clock source is not enabled (HSI, PLLs clock output..)
  * @param  PeriphClk: Peripheral clock identifier
  *         This parameter can be one of the following values:
  *            @arg RCC_PERIPHCLK_XSPI1       : Xspi1 peripheral clock
  *            @arg RCC_PERIPHCLK_XSPI2       : Xspi2 peripheral clock
  * @retval Frequency in KHz
  */
uint32_t RCCEx_GetXspiCLKFreq(uint32_t PeriphClk)
{
  uint32_t frequency = 0; /* Set to 0 for returned value if no source clock */

  /* Check that PLL2S is used */
  if (PeriphClk == RCC_PERIPHCLK_XSPI1)
  {
    if (__HAL_RCC_GET_XSPI1_SOURCE() != RCC_XSPI1CLKSOURCE_PLL2S)
    {
      Error_Handler();
    }
  }
  else if (PeriphClk == RCC_PERIPHCLK_XSPI2)
  {
    if (__HAL_RCC_GET_XSPI2_SOURCE() != RCC_XSPI2CLKSOURCE_PLL2S)
    {
      Error_Handler();
    }
  }
  else
  {
    /* The function should not have been called */
    Error_Handler();
  }

  /* Get the frequency */
  if (__HAL_RCC_GET_PLL2CLKOUT_CONFIG(RCC_PLL_SCLK) != 0U)
  {
    frequency = HAL_RCC_GetPLL2SFreq();
  }

  return frequency;
}

/**
  * @brief RTC MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  * @param hrtc: RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  RCC_OscInitTypeDef rcc_osc_init_struct_rtc;
  RCC_PeriphCLKInitTypeDef periph_clk_init_struct_rtc;

  HAL_PWR_EnableBkUpAccess();
  /* Initialize rcc_osc_init_struct_rtc */
  memset(&rcc_osc_init_struct_rtc, 0, sizeof(RCC_OscInitTypeDef));
  rcc_osc_init_struct_rtc.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE,
#if defined(RTC_CLOCK_SOURCE_LSE)
  rcc_osc_init_struct_rtc.LSEState = RCC_LSE_ON;
  rcc_osc_init_struct_rtc.LSIState = RCC_LSI_OFF;
#elif defined (RTC_CLOCK_SOURCE_LSI)
  rcc_osc_init_struct_rtc.LSEState = RCC_LSE_OFF;
  rcc_osc_init_struct_rtc.LSIState = RCC_LSI_ON;
#else
#error
#endif /* defined (RTC_CLOCK_SOURCE_LSI) */
  /* Initialize periph_clk_init_struct_rtc */
  memset(&periph_clk_init_struct_rtc, 0, sizeof(RCC_PeriphCLKInitTypeDef));
  periph_clk_init_struct_rtc.PeriphClockSelection = RCC_PERIPHCLK_RTC;
#if defined(RTC_CLOCK_SOURCE_LSE)
  periph_clk_init_struct_rtc.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
#elif defined (RTC_CLOCK_SOURCE_LSI)
  periph_clk_init_struct_rtc.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
#else
#error
#endif /* RTC_CLOCK_SOURCE_LSE */
  /* Reset the whole backup domain, RTC included */
  if (RCC_OscConfig((RCC_OscInitTypeDef *)&rcc_osc_init_struct_rtc) != HAL_OK)
  {
    Error_Handler();
  }
  if (RCCEx_PeriphCLKConfig((RCC_PeriphCLKInitTypeDef *)&periph_clk_init_struct_rtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* Enable RTC */
  __HAL_RCC_RTC_ENABLE();

  /* Enable RTC clock  */
  __HAL_RCC_RTCAPB_CLK_ENABLE();

  HAL_NVIC_SetPriority(TAMP_IRQn, 0x4, 0);
  HAL_NVIC_EnableIRQ(TAMP_IRQn);
}

/**
  * @brief RTC MSP De-Initialization
  *        This function freeze the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param hrtc: RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
  /*##-1- Reset peripherals ##################################################*/
  __HAL_RCC_RTC_DISABLE();
}

/**
  * @brief XSPI MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hxspi: XSPI handle pointer
  * @retval None
  */
void HAL_XSPI_MspInit(XSPI_HandleTypeDef *hxspi)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  /* Enable the XSPIM_P1 interface */
  HAL_PWREx_EnableXSPIM1();

  /* Enable the XSPIM_P2 interface */
  HAL_PWREx_EnableXSPIM2();

  if (hxspi->Instance == XSPI2)
  {
    /* Remove clock protection on XSPI memory interface */
    HAL_RCCEx_DisableClockProtection(RCC_CLOCKPROTECT_XSPI);

    /* Initializes the peripherals clock */
    __HAL_RCC_PLL2CLKOUT_ENABLE(RCC_PLL_SCLK);
    __HAL_RCC_XSPI2_CONFIG(RCC_XSPI2CLKSOURCE_PLL2S);

    RccXspimClkEnabled++;
    if (!(__HAL_RCC_XSPIM_IS_CLK_ENABLED()))
    {
      __HAL_RCC_XSPIM_CLK_ENABLE();
    }

    /* Enable the XSPI memory interface clock */
    __HAL_RCC_XSPI2_CLK_ENABLE();

    /* Set clock protection on XSPI memory interface */
    HAL_RCCEx_EnableClockProtection(RCC_CLOCKPROTECT_XSPI);
#if defined(STM32H7S3xx) || defined(STM32H7S7xx)
    /* Initialize the XSPIM port 1 (swapped mode) */
    XSPIM_InitPort1();
#elif defined(STM32H7R3xx) || defined(STM32H7R7xx)
    /* Initialize the XSPIM port 2 (direct mode) */
    XSPIM_InitPort2();
#endif /*  STM32H7S3xx || STM32H7S7xx */
  }
  else if (hxspi->Instance == XSPI1)
  {
    /* Remove clock protection on XSPI memory interface */
    HAL_RCCEx_DisableClockProtection(RCC_CLOCKPROTECT_XSPI);

    /* Initializes the peripherals clock */
    __HAL_RCC_PLL2CLKOUT_ENABLE(RCC_PLL_SCLK);
    __HAL_RCC_XSPI1_CONFIG(RCC_XSPI1CLKSOURCE_PLL2S);

    RccXspimClkEnabled++;
    if (!(__HAL_RCC_XSPIM_IS_CLK_ENABLED()))
    {
      __HAL_RCC_XSPIM_CLK_ENABLE();
    }

    /* Enable the XSPI memory interface clock */
    __HAL_RCC_XSPI1_CLK_ENABLE();

    /* Set clock protection on XSPI memory interface */
    HAL_RCCEx_EnableClockProtection(RCC_CLOCKPROTECT_XSPI);
#if defined(STM32H7S3xx) || defined(STM32H7S7xx)
    /* Initialize the XSPIM port 2 (swapped mode) */
    XSPIM_InitPort2();
#elif defined(STM32H7R3xx) || defined(STM32H7R7xx)
    /* Initialize the XSPIM port 1 (direct mode) */
    XSPIM_InitPort1();
#endif /* STM32H7S3xx || STM32H7S7xx */
  }

  /* Remove clock protection on XSPI memory interface */
  HAL_RCCEx_DisableClockProtection(RCC_CLOCKPROTECT_XSPI);

  /* The CSI is used by the compensation cells and must be enabled before enabling the
     compensation cells.
     For more details refer to RM0477 [SBS I/O compensation cell management] chapter.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_CSI;
  RCC_OscInitStruct.CSIState = RCC_CSI_ON;
  if (RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /* Configure the compensation cell */
  HAL_SBS_ConfigCompensationCell(SBS_IO_XSPI1_CELL, SBS_IO_CELL_CODE, 0U, 0U);
  HAL_SBS_ConfigCompensationCell(SBS_IO_XSPI2_CELL, SBS_IO_CELL_CODE, 0U, 0U);
  /* Enable compensation cell */
  HAL_SBS_EnableCompensationCell(SBS_IO_XSPI1_CELL);
  HAL_SBS_EnableCompensationCell(SBS_IO_XSPI2_CELL);
  /* wait ready before enabled IO */
  while(HAL_SBS_GetCompensationCellReadyStatus(SBS_IO_XSPI1_CELL_READY) != 1U);
  while(HAL_SBS_GetCompensationCellReadyStatus(SBS_IO_XSPI2_CELL_READY) != 1U);

  /* Configure "high speed low voltage" */
  /* Both XSPI must be configured when XSPIM is used */
  HAL_SBS_EnableIOSpeedOptimize(SBS_IO_XSPI1_HSLV);
  HAL_SBS_EnableIOSpeedOptimize(SBS_IO_XSPI2_HSLV);
}

/**
  * @brief XSPI MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @retval None
  */
void HAL_XSPI_MspDeInit(XSPI_HandleTypeDef *hxspi)
{
  /* Remove clock protection on XSPI memory interface */
  HAL_RCCEx_DisableClockProtection(RCC_CLOCKPROTECT_XSPI);

  if (hxspi->Instance == XSPI2)
  {
    /* Do not deconfigure the port: useless and memory consuming */
    /* De-Initialize the XSPIM port 1 (swapped mode) */
    /* XSPIM_DeInitPort1(); */

    /* Do not reset XSPI2 as this has side effects on XSPI2 configuration */
    /* Reset the XSPI memory interface */
    /* __HAL_RCC_XSPI2_FORCE_RESET(); */
    /* __HAL_RCC_XSPI2_RELEASE_RESET(); */

    /* Peripheral clock disable */
    __HAL_RCC_XSPI2_CLK_DISABLE();

    RccXspimClkEnabled--;
  }
  else if (hxspi->Instance == XSPI1)
  {
    /* Do not deconfigure the port: useless and memory consuming */
    /* De-Initialize the XSPIM port 2 (swapped mode) */
    /* XSPIM_DeInitPort2(); */

    /* Do not reset XSPI1 as this has side effects on XSPI2 configuration */
    /* Reset the XSPI memory interface */
    /* __HAL_RCC_XSPI1_FORCE_RESET(); */
    /* __HAL_RCC_XSPI1_RELEASE_RESET(); */

    /* Peripheral clock disable */
    __HAL_RCC_XSPI1_CLK_DISABLE();

    RccXspimClkEnabled--;
  }
  else
  {
    /* Do nothing */
  }

  if (RccXspimClkEnabled == 0U)
  {
    __HAL_RCC_XSPIM_CLK_DISABLE();
  }
  else
  {
    /* Set clock protection on XSPI memory interface */
    HAL_RCCEx_EnableClockProtection(RCC_CLOCKPROTECT_XSPI);
  }
}

/**
  * @brief Initialize the XSPIM Port1 (XSPI-PHY1)
  * @retval None
  */
void XSPIM_InitPort1(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Enable GPIO clocks */
  __HAL_RCC_GPIOO_CLK_ENABLE();
  __HAL_RCC_GPIOP_CLK_ENABLE();

  /* Configure XSPI GPIO pins */
  GPIO_InitStruct.Pin       = GPIO_PIN_ALL;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_XSPIM_P1;
  HAL_GPIO_Init(GPIOP, &GPIO_InitStruct);

  GPIO_InitStruct.Pin       =  GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4;
  HAL_GPIO_Init(GPIOO, &GPIO_InitStruct);

  /* XSPI NCS GPIO pin configuration  */
  GPIO_InitStruct.Pin       = GPIO_PIN_0;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOO, &GPIO_InitStruct);
}

/**
  * @brief Initialize the XSPIM Port2 (XSPI-PHY2)
  * @retval None
  */
void XSPIM_InitPort2(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Enable GPIO clocks */
  __HAL_RCC_GPION_CLK_ENABLE();

  /* Configure XSPI GPIO pins */
  GPIO_InitStruct.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | \
                              GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_8 | \
                              GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Alternate = GPIO_AF9_XSPIM_P2;
  HAL_GPIO_Init(GPION, &GPIO_InitStruct);
}

/**
  * @brief DeInitialize the XSPIM Port1 (XSPI-PHY1)
  * @retval None
  */
void XSPIM_DeInitPort1(void)
{
  /* De-Configure pins */
  HAL_GPIO_DeInit(GPIOO, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);
  HAL_GPIO_DeInit(GPIOP, GPIO_PIN_ALL);

  /* Disable GPIO clocks */
  __HAL_RCC_GPIOO_CLK_DISABLE();
  __HAL_RCC_GPIOP_CLK_DISABLE();

  /* Disable the XSPIM_P1 interface */
  HAL_PWREx_DisableXSPIM1();
}

/**
  * @brief DeInitialize the XSPIM Port2 (XSPI-PHY2)
  * @retval None
  */
void XSPIM_DeInitPort2(void)
{
  /* De-Configure pins */
  HAL_GPIO_DeInit(GPION, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | \
                  GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_8 | \
                  GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11);

  /* Disable GPIO clocks */
  __HAL_RCC_GPION_CLK_DISABLE();

  /* Disable the XSPIM_P2 interface */
  HAL_PWREx_DisableXSPIM2();
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
