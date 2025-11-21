## <b>I2C_TwoBoards_RestartAdvComIT Example Description</b>

How to perform multiple I2C data buffer transmission/reception between two boards, 
in interrupt mode and with restart condition.

    Board: NUCLEO-H7S3L8 (embeds a STM32H7S3L8Hx device)
    SCL Pin: PF1 (Arduino D69 CN9 pin 19)
    SDA Pin: PF0 (Arduino D68 CN9 pin 21)

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.

This project runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).

Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

The I2C peripheral configuration is ensured by the HAL_I2C_Init() function.

This later is calling the HAL_I2C_MspInit()function which core is implementing
the configuration of the needed I2C resources according to the used hardware (CLOCK, 
GPIO and NVIC). You may update this function to change I2C configuration.

The USER push-button is used to initiate a communication between Master device to Slave.

User can initiate a new communication after each previous transfer completed.

The I2C communication is then initiated.

The project is split in two parts the Master Board and the Slave Board :

 - Master Board :
   The HAL_I2C_Master_Sequential_Transmit_IT() and the HAL_I2C_Master_Sequential_Receive_IT() functions 
   allow respectively the transmission and the reception of a predefined data buffer
   in Master mode.

 - Slave Board :
   The HAL_I2C_EnableListen_IT(), HAL_I2C_Slave_Sequential_Receive_IT() and the HAL_I2C_Slave_Sequential_Transmit_IT() functions 
   allow respectively the "Listen" the I2C bus for address match code event, reception and the transmission of a predefined data buffer
   in Slave mode.

The user can choose between Master and Slave through "#define MASTER_BOARD"
in the "main.c" file.

If the Master board is used, the "#define MASTER_BOARD" must be uncommented.

If the Slave board is used the "#define MASTER_BOARD" must be commented.

Example execution:

On Master board side:

 - Wait USER push-button to be pressed.

This action initiate a write request by Master through HAL_I2C_Master_Sequential_Transmit_IT() or a write then read request
through HAL_I2C_Master_Sequential_Transmit_IT() then HAL_I2C_Master_Sequential_Receive_IT() routine depends on Command Code type.

Initially at power on Slave device through Interrupt "Listen" the I2C bus to perform an acknowledge of Match Address when necessary.

This "Listen" action is initiated by calling HAL_I2C_EnableListen_IT().

Command code type is decomposed in two categories :

 1. Action Command code

    a. Type of command which need an internal action from Slave Device without sending any specific answer to Master.
    b. I2C sequence is composed like that :
     _____________________________________________________________________________________
    |_START_|_Slave_Address_|_Wr_|_A_|_Command_Code_BYTE_1_|_A_|_Command_Code_BYTE_2_|_A_|....
     ________________________________
    |_Command_Code_BYTE_M_|_A_|_STOP_|

First of all, through HAL_I2C_Master_Sequential_Transmit_IT() routine, Master device generate an I2C start condition
with the Slave address and a write bit condition.

In Slave side, when address Slave match code is received on I2C2, an event interrupt (ADDR) occurs.

I2C2 IRQ Handler routine is then calling HAL_I2C_AddrCallback() which check Address Match Code and direction Write (Transmit)
to call the correct HAL_I2C_Slave_Sequential_Receive_IT() function.
This will allow Slave to enter in receiver mode and then acknowledge Master to send the Command code bytes through Interrupt.

The Command code data is received and treated byte per byte through HAL_I2C_SlaveRxCpltCallback() in Slave side until a STOP condition.

And so in Master side, each time the Slave acknowledge the byte received,
Master transfer the next data from flash memory buffer to I2C2 TXDR register until "Action Command code" Transfer completed.
Master auto-generate a Stop condition when transfer is achieved.

The STOP condition generate a STOP interrupt and initiate the end of reception on Slave side.

Thanks to HAL_I2C_ListenCpltCallback(), Slave is informed of the end of Communication with Master
and "Listen" mode is also terminated.

NUCLEO-H7S3L8 board's LEDs can be used to monitor the transfer status :

Slave board side only :

 - LD1 is turned ON when the reception process is completed.

Master board side only :

 - LD1 is turned ON when the transmission process is completed.
 
Both sides :

 - LD3 is slowly blinking (1 sec. period) when there is an error in communication process.(communication is stopped if any, using infinite loop)

