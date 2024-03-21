/**
  ******************************************************************************
  * @file    system_stm32h7rsxx.c
  * @author  MCD Application Team
  * @brief   CMSIS Cortex-M7 Device Peripheral Access Layer System Source File
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
  *
  *   This file provides two functions and one global variable to be called from
  *   user application:
  *      - SystemInit(): This function is called at startup just after reset and
  *                      before branch to main program. This call is made inside
  *                      the "startup_stm32h7rsxx.s" file.
  *
  *      - SystemCoreClock variable: Contains the core clock (sys_cpu_ck), it can
  *                                  be used by the user application to setup the
  *                                  SysTick timer or configure other parameters.
  *
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.
  *
  *   After each device reset the HSI (64 MHz) is used as system clock source.
  *   Then SystemInit() function is called, in "startup_stm32h7rsxx.s" file, to
  *   optionally configure the system clock before to branch to main program.
  *
  *=============================================================================
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup STM32H7RSxx_System
  * @{
  */

/** @addtogroup STM32H7RSxx_System_Private_Includes
  * @{
  */

#include "stm32h7rsxx.h"
#include <math.h>

/**
  * @}
  */

/** @addtogroup STM32H7RSxx_System_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32H7RSxx_System_Private_Defines
  * @{
  */
#if !defined  (HSE_VALUE)
  #define HSE_VALUE    24000000UL /*!< Value of the High-Speed External oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined  (HSI_VALUE)
  #define HSI_VALUE    64000000UL /*!< Value of the High-Speed Internal oscillator in Hz */
#endif /* HSI_VALUE */

#if !defined  (CSI_VALUE)
  #define CSI_VALUE    4000000UL  /*!< Value of the Low-power Internal oscillator in Hz */
#endif /* CSI_VALUE */

/*!< The VTOR location information is based on information from the linker with a dependency
     on the IDE, the cortex register is updated using the INTVECT_START.
*/
extern uint32_t __VECTOR_TABLE;
#define INTVECT_START ((uint32_t)& __VECTOR_TABLE)



/******************************************************************************/
/**
  * @}
  */

/** @addtogroup STM32H7RSxx_System_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32H7RSxx_System_Private_Variables
  * @{
  */
  /* The SystemCoreClock variable is updated in two ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
         Note: If you use this function to configure the system clock; then there
               is no need to call the first function listed above, since SystemCoreClock
               variable is updated automatically.
  */
uint32_t SystemCoreClock = HSI_VALUE;

/**
  * @}
  */

/** @addtogroup STM32H7RSxx_System_Private_FunctionPrototypes
  * @{
  */
static void SetSysClock(void);

/**
  * @}
  */

/** @addtogroup STM32H7RSxx_System_Private_Functions
  * @{
  */

/**
  * @brief  Setup the microcontroller system.
  * @retval None
  */

void SystemInit(void)
{
  /* Configure the Vector Table location -------------------------------------*/
  SCB->VTOR = INTVECT_START;

  /* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
  SCB->CPACR |= ((3UL << 20U)|(3UL << 22U));  /* set CP10 and CP11 Full Access */
#endif

  /* Configure the System clock source, PLL Multiplier and Divider factors,
     AHB/APBx prescalers and Flash settings for System clock 464 MHz */
  SetSysClock();
}

