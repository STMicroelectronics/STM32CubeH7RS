## <b>TIM_BreakAndDeadtime_Init Example Description</b>

Configuration of the TIM peripheral to
generate three center-aligned PWM and complementary PWM signals,
insert a defined deadtime value,
use the break feature,
and lock the break and dead-time configuration.

This example is based on the STM32H7RSxx TIM LL API. The peripheral initialization
uses LL initialization function to demonstrate LL Init.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

TIM1CLK is fixed to 300 MHz, the TIM1 Prescaler is set to have
TIM1 counter clock = 10 MHz.

The TIM1 auto-reload is set to generate PWM signal at 4000 Hz:

The Three Duty cycles are computed as the following description:

The channel 1 duty cycle is set to 50% so channel 1N is set to 50%.

The channel 2 duty cycle is set to 25% so channel 2N is set to 75%.

The channel 3 duty cycle is set to 12.5% so channel 3N is set to 87.5%.

A dead time equal to 3 us is inserted between
the different complementary signals, and the Lock level 1 is selected.

  - The OCx output signal is the same as the reference signal except for the rising edge,
    which is delayed relative to the reference rising edge.
  - The OCxN output signal is the opposite of the reference signal except for the rising
    edge, which is delayed relative to the reference falling edge

**Note** that calculated duty cycles apply to the reference signal (OCxREF) from
which outputs OCx and OCxN are generated. As dead time insertion is enabled the
duty cycle measured on OCx will be slightly lower.

The break Polarity is used at High level.

The TIM1 waveforms can be displayed using an oscilloscope.

#### <b>Notes</b>

 1. Whenever the application is using ITCM/DTCM memories (@0x0000000 / @0x20000000: not cacheable and only accessible
    by the Cortex M7 and the GPDMA/HPDMA), there is no need for cache maintenance.
    If the application needs to put DMA buffers in AXI SRAM (starting from @0x24000000), the user has to:
    - either define a non-cacheable region in the MPU and linker configuration file to locate DMA buffers
      (a proposed dma_buffer section is available from CMSIS Device linker template file and its size must
      be adapted to the application requirements)
    - or to ensure cache maintenance operations to ensure the cache coherence between the CPU and the DMAs.

	This is true also for any other data buffers accessed by the CPU and other masters (DMA2D, LTDC)
    The addresses and the size of cacheable buffers (shared between CPU and other masters)
    must be properly defined to be aligned to data cache line size (32 bytes) and of a size of being multiple
    of this cache line size.
    - Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
    - Please refer to the AN4839 "Level 1 cache on STM32F7 Series"

### <b>Keywords</b>

Timer, TIM, PWM, Signal, Duty cycle, Dead-time, Break-time, Break polarity, Oscilloscope.

### <b>Directory contents</b>

  - TIM/TIM_BreakAndDeadtime_Init/Boot/Inc/stm32h7rsxx_it.h        Interrupt handlers header file
  - TIM/TIM_BreakAndDeadtime_Init/Boot/Inc/main.h                  Header for main.c module
  - TIM/TIM_BreakAndDeadtime_Init/Boot/Inc/stm32_assert.h          Template file to include assert_failed function
  - TIM/TIM_BreakAndDeadtime_Init/Boot/Src/stm32h7rsxx_it.c        Interrupt handlers
  - TIM/TIM_BreakAndDeadtime_Init/Boot/Src/main.c                  Main program
  - TIM/TIM_BreakAndDeadtime_Init/Boot/Src/system_stm32h7rsxx.c    STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H7S3L8 Set-up

    - Connect the TIM1 pins to an oscilloscope to monitor the different waveforms:
      - TIM1_CH1  PE9: connected to pin 4 of CN10 connector
      - TIM1_CH1N PB13: connected to pin 5 of CN7 connector
      - TIM1_CH2  PE11: connected to pin 6 of CN10 connector
      - TIM1_CH2N PB0: connected to pin 31 of CN10 connector
      - TIM1_CH3  PE13: connected to pin 10 of CN10 connector
      - TIM1_CH3N PB1: connected to pin 7 of CN10 connector

    - Connect the TIM1 break to the GND. To generate a break event, switch this
      pin level from 0V to 3.3V.
      - TIM1_BKIN  PA6: connected to pin 12 of CN7 connector

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


