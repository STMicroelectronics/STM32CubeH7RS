## <b>USART_SlaveMode_DMA Example Description</b>

This example describes an USART-SPI communication (transmit/receive) with DMA between two
boards where the USART is configured as a slave.

Boards: NUCLEO-H7S3L8 (embeds a STM32H7S3L8Hx device)


    Two identical boards are connected as shown on the picture above.
    Board 1: SPI Master board
    Board 2: USART Slave board

In a first step, after the user press on the User push-button, Master (SPI) starts
the communication by sending aTxBuffer and receiving aRxBuffer through
HAL_SPI_TransmitReceive(), at the same time Slave (USART) transmits aTxBuffer
and receives aRxBuffer through HAL_USART_TransmitReceive().
Finally, aRxBuffer and aTxBuffer are compared through Buffercmp() in order to
check buffers correctness.

WARNING: as both boards do not behave the same way, "SLAVE_BOARD" compilation
switch is defined in Inc/main.h and must be enabled at compilation time before
loading the executable in the USART slave board.
The SPI master board needs to be loaded with an executable software obtained
with SLAVE_BOARD disabled.
if you want to repeat the execution of the example, you should reset the master board
first else the execution will fail and a red led will be on.

NUCLEO-H7S3L8 board LEDs are used to monitor the transfer status:

- While master board is waiting for the user to press the User push-button, its
  LD2 is blinking rapidly (100 ms period).
- While slave board is waiting for the reception of the buffer, its
  LD2 is blinking rapidly (100 ms period).
- When the transmission/reception processes starts, LD2 is turned off.
- When the test passes, LD1 on both boards is turned on, otherwise the
  test has failed.
- If there is an initialization or transfer error, LD3 is turned on.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

This project runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

The HCLK for AHB peripherals to run at 600 MHz.
The APB clock dividers APB1, APB2, APB3 peripherals to run at 600 MHz.

**Note:** USARTx/SPIx instances used and associated resources can be updated in
"main.h" file depending hardware configuration used.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate
    delay (in milliseconds) based on variable incremented in SysTick ISR. This
    implies that if HAL_Delay() is called from a peripheral ISR process, then
    the SysTick interrupt must have higher priority (numerically lower) than
    the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use
    HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to
    1 millisecond to have correct HAL operation.

 3. It is highly recommended to set the SPI parameter "MasterKeepIOState" to "SPI_MASTER_KEEP_IO_STATE_ENABLE"
    at init time. This configuration allows to avoid glitches on SPI lines especially the clock.
    Glitches that could lead to Master/Slave desynchronization. This config allows SPI Master to keep
    control on output state even if it is disabled.

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
    - Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
    - Please refer to the AN4839 "Level 1 cache on STM32F7 Series"

### <b>Keywords</b>

Connectivity, UART/USART, SPI interface, full-duplex, Polling, Receiver, Transmitter, Synchronous, Slave, DMA

### <b>Directory contents</b> 

  - USART/USART_SlaveMode_DMA/Appli/Inc/stm32h7rsxx_hal_conf.h     HAL configuration file
  - USART/USART_SlaveMode_DMA/Appli/Inc/stm32h7rsxx_it.h           interrupt handlers header file
  - USART/USART_SlaveMode_DMA/Appli/Inc/main.h                     Header for main.c module
  - USART/USART_SlaveMode_DMA/Appli/Inc/stm32h7rsxx_nucleo_conf.h  STM32H7RSxx_Nucleo board configuration file
  - USART/USART_SlaveMode_DMA/Appli/Src/stm32h7rsxx_it.c           interrupt handlers
  - USART/USART_SlaveMode_DMA/Appli/Src/main.c                     Main program
  - USART/USART_SlaveMode_DMA/Appli/Src/stm32h7rsxx_hal_msp.c      HAL MSP module
  - USART/USART_SlaveMode_DMA/Appli/Src/system_stm32h7rsxx.c       STM32H7RSxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with two NUCLEO-H7S3L8 boards embedding
    a STM32H7S3L8Hx device and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

  - NUCLEO-H7S3L8 set-up
    - Connect Master board PA05 (CN7 , PIN 10) (SCK) to Slave Board PA08 (CN12, PIN 23) (USART CK)
    - Connect Master board PA06 (CN7 , PIN 12) (MISO) to Slave Board PB14 (CN12, PIN 28) (USART TX)
    - Connect Master board PB05 (CN7 , PIN 14) (MOSI) to Slave Board PA10 (CN12, PIN 33) (USART RX)
    - Connect Master board GND to Slave Board GND

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Rebuild all files and load your image into target memory
   - Uncomment "#define SLAVE_BOARD" and load the project in Slave Board
   - Comment "#define SLAVE_BOARD" and load the project in Master Board

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

