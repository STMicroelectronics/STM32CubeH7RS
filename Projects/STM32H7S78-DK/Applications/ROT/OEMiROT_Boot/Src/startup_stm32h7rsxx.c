/*
 * Copyright (c) 2009-2020 Arm Limited. All rights reserved.
 * Copyright (c) 2023 STMicroelectronics. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * This file is derivative of CMSIS V5.6.0 startup_ARMv81MML.c
 * Git SHA: b5f0603d6a584d1724d952fd8b0737458b90d62b
 */

#include "stm32h7rsxx.h"


/*----------------------------------------------------------------------------
  Exception / Interrupt Handler Function Prototype
 *----------------------------------------------------------------------------*/
typedef void(*pFunc)(void);

/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;
extern uint32_t __STACK_LIMIT;

extern void __PROGRAM_START(void) __NO_RETURN;

extern void Error_Handler(void);

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
void Reset_Handler(void) __NO_RETURN;

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
#define DEFAULT_IRQ_HANDLER(handler_name)  \
  void handler_name(void); \
  __WEAK void handler_name(void) { \
    while(1); \
  }

/* Exceptions */
DEFAULT_IRQ_HANDLER(NMI_Handler)
DEFAULT_IRQ_HANDLER(HardFault_Handler)
DEFAULT_IRQ_HANDLER(MemManage_Handler)
DEFAULT_IRQ_HANDLER(BusFault_Handler)
DEFAULT_IRQ_HANDLER(UsageFault_Handler)
DEFAULT_IRQ_HANDLER(SVC_Handler)
DEFAULT_IRQ_HANDLER(DebugMon_Handler)
DEFAULT_IRQ_HANDLER(PendSV_Handler)
DEFAULT_IRQ_HANDLER(SysTick_Handler)

