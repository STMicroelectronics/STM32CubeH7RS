## <b>UART_WakeUpFromStopUsingFIFO Example Description</b>

Configuration of an UART to wake up the MCU from Stop mode with a FIFO level
when a given stimulus is received.

    Board: NUCLEO-H7S3L8 (embeds a STM32H7S3L8Hx device)
    Tx Pin: PD8
    Rx Pin: PD9

LD1 is ON when MCU is not in STOP mode and turn off when MCU is in STOP mode.
If the test fails or if there is an initialization or transfer error, LD1
transmits a sequence of three dots, three dashes, three dots.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

This example runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [regions 0, 1], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

In the first part of the example, the USART3 is configured for
reception with Rx FIFO threshold set to one 1 QUARTER and the CPU enters into
Stop Mode. The UART FIFO threshold interrupt is enabled and, by default,
the UART FIFO full is disabled. The user has to send 2 bytes from the
HyperTerminal. After 2 received bytes, the UART FIFO will reach the quarter
level which will generate an interrupt and wake up the CPU.

In the second part of the example, the USART3 is configured for
reception with FIFO threshold interrupt disabled and FIFO full enabled.
Then the CPU goes to stop mode. The user has to send 8 bytes (size of the UART
FIFO) from the HyperTerminal. After 8 received bytes, the UART FIFO will be
full, which will generate an interrupt and wake up the CPU.

The USART3 is configured as follows:

    - BaudRate = 115200 baud
    - Word Length = 8 Bits (7 data bit + 1 parity bit)
    - One Stop Bit
    - Odd parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Reception and transmission are enabled in the time

#### <b>Notes</b>

 1. When the parity is enabled, the computed parity is inserted at the MSB
    position of the transmitted data.

 2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 3. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

 4. Whenever the application is using ITCM/DTCM memories (@0x0000000 / @0x20000000: not cacheable and only accessible
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

Connectivity, UART/USART, baud rate, Wake Up, Low Power, HyperTerminal, full-duplex, Interrupt
Transmitter, Receiver, Asynchronous

### <b>Directory contents</b>

  - UART/UART_WakeUpFromStopUsingFIFO/Appli/Inc/stm32h7rsxx_nucleo_conf.h     BSP configuration file
  - UART/UART_WakeUpFromStopUsingFIFO/Appli/Inc/stm32h7rsxx_hal_conf.h    HAL configuration file
  - UART/UART_WakeUpFromStopUsingFIFO/Appli/Inc/stm32h7rsxx_it.h          DMA interrupt handlers header file
  - UART/UART_WakeUpFromStopUsingFIFO/Appli/Inc/main.h                   Header for main.c module
  - UART/UART_WakeUpFromStopUsingFIFO/Appli/Src/stm32h7rsxx_it.c          DMA interrupt handlers
  - UART/UART_WakeUpFromStopUsingFIFO/Appli/Src/main.c                   Main program
  - UART/UART_WakeUpFromStopUsingFIFO/Appli/Src/stm32h7rsxx_hal_msp.c     HAL MSP module
  - UART/UART_WakeUpFromStopUsingFIFO/Appli/Src/system_stm32h7rsxx.c      STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

  - NUCLEO-H7S3L8 Set-up
      Connect a USB cable between the ST-Link USB connector
      and PC to display data on the HyperTerminal.

  - Hyperterminal configuration:
    - Data Length = 7 Bits
    - One Stop Bit
    - Odd parity
    - BaudRate = 115200 baud
    - Flow control: None

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

**EWARM** and **MDK-ARM**:

 - Open your preferred toolchain
 - Rebuild all files from sub-project Appli and load your images into memories: This sub-project will first load Boot_XIP.hex in internal Flash,
   than load Appli part in External memory available on NUCLEO-H7S3L8 board.
 - Run the example

**CubeIDE**:

 - Compile the example/application; the elf file is required to configure the debug profile (the "active configuration" must be "debug", else only assembly debug is available)
 - Open the menu [Run]->[Debug configuration] and double click on  [STM32 C/C++ Application] (it creates a default debug configuration for the current project selected)
 - In [Debugger] tab, section "External  loaders" add the external loader corresponding to your Board/Memory as described below:
 - In "External loaders" section, click on [Add]
 - Select the loader among the available list (**MX25UW25645G_NUCLEO-H7S3L8.stldr** or **MX66UW1G45G_STM32H7S78-DK.stldr**)
 - Option "Enabled" checked and Option "Initialize" unchecked
 - In "Misc" section, uncheck the option "Verify flash download"
 - In [Startup] tab, section "Load Image and Symbols":
   - Click on [Add]
   - If your project contains a boot project:
     - click on "Project" and then select the boot project.
     - click on Build configuration and select "Use active".
     - then select the following options:
       - "Perform build" checked.
       - "Download" checked.
       - "Load symbols" unchecked.
   - If your project doesn't contain a boot project:
     - click on [File System] and select the Boot HEX file corresponding to your board

        Boot_XIP.hex can be found in folder [Binary] on each Template_XIP project

        You may need to force the capability to select a .hex file by typing " * " + pressing the "Enter" key in the file name dialog

     - then select the following options:
       - "Download"      checked.
       - "Load symbols" unchecked.
       - Click Ok
     - Back in the in the [Startup] tab, move down the boot project for it to be in second position
 - Our debug configuration is ready to be used.

