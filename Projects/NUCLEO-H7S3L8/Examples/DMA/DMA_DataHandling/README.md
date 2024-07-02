## <b>DMA_DataHandling Example Description</b>

This example describes how to use the DMA Controller to do data handling between transferred data from the source
and transfer to the destination through the HAL API.

This project is targeted to run on STM32H7S3L8Hx devices on NUCLEO-H7S3L8 board from STMicroelectronics.

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals and initialize the systick used as 1ms HAL timebase.

This project runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches. 

DMA channelx is configured to transfer the contents of a 8-word data
buffer placed on FLASH memory to the reception buffer placed on SRAM.

The start of transfers are triggered by software.

 - Right aligned / Zero padding data handling : DMA loads data from source memory by byte, then will align
   data to the right, pad zeros and write it to the destination memory by half word.
 - Right aligned / Sign extended data handling : DMA loads data from source memory by byte, then will align
   data to the right, extend the sign bit and write it to the destination memory by half word.
 - Right aligned / Left truncated data handling : DMA loads data from source memory by half word, then will
   align data to the right, truncate data left and write it to the destination memory by byte.
 - Left aligned / Right truncated data handling : DMA loads data from source memory by half word, then will
   align data to the left, truncate data right and write it to the destination memory by byte.
 - Source byte exchange data handling : DMA loads data from source memory by word, then will exchange
   two bytes in the middle of the word and write it to the destination memory by word.
 - Destination byte exchange data handling : DMA loads data from source memory by word, then will exchange
   the two bytes of the first half word, exchange the two bytes of the second half word and write it to the
   destination memory by word.
 - Destination Half-Word exchange data handling : DMA loads data from source memory by word, then will exchange
   the two half words and write it to the destination memory by word.
 - Destination Word exchange data handling : DMA loads data from source memory by double-word, then will exchange
   the two words and write it to the destination memory by double-word.

NUCLEO-H7S3L8 board's LED can be used to monitor the project operation status:

 - LD1 toggles when no error detected.
 - LD3 is ON when any error occurred.

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
    must be properly defined to be aligned to data cache line size (32 bytes) and of a size of being multiple
    of this cache line size.
    Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
    Please refer to the AN4839 "Level 1 cache on STM32F7 Series"

### <b>Keywords</b>

DMA, Data Transfer, Memory to Memory, Channel, SRAM

### <b>Directory contents</b>

File | Description
 --- | ---
DMA/DMA_DataHandling/Appli/Inc/stm32h7rsxx_nucleo_conf.h    | BSP configuration file
DMA/DMA_DataHandling/Appli/Inc/stm32h7rsxx_hal_conf.h   | HAL configuration file
DMA/DMA_DataHandling/Appli/Inc/stm32h7rsxx_it.h         | Header for stm32h7rsxx_it.c
DMA/DMA_DataHandling/Appli/Inc/main.h                 | Header for main.c module
DMA/DMA_DataHandling/Appli/Src/stm32h7rsxx_it.c         | Interrupt handlers
DMA/DMA_DataHandling/Appli/Src/main.c                 | Main program
DMA/DMA_DataHandling/Appli/Src/stm32h7rsxx_hal_msp.c    | HAL MSP module
DMA/DMA_DataHandling/Appli/Src/system_stm32h7rsxx.c     | stm32h7rsxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with STMicroelectronics NUCLEO-H7S3L8
    boards and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

**EWARM** and **MDK-ARM**:

 - Open your preferred toolchain
 - Rebuild all files from sub-project Appli and load your images into memories: This sub-project will first load Boot_XIP.hex in internal Flash,
   then load the Appli part in External memory available on NUCLEO-H7S3L8 board.
 - Run the example

**CubeIDE**:

 - Compile the example/application; the elf file is required to configure the debug profile (the "active configuration" must be "debug", else only assembly debug is available)
 - Open the menu [Run]->[Debug configuration] and double click on  [STM32 C/C++ Application] (it creates a default debug configuration for the current project selected)
 - In [Debugger] tab, section "External  loaders" add the external loader corresponding to your Board/Memory as described below:
 - In "External loaders" section, click on [Add]
 - Select the loader among the available list (MX25UW25645G_NUCLEO-H7S3L8.stldr)
 - Option "Enabled" checked and Option "Initialize" unchecked
 - In "Misc" section, uncheck the option "Verify flash download"
 - In [Startup] tab, section "Load Image and Symbols":
   - Click on [Add]
   - Click on [File System] and select the Boot HEX file corresponding to your board

      Boot_XIP.hex can be found in folder [Binary] on each Template_XIP project

      You may need to force the capability to select a .hex file by typing " * " + pressing the "Enter" key in the file name dialog

   - Then select the following options:
     - "Download"     checked.
     - "Load symbols" unchecked.
     - Click Ok
   - Back to the [Startup] tab, move down the boot project to the second position.
 - The debug configuration is ready to be used.

