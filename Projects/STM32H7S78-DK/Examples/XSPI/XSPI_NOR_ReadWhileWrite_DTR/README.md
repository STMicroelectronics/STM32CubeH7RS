## <b>XSPI_NOR_ReadWhileWrite_DTR Example Description</b>

How to write and read data using XSPI NOR memory.

This example describes how to write and read data in Automatic polling mode in an XSPI
NOR memory and compare the result.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32H7S7xx Devices :

 - The SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 600 Mhz.
 - The template project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches. 

This is provided as template implementation that the User may integrate in his application in order to enhance the performance.

LED_GREEN is ON when the checked data is correct.

LED_RED is ON as soon as a comparison error occurs.

LED_RED toggles as soon as an error is returned by HAL API.

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
    must be properly defined to be aligned to data cache line size (32 bytes) and of a size of being multiple
    of this cache line size.
    Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
    Please refer to the AN4839 "Level 1 cache on STM32F7 Series"

### <b>Keywords</b>

Memory, XSPI, Read, Write, auto polling, NOR, DTR Mode

### <b>Directory contents</b>

  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Boot/Inc/stm32h7rsxx_hal_conf.h    HAL configuration file
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Boot/Inc/stm32h7rsxx_it.h          Interrupt handlers header file
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Boot/Inc/main.h                    Header for main.c module
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Boot/Src/stm32h7rsxx_it.c          Interrupt handlers
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Boot/Src/main.c                    Main program
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Boot/Src/stm32h7rsxx_hal_msp.c     HAL MSP module
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Boot/Src/system_stm32h7rsxx.c      STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This example has been tested with STMicroelectronics STM32H7S78-DK (MB1736)
    board and can be easily tailored to any other supported device
    and development board.

  - On STM32H7S78-DK board, the BOOT0 mechanical slide switch must be set to SW1.

  - User Option Bytes requirement (with STM32CubeProgrammer tool):

    - XSPI1_HSLV=1     I/O XSPIM_P1 High speed option enabled
    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled
    - VDDIO_HSLV=0     I/O speed optimized to operate at 3.3V

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
