## <b>UART_TwoBoards_ComDMAlinkedlist Example Description</b>

UART transmission (transmit/receive) in DMA mode using linkedlist between two boards.

    Board: NUCLEO-H7S3L8 (embeds a STM32H7S3L8Hx device)
    UART Instance : USART2
    Tx Pin: PD5
    Rx Pin: PD6

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

This example runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [regions 0, 1], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

Two identical boards are connected:

- Board 1: transmitting then receiving board
- Board 2: receiving then transmitting board

The user presses the User push-button on board 1.
Then, board 1 sends in DMA mode using linkedlist three message to board 2 that sends it back to
board 1 in DMA mode with linkedlist as well.
Finally, board 1 and 2 compare the received messages to that sent.
If the messages are the same, the test passes.

WARNING: as both boards do not behave the same way, "TRANSMITTER_BOARD" compilation
switch is defined in /Src/main.c and must be enabled
at compilation time before loading the executable in the board that first transmits
then receives.
The receiving then transmitting board needs to be loaded with an executable
software obtained with TRANSMITTER_BOARD disabled.

NUCLEO-H7S3L8 board LEDs are used to monitor the transfer status:

- While board 1 is waiting for the user to press the User push-button, its LD2 is
  blinking rapidly (100 ms period).
- While board 2 is waiting for the message from board 1, its LD2 is emitting
  a couple of flashes every half-second.
- When the test passes, LD1 on both boards is turned on, otherwise the test has failed.
- Once the transmission/reception processes are completed LD2 is turned on.
- If there is an initialization or transfer error, LED3 is turned on.

The UART is configured as follows:

    - BaudRate = 9600 baud
    - Word Length = 8 bits (8 data bits, no parity bit)
    - One Stop Bit
    - No parity
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

 3. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

 4. Whenever the application is using ITCM/DTCM memories (@0x0000000 / @0x20000000: not cacheable and only accessible
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
    Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
    Please refer to the AN4839 "Level 1 cache on STM32F7 Series"

### <b>Keywords</b>

Connectivity, UART/USART, baud rate, RS-232, full-duplex, DMA, parity, stop bit,
Transmitter, Receiver, Asynchronous

### <b>Directory contents</b>

  - UART/UART_TwoBoards_ComDMAlinkedlist/Appli/Inc/linked_list.h             Linked list header file
  - UART/UART_TwoBoards_ComDMAlinkedlist/Appli/Inc/main.h                    Main program header file
  - UART/UART_TwoBoards_ComDMAlinkedlist/Appli/Inc/stm32h7rsxx_hal_conf.h    HAL configuration file
  - UART/UART_TwoBoards_ComDMAlinkedlist/Appli/Inc/stm32h7rsxx_it.h          Interrupt handlers header file
  - UART/UART_TwoBoards_ComDMAlinkedlist/Appli/Inc/stm32h7rsxx_nucleo_conf.h BSP Configuration file
  - UART/UART_TwoBoards_ComDMAlinkedlist/Appli/Src/linked_list.c             Linked list source file
  - UART/UART_TwoBoards_ComDMAlinkedlist/Appli/Src/main.c                    Main program source file
  - UART/UART_TwoBoards_ComDMAlinkedlist/Appli/Src/stm32h7rsxx_hal_msp.c     HAL MSP module
  - UART/UART_TwoBoards_ComDMAlinkedlist/Appli/Src/stm32h7rsxx_it.c          Interrupt handlers source file
  - UART/UART_TwoBoards_ComDMAlinkedlist/Appli/Src/system_stm32h7rsxx.c      STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with two NUCLEO-H7S3L8 boards embedding
    a STM32H7S3L8Hx device and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

  - NUCLEO-H7S3L8 set-up
  
    - Connect a wire between 1st board PD5 (D53 pin 6 CN9) pin (Uart Tx) and 2nd board PD6 (D52 pin 4 CN9) pin (Uart Rx)
    - Connect a wire between 1st board PD6 (D52 pin 4 CN9) pin (Uart Rx) and 2nd board PD5 (D53 pin 6 CN9) pin (Uart Tx)
    - Connect 1st board GND to 2nd Board GND

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