These LEDs status are keeped at same value during 1 Second and then clear, this will allow to monitor a next transfer status.
 
Also only on Master board side, Terminal I/O can be used to watch the Action Command Code sent by Master and associated Slave action with IDE in debug mode.

Depending of IDE, to watch content of Terminal I/O note that :

 - When resorting to EWARM IAR IDE:
   Command Code is displayed on debugger as follows: View --> Terminal I/O

 - When resorting to MDK-ARM KEIL IDE:
   Command Code could not be displayed on debugger but user can use the Virtual Com port of the NUCLEO-H7S3L8
   to display Command Code on PC side using an hyperterminal. See below, information related to this possibility.

 When resorting to STM32CubeIDE:

 Command Code is displayed on debugger as follows: Window--> Show View--> Console.

 In Debug configuration : 

- Window\Debugger, select the Debug probe : ST-LINK(OpenOCD)
- Window\Startup,add the command "monitor arm semihosting enable"

Other proposal to retrieve display of Command Code for all IDE is to use the Virtual Com.

In order to select use of Virtual Com port feature of STLINK for connection between NUCLEO-H7S3L8 and PC,
User has to set USE_VCP_CONNECTION define to 1 in main.h file.

If so, please ensure that USART communication between the target MCU and ST-LINK MCU is properly enabled 
on HW board in order to support Virtual Com Port (Default HW SB configuration allows use of VCP)

 2. Request Command code :

    a. Type of command which need a specific data answer from Slave Device.
    b. I2C sequence is composed like that :
     _____________________________________________________________________________________
    |_START_|_Slave_Address_|_Wr_|_A_|_Command_Code_BYTE_1_|_A_|_Command_Code_BYTE_2_|_A_|....
     ______________________________________________________________________________
    |_Command_Code_BYTE_M_|_A_|_RESTART_|_Slave_Address_|_Rd_|_A_|_Data_BYTE_1_|_A_|...
     ___________________________________________
    |_Data_BYTE_2_|_A_|_Data_BYTE_N_|_NA_|_STOP_|

First of all, through HAL_I2C_Master_Sequential_Transmit_IT() routine, Master device generate an I2C start condition
with the Slave address and a write bit condition.

In Slave side, when address Slave match code is received on I2C2, an event interrupt (ADDR) occurs.

I2C2 IRQ Handler routine is then calling HAL_I2C_AddrCallback() which check Address Match Code and direction Write (Transmit)
to call the correct HAL_I2C_Slave_Sequential_Receive_IT() function.

This will allow Slave to enter in receiver mode and then acknowledge Master to send the Command code bytes through Interrupt.

The Command code data is received and treated byte per byte through HAL_I2C_SlaveRxCpltCallback() in Slave side.

If data received match with a Internal Command Code, set the associated index, which will use for Transmission process when requested by Master

And so in Master side, each time the Slave acknowledge the byte received,
Master transfer the next data from flash memory buffer to I2C2 TXDR register until "Request Command code" transfer completed.

Then through HAL_I2C_Master_Sequential_Receive_IT() routine, Master device generate a RESTART condition
with Slave address and a read bit condition.

In Slave side, when address Slave match code is received on I2C2, an event interrupt (ADDR) occurs.

I2C2 IRQ Handler routine is then calling HAL_I2C_AddrCallback() which check Address Match Code and direction Read (Reception)
to call the correct HAL_I2C_Slave_Sequential_Transmit_IT() function.

Slave enter in transmitter mode and send send answer bytes through interrupt until end of transfer size.

Master auto-generate a NACK and STOP condition to inform the Slave that the transfer and communication are finished.

The STOP condition generate a STOP interrupt and initiate the end of reception on Slave side.

Thanks to HAL_I2C_ListenCpltCallback(), Slave is informed of the end of Communication with Master
and "Listen" mode is also terminated.

NUCLEO-H7S3L8 board's LEDs can be used to monitor the transfer status in both sides :

Slave board side :

 - LD1 is turned ON when the reception process is completed.
 - LD1 is turned OFF when the transmission process is completed.

Master board side :

 - LD1 is turned ON when the transmission process is completed.
 - LD1 is turned OFF when the reception process is completed.
 
Both sides :

 - LD3 is slowly blinking (1 sec. period) when there is an error in communication process.(communication is stopped if any, using infinite loop)

