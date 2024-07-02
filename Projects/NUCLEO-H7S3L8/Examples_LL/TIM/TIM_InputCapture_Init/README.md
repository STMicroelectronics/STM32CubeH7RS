## <b>TIM_InputCapture_Init Example Description</b>

Use of the TIM peripheral to measure a periodic signal frequency
provided either by an external signal generator or by
another timer instance. This example is based on the STM32H7RSxx TIM
LL API. The peripheral initialization uses LL unitary service functions
for optimization purposes (performance and size).

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

TIM1_CH1 is configured in input capture mode. The TIM1CLK frequency is set to
its maximum value (Prescaler is /2 * 2 = 1) in order to get the best possible resolution.
So the TIM1 counter clock is SystemCoreClock/2.

SystemCoreClock is set to 600 MHz for STM32H7RSxx Devices.

The "uwMeasuredFrequency" variable contains the measured signal frequency.
It is calculated within the capture/compare 1 interrupt service routine.

The minimum frequency value to measure is TIM1 counter clock / TIMx_CCR1 MAX
                                              = 300 MHz / 65535

Due to TIM1 interrupt routine processing time (around 4us), the maximum
frequency value to measure is around 700 KHz.

TIM3_CH1 is configured to generate a PWM signal.  USER push-button can be used to
change the frequency of this signal from 6 kHz up to 24 kHz by steps of 2 kHz.
It is then possible to run this example without a signal generator by connecting
TIM3_CH1 to TIM1_CH1.

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

Timer, TIM, Frequency, Input, Capture, External Signal, Measurement

### <b>Directory contents</b>

  - TIM/TIM_InputCapture_Init/Boot/Inc/stm32h7rsxx_it.h        Interrupt handlers header file
  - TIM/TIM_InputCapture_Init/Boot/Inc/main.h                  Header for main.c module
  - TIM/TIM_InputCapture_Init/Boot/Inc/stm32_assert.h          Template file to include assert_failed function
  - TIM/TIM_InputCapture_Init/Boot/Src/stm32h7rsxx_it.c        Interrupt handlers
  - TIM/TIM_InputCapture_Init/Boot/Src/main.c                  Main program
  - TIM/TIM_InputCapture_Init/Boot/Src/system_stm32h7rsxx.c    STM32H7RSxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H7S3L8 Set-up
    - When no signal generator is used TIM3_CH1 can be connected to TIM1_CH1:
      - TIM1_CH1  PE9: connected to pin 4 of CN10 connector
      - TIM3_CH1  PB4: connected to pin 19 of CN7 connector

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

