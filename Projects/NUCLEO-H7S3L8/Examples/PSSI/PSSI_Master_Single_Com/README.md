## <b>PSSI_Master_Single_Com Example Description</b>
How to handle a single communication procedure using two boards with PSSI in polling mode.

      - Boards: NUCLEO-H7S3L8's (embeds a STM32H7S3L8 device)
      - PSSI_PDCK Pin: PA6  (Arduino D12 CN7 pin 12,   Morpho CN12 pin13)
      - PSSI_D0   Pin: PC6  (Arduino D16 CN7 pin 1,    Morpho CN12 pin4)
      - PSSI_D1   Pin: PA10 (Arduino D0 CN10 pin 16,   Morpho CN12 pin33)
      - PSSI_D2   Pin: PC8  (Arduino D43 CN8 pin 2,    Morpho CN12 pin2)
      - PSSI_D3   Pin: PC9  (Arduino D44 CN8 pin 4,    Morpho CN12 pin1)
      - PSSI_D4   Pin: PC11 (Arduino D46 CN8 pin 8,    Morpho CN11 pin2)
      - PSSI_D5   Pin: PD3  (Arduino D55 CN9 pin 10,   Morpho CN8 pin40)
      - PSSI_D6   Pin: PB8  (Arduino D15 CN7 pin 2,    Morpho CN12 pin3)
      - PSSI_D7   Pin: PB9  (Arduino D14 CN7 pin 4,    Morpho CN12 pin5)
      - PSSI_DE   Pin: PG3  (Arduino D50 CN8 pin 16,   Morpho CN11 pin44)
      - TIM1      Pin: PC2  (Arduino A7 CN10 pin 9,    Morpho CN11 pin35)
      Connect each PSSI_Dx with wires between the boards.
      Connect PSSI_DE together to allow the synchronization with the Slave board.
      Connect PSSI_PDCK together to allow the synchronization with the Slave board.
      Connect PSSI_PDCK pin to the TIM1 pin on the Master Board.
      Connect GND between the two boards.

- This project provides a reference example based on the STM32Cube HAL API that can be used
to build any firmware application to execute from external Flash (Sub-project Appli). It boots from external Flash and jumps
to the application code in external Flash (Sub-project Boot).
- This project is targeted to run on STM32H7S3xx device on NUCLEO-H7S3L8 board from STMicroelectronics.
At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals and initialize the systick used as 1ms HAL timebase.
- This project runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.
- The example project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches. This is provided as example implementation that the User may
integrate in his application in order to enhance the performance.

The PSSI peripheral configuration is ensured by the HAL_PSSI_Init() function.
This later is calling the HAL_PSSI_MspInit()function which core is implementing the configuration of the needed PSSI
resources according to the used hardware (CLOCK, GPIO and NVIC).
User may update this function to change PSSI configuration.

Then, the TIM peripheral is initialized to generate a PWM signal to simulate a clock on the Master board.

In this example, the PSSI is set to 8-bit width data with 8 lines bus width.
The project is split in two parts : the Master Board and the Slave Board.
Master Board :
  The Master Board will generate a clock for the Slave with the function Start_Clock_Generation() and will send a buffer
  using the function HAL_PSSI_Transmit().

Slave Board :
  The Slave Board will receive the buffer sent by the Master Board with the function HAL_PSSI_Receive(). It will then
  compare the buffer received to the buffer expected.

For this example the buffer sent is predefined.

In a first step, after the user press the USER push-button on the Master Board, the simulated Master will generate a
signal PWM as PSSI_PDCK clock generation with the function Start_Clock_Generation() before launching the transmission of
a buffer through HAL_PSSI_Transmit(). When the transmission is done, the Master will stop the PWM generation with
Stop_Clock_Generation() and the LD1 will light up. On the Slave side, it will receive the buffer sent through
HAL_PSSI_Receive(). After the reception is done, the Slave will compare the buffer received to the buffer sent. If the
message expected is in the buffer received, the LD1 will also light up on the Slave Board.

NUCLEO-H7S3L8's LEDs can be used to monitor the transfer status:
 - LD1 is toggle when the transmission and the reception are done.
 - LD2 is toggle slowly when there is an error in the initialization and transmission process.


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
    Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
    Please refer to the AN4839 "Level 1 cache on STM32F7 Series"

### <b>Keywords</b>
Transmission, Reception, Master, Slave, PSSI, PWM, Signal, Buffer, Bus Width.

### <b>Directory contents</b>
  - PSSI/PSSI_Master_Receive/Appli/Inc/stm32h7rsxx_nucleo_conf.h   BSP configuration file
  - PSSI/PSSI_Master_Receive/Appli/Inc/stm32h7rsxx_hal_conf.h      HAL configuration file
  - PSSI/PSSI_Master_Receive/Appli/Inc/stm32h7rsxx_it.h            PSSI interrupt handlers header file
  - PSSI/PSSI_Master_Receive/Appli/Inc/main.h                      Header for main.c module
  - PSSI/PSSI_Master_Receive/Appli/Src/stm32h7rsxx_it.c            PSSI interrupt handlers
  - PSSI/PSSI_Master_Receive/Appli/Src/main.c                      Main program
  - PSSI/PSSI_Master_Receive/Appli/Src/system_stm32h7rsxx.c        stm32h7rsxx system source file
  - PSSI/PSSI_Master_Receive/Appli/Src/stm32h7rsxx_hal_msp.c       HAL MSP file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8 devices.
  - This example has been tested with STMicroelectronics NUCLEO-H7S3L8 boards and can be easily tailored to any other
    supported device and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

  - NUCLEO-H7S3L8 Set-up
    - Use a wire to connect the TIM1 pin (PC2 Arduino A7 CN10 pin 9, Morpho CN11 pin35) of the Master board to the
    PSSI_PDCK Pin (PA6 Arduino D12 CN7 pin 12, Morpho CN12 pin13) of the two boards.
    - Connect each PSSI_Dx pin with wires between the two boards.
    - Connect GND of both boards together.

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