DEFAULT_IRQ_HANDLER(PVD_PVM_IRQHandler)
DEFAULT_IRQ_HANDLER(DTS_IRQHandler)
DEFAULT_IRQ_HANDLER(IWDG_IRQHandler)
DEFAULT_IRQ_HANDLER(WWDG_IRQHandler)
DEFAULT_IRQ_HANDLER(RCC_IRQHandler)
DEFAULT_IRQ_HANDLER(FLASH_IRQHandler)
DEFAULT_IRQ_HANDLER(RAMECC_IRQHandler)
DEFAULT_IRQ_HANDLER(FPU_IRQHandler)
DEFAULT_IRQ_HANDLER(TAMP_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI0_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI1_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI2_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI3_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI4_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI5_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI6_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI7_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI8_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI9_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI10_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI11_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI12_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI13_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI14_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI15_IRQHandler)
DEFAULT_IRQ_HANDLER(RTC_IRQHandler)
#if defined(STM32H7S3xx) || defined(STM32H7S7xx)
DEFAULT_IRQ_HANDLER(SAES_IRQHandler)
DEFAULT_IRQ_HANDLER(CRYP_IRQHandler)
#endif /* STM32H7S3xx || STM32H7S7xx */
DEFAULT_IRQ_HANDLER(PKA_IRQHandler)
DEFAULT_IRQ_HANDLER(HASH_IRQHandler)
DEFAULT_IRQ_HANDLER(RNG_IRQHandler)
DEFAULT_IRQ_HANDLER(ADC1_2_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel0_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel1_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel2_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel3_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel4_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel5_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel6_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel7_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM1_BRK_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM1_UP_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM1_TRG_COM_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM1_CC_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM2_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM3_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM4_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM5_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM6_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM7_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM9_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI1_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI2_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI3_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI4_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI5_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI6_IRQHandler)
DEFAULT_IRQ_HANDLER(HPDMA1_Channel0_IRQHandler)
DEFAULT_IRQ_HANDLER(HPDMA1_Channel1_IRQHandler)
DEFAULT_IRQ_HANDLER(HPDMA1_Channel2_IRQHandler)
DEFAULT_IRQ_HANDLER(HPDMA1_Channel3_IRQHandler)
DEFAULT_IRQ_HANDLER(HPDMA1_Channel4_IRQHandler)
DEFAULT_IRQ_HANDLER(HPDMA1_Channel5_IRQHandler)
DEFAULT_IRQ_HANDLER(HPDMA1_Channel6_IRQHandler)
DEFAULT_IRQ_HANDLER(HPDMA1_Channel7_IRQHandler)
DEFAULT_IRQ_HANDLER(SAI1_A_IRQHandler)
DEFAULT_IRQ_HANDLER(SAI1_B_IRQHandler)
DEFAULT_IRQ_HANDLER(SAI2_A_IRQHandler)
DEFAULT_IRQ_HANDLER(SAI2_B_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C1_EV_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C1_ER_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C2_EV_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C2_ER_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C3_EV_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C3_ER_IRQHandler)
DEFAULT_IRQ_HANDLER(USART1_IRQHandler)
DEFAULT_IRQ_HANDLER(USART2_IRQHandler)
DEFAULT_IRQ_HANDLER(USART3_IRQHandler)
DEFAULT_IRQ_HANDLER(UART4_IRQHandler)
DEFAULT_IRQ_HANDLER(UART5_IRQHandler)
DEFAULT_IRQ_HANDLER(UART7_IRQHandler)
DEFAULT_IRQ_HANDLER(UART8_IRQHandler)
DEFAULT_IRQ_HANDLER(I3C1_EV_IRQHandler)
DEFAULT_IRQ_HANDLER(I3C1_ER_IRQHandler)
DEFAULT_IRQ_HANDLER(OTG_HS_IRQHandler)
DEFAULT_IRQ_HANDLER(ETH_IRQHandler)
DEFAULT_IRQ_HANDLER(CORDIC_IRQHandler)
DEFAULT_IRQ_HANDLER(GFXTIM_IRQHandler)
DEFAULT_IRQ_HANDLER(DCMIPP_IRQHandler)
DEFAULT_IRQ_HANDLER(LTDC_IRQHandler)
DEFAULT_IRQ_HANDLER(LTDC_ER_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA2D_IRQHandler)
DEFAULT_IRQ_HANDLER(JPEG_IRQHandler)
DEFAULT_IRQ_HANDLER(GFXMMU_IRQHandler)
DEFAULT_IRQ_HANDLER(I3C1_IRQHandler)
#if defined(STM32H7S3xx) || defined(STM32H7S7xx)
DEFAULT_IRQ_HANDLER(MCE1_IRQHandler)
DEFAULT_IRQ_HANDLER(MCE2_IRQHandler)
DEFAULT_IRQ_HANDLER(MCE3_IRQHandler)
#endif /* STM32H7S3xx || STM32H7S7xx */
DEFAULT_IRQ_HANDLER(XSPI1_IRQHandler)
DEFAULT_IRQ_HANDLER(XSPI2_IRQHandler)
DEFAULT_IRQ_HANDLER(FMC_IRQHandler)
DEFAULT_IRQ_HANDLER(SDMMC1_IRQHandler)
DEFAULT_IRQ_HANDLER(SDMMC2_IRQHandler)
DEFAULT_IRQ_HANDLER(OTG_FS_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM12_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM13_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM14_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM15_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM16_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM17_IRQHandler)
DEFAULT_IRQ_HANDLER(LPTIM1_IRQHandler)
DEFAULT_IRQ_HANDLER(LPTIM2_IRQHandler)
DEFAULT_IRQ_HANDLER(LPTIM3_IRQHandler)
DEFAULT_IRQ_HANDLER(LPTIM4_IRQHandler)
DEFAULT_IRQ_HANDLER(LPTIM5_IRQHandler)
DEFAULT_IRQ_HANDLER(SPDIF_RX_IRQHandler)
DEFAULT_IRQ_HANDLER(MDIOS_IRQHandler)
DEFAULT_IRQ_HANDLER(ADF1_FLT0_IRQHandler)
DEFAULT_IRQ_HANDLER(CRS_IRQHandler)
DEFAULT_IRQ_HANDLER(UCPD1_IRQHandler)
DEFAULT_IRQ_HANDLER(CEC_IRQHandler)
DEFAULT_IRQ_HANDLER(PSSI_IRQHandler)
DEFAULT_IRQ_HANDLER(LPUART1_IRQHandler)
DEFAULT_IRQ_HANDLER(WAKEUP_PIN_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel8_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel9_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel10_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel11_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel12_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel13_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel14_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel15_IRQHandler)
DEFAULT_IRQ_HANDLER(HPDMA1_Channel8_IRQHandler)
DEFAULT_IRQ_HANDLER(HPDMA1_Channel9_IRQHandler)
DEFAULT_IRQ_HANDLER(HPDMA1_Channel10_IRQHandler)
DEFAULT_IRQ_HANDLER(HPDMA1_Channel11_IRQHandler)
DEFAULT_IRQ_HANDLER(HPDMA1_Channel12_IRQHandler)
DEFAULT_IRQ_HANDLER(HPDMA1_Channel13_IRQHandler)
DEFAULT_IRQ_HANDLER(HPDMA1_Channel14_IRQHandler)
DEFAULT_IRQ_HANDLER(HPDMA1_Channel15_IRQHandler)
DEFAULT_IRQ_HANDLER(GPU2D_IRQHandler)
DEFAULT_IRQ_HANDLER(GPU2D_ER_IRQHandler)
DEFAULT_IRQ_HANDLER(ICACHE_IRQHandler)
DEFAULT_IRQ_HANDLER(FDCAN1_IT0_IRQHandler)
DEFAULT_IRQ_HANDLER(FDCAN1_IT1_IRQHandler)
DEFAULT_IRQ_HANDLER(FDCAN2_IT0_IRQHandler)
DEFAULT_IRQ_HANDLER(FDCAN2_IT1_IRQHandler)

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif /* __GNUC__ */