These LEDs status are keeped at same value during 1 Second and then clear, this will allow to monitor a next transfer status.

Also only on Master board side, Terminal I/O can be used to watch the Request Command Code sent by Master and associated Slave answer with IDE in debug mode.

 - When resorting to EWARM IAR IDE:
   Command Code is displayed on debugger as follows: View --> Terminal I/O

 - When resorting to MDK-ARM KEIL IDE:
   Command Code could not be displayed on debugger but user can use the Virtual Com port of the NUCLEO-H7S3L8
   to display Command Code on PC side using an hyperterminal. See below, information related to this possibility.
 
 When resorting to STM32CubeIDE:

 Command Code is displayed on debugger as follows: Window--> Show View--> Console.

 In Debug configuration :

- Window\Debugger, select the Debug probe : ST-LINK(OpenOCD)
- Window\Startup,add the command "monitor arm semihosting enable"

Other proposal to retrieve display of Command Code for all IDE is to use the Virtual Com.

In order to select use of Virtual Com port feature of STLINK for connection between NUCLEO-H7S3L8 and PC,
User has to set USE_VCP_CONNECTION define to 1 in main.h file.
If so, please ensure that USART communication between the target MCU and ST-LINK MCU is properly enabled 
on HW board in order to support Virtual Com Port (Default HW SB configuration allows use of VCP)

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
    - Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
    - Please refer to the AN4839 "Level 1 cache on STM32F7 Series"

### <b>Keywords</b>

Connectivity, I2C, Communication, Transmission, Reception, SCL, SDA, Interrupt, Request, Sequential

### <b>Directory contents</b> 

  - I2C/I2C_TwoBoards_RestartAdvComIT/Appli/Inc/stm32h7rsxx_nucleo_conf.h     BSP configuration file
  - I2C/I2C_TwoBoards_RestartAdvComIT/Appli/Inc/stm32h7rsxx_hal_conf.h    HAL configuration file
  - I2C/I2C_TwoBoards_RestartAdvComIT/Appli/Inc/stm32h7rsxx_it.h          I2C interrupt handlers header file
  - I2C/I2C_TwoBoards_RestartAdvComIT/Appli/Inc/main.h                  Header for main.c module
  - I2C/I2C_TwoBoards_RestartAdvComIT/Appli/Src/stm32h7rsxx_it.c          I2C interrupt handlers
  - I2C/I2C_TwoBoards_RestartAdvComIT/Appli/Src/main.c                  Main program
  - I2C/I2C_TwoBoards_RestartAdvComIT/Appli/Src/system_stm32h7rsxx.c      STM32H7RSxx system source file
  - I2C/I2C_TwoBoards_RestartAdvComIT/Appli/Src/stm32h7rsxx_hal_msp.c     HAL MSP file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7RSXX devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

  - NUCLEO-H7S3L8 Set-up

    - Connect I2C_SCL line of Master board (PF1, Arduino D69 CN9 pin 19) to I2C_SCL line of Slave Board (PF1, Arduino D69 CN9 pin 19).
    - Connect I2C_SDA line of Master board (PF0, Arduino D68 CN9 pin 21) to I2C_SDA line of Slave Board (PF0, Arduino D68 CN9 pin 21).
    - Connect GND of Master board to GND of Slave Board.

  - Launch the program in debug mode on Master board side, and in normal mode on Slave side
    to benefit of Terminal I/O information on Master side.

    Or

  - Launch serial communication SW on PC (as HyperTerminal or TeraTerm) with proper configuration 
    (115200 bauds, 8 bits data, 1 stop bit, no parity, no HW flow control) connected with the com port number of STLINK.
    Then Launch the program on both side.

    For **CubeIDE** (Both Configurations in Standalone mode):

      - Comment the function initialise_monitor_handles() in main.c at line 158.
      - Delete the miscellaneous linker flag "-specs=rdimon.specs -lc -lrdimon" under Project -> Properties -> C/C++ Build -> Settings (Tool Settings Tab) -> MCU GCC Linker
      - Include syscalls.c to the build.

  - Press USER push-button to initiate a communication by Master device to Slave device.
    User can initiate a new communication after each previous transfer completed.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Rebuild all files differently for Master and Slave Board:
    - Uncomment "#define MASTER_BOARD" and load the project in Master Board
    - Comment "#define MASTER_BOARD" and load the project in Slave Board

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

