## <b>PSSI_Transmit Example Description</b>
How to handle a simple Transmit procedure with PSSI.


      - Board: NUCLEO-H7S3L8's (embeds a STM32H7S3L8 device)
      - PSSI_PDCK Pin: PA6  (Arduino D12 CN7 pin 12,   Morpho CN12 pin13)
      - PSSI_D0   Pin: PC6  (Arduino D16 CN7 pin 1,    Morpho CN12 pin4)
      - PSSI_D1   Pin: PA10 (Arduino D0 CN10 pin 16,   Morpho CN12 pin33)
      - PSSI_D2   Pin: PC8  (Arduino D43 CN8 pin 2,    Morpho CN12 pin2)
      - PSSI_D3   Pin: PE1  (No Arduino Pin available, Morpho CN11 pin61)
      - PSSI_D4   Pin: PC11 (Arduino D46 CN8 pin 8,    Morpho CN11 pin2)
      - PSSI_D5   Pin: PD3  (Arduino D55 CN9 pin 10,   Morpho CN8 pin40)
      - PSSI_D6   Pin: PB8  (Arduino D15 CN7 pin 2,    Morpho CN12 pin3)
      - PSSI_D7   Pin: PB9  (Arduino D14 CN7 pin 4,    Morpho CN12 pin5)
      - PSSI_D8   Pin: PA5  (Arduino D13 CN7 pin 10,   Morpho CN12 pin1)
      - PSSI_D9   Pin: PC12 (Arduino D47 CN8 pin 10,   Morpho CN11 pin3)
      - PSSI_D10  Pin: PB5  (Arduino D11 CN7 pin 14,   Morpho CN12 pin29)
      - PSSI_D11  Pin: PD2  (Arduino D48 CN8 pin 12,   Morpho CN11 pin4)
      - PSSI_D12  Pin: PF11 (Arduino A5 CN9 pin 11,    Morpho CN12 pin62)
      - PSSI_D13  Pin: PG7  (Arduino D56 CN9 pin 14,   Morpho CN12 pin67)
      - PSSI_D14  Pin: PC10 (Arduino D45 CN8 pin 6,    Morpho CN11 pin1)
      - PSSI_D15  Pin: PC5  (Arduino A4 CN9 pin 9,     Morpho CN12 pin6)
      - MCO2      Pin: PC9  (Arduino D44 CN8 pin 4,    Morpho CN12 pin1)
      Connect PSSI_PDCK pin to MCO2 pin.
      Connect each PSSI_Dx pin to an oscilloscope (ideally digital probe with several wires to have all signals) to
      check the lines behavior.
      Connect GND between the oscilloscope and the board.

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

Then, the LSI clock is initialized to simulate an external master clock on MCO that is initialized just after with
the HAL_RCC_MCOconfig function.

In this example, transmission is set to 16-bit width data with bus width varying between 8 and 16 lines.
The transmission steps start with the initialization of the PSSI with the size of the bus width before launching the
process. To check if the transmission is successful, use the oscilloscope (ideally digital probe with several wires) to
have all the signals to check the PSSI_Dx lines behavior (from PSSI_D0 to PSSI_D15). The signals should look like this:

                                      __
                         PSSI_D0:  __|  |______________________________________
                                         __
                         PSSI_D1:  _____|  |___________________________________
                                            __
                         PSSI_D2:  ________|  |________________________________
                                               __
                         PSSI_D3:  ___________|  |_____________________________
                            ...                                            __
                         PSSI_D15: _______________________________________|  |_

Be aware that, for the transmission process with 8 lines as a bus width, you should only see the lines from PSSI_D0 to
PSSI_D7.

NUCLEO-H7S3L8's LEDs can be used to monitor the transfer status:
 - LD1 is toggle each time the transmission is completed with the 8 lines bus width.
 - LD2 is toggle each time the transmission is completed with the 16 lines bus width.
 - LD3 is toggle slowly when there is an error in the initialization and transmission process.


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
Transmission, PSSI, Oscilloscope, Signal, Bus Width.

### <b>Directory contents</b>
  - PSSI/PSSI_Transmit/Inc/stm32h7rsxx_nucleo_conf.h   BSP configuration file
  - PSSI/PSSI_Transmit/Inc/stm32h7rsxx_hal_conf.h      HAL configuration file
  - PSSI/PSSI_Transmit/Inc/stm32h7rsxx_it.h            PSSI interrupt handlers header file
  - PSSI/PSSI_Transmit/Inc/main.h                      Header for main.c module
  - PSSI/PSSI_Transmit/Src/stm32h7rsxx_it.c            PSSI interrupt handlers
  - PSSI/PSSI_Transmit/Src/main.c                      Main program
  - PSSI/PSSI_Transmit/Src/system_stm32h7rsxx.c        stm32h7rsxx system source file
  - PSSI/PSSI_Transmit/Src/stm32h7rsxx_hal_msp.c       HAL MSP file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8 devices.
  - This example has been tested with STMicroelectronics NUCLEO-H7S3L8 boards and can be easily tailored to any other
    supported device and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

  - NUCLEO-H7S3L8 Set-up

    - Use a wire to connect the MCO pin (PC9 Arduino D44 CN8 pin 4, Morpho CN12 pin1) to the PSSI_PDCK Pin
    (Arduino D12 CN7 pin 12, Morpho CN12 pin13).
    - Connect the oscilloscope (ideally digital probe with several wires to have all signals) to each PSSI_Dx pin.
    - Connect GND of oscilloscope to GND of board.

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
