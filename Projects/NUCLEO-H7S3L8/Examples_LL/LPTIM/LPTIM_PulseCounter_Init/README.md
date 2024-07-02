## <b>LPTIM_PulseCounter_Init Example Description</b>

How to use the LPTIM peripheral in counter mode to generate a PWM output signal 
and update its duty cycle. This example is based on the STM32H7RSxx
LPTIM LL API. The peripheral is initialized with LL initialization 
function to demonstrate LL init usage.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

To reduce power consumption, MCU enters stop mode after starting counting. Each
time the counter reaches the maximum value (Period/Autoreload), an interruption
is generated, the MCU is woken up from stop mode and LD1 toggles the last state.

In this example Period value is set to 1000, so each time the counter counts
(1000 + 1) rising edges on LPTIM1_IN1 pin, an interrupt is generated and LD1
toggles. If the external function generator is set to provide a square waveform at 1Khz,
the led will toggle each second.

In this example the internal clock provided to the LPTIM1 is LSI (32 kHz),
so the external input is sampled with LSI clock. In order not to miss any event,
the frequency of the changes on the external Input1 signal should never exceed the
frequency of the internal clock provided to the LPTIM1 (LSI for the
present example).

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
    Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
    Please refer to the AN4839 "Level 1 cache on STM32F7 Series"


### <b>Keywords</b>

Timer, Low Power, Pulse Counter, Stop mode, Interrupt

### <b>Directory contents</b>

  - LPTIM/LPTIM_PulseCounter_Init/Boot/Inc/stm32h7rsxx_it.h        Interrupt handlers header file
  - LPTIM/LPTIM_PulseCounter_Init/Boot/Inc/main.h                  Header for main.c module
  - LPTIM/LPTIM_PulseCounter_Init/Boot/Inc/stm32_assert.h          Template file to include assert_failed function
  - LPTIM/LPTIM_PulseCounter_Init/Boot/Src/stm32h7rsxx_it.c        Interrupt handlers
  - LPTIM/LPTIM_PulseCounter_Init/Boot/Src/main.c                  Main program
  - LPTIM/LPTIM_PulseCounter_Init/Boot/Src/system_stm32h7rsxx.c    STM32H7RSxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.
    
  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.
    
  - Connect a square waveform generator to PG12 (connector CN11 pin 65).
    If the frequency of the signal is 1 kHz, LD1 toggles every second.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

