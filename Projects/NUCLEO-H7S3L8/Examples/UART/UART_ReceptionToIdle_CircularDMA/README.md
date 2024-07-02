## <b>UART_ReceptionToIdle_CircularDMA Example Description</b>

How to use the HAL UART API for reception to IDLE event in circular DMA mode.


- Board: NUCLEO-H7S3L8 (embeds a STM32H7S3L8Hx device)
- UART Instance : USART1
- Tx Pin: PB14
- Rx Pin: PA10

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

This example runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [regions 0, 1], system clock at 600 MHz and external memory interface at the highest speed).

Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

The UART peripheral configuration is ensured by the HAL_UART_Init() function.

This later is calling the HAL_UART_MspInit() function which core is implementing
the configuration of the needed UART resources according to the used hardware (CLOCK,
GPIO, DMA and NVIC). You may update this function to change UART configuration.

In this example, a DMA channel is associated to USART1 RX (Circular mode).

The UART/Hyperterminal communication is then initiated (Board is sending an information message,
inviting user to enter characters on Terminal).

Application then starts the Reception To Idle process, using dedicated HAL UART API :

    - HAL_UARTEx_ReceiveToIdle_DMA()

The HAL_UARTEx_ReceiveToIdle_DMA() function allows to handle reception of Data from Hyperterminal
using DMA and notify application of already received data while the reception is still ongoing.

Received characters are handled by DMA and are stored in the user aRXBufferUser buffer.

Notifications to application, that some data are available in reception buffer, are done
through the execution of a user callback : HAL_UARTEx_RxEventCallback().

This callback will be executed when any of following events occurs :
    - HT (Half Transfer) : Half of Rx buffer is filled)
    - TC (Transfer Complete) : Rx buffer is full.
      (In case of Circular DMA, reception could go on, and next reception data will be stored
      in index 0 of reception buffer by DMA).
    - Idle Event on Rx line : Triggered when RX line has been in idle state (normally high state)
      for 1 frame time, after last received byte.

When any of the HT, TC or Idle event occurs, HAL_UARTEx_RxEventCallback() is called,
and provides (as callback parameter) the index in user buffer until which, received data have been stored.

Example : case of a reception of 22 bytes before Idle event occurs, using Circular DMA and a Rx buffer
of size of 20 bytes.
    - User calls HAL_UARTEx_ReceiveToIdle_DMA() providing buffer address and buffer size (20)
    - HAL_UARTEx_RxEventCallback() will be executed on HT DMA event with Size = 10
      Data in user Rx buffer could be retrieved by application from index 0 to 9
    - HAL_UARTEx_RxEventCallback() will be executed on TC DMA event with Size = 20
      New data in user Rx buffer could be retrieved by application from index 10 to 19
    - HAL_UARTEx_RxEventCallback() will be executed after IDLE event occurs with Size = 2
      New data in user Rx buffer could be retrieved by application from index 0 to 1

In this example, provided HAL_UARTEx_RxEventCallback callback implementation example, aims to
echo received characters to HyperTerminal.

The UART instance (USART1) is configured as follows:
    - BaudRate = 115200 baud
    - Word Length = 8 bits (8 data bits, no parity bit)
    - One Stop Bit
    - Hardware flow control disabled (RTS and CTS signals)
    - Reception and transmission are enabled

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

Connectivity, UART/USART, baud rate, RS-232, full-duplex, DMA, parity, stop bit, Receiver, Asynchronous

### <b>Directory contents</b>

  - UART/UART_ReceptionToIdle_CircularDMA/Appli/Inc/stm32h7rsxx_nucleo_conf.h     BSP configuration file
  - UART/UART_ReceptionToIdle_CircularDMA/Appli/Inc/stm32h7rsxx_hal_conf.h        HAL configuration file
  - UART/UART_ReceptionToIdle_CircularDMA/Appli/Inc/stm32h7rsxx_it.h              DMA interrupt handlers header file
  - UART/UART_ReceptionToIdle_CircularDMA/Appli/Inc/main.h                        Header for main.c module
  - UART/UART_ReceptionToIdle_CircularDMA/Appli/Src/stm32h7rsxx_it.c              DMA interrupt handlers
  - UART/UART_ReceptionToIdle_CircularDMA/Appli/Src/main.c                        Main program
  - UART/UART_ReceptionToIdle_CircularDMA/Appli/Src/stm32h7rsxx_hal_msp.c         HAL MSP module
  - UART/UART_ReceptionToIdle_CircularDMA/Appli/Src/system_stm32h7rsxx.c          STM32H7RSxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.
  - This example has been tested with one NUCLEO-H7S3L8 board embedding
    a STM32H7S3L8Hx device and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

  - NUCLEO-H7S3L8 Set-up
     - Connect ST-Link cable to the PC USB port to send/receive data between PC and board.
       A virtual COM port will then appear in the HyperTerminal.

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