## <b>GPIO_InfiniteLedToggling_Init Example Description</b>

How to configure and use GPIOs to toggle the on-board user LEDs
every 250 ms. This example is based on the STM32H7RSxx LL API. The peripheral
is initialized with LL initialization function to demonstrate LL init usage.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

PD.10 IO (configured in output pushpull mode) toggles in a forever loop.
On NUCLEO-H7S3L8 board this IO is connected to LD1.

In this example, HCLK is configured at 600 MHz.

### <b>Keywords</b>

System, GPIO, Output, No pull, Speed, Push-pull, Toggle

### <b>Directory contents</b>

File | Description
 --- | ---
GPIO/GPIO_InfiniteLedToggling_Init/Boot/Inc/stm32h7rsxx_it.h        |     Interrupt handlers header file
GPIO/GPIO_InfiniteLedToggling_Init/Boot/Inc/main.h                  |     Header for main.c module
GPIO/GPIO_InfiniteLedToggling_Init/Boot/Inc/stm32_assert.h          |     Template file to include assert_failed function
GPIO/GPIO_InfiniteLedToggling_Init/Boot/Src/stm32h7rsxx_it.c        |     Interrupt handlers
GPIO/GPIO_InfiniteLedToggling_Init/Boot/Src/main.c                  |     Main program
GPIO/GPIO_InfiniteLedToggling_Init/Boot/Src/system_stm32h7rsxx.c    |     STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files from sub-project Appli and load your images into memories: This sub-project will first load Boot_XIP.hex in internal Flash,
   than load Appli part in External memory available on NUCLEO-H7S3L8 board.
 - Run the example


