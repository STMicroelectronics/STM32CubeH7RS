## <b>Templates_Board Example Description</b>

This project provides a reference template for the NUCLEO-H7S3L8 board based on the STM32Cube HAL API and the
BSP drivers that can be used to build any firmware application to execute from external Flash (Sub-project Appli),
It boots from internal Flash and jumps to the application code in external Flash (Sub-project Boot).

It was created from STM32CubeMX using the "Start My project from ST board" feature.

This template shows how to use the BSP drivers for the leds, user push-button and VCP COM port of the NUCLEO-H7S3L8
board thanks to BSP resources initialization and demonstration codes:

  - BSP Leds:  LED_GREEN, LED_BLUE and LED_RED are initialized and switched on.

  - BSP Push-button: BUTTON_USER is initialized in interrupt mode and each key-press is detected through
    the corresponding EXTI13_IRQHandler(). All leds are toggled at each key press.

  - BSP COM:  Virtual COM port associated to ST-LINK is initialized and used to redirect the C library printf().
    As an example it outputs a welcome message on a terminal.
    The USART is configured as follows:
      - Baudrate = 115200 bps
      - Word length = 8 bits (7-data bit + 1 stop bit)
      - One stop bit
      - No parity
      - Hardware flow control disabled (RTS and CTS signals)

At the beginning of the main program, the HAL_Init() function is called to initialize the systick used as 1ms HAL timebase.

This project runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).

The template project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

 3. Whenever the application is using ITCM/DTCM memories (@0x0000000 / @0x20000000: not cacheable and only accessible
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

Reference, Template, Start, Board, BSP

### <b>Directory contents</b>

File | Description
 --- | ---
  Templates_Board/Appli/Inc/main.h                      |  Header for main.c module
  Templates_Board/Appli/Inc/stm32h7rsxx_hal_conf.h      |  HAL Configuration file
  Templates_Board/Appli/Inc/stm32h7rsxx_it.h            |  Interrupt handlers header file
  Templates_Board/Appli/Inc/stm32h7rsxx_nucleo_conf.h   |  BSP Configuration file
  Templates_Board/Appli/Src/main.c                      |  Main program
  Templates_Board/Appli/Src/stm32h7rsxx_hal_msp.c       |  HAL MSP module
  Templates_Board/Appli/Src/stm32h7rsxx_it.c            |  Interrupt handlers
  Templates_Board/Appli/Src/system_stm32h7rsxx.c        |  STM32H7RSxx system source file
  Templates_Board/Boot/Inc/main.h                       |  Header for main.c module
  Templates_Board/Boot/Inc/stm32h7rsxx_hal_conf.h       |  HAL Configuration file
  Templates_Board/Boot/Inc/stm32h7rsxx_it.h             |  Interrupt handlers header file
  Templates_Board/Boot/Src/main.c                       |  Main program
  Templates_Board/Boot/Src/stm32h7rsxx_hal_msp.c        |  HAL MSP module
  Templates_Board/Boot/Src/stm32h7rsxx_it.c             |  Interrupt handlers
  Templates_Board/Boot/Src/system_stm32h7rsxx.c         |  STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This template runs on STM32H7S3xx devices.

  - This template has been tested with STMicroelectronics NUCLEO-H7S3L8
    boards and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

#### <b>IAR</b>
  1. Open your toolchain
  2. Open Multi-projects workspace file Project.eww
  3. Select first "Template_Board_Boot" workspace
  4. Rebuild all files from sub-project Boot

>
>  If no modification is done on boot project, steps 3 and 4 could be skipped.

  5. Select then "Template_Board_Appli" workspace
  6. Rebuild all files from sub-project Appli and load your images into memories: This sub-project
     will first load the Appli part in External memory available on NUCLEO-H7S3L8 board, then load
     the Template_Board_Boot.hex in internal Flash.
  7. Run the example


#### <b>MDK-ARM</b>
  1. Open your toolchain
  2. Open Multi-projects workspace file Project.uvmpw
  3. Select first "Template_Board_Boot" workspace
  4. Rebuild all files from sub-project Boot

>
>  If no modification is done on boot project, steps 3 and 4 could be skipped.

  5. Select then "Template_Board_Appli" workspace
  6. Rebuild all files from sub-project Appli and load your images into memories: This sub-project
     will first load the Appli part in External memory available on NUCLEO-H7S3L8 board, then load
     the Template_Board_Boot.hex in internal Flash.
  7. Run the example


#### <b>STM32CubeIDE</b>
  1. Open your toolchain
  2. Open Multi-projects workspace file .project
  3. Select first "Template_Board_Boot" workspace
  4. Build the project

>
> - If the project is not compiled, Appli debugging will manage its compilation
>   for debugging
> - With the debug icon select the configuration "Template_Board_Boot Debug".
>   This operation loads the boot in internal Flash.

  5. Select the "Template_Board_Appli" project
  6. Build the project
  7. With the Debug icon select the configuration "Template_Board_Appli Debug". This sub-project
     will first load the Boot binary in internal Flash, then load the Appli binary in
     External memory available on NUCLEO-H7S3L8 board
  8. Run the example