/**
  * @brief  Update SystemCoreClock variable according to RCC registers values.
  *         The SystemCoreClock variable contains the core clock (sys_cpu_ck), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *
  * @note   Each time the core clock changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.
  *
  * @note   - The system frequency computed by this function is not the real
  *           frequency in the chip. It is calculated based on the predefined
  *           constant and the selected clock source:
  *
  *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
  *
  *           - If SYSCLK source is CSI, SystemCoreClock will contain the CSI_VALUE(**)
  *
  *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(***)
  *
  *           - If SYSCLK source is PLL, SystemCoreClock will contain the HSI_VALUE(*)
  *             or CSI_VALUE(**) or HSE_VALUE(***) multiplied/divided by the PLL factors.
  *
  *         (*) HSI_VALUE is a constant defined in stm32h7rsxx_hal.h file (default value
  *              64 MHz) but the real value may vary depending on the variations
  *              in voltage and temperature.
  *
  *         (**) CSI_VALUE is a constant defined in stm32h7rsxx_hal.h file (default value
  *             4 MHz) but the real value may vary depending on the variations
  *             in voltage and temperature.
  *
  *         (***) HSE_VALUE is a constant defined in stm32h7rsxx_hal.h file (default value
  *              24 MHz), user has to ensure that HSE_VALUE is same as the real
  *              frequency of the crystal used. Otherwise, this function may
  *              have wrong result.
  *
  *         - The result of this function could be not correct when using fractional
  *           value for HSE crystal.
  *
  * @retval None
  */
void SystemCoreClockUpdate(void)
{
  uint32_t sysclk, hsivalue, pllsource, pllm, pllp, core_presc;
  float_t pllfracn, pllvco;

  /* Get SYSCLK source -------------------------------------------------------*/
  switch (RCC->CFGR & RCC_CFGR_SWS)
  {
  case 0x00:  /* HSI used as system clock source (default after reset) */
    sysclk = (HSI_VALUE >> ((RCC->CR & RCC_CR_HSIDIV) >> RCC_CR_HSIDIV_Pos));
    break;

  case 0x08:  /* CSI used as system clock source */
    sysclk = CSI_VALUE;
    break;

  case 0x10:  /* HSE used as system clock source */
    sysclk = HSE_VALUE;
    break;

  case 0x18:  /* PLL1 used as system clock  source */
    /* PLL1_VCO = (HSE_VALUE or HSI_VALUE or CSI_VALUE/ PLLM) * PLLN
       SYSCLK = PLL1_VCO / PLL1R
       */
    pllsource = (RCC->PLLCKSELR & RCC_PLLCKSELR_PLLSRC);
    pllm = ((RCC->PLLCKSELR & RCC_PLLCKSELR_DIVM1) >> RCC_PLLCKSELR_DIVM1_Pos)  ;
    if ((RCC->PLLCFGR & RCC_PLLCFGR_PLL1FRACEN) != 0U)
    {
      pllfracn = (float_t)(uint32_t)(((RCC->PLL1FRACR & RCC_PLL1FRACR_FRACN)>> RCC_PLL1FRACR_FRACN_Pos));
    }
    else
    {
      pllfracn = (float_t)0U;
    }

    if (pllm != 0U)
    {
      switch (pllsource)
      {
      case 0x02:  /* HSE used as PLL1 clock source */
        pllvco = ((float_t)HSE_VALUE / (float_t)pllm) * ((float_t)(uint32_t)(RCC->PLL1DIVR1 & RCC_PLL1DIVR1_DIVN) + (pllfracn/(float_t)0x2000) +(float_t)1 );
        break;

      case 0x01:  /* CSI used as PLL1 clock source */
        pllvco = ((float_t)CSI_VALUE / (float_t)pllm) * ((float_t)(uint32_t)(RCC->PLL1DIVR1 & RCC_PLL1DIVR1_DIVN) + (pllfracn/(float_t)0x2000) +(float_t)1 );
        break;

      case 0x00:  /* HSI used as PLL1 clock source */
      default:
        hsivalue = (HSI_VALUE >> ((RCC->CR & RCC_CR_HSIDIV) >> RCC_CR_HSIDIV_Pos));
        pllvco = ( (float_t)hsivalue / (float_t)pllm) * ((float_t)(uint32_t)(RCC->PLL1DIVR1 & RCC_PLL1DIVR1_DIVN) + (pllfracn/(float_t)0x2000) +(float_t)1 );
        break;
      }
      pllp = (((RCC->PLL1DIVR1 & RCC_PLL1DIVR1_DIVP) >> RCC_PLL1DIVR1_DIVP_Pos) + 1U ) ;
      sysclk =  (uint32_t)(float_t)(pllvco/(float_t)pllp);
    }
    else
    {
      sysclk = 0U;
    }
    break;

  default:  /* Unexpected, default to HSI used as system clock source (default after reset) */
    sysclk = (HSI_VALUE >> ((RCC->CR & RCC_CR_HSIDIV) >> RCC_CR_HSIDIV_Pos));
    break;
  }

  /* system clock frequency : CM7 CPU frequency  */
  core_presc = (RCC->CDCFGR & RCC_CDCFGR_CPRE);
  if (core_presc >= 8U)
  {
    SystemCoreClock = (sysclk >> (core_presc - RCC_CDCFGR_CPRE_3 + 1U));
  }
  else
  {
    SystemCoreClock = sysclk;
  }
}

