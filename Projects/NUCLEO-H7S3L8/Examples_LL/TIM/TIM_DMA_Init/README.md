
## <b>TIM_DMA_Init Example Description</b>

Use of the DMA with a timer update request 
to transfer data from memory to Timer Capture Compare Register 3 (TIMx_CCR3). This
example is based on the STM32H7RSxx TIM LL API. The peripheral initialization
uses LL unitary service functions for optimization purposes (performance and size).

The following configuration values are used in this example:

  - TIM1CLK = SystemClock
  - Counter repetition = 3
  - Prescaler = 0
  - TIM1 counter clock = SystemClock
  - SystemCoreClock is set to 600 MHz for STM32H7RSxx Devices.

The objective is to configure TIM1 channel 3 to generate PWM edge aligned
signal with a frequency equal to 17.57 KHz, and a variable duty cycle that
is changed by the DMA after a specific number of Update DMA request.

The number of this repetitive requests is defined by the TIM1 Repetition counter,
each 4 Update Requests, the TIM1 Channel 3 Duty Cycle changes to the next new
value defined by the aCCValue buffer.

The PWM waveform can be displayed using an oscilloscope.

Whenever a DMA transfer fails LD1 flashes with a frequency of 1 Hz.

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

2. PWM signal frequency value mentioned above is theoretical (obtained when
      the system clock frequency is exactly 600 MHz). Since the generated system
      clock frequency may vary from one board to another observed PWM signal
      frequency might be slightly different.

### <b>Keywords</b>

Timers, DMA, Circular DMA, PWM, Frequency, Duty Cycle, Waveform, Oscilloscope, Output, Signal

### <b>Directory contents</b>

  - TIM/TIM_DMA_Init/Boot/Inc/stm32h7rsxx_it.h        Interrupt handlers header file
  - TIM/TIM_DMA_Init/Boot/Inc/main.h                  Header for main.c module
  - TIM/TIM_DMA_Init/Boot/Inc/stm32_assert.h          Template file to include assert_failed function
  - TIM/TIM_DMA_Init/Boot/Src/stm32h7rsxx_it.c        Interrupt handlers
  - TIM/TIM_DMA_Init/Boot/Src/main.c                  Main program
  - TIM/TIM_DMA_Init/Boot/Src/system_stm32h7rsxx.c    STM32H7RSxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H7S3L8 Set-up
    - Connect the TIM1 pins to an oscilloscope to monitor the different waveforms:
      - TIM1_CH3 : PE.13 (PIN10 CN10)

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

