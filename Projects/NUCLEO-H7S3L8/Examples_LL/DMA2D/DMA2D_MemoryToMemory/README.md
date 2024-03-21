## <b>DMA2D_MemoryToMemory Example Description</b>

- This example provides a description of how to configure DMA2D peripheral in Memory_to_Memory transfer mode.
  The example is based on the STM32H7xx DMA2D LL API. The peripheral initialization uses LL unitary service functions for optimization purposes
  (performance and size).

- The example transfers with DMA2D an internal SRAM static texture array aBufferInput[] of size 16x32
  and format ARGB4444 (16 bpp) to destination buffer in internal SRAM in array aBufferResult[].
  The number of bytes transferred is then 16x32x2=1024 bytes.

- Peripheral initialization done using LL unitary service functions
  for optimization purpose (performance and size).

- After DMA2D configuration, the data transfer is performed in polling mode and then the transferred
  data are checked in comparing the source and destination buffers aBufferInput[] and aBufferResult[].

- The project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
  the Layer 1 Core Instruction and Data Caches.


STM32H7S3L8-Nucleo board's LEDs can be used to monitor the transfer status:

 - [**LD1**]{style="color: GREEN"} is ON when the DMA2D transfer is complete and when the data are correctly transferred.
 - [**LD3**]{style="color: RED"} is blinking when there is a DMA2D transfer error or when data are not correctly transferred


### <b>Keywords</b>

Graphics, DMA2D, GPIO, RGB

### <b>Directory contents</b>

File                                                                     | Description
 ---                                                                     | ---
  DMA2D/DMA2D_MemoryToMemory/Boot/Src/main.c                             | Main program
  DMA2D/DMA2D_MemoryToMemory/Boot/Src/system_stm32h7rsxx.c               | STM32H7RSxx system clock configuration file
  DMA2D/DMA2D_MemoryToMemory/Boot/Src/stm32h7rsxx_it.c                   | Interrupt handlers
  DMA2D/DMA2D_MemoryToMemory/Boot/Inc/main.h                             | Main program header file
  DMA2D/DMA2D_MemoryToMemory/Boot/Inc/stm32_assert.h                     | Template file to include assert_failed function
  DMA2D/DMA2D_MemoryToMemory/Boot/Inc/stm32h7rsxx_it.h                   | Interrupt handlers header file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.
  - This example has been tested with NUCLEO-H7S3L8 board and can be easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files from sub-project Appli and load your images into memories: This sub-project will
   first load Boot_XIP.hex in internal Flash, than load Appli part in External memory available on
   NUCLEO-H7S3L8 board.
 - Run the example
:::
:::