extern const pFunc __VECTOR_TABLE[];
const pFunc __VECTOR_TABLE[] __VECTOR_TABLE_ATTRIBUTE =
{
  (pFunc)(&__INITIAL_SP),           /*      Initial Stack Pointer */
  Reset_Handler,                    /*      Reset Handler */
  NMI_Handler,                      /* -14: NMI Handler */
  Error_Handler,                    /* -13: Hard Fault Handler */
  Error_Handler,                    /* -12: MPU Fault Handler */
  Error_Handler,                    /* -11: Bus Fault Handler */
  Error_Handler,                    /* -10: Usage Fault Handler */
  0,                                /*      Reserved */
  0,                                /*      Reserved */
  0,                                /*      Reserved */
  0,                                /*      Reserved */
  SVC_Handler,                      /*  -5: SVCall Handler */
  DebugMon_Handler,                 /*  -4: Debug Monitor Handler */
  0,                                /*      Reserved */
  PendSV_Handler,                   /*  -2: PendSV Handler */
  SysTick_Handler,                  /*  -1: SysTick Handler */
  PVD_PVM_IRQHandler,               /*   0: PVD/PVM through EXTI Line detection */
  0,                                /*   1: Reserved */
  DTS_IRQHandler,                   /*   2: Digital Temperature Sensor global interrupt */
  IWDG_IRQHandler,                  /*   3: Internal Watchdog global interrupt */
  WWDG_IRQHandler,                  /*   4: Window WatchDog global interrupt */
  RCC_IRQHandler,                   /*   5: RCC global interrupts through EXTI Line detection */
  0,                                /*   6: Reserved */
  0,                                /*   7: Reserved */
  FLASH_IRQHandler,                 /*   8: FLASH global interrupt */
  RAMECC_IRQHandler,                /*   9: SRAM ECC flags, FPU errors */
  FPU_IRQHandler,                   /*  10: FPU global interrupt */
  0,                                /*  11: Reserved */
  0,                                /*  12: Reserved */
  TAMP_IRQHandler,                  /*  13: Tamper non-secure interrupt  */
  0,                                /*  14: Reserved */
  0,                                /*  15: Reserved */
  EXTI0_IRQHandler,                 /*  16: EXTI Line0 interrupt */
  EXTI1_IRQHandler,                 /*  17: EXTI Line1 interrupt */
  EXTI2_IRQHandler,                 /*  18: EXTI Line2 interrupt */
  EXTI3_IRQHandler,                 /*  19: EXTI Line3 interrupt */
  EXTI4_IRQHandler,                 /*  20: EXTI Line4 interrupt */
  EXTI5_IRQHandler,                 /*  21: EXTI Line5 interrupt */
  EXTI6_IRQHandler,                 /*  22: EXTI Line6 interrupt */
  EXTI7_IRQHandler,                 /*  23: EXTI Line7 interrupt */
  EXTI8_IRQHandler,                 /*  24: EXTI Line8 interrupt */
  EXTI9_IRQHandler,                 /*  25: EXTI Line9 interrupt */
  EXTI10_IRQHandler,                /*  26: EXTI Line10 interrupt */
  EXTI11_IRQHandler,                /*  27: EXTI Line11 interrupt */
  EXTI12_IRQHandler,                /*  28: EXTI Line12 interrupt */
  EXTI13_IRQHandler,                /*  29: EXTI Line13 interrupt */
  EXTI14_IRQHandler,                /*  30: EXTI Line14 interrupt */
  EXTI15_IRQHandler,                /*  31: EXTI Line15 interrupt */
  RTC_IRQHandler,                   /*  32: RTC wakeup and alarm interrupts through EXTI Line detection */
#if defined(STM32H7S3xx) || defined(STM32H7S7xx)
  SAES_IRQHandler,                  /*  33: SAES global interrupt */
  CRYP_IRQHandler,                  /*  34: CRYP global interrupt */
#elif defined(STM32H7R3xx) || defined(STM32H7R7xx)
  0,                                /*  33: Reserved */
  0,                                /*  34: Reserved */
#endif /* STM32H7S3xx || STM32H7S7xx */
  PKA_IRQHandler,                   /*  35: PKA global interrupt */
  HASH_IRQHandler,                  /*  36: HASH global interrupt */
  RNG_IRQHandler,                   /*  37: RNG global interrupt */
  ADC1_2_IRQHandler,                /*  38: ADC1 & ADC2 global interrupt */
  GPDMA1_Channel0_IRQHandler,       /*  39: GPDMA1 Channel 0 global interrupt */
  GPDMA1_Channel1_IRQHandler,       /*  40: GPDMA1 Channel 1 global interrupt */
  GPDMA1_Channel2_IRQHandler,       /*  41: GPDMA1 Channel 2 global interrupt */
  GPDMA1_Channel3_IRQHandler,       /*  42: GPDMA1 Channel 3 global interrupt */
  GPDMA1_Channel4_IRQHandler,       /*  43: GPDMA1 Channel 4 global interrupt */
  GPDMA1_Channel5_IRQHandler,       /*  44: GPDMA1 Channel 5 global interrupt */
  GPDMA1_Channel6_IRQHandler,       /*  45: GPDMA1 Channel 6 global interrupt */
  GPDMA1_Channel7_IRQHandler,       /*  46: GPDMA1 Channel 7 global interrupt */
  TIM1_BRK_IRQHandler,              /*  47: TIM1 Break interrupt */
  TIM1_UP_IRQHandler,               /*  48: TIM1 Update interrupt */
  TIM1_TRG_COM_IRQHandler,          /*  49: TIM1 Trigger and Commutation interrupt */
  TIM1_CC_IRQHandler,               /*  50: TIM1 Capture Compare interrupt */
  TIM2_IRQHandler,                  /*  51: TIM2 global interrupt */
  TIM3_IRQHandler,                  /*  52: TIM3 global interrupt */
  TIM4_IRQHandler,                  /*  53: TIM4 global interrupt */
  TIM5_IRQHandler,                  /*  54: TIM5 global interrupt */
  TIM6_IRQHandler,                  /*  55: TIM6 global interrupt */
  TIM7_IRQHandler,                  /*  56: TIM7 global interrupt */
  TIM9_IRQHandler,                  /*  57: TIM9 global interrupt */
  SPI1_IRQHandler,                  /*  58: SPI1 global interrupt */
  SPI2_IRQHandler,                  /*  59: SPI2 global interrupt */
  SPI3_IRQHandler,                  /*  60: SPI3 global interrupt */
  SPI4_IRQHandler,                  /*  61: SPI4 global interrupt */
  SPI5_IRQHandler,                  /*  62: SPI5 global interrupt */
  SPI6_IRQHandler,                  /*  63: SPI6 global interrupt */
  HPDMA1_Channel0_IRQHandler,       /*  64: HPDMA1 Channel 0 global interrupt */
  HPDMA1_Channel1_IRQHandler,       /*  65: HPDMA1 Channel 1 global interrupt */
  HPDMA1_Channel2_IRQHandler,       /*  66: HPDMA1 Channel 2 global interrupt */
  HPDMA1_Channel3_IRQHandler,       /*  67: HPDMA1 Channel 3 global interrupt */
  HPDMA1_Channel4_IRQHandler,       /*  68: HPDMA1 Channel 4 global interrupt */
  HPDMA1_Channel5_IRQHandler,       /*  69: HPDMA1 Channel 5 global interrupt */
  HPDMA1_Channel6_IRQHandler,       /*  70: HPDMA1 Channel 6 global interrupt */
  HPDMA1_Channel7_IRQHandler,       /*  71: HPDMA1 Channel 7 global interrupt */
  SAI1_A_IRQHandler,                /*  72: Serial Audio Interface 1 block A global interrupt */
  SAI1_B_IRQHandler,                /*  73: Serial Audio Interface 1 block B global interrupt */
  SAI2_A_IRQHandler,                /*  74: Serial Audio Interface 2 block A global interrupt */
  SAI2_B_IRQHandler,                /*  75: Serial Audio Interface 2 block B global interrupt */
  I2C1_EV_IRQHandler,               /*  76: I2C1 event interrupt */
  I2C1_ER_IRQHandler,               /*  77: I2C1 error interrupt */
  I2C2_EV_IRQHandler,               /*  78: I2C2 event interrupt */
  I2C2_ER_IRQHandler,               /*  79: I2C2 error interrupt */
  I2C3_EV_IRQHandler,               /*  80: I2C3 event interrupt */
  I2C3_ER_IRQHandler,               /*  81: I2C3 error interrupt */
  USART1_IRQHandler,                /*  82: USART1 global interrupt */
  USART2_IRQHandler,                /*  83: USART2 global interrupt */
  USART3_IRQHandler,                /*  84: USART3 global interrupt */
  UART4_IRQHandler,                 /*  85: UART4 global interrupt */
  UART5_IRQHandler,                 /*  86: UART5 global interrupt */
  UART7_IRQHandler,                 /*  87: UART7 global interrupt */
  UART8_IRQHandler,                 /*  88: UART8 global interrupt */
  I3C1_EV_IRQHandler,               /*  89: I3C1 event interrupt */
  I3C1_ER_IRQHandler,               /*  90: I3C1 error interrupt */
  OTG_HS_IRQHandler,                /*  91: USB OTG HS global interrupt */
  ETH_IRQHandler,                   /*  92: Ethernet global interrupt */
  CORDIC_IRQHandler,                /*  93: CORDIC global interrupt */
  GFXTIM_IRQHandler,                /*  94: GFXTIM global interrupt */
  DCMIPP_IRQHandler,                /*  95: DCMIPP global interrupt */
  LTDC_IRQHandler,                  /*  96: LTDC global interrupt */
  LTDC_ER_IRQHandler,               /*  97: LTDC error global interrupt */
  DMA2D_IRQHandler,                 /*  98: DMA2D global interrupt */
  JPEG_IRQHandler,                  /*  99: JPEG global interrupt */
  GFXMMU_IRQHandler,                /* 100: GFXMMU global interrupt */
  I3C1_IRQHandler,                  /* 101: I3C1 wakeup interrupt */
#if defined(STM32H7S3xx) || defined(STM32H7S7xx)
  MCE1_IRQHandler,                  /* 102: MCE1 global interrupt */
  MCE2_IRQHandler,                  /* 103: MCE2 global interrupt */
  MCE3_IRQHandler,                  /* 104: MCE3 global interrupt */
#elif defined(STM32H7R3xx) || defined(STM32H7R7xx)
  0,                                /* 102: Reserved */
  0,                                /* 103: Reserved */
  0,                                /* 104: Reserved */
#endif /* STM32H7S3xx || STM32H7S7xx */
  XSPI1_IRQHandler,                 /* 105: XSPI1 global interrupt */
  XSPI2_IRQHandler,                 /* 106: XSPI2 global interrupt */
  FMC_IRQHandler,                   /* 107: FMC global interrupt */
  SDMMC1_IRQHandler,                /* 108: SDMMC1 global interrupt */
  SDMMC2_IRQHandler,                /* 109: SDMMC2 global interrupt */
  0,                                /* 110: Reserved */
  0,                                /* 111: Reserved */
  OTG_FS_IRQHandler,                /* 112: USB OTG FS global interrupt */
  TIM12_IRQHandler,                 /* 113: TIM12 global interrupt */
  TIM13_IRQHandler,                 /* 114: TIM13 global interrupt */
  TIM14_IRQHandler,                 /* 115: TIM14 global interrupt */
  TIM15_IRQHandler,                 /* 116: TIM15 global interrupt */
  TIM16_IRQHandler,                 /* 117: TIM16 global interrupt */
  TIM17_IRQHandler,                 /* 118: TIM17 global interrupt */
  LPTIM1_IRQHandler,                /* 119: LPTIM1 global interrupt */
  LPTIM2_IRQHandler,                /* 120: LPTIM2 global interrupt */
  LPTIM3_IRQHandler,                /* 121: LPTIM3 global interrupt */
  LPTIM4_IRQHandler,                /* 122: LPTIM4 global interrupt */
  LPTIM5_IRQHandler,                /* 123: LPTIM5 global interrupt */
  SPDIF_RX_IRQHandler,              /* 124: SPDIF_RX interrupt */
  MDIOS_IRQHandler,                 /* 125: MDIOS global interrupt */
  ADF1_FLT0_IRQHandler,             /* 126: ADF1 Filter 0 interrupt */
  CRS_IRQHandler,                   /* 127: CRS global interrupt */
  UCPD1_IRQHandler,                 /* 128: UCPD1 global interrupt */
  CEC_IRQHandler,                   /* 129: HDMI_CEC global interrupt */
  PSSI_IRQHandler,                  /* 130: PSSI global interrupt */
  LPUART1_IRQHandler,               /* 131: LPUART1 global interrupt */
  WAKEUP_PIN_IRQHandler,            /* 132: Wake-up pins interrupt */
  GPDMA1_Channel8_IRQHandler,       /* 133: GPDMA1 Channel 8 global interrupt */
  GPDMA1_Channel9_IRQHandler,       /* 134: GPDMA1 Channel 9 global interrupt */
  GPDMA1_Channel10_IRQHandler,      /* 135: GPDMA1 Channel 10 global interrupt */
  GPDMA1_Channel11_IRQHandler,      /* 136: GPDMA1 Channel 11 global interrupt */
  GPDMA1_Channel12_IRQHandler,      /* 137: GPDMA1 Channel 12 global interrupt */
  GPDMA1_Channel13_IRQHandler,      /* 138: GPDMA1 Channel 13 global interrupt */
  GPDMA1_Channel14_IRQHandler,      /* 139: GPDMA1 Channel 14 global interrupt */
  GPDMA1_Channel15_IRQHandler,      /* 140: GPDMA1 Channel 15 global interrupt */
  HPDMA1_Channel8_IRQHandler,       /* 141: HPDMA1 Channel 8 global interrupt */
  HPDMA1_Channel9_IRQHandler,       /* 142: HPDMA1 Channel 9 global interrupt */
  HPDMA1_Channel10_IRQHandler,      /* 143: HPDMA1 Channel 10 global interrupt */
  HPDMA1_Channel11_IRQHandler,      /* 144: HPDMA1 Channel 11 global interrupt */
  HPDMA1_Channel12_IRQHandler,      /* 145: HPDMA1 Channel 12 global interrupt */
  HPDMA1_Channel13_IRQHandler,      /* 146: HPDMA1 Channel 13 global interrupt */
  HPDMA1_Channel14_IRQHandler,      /* 147: HPDMA1 Channel 14 global interrupt */
  HPDMA1_Channel15_IRQHandler,      /* 148: HPDMA1 Channel 15 global interrupt */
  GPU2D_IRQHandler,                 /* 149: GPU2D error interrupt */
  GPU2D_ER_IRQHandler,              /* 150: I2C4 error interrupt */
  ICACHE_IRQHandler,                /* 151: ICACHE interrupt */
  FDCAN1_IT0_IRQHandler,            /* 152: FDCAN1 Interrupt 0 */
  FDCAN1_IT1_IRQHandler,            /* 153: FDCAN1 Interrupt 1 */
  FDCAN2_IT0_IRQHandler,            /* 154: FDCAN2 Interrupt 0 */
  FDCAN2_IT1_IRQHandler,            /* 155: FDCAN2 Interrupt 1 */
};
#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif /* __GNUC__ */

