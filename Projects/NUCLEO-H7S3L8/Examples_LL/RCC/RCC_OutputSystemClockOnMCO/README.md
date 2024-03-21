## <b>RCC_OutputSystemClockOnMCO Example Description</b>

Configuration of MCO pin (PA8) to output the system clock.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

At start-up, USER push-button and MCO pin are configured. The program configures SYSCLK
to the max frequency using the PLL with HSI as clock source.

The signal on PA8 (pin 23 of CN12 connector) can be monitored with an oscilloscope
to check the different MCO configuration set at each USER push-button press.

Different configuration will be observed :

 - HSI frequency value divided by 1, hence around @64MHz.
 - HSE frequency value divided by 1, hence around @24MHz.
 - HSI48 frequency value divided by 1, hence around @48MHz.

When user press User push-button, a LD1 toggle is done to indicate a change in MCO config.

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

System, RCC, PLL, HSI, PLLCLK, SYSCLK, HSE, Clock, Oscillator

### <b>Directory contents</b>

  - RCC/RCC_OutputSystemClockOnMCO/Boot/Inc/stm32h7rsxx_it.h        Interrupt handlers header file
  - RCC/RCC_OutputSystemClockOnMCO/Boot/Inc/main.h                  Header for main.c module
  - RCC/RCC_OutputSystemClockOnMCO/Boot/Inc/stm32_assert.h          Template file to include assert_failed function
  - RCC/RCC_OutputSystemClockOnMCO/Boot/Src/stm32h7rsxx_it.c        Interrupt handlers
  - RCC/RCC_OutputSystemClockOnMCO/Boot/Src/main.c                  Main program
  - RCC/RCC_OutputSystemClockOnMCO/Boot/Src/system_stm32h7rsxx.c    STM32H7RSxx system source file


### <b>Hardware and Software environment</b> 

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H7S3L8 Set-up
    - Connect the MCO pin to an oscilloscope to monitor the different waveforms:
      - PA.08: connected to pin 23 of CN12 connector

### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
