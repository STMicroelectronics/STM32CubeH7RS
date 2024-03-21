## <b>IWDG_RefreshUntilUserEvent_Init Example Description</b>

How to configure the IWDG peripheral to ensure periodical counter update and
generate an MCU IWDG reset when a USER push-button is pressed. The peripheral
is initialized with LL unitary service functions to optimize
for performance and size.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

Example Configuration:

Configure the IWDG (prescaler, counter) and enable it.

Infinite refresh of the IWDG down-counter done in the main loop.
LD1 is blinking fast & continuously.

Example Execution:

When USER push-button is pressed, the down-counter automatic refresh mechanism is
disable and thus, reset will occur. After a reset, when re-entering in the main,
RCC IWDG Reset Flag will be checked and if we are back from a IWDG reset the LD1
will be switch ON.

Waiting a new USER push-button pressed to re-activate the IWDG

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

System, IWDG, reload counter, MCU Reset

### <b>Directory contents</b>

  - IWDG/IWDG_RefreshUntilUserEvent_Init/Boot/Inc/stm32h7rsxx_it.h          Interrupt handlers header file
  - IWDG/IWDG_RefreshUntilUserEvent_Init/Boot/Inc/main.h                    Header for main.c module
  - IWDG/IWDG_RefreshUntilUserEvent_Init/Boot/Inc/stm32_assert.h            Template file to include assert_failed function
  - IWDG/IWDG_RefreshUntilUserEvent_Init/Boot/Src/stm32h7rsxx_it.c          Interrupt handlers
  - IWDG/IWDG_RefreshUntilUserEvent_Init/Boot/Src/main.c                    Main program
  - IWDG/IWDG_RefreshUntilUserEvent_Init/Boot/Src/system_stm32h7rsxx.c      STM32H7RSxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