#ifdef OEMIROT_DEV_MODE
#ifdef __ICCARM__
__no_init volatile uint32_t TamperEventCleared;
#elif defined(__ARMCC_VERSION)
volatile uint32_t TamperEventCleared  __attribute__((section(".bss.noinit")));
#else /* not __ICCARM__ */
volatile uint32_t TamperEventCleared  __attribute__((section(".noinit")));
#endif /* __ICCARM__ */
#endif /* OEMIROT_DEV_MODE */
/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(void)
{
  /* Configure DWT to enable cycles counter */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk | CoreDebug_DEMCR_MON_EN_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  __IO uint64_t *pt = (uint64_t *)DTCM_BASE;
  uint32_t index;
  /* Write in the DTCM to prevent RAMECC notifications
   * due to speculative reads (.noinit has to be set
   * in another NVM memory */
  for (index = 0; index < (DTCM_SIZE / 8); index++)
  {
    pt[index] = 0;
  }
  /* Write in the SRAM1 to prevent RAMECC notifications
   * due to speculative reads (if SRAM1 is not used in an OEMuROT config) */
  if (!((((uint32_t)__VECTOR_TABLE) >= SRAM1_AXI_BASE) &&
        (((uint32_t)__VECTOR_TABLE) <= (SRAM1_AXI_BASE + SRAM1_AXI_SIZE))))
  {
    pt = (uint64_t *)SRAM1_AXI_BASE;
    for (index = 0; index < (SRAM1_AXI_SIZE / 8); index++)
    {
      pt[index] = 0;
    }
  }
  /* Write in the SRAM3 to prevent RAMECC notifications
   * due to speculative reads (if SRAM3 is not used in an OEMuROT config) */
  if (!((((uint32_t)__VECTOR_TABLE) >= SRAM3_AXI_BASE) &&
        (((uint32_t)__VECTOR_TABLE) <= (SRAM3_AXI_BASE + SRAM3_AXI_SIZE))))
  {
    pt = (uint64_t *)SRAM3_AXI_BASE;
    for (index = 0; index < (SRAM3_AXI_SIZE / 8); index++)
    {
      pt[index] = 0;
    }
  }
  /* CMSIS System Initialization */
  SystemInit();
  /* enable access to tamper register */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_RTC_ENABLE();
  /* Release reset of back-up domain in case it is set, to avoid blocking the device (system reset
  does not release it) */
  __HAL_RCC_BACKUPRESET_RELEASE();

#ifdef OEMIROT_DEV_MODE
  /* Reset the tamper event status */
  TamperEventCleared = 0;
#endif /* OEMIROT_DEV_MODE */
  /* Get tamper status */
  if (READ_REG(TAMP->SR))
  {
#ifdef OEMIROT_DEV_MODE
#if 1
    /* avoid several re-boot in DEV_MODE with Tamper active, clean tamper configuration */
    __HAL_RCC_BACKUPRESET_FORCE();
    __HAL_RCC_BACKUPRESET_RELEASE();
    /* wait for event being cleared */
    while (READ_REG(TAMP->SR));
#else /* 0 */
    /* clear tamper event */
    WRITE_REG(TAMP->SCR, READ_REG(TAMP->SR));
#endif /* 1 */
    /* memorize for log that event has been cleared */
    TamperEventCleared = 1;
#else /* not OEMIROT_DEV_MODE */
    /* VBAT and VDD must be set to zero to allow board to restart */
    Error_Handler();
#endif /* OEMIROT_DEV_MODE */
  }
  /*  Enable TAMP IRQ , to catch tamper interrupt in TAMP_IRQHandler
   *  else a stack in SRAM2 is cleaned a HardFault can occur, at every pop of
   *  function */
#ifndef OEMIROT_DEV_MODE
  HAL_NVIC_EnableIRQ(TAMP_IRQn);
#endif /* OEMIROT_DEV_MODE */
  __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
