## <b>XSPI_NOR_AutoPolling_DTR Example Description</b>

How to use an XSPI NOR memory in Automatic polling mode.

This example describes how to write and read data in Automatic polling mode in an XSPI 
NOR memory and compare the result.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.  
The SystemClock_Config() function is used to configure the system clock for STM32H7S7xx Devices :  
- The SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 600 MHz.
- The template project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable 
the Layer 1 Core Instruction and Data Caches. This is provided as template implementation that the User may
integrate in his application in order to enhance the performance.

LED_GREEN LED is ON when the checked data is correct.  
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

### <b>Keywords</b>

Memory, XSPI, Read, Write, auto polling, NOR, DTR Mode

### <b>Directory contents</b>

  - XSPI/XSPI_NOR_AutoPolling_DTR/Boot/Inc/stm32h7rsxx_hal_conf.h    HAL configuration file
  - XSPI/XSPI_NOR_AutoPolling_DTR/Boot/Inc/stm32h7rsxx_it.h          Interrupt handlers header file
  - XSPI/XSPI_NOR_AutoPolling_DTR/Boot/Inc/main.h                    Header for main.c module
  - XSPI/XSPI_NOR_AutoPolling_DTR/Boot/Src/stm32h7rsxx_it.c          Interrupt handlers
  - XSPI/XSPI_NOR_AutoPolling_DTR/Boot/Src/main.c                    Main program
  - XSPI/XSPI_NOR_AutoPolling_DTR/Boot/Src/stm32h7rsxx_hal_msp.c     HAL MSP module
  - XSPI/XSPI_NOR_AutoPolling_DTR/Boot/Src/system_stm32h7rsxx.c      STM32H7RSxx system source file

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
 
