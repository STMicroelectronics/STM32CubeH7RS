## <b>TIM_OutputCompare_Init Example Description</b>

Configuration of the TIM peripheral to generate an output
waveform in different output compare modes. This example is based on the
STM32H7RSxx TIM LL API. The peripheral initialization uses
LL unitary service functions for optimization purposes (performance and size).

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

      In this example TIM2 input clock (TIM2CLK) frequency is set to APB1 clock (PCLK1),
      since APB1 pre-scaler is equal to 2.
      TIM2CLK = PCLK1
      PCLK1 = HCLK
      => TIM2CLK = HCLK = SystemCoreClock/2 (300 Mhz)

To set the TIM2 counter clock frequency to 10 KHz, the pre-scaler (PSC) is
calculated as follows:

    PSC = (TIM2CLK / TIM2 counter clock) - 1
    PSC = ((SystemCoreClock/2) /10 KHz) - 1

SystemCoreClock is set to 600 MHz for STM32H7RSxx Devices.

Auto-reload (ARR) is calculated to get a time base period of 100 ms,
meaning a time base frequency of 10 Hz.

    ARR = (TIM2 counter clock / time base frequency) - 1
    ARR = (TIM2 counter clock / 10) - 1

The capture/compare register (CCR1) of the output channel is set to half the
auto-reload value. Therefore the timer output compare delay is 50 ms.
Generally speaking this delay is calculated as follows:

    CC1_delay = TIM2 counter clock / CCR1

The timer output channel must be connected to PA15 on board NUCLEO-H7S3L8.
Thus TIM2_CH1 status (on/off) mirrors the timer output level (active v.s. inactive).

USER push-button can be used to change the output compare mode:

  - When the output channel is configured in output compare toggle:  TIM2_CH1
    TOGGLES when the counter (CNT) matches the capture/compare register (CCR1).
  - When the output channel is configured in output compare active:  TIM2_CH1
    switched ON when the counter (CNT) matches the capture/compare register
    (CCR1).
  - When the output channel is configured in output compare inactive:  TIM2_CH1
    switched OFF when the counter (CNT) matches the capture/compare register
    (CCR1).

Initially the output channel is configured in output compare toggle mode.

#### <b>Notes</b>

 1. Whenever the application is using ITCM/DTCM memories (@0x0000000 / @0x20000000: not cacheable and only accessible
    by the Cortex M7 and the GPDMA/HPDMA), there is no need for cache maintenance.
    If the application needs to put DMA buffers in AXI SRAM (starting from @0x24000000), the user has to:
    - either define a non-cacheable region in the MPU and linker configuration file to locate DMA buffers
      (a proposed noncacheable_buffer section is available from CMSIS Device linker template file and its size must
      be adapted to the application requirements)
    - or to ensure cache maintenance operations to ensure the cache coherence between the CPU and the DMAs.

	This is true also for any other data buffers accessed by the CPU and other masters (DMA2D, LTDC)
    The addresses and the size of cacheable buffers (shared between CPU and other masters)
    must be properly defined to be aligned to data cache line size (32 bytes) and of a size of being multiple
    of this cache line size.
    - Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
    - Please refer to the AN4839 "Level 1 cache on STM32F7 Series"

### <b>Keywords</b>

Timers, TIM, Output, signals, Outpur compare toggle, PWM, Oscilloscope

### <b>Directory contents</b>

  - TIM/TIM_OutputCompare_Init/Boot/Inc/stm32h7rsxx_it.h        Interrupt handlers header file
  - TIM/TIM_OutputCompare_Init/Boot/Inc/main.h                  Header for main.c module
  - TIM/TIM_OutputCompare_Init/Boot/Inc/stm32_assert.h          Template file to include assert_failed function
  - TIM/TIM_OutputCompare_Init/Boot/Src/stm32h7rsxx_it.c        Interrupt handlers
  - TIM/TIM_OutputCompare_Init/Boot/Src/main.c                  Main program
  - TIM/TIM_OutputCompare_Init/Boot/Src/system_stm32h7rsxx.c    STM32H7RSxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.
  - NUCLEO-H7S3L8 Set-up:
      - TIM2_CH1  PA15: connected to pin 9 of CN7 connector

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

