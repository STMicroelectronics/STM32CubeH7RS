## <b>DMA_FLASHToRAM Example Description</b>

This example describes how to use DMA to transfer a word data buffer from Flash memory to embedded
SRAM through the HAL API.

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals and initialize the systick used as 1ms HAL timebase.

This project runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches. 

HPDMA1_Channel12 is configured to transfer the contents of a 32-word data
buffer stored in Flash memory to the reception buffer declared in RAM.

As Double Word is available in this family, it is selected as data width for fast transfer.

The start of transfer is triggered by software. HPDMA1_Channel12 memory-to-memory
transfer is enabled. Source and destination addresses incrementing is also enabled.

The transfer is started by setting the channel enable bit for HPDMA1_Channel12.

At the end of the transfer a Transfer Complete interrupt is generated since it
is enabled and the callback function (customized by user) is called.

NUCLEO-H7S3L8 board's LEDs can be used to monitor the transfer status:

 - LD1 is ON when the transfer is complete (into the Transfer Complete interrupt
   routine).
 - LD3 is ON when an Error_Handler is called

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

System, DMA, Data Transfer, Memory to memory, Stream, Flash, RAM

### <b>Directory contents</b>

File | Description
 --- | ---
DMA/DMA_FLASHToRAM/Appli/Inc/stm32h7rsxx_nucleo_conf.h   | BSP configuration file
DMA/DMA_FLASHToRAM/Appli/Inc/stm32h7rsxx_hal_conf.h      | HAL configuration file
DMA/DMA_FLASHToRAM/Appli/Inc/stm32h7rsxx_it.h            | Header for stm32h7rsxx_it.c
DMA/DMA_FLASHToRAM/Appli/Inc/main.h                      | Header for main.c module
DMA/DMA_FLASHToRAM/Appli/Src/stm32h7rsxx_it.c            | Interrupt handlers
DMA/DMA_FLASHToRAM/Appli/Src/main.c                      | Main program
DMA/DMA_FLASHToRAM/Appli/Src/stm32h7rsxx_hal_msp.c       | HAL MSP module
DMA/DMA_FLASHToRAM/Appli/Src/system_stm32h7rsxx.c        | stm32h7rsxx system source file

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