/**
  * @brief  Configures the System clock source, PLL Multiplier and Divider factors,
  *         AHB/APBx prescalers and Flash settings for System clock 380 MHz
  * @retval None
  */
static void SetSysClock(void)
{
  /* Enable voltage range 1 for high performance */
  MODIFY_REG(PWR->CSR4, PWR_CSR4_VOS, PWR_REGULATOR_VOLTAGE_SCALE1);
  while ((PWR->CSR4 & PWR_CSR4_VOSRDY) != PWR_CSR4_VOSRDY)
  {
  }

  /* HSI Calibration with default value */
  MODIFY_REG(RCC->HSICFGR, RCC_HSICFGR_HSITRIM, RCC_HSICALIBRATION_DEFAULT << RCC_HSICFGR_HSITRIM_Pos);
  MODIFY_REG(RCC->CR, RCC_CR_HSIDIV, RCC_HSI_DIV1);
  /* Enable HSI oscillator */
  SET_BIT(RCC->CR, RCC_CR_HSION);
  while((RCC->CR & RCC_CR_HSIRDY) != RCC_CR_HSIRDY)
  {
  }

  /* In case of clock reconfiguration, select HSI as system clock and disable PLL1  */
  MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_SYSCLKSOURCE_HSI);
  CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLL1PEN | RCC_PLLCFGR_PLL1QEN | RCC_PLLCFGR_PLL1REN | RCC_PLLCFGR_PLL1SEN | 0x00000200U);
  CLEAR_BIT(RCC->CR, RCC_CR_PLL1ON);

  /* Configure PLL clock source */
  MODIFY_REG(RCC->PLLCKSELR, RCC_PLLCKSELR_PLLSRC, RCC_PLLSOURCE_HSI);

  /* Main PLL configuration and activation  (PLL1)*/
  /* Configure PLLM divider */
  MODIFY_REG(RCC->PLLCKSELR, RCC_PLLCKSELR_DIVM1, 32U << RCC_PLLCKSELR_DIVM1_Pos);
  /* pllvco = HSI_VALUE / PLLM = 64000000 / 32 = 2 MHz */
  /* Configure VCO input range, VCO selection and clear FRACEN */
  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLL1RGE, RCC_PLL_VCOINPUT_RANGE1);
  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLL1VCOSEL, RCC_PLL_VCO_HIGH);
  CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLL1FRACEN);
  /* Configure PLLN, PLLP, PLLQ, PLLR, PLLS and PLLT dividers */
  WRITE_REG(RCC->PLL1DIVR1, ((380U - 1U) |
                                   ((2U - 1U) << RCC_PLL1DIVR1_DIVP_Pos) |
                                   ((2U - 1U) << RCC_PLL1DIVR1_DIVQ_Pos) |
                                   ((2U - 1U) << RCC_PLL1DIVR1_DIVR_Pos)));
  WRITE_REG(RCC->PLL1DIVR2, ((2U - 1U) |
                                   ((2U - 1U) << RCC_PLL2DIVR2_DIVT_Pos)));

  SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLL1PEN);

  /* Enable the PLLx */
  SET_BIT(RCC->CR, RCC_CR_PLL1ON);
  while (READ_BIT(RCC->CR, RCC_CR_PLL1RDY) != RCC_CR_PLL1RDY)
  {
  }

  /* PLL2 selected as external memory clock source */
  /* Configure PLLM divider */
  MODIFY_REG(RCC->PLLCKSELR, RCC_PLLCKSELR_DIVM2, 4U << RCC_PLLCKSELR_DIVM2_Pos);
  /* pllvco = HSI_VALUE / PLLM = 64000000 / 4 = 16 MHz */
  /* Configure VCO input range, VCO selection and clear FRACEN */
  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLL2RGE, (RCC_PLL_VCOINPUT_RANGE3 << (RCC_PLLCFGR_PLL2RGE_Pos - RCC_PLLCFGR_PLL1RGE_Pos)));
  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLL2VCOSEL, (RCC_PLL_VCO_HIGH << (RCC_PLLCFGR_PLL2RGE_Pos - RCC_PLLCFGR_PLL1RGE_Pos)));
  CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLL2FRACEN);
  /* Configure PLLN, PLLP, PLLQ, PLLR, PLLS and PLLT dividers */
  WRITE_REG(RCC->PLL2DIVR1, ((25U - 1U) |
                                   ((2U - 1U) << RCC_PLL2DIVR1_DIVP_Pos) |
                                   ((2U - 1U) << RCC_PLL2DIVR1_DIVQ_Pos) |
                                   ((2U - 1U) << RCC_PLL2DIVR1_DIVR_Pos)));
  WRITE_REG(RCC->PLL2DIVR2, ((2U - 1U) |
                                   ((2U - 1U) << RCC_PLL2DIVR2_DIVT_Pos)));

  /* Enable the PLL2 */
  SET_BIT(RCC->CR, RCC_CR_PLL2ON);
  while (READ_BIT(RCC->CR, RCC_CR_PLL2RDY) != RCC_CR_PLL2RDY)
  {
  }

  /* Set FLASH latency */
  MODIFY_REG(FLASH->ACR, (FLASH_ACR_LATENCY | FLASH_ACR_WRHIGHFREQ), FLASH_LATENCY_5);
  while (READ_BIT(FLASH->ACR, (FLASH_ACR_LATENCY | FLASH_ACR_WRHIGHFREQ)) != FLASH_LATENCY_5)
  {
  }

  /* Set APB1, APB2, APB4, APB5 and AHB prescalers */
  MODIFY_REG(RCC->APBCFGR, RCC_APBCFGR_PPRE1, RCC_APB1_DIV2);
  MODIFY_REG(RCC->APBCFGR, RCC_APBCFGR_PPRE2, RCC_APB2_DIV2);
  MODIFY_REG(RCC->APBCFGR, RCC_APBCFGR_PPRE4, RCC_APB4_DIV2);
  MODIFY_REG(RCC->APBCFGR, RCC_APBCFGR_PPRE5, RCC_APB5_DIV2);
  MODIFY_REG(RCC->BMCFGR, RCC_BMCFGR_BMPRE, RCC_HCLK_DIV2);

  /* SYSCLK Configuration */
  MODIFY_REG(RCC->CDCFGR, RCC_CDCFGR_CPRE, RCC_SYSCLK_DIV1);
  MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_SYSCLKSOURCE_PLLCLK);
  while ((READ_BIT(RCC->CFGR, RCC_CFGR_SWS)) != (RCC_SYSCLKSOURCE_PLLCLK << RCC_CFGR_SWS_Pos))
  {
  }

  /* Enable the HSI48 oscillator (HSI48) for RNG peripheral */
  SET_BIT(RCC->CR, RCC_CR_HSI48ON);
  while (READ_BIT(RCC->CR, RCC_CR_HSI48RDY) == 0U)
  {
  }

  /* Enable the IPs */
  __HAL_RCC_SRAM2_CLK_ENABLE(); /* Enable AHB SRAM2 */
  __HAL_RCC_SBS_CLK_ENABLE();   /* Enable SBS IP */
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
