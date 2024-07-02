## <b>RCC_UseHSI_PLLasSystemClock Example Description</b>

Modification of the PLL parameters in run time.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

In this example, the toggling frequency of the green LD1 depends on the system clock
frequency and, each time the USER push-button is pressed, the PLL switches between two configurations.
This make the LD1 blinking speed to highlight the system clock frequency changes.

In this example, after start-up, SYSCLK is configured to the max frequency using the PLL with
HSI as clock source.
Then, when pressing USER push-button, an automatic switch is done between PLL with HSI as clock source (SYSCLK
set to 8MHz) and PLL with HSI as clock source (SYSCLK set to 600MHz).
LD1 will toggle differently between the 2 configurations (quick toggle with SYSCLK configuration at 600MHz).

To detect a problem with PLL configuration, switch USE_TIMEOUT can be enabled. Then in case of issues,
LD1 will toggle every 1 second.

**Note:** "uwFrequency" variable can be added in LiveWatch to monitor the system clock frequency.

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

System, RCC, PLL, PLLCLK, SYSCLK, HSE, Clock, Oscillator, HSI

### <b>Directory contents</b>

  - RCC/RCC_UseHSI_PLLasSystemClock/Boot/Inc/stm32h7rsxx_it.h          Interrupt handlers header file
  - RCC/RCC_UseHSI_PLLasSystemClock/Boot/Inc/main.h                  Header for main.c module
  - RCC/RCC_UseHSI_PLLasSystemClock/Boot/Inc/stm32_assert.h          Template file to include assert_failed function
  - RCC/RCC_UseHSI_PLLasSystemClock/Boot/Src/stm32h7rsxx_it.c          Interrupt handlers
  - RCC/RCC_UseHSI_PLLasSystemClock/Boot/Src/main.c                  Main program
  - RCC/RCC_UseHSI_PLLasSystemClock/Boot/Src/system_stm32h7rsxx.c      STM32H7RSxx system source file


### <b>Hardware and Software environment</b> 

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.


### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

