## <b>XSPI_NOR_ReadWhileWrite_DTR Example Description</b>

How to write and read data on XSPI NOR memory (bank1) when code is executed from bank0

This example demonstrates the Read While Write feature using Macronix NOR Flash
which supports simultaneous Read and Write operations (WRBI, WRCT, WRCF)

- Project structure:
  - Boot Sub-project: Boot from internal Flash and jumps to the application code in external Flash (Appli Sub-project ).
  - Appli Sub-Project: Firmware application executed from external FLash (Bank0).
- This project is designed to run on the STM32H7S7xx device on the STM32H7S78-DK board from STMicroelectronics.
  The HAL_Init() function is called at the beginning of the main program to reset all peripherals and initialize
  the systick used as a 1ms HAL timebase.
- This project runs from the external Flash memory. It is launched from a first boot stage and inherits
  from this boot project configuration (caches, MPU regions [regions 0, 1 and 2 here], system clock at
  600 MHz and external memory interface at the highest speed).

The boot part is automatically downloaded from the IDE environment via the boot sub-project.
Binary file: XSPI_NOR_ReadWhileWrite_DTR_Boot.hex

Program Execution from Bank0:

 1. Erase Memory (Bank1 area)
    - Enable Write Operation
    - Erasing Block(64KB) on bank1
    - Wait for end of block erase in bank1

 2. Write data on Bank1 using RWW feature
    - Enable Write Operation on NOR Flash (Global)
    - Write Buffer Initial command (WRBI)
    - Write Buffer Continue command (WRCT)
    - Write Buffer Confirm (WRCF)
    - Wait for end of programming in bank1

 3. Read and compare data previously written
    - Read data previously written on bank1
    - Compare results

STM32H7S78-DK board's LEDs are used to monitor the example status:
 - LED_GREEN is ON when the checked data is correct.
 - LED_RED is ON when the checked data is correct.

Error_Handler() function is called (infinite loop) if an error is returned by the HAL API..

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
   Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
   Please refer to the AN4839 "Level 1 cache on STM32F7 Series"

### <b>Keywords</b>

Memory, XSPI, Read, Write, RWW, XIP, NOR, DTR Mode, DMA

### <b>Directory contents</b>

#### <b>Sub-project Boot</b>

File | Description
 --- | ---
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Boot/Inc/main.h                       |  Header for main.c module
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Boot/Inc/extmem_manager.h             |  Header for extmem_manager.c module
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Boot/Inc/stm32h7rsxx_hal_conf.h       |  HAL Configuration file
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Boot/Inc/stm32h7rsxx_it.h             |  Interrupt handlers header file
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Boot/Inc/stm32h7s78_discovery_conf.h  |  BSP Configuration file
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Boot/Inc/stm32_extmem_conf.h          |  External memory manager Configuration file
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Boot/Src/main.c                       |  Main program
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Boot/Src/extmem_manager.c             |  code to initialize external memory
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Boot/Src/stm32h7rsxx_hal_msp.c        |  HAL MSP module
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Boot/Src/stm32h7rsxx_it.c             |  Interrupt handlers
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Boot/Src/system_stm32h7rsxx.c         |  STM32H7RSxx system source file

#### <b>Sub-project Appli</b>

File | Description
 --- | ---
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Appli/Inc/main.h                      |  Header for main.c module
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Appli/Inc/stm32h7rsxx_hal_conf.h      |  HAL Configuration file
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Appli/Inc/stm32h7rsxx_it.h            |  Interrupt handlers header file
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Appli/Inc/stm32h7s78_discovery_conf.h |  BSP Configuration file
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Appli/Src/main.c                      |  Main program
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Appli/Src/stm32h7rsxx_hal_msp.c       |  HAL MSP module
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Appli/Src/stm32h7rsxx_it.c            |  Interrupt handlers
  - XSPI/XSPI_NOR_ReadWhileWrite_DTR/Appli/Src/system_stm32h7rsxx.c        |  STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This example has been tested with STMicroelectronics STM32H7S78-DK (MB1736)
    board and can be easily tailored to any other supported device
    and development board.

  - On STM32H7S78-DK board, the BOOT0 mechanical slide switch (SW1) must be set to default configuration 0
    (Boot from user flash memory at address 0x0800 0000)

  - User Option Bytes requirement (with STM32CubeProgrammer tool):

    - XSPI1_HSLV=1     I/O XSPIM_P1 High speed option enabled
    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled
    - VDDIO_HSLV=0     I/O speed optimized to operate at 3.3V

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

**EWARM**

 1. Open Multi-projects workspace file Project.eww

 <b>Optional:</b>
  - Select first "XSPI_NOR_ReadWhileWrite_DTR_Boot" workspace
  - Rebuild all files from sub-project Boot
  - If no modification is done on boot project, this step could be skipped.

 2. Select first "XSPI_NOR_ReadWhileWrite_DTR_Appli" workspace
 3. Rebuild all files from sub-project Appli and load your images into memories: This sub-project
    will first load the Appli part in External memory available on STM32H7S78-DK board, then load
    the XSPI_NOR_ReadWhileWrite_DTR_Boot.hex in internal Flash.
 4. Run the example

**MDK-ARM**:

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
