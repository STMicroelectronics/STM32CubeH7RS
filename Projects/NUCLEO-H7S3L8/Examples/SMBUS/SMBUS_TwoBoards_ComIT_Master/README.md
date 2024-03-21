## <b>SMBUS_TwoBoards_ComIT_Master Example Description</b>
 
How to handle SMBUS data buffer transmission/reception between two boards,
in interrupt mode.
5
This example runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [regions 0, 1 and 2 here], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

    Board: NUCLEO-H7S3L8 (embeds a STM32H7S3L8Hx device)
    - SCL Pin: PF1 (CN9, D69 pin 19)
    - SDA Pin: PF0 (CN9, D68 pin 21)

     _________________________                       _________________________
    |           ______________|                     |______________           |
    |          |I2C2          |                     |          I2C2|          |
    |          |              |                     |              |          |
    |          |          SCL |_____________________| SCL          |          |
    |          |              |                     |              |          |
    |          |              |                     |              |          |
    |          |              |                     |              |          |
    |          |          SDA |_____________________| SDA          |          |
    |          |              |                     |              |          |
    |          |______________|                     |______________|          |
    |                         |                     |                         |
    |                      GND|_____________________|GND                      |
    |_STM32_Board 1___________|                     |_STM32_Board 2___________|


At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

The SMBUS peripheral configuration is ensured by HAL_SMBUS_Init() function which calls HAL_SMBUS_MspInit() function.
This HAL_SMBUS_MspInit() function allows the user to configure the needed SMBUS resources according to the used
hardware (CLOCK,GPIO).

The SMBUS communication is then initiated.
The project is split in two parts: the Master Board and the Slave Board

- Master Board
  The HAL_SMBUS_Master_Receive_IT() and the HAL_SMBUS_Master_Transmit_IT() functions
  allow respectively the reception and the transmission of a predefined data buffer
  and packet error checking (PEC) byte in Master mode using interrupt.

- Slave Board
  The HAL_SMBUS_Slave_Receive_IT() and the HAL_SMBUS_Slave_Transmit_IT() functions
  allow respectively the reception and the transmission of a predefined data buffer
  and packet error checking (PEC) byte in Slave mode using interrupt.

If the Master board is used, the project SMBUS_TwoBoards_ComIT_Master must be used.

If the Slave board is used, the project SMBUS_TwoBoards_ComIT_Slave must be used.

For this example the aTxBuffer is predefined and the aRxBuffer size is same as aTxBuffer.

In a first step after the user press the User push-button on the Master Board,
SMBUS Master starts the communication by sending aTxBuffer through HAL_SMBUS_Master_Transmit_IT()
to SMBUS Slave which receives aRxBuffer through HAL_SMBUS_Slave_Receive_IT().
The second step starts when the user press the User push-button on the Master Board,
the SMBUS Slave sends aTxBuffer through HAL_SMBUS_Slave_Transmit_IT()
to the SMBUS Master which receives aRxBuffer through HAL_SMBUS_Master_Receive_IT().
Finally, aTxBuffer and aRxBuffer are compared through Buffercmp() in order to
check buffers correctness.

NUCLEO-H7S3L8's LEDs can be used to monitor the transfer status:

 - LD2 toggles quickly on master board waiting User push-button to be pressed.
 - LD2 turns OFF as soon as the push-button is pressed.
 - LD1 is ON when the transmission process is complete.
 - LD2 toggles quickly on master board waiting User push-button to be pressed.
 - LD2 turns OFF as soon as the push-button is pressed.
 - LD1 is OFF when the reception process is complete.
 - LD3 is ON when there is an error in transmission/reception process.

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
      (a proposed dma_buffer section is available from CMSIS Device linker template file and its size must
      be adapted to the application requirements)
    - or to ensure cache maintenance operations to ensure the cache coherence between the CPU and the DMAs.

    This is true also for any other data buffers accessed by the CPU and other masters (DMA2D, LTDC)
    The addresses and the size of cacheable buffers (shared between CPU and other masters)
    must be properly defined to be aligned to data cache line size (32 bytes) and of a size of being multiple
    of this cache line size.
    - Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
    - Please refer to the AN4839 "Level 1 cache on STM32F7 Series"

### <b>Keywords</b>

Connectivity, SMBUS, Timing, half-duplex, Interrupt, Master, Slave, Transmitter, Receiver

### <b>Directory contents</b>

  - SMBUS/SMBUS_TwoBoards_ComIT_Master/Appli/Inc/stm32h7rsxx_nucleo_conf.h             BSP configuration file
  - SMBUS/SMBUS_TwoBoards_ComIT_Master/Appli/Src/main.c                                Main program
  - SMBUS/SMBUS_TwoBoards_ComIT_Master/Appli/Src/system_stm32h7rsxx.c                  STM32H7RSxx system clock configuration file
  - SMBUS/SMBUS_TwoBoards_ComIT_Master/Appli/Src/stm32h7rsxx_it.c                      Interrupt handlers
  - SMBUS/SMBUS_TwoBoards_ComIT_Master/Appli/Src/stm32h7rsxx_hal_msp.c                 HAL MSP module
  - SMBUS/SMBUS_TwoBoards_ComIT_Master/Appli/Src/i2c_timing_utility.c                  Timing utility Source file
  - SMBUS/SMBUS_TwoBoards_ComIT_Master/Appli/Inc/main.h                                Main program header file
  - SMBUS/SMBUS_TwoBoards_ComIT_Master/Appli/Inc/stm32h7rsxx_hal_conf.h                HAL Configuration file
  - SMBUS/SMBUS_TwoBoards_ComIT_Master/Appli/Inc/stm32h7rsxx_it.h                      Interrupt handlers header file
  - SMBUS/SMBUS_TwoBoards_ComIT_Master/Appli/Inc/i2c_timing_utility.h                  Timing utility header file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with STMicroelectronics NUCLEO-H7S3L8
    board and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

  - NUCLEO-H7S3L8 Set-up
    - Connect I2C_SCL line of Master board (PF.01, D69 pin 19 CN9) to I2C_SCL line of Slave Board (PF.01, D69 pin 19 CN9).
    - Connect I2C_SDA line of Master board (PF.00, D68 Pin 21 CN9) to I2C_SDA line of Slave Board (PF.00, D68 Pin 21 CN9).
    - Connect GND of Master board to GND of Slave Board.

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

