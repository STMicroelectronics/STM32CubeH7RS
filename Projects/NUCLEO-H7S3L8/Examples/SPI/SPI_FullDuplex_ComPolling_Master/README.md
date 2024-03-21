## <b>SPI_FullDuplex_ComPolling_Master Example Description</b>

Data buffer transmission/reception between two boards via SPI using Polling mode.

This example runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [regions 0, 1 and 2 here], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

       Board: NUCLEO-H7S3L8 (embeds a STM32H7S3L8Hx device)
       - CLK Pin: PA5 (pin 10 on CN7 connector)
       - MISO Pin: PA6 (pin 12 on CN7 connector)
       - MOSI Pin: PB5 (pin 14 on CN7 connector)

HAL architecture allows user to easily change code to move to IT or DMA mode. 
To see others communication modes please check following examples:
SPI\SPI_FullDuplex_ComDMA_Master and SPI\SPI_FullDuplex_ComDMA_Slave
SPI\SPI_FullDuplex_ComIT_Master and SPI\SPI_FullDuplex_ComIT_Slave

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 600 MHz.

The SPI peripheral configuration is ensured by the HAL_SPI_Init() function.
This later is calling the HAL_SPI_MspInit()function which core is implementing
the configuration of the needed SPI resources according to the used hardware (CLOCK & 
GPIO). You may update this function to change SPI configuration.

The SPI communication is then initiated.
The HAL_SPI_TransmitReceive() function allows the reception and the 
transmission of a predefined data buffer at the same time (Full Duplex Mode).
If the Master board is used, the project SPI_FullDuplex_ComPolling_Master must be used.
If the Slave board is used, the project SPI_FullDuplex_ComPolling_Slave must be used.

For this example the aTxBuffer is predefined and the aRxBuffer size is same as aTxBuffer.

In a first step after the user press the USER push-button, SPI Master starts the
communication by sending aTxBuffer and receiving aRxBuffer through 
HAL_SPI_TransmitReceive(), at the same time SPI Slave transmits aTxBuffer 
and receives aRxBuffer through HAL_SPI_TransmitReceive(). 
The end of this step is monitored through the HAL_SPI_GetState() function
result.
Finally, aRxBuffer and aTxBuffer are compared through Buffercmp() in order to 
check buffers correctness.  

#### <b>Notes</b>
Timeout is set to 5 seconds which means that if no communication occurs during 5 seconds,
      a timeout error is generated.

STM32 board's LEDs can be used to monitor the transfer status:

 - LD2 toggles quickly on master board waiting USER push-button to be pressed.
 - LD2 turns ON when the transmission process is complete.
 - LD2 turns ON when the reception process is complete.
 - LD3 turns ON when there is an error in transmission/reception process. 
 - LD3 toggles when there is a timeout error in transmission/reception process. 

#### <b>Notes</b>
 You need to perform a reset on Slave board, then perform it on Master board
      to have the correct behaviour of this example.


#### <b>Notes</b>
1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
2. The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

### <b>Keywords</b>

Connectivity, SPI, Full-duplex, Polling, Transmission, Reception, Master, Slave, MISO, MOSI

### <b>Directory contents</b> 

  - SPI/SPI_FullDuplex_ComPolling_Master/Appli/Inc/stm32h7rsxx_nucleo_conf.h               BSP configuration file
  - SPI/SPI_FullDuplex_ComPolling_Master/Appli/Inc/stm32h7rsxx_hal_conf.h                  HAL configuration file
  - SPI/SPI_FullDuplex_ComPolling_Master/Appli/Inc/stm32h7rsxx_it.h                        Interrupt handlers header file
  - SPI/SPI_FullDuplex_ComPolling_Master/Appli/Inc/main.h                                  Header for main.c module  
  - SPI/SPI_FullDuplex_ComPolling_Master/Appli/Src/stm32h7rsxx_it.c                        Interrupt handlers
  - SPI/SPI_FullDuplex_ComPolling_Master/Appli/Src/main.c                                  Main program
  - SPI/SPI_FullDuplex_ComPolling_Master/Appli/Src/system_stm32h7rsxx.c                    Stm32h7rsxx system source file
  - SPI/SPI_FullDuplex_ComPolling_Master/Appli/Src/stm32h7rsxx_hal_msp.c                   HAL MSP file

### <b>Hardware and Software environment</b> 

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

  - NUCLEO-H7S3L8 Set-up
    - Connect Master board PA5 (pin 10 on CN7 connector) to Slave Board PA5 (pin 10 on CN7 connector)
    - Connect Master board PA6 (pin 12 on CN7 connector) to Slave Board PA6 (pin 12 on CN7 connector)
    - Connect Master board PB5 (pin 14 on CN7 connector) to Slave Board PB5 (pin 14 on CN7 connector)
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