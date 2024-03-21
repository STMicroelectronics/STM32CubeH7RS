## <b>SPI_FullDuplex_ComDMA_Master Example Description</b>

Data buffer transmission/reception between two boards via SPI using DMA.

	Board: NUCLEO-H7S3L8 (embeds a STM32H7S3L8Hx device)
	CLK Pin: PA.05 (Pin 10 on CN7 connector)
	MISO Pin: PA.06 (Pin 12 on CN7 connector )
	MOSI Pin: PB.05 (Pin 14 on CN7 connector)

HAL architecture allows user to easily change code to move to Polling or IT
mode. To see others communication modes please check following examples:
- SPI\SPI_FullDuplex_ComPolling_Master and SPI\SPI_FullDuplex_ComPolling_Slave
- SPI\SPI_FullDuplex_ComIT_Master and SPI\SPI_FullDuplex_ComIT_Slave

This project runs from the external Flash memory. It is launched from a first boot stage 
and inherits from this boot project configuration (caches, MPU regions [region 0 and 1], 
system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment 
via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

The template project calls also SCB_EnableICache() and SCB_EnableDCache() functions 
in order to enable the Layer 1 Core Instruction and Data Caches. This is provided as 
template implementation that the User may integrate in his application in order to 
enhance the performance.

The SPI peripheral configuration is ensured by the HAL_SPI_Init() function.
This later is calling the HAL_SPI_MspInit()function which core is implementing
the configuration of the needed SPI resources according to the used hardware (CLOCK,
GPIO, DMA and NVIC). You may update this function to change SPI configuration.

The SPI communication is then initiated.
The HAL_SPI_TransmitReceive_DMA() function allows the reception and the
transmission of a predefined data buffer at the same time (Full Duplex Mode).

If the Master board is used, the project SPI_FullDuplex_ComDMA_Master must be used.
If the Slave board is used, the project SPI_FullDuplex_ComDMA_Slave must be used.

For this example the aTxBuffer is predefined and the aRxBuffer size is same as aTxBuffer.

In a first step after the user press the USER push-button, SPI Master starts the
communication by sending aTxBuffer and receiving aRxBuffer through
HAL_SPI_TransmitReceive_DMA(), at the same time SPI Slave transmits aTxBuffer
and receives aRxBuffer through HAL_SPI_TransmitReceive_DMA().
The callback functions (HAL_SPI_TxRxCpltCallback and HAL_SPI_ErrorCallbackand) update
the variable wTransferState used in the main function to check the transfer status.
Finally, aRxBuffer and aTxBuffer are compared through Buffercmp() in order to
check buffers correctness.

STM32 board's LEDs can be used to monitor the transfer status:

 - LD1 toggles quickly on master board waiting USER push-button to be pressed.
 - LD1 turns ON when the transmission process is complete.
 - LD2 turns ON when the reception process is complete.
 - LD3 turns ON when there is an error in transmission/reception process.

#### <b>Notes</b>

 1. You need to perform a reset on Slave board, then perform it on Master board
    to have the correct behaviour of this example.

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

Connectivity, SPI, Full-duplex, Interrupt, Transmission, Reception, Master, Slave, MISO, MOSI, DMA

### <b>Directory contents</b>

  - SPI/SPI_FullDuplex_ComDMA_Master/Appli/Inc/stm32h7rsxx_nucleo_conf.h   BSP configuration file
  - SPI/SPI_FullDuplex_ComDMA_Master/Appli/Inc/stm32h7rsxx_hal_conf.h      HAL configuration file
  - SPI/SPI_FullDuplex_ComDMA_Master/Appli/Inc/stm32h7rsxx_it.h            Interrupt handlers header file
  - SPI/SPI_FullDuplex_ComDMA_Master/Appli/Inc/main.h                      Header for main.c module
  - SPI/SPI_FullDuplex_ComDMA_Master/Appli/Src/stm32h7rsxx_it.c            Interrupt handlers
  - SPI/SPI_FullDuplex_ComDMA_Master/Appli/Src/main.c                      Main program
  - SPI/SPI_FullDuplex_ComDMA_Master/Appli/Src/system_stm32h7rsxx.c        stm32h7rsxx system source file
  - SPI/SPI_FullDuplex_ComDMA_Master/Appli/Src/stm32h7rsxx_hal_msp.c       HAL MSP file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

  - NUCLEO-H7S3L8 Set-up
    - Connect Master board PA.05 (Pin 10 on CN7 connector) to Slave Board PA.05 (Pin 10 on CN7 connector)
    - Connect Master board PA.06 (Pin 12 on CN7 connector ) to Slave Board PA.06 (Pin 12 on CN7 connector )
    - Connect Master board PB.05 (Pin 14 on CN7 connector) to Slave Board PB.05 (Pin 14 on CN7 connector)
    - Connect Master board GND  to Slave Board GND

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Rebuild all files from (master) sub-project Appli and load your images into memories: 
   This sub-project will first load Boot_XIP.hex in internal Flash, than load Appli part 
   in External memory available on NUCLEO-H7S3L8 board.
 - Rebuild all files from (Slave) sub-project Appli and load your images into memories: 
   This sub-project will first load Boot_XIP.hex in internal Flash, than load Appli part 
   in External memory available on NUCLEO-H7S3L8 board.

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