## <b>RNG_MultiRNG_IT Example Description</b>

Configuration of the RNG using the HAL API. This example uses RNG interrupts to generate 32-bit long random numbers.

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals and initialize the systick used as 1ms HAL timebase.

This example runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

The RNG peripheral configuration is ensured by the HAL_RNG_Init() function.
The latter is calling the HAL_RNG_MspInit() function which implements
the configuration of the needed RNG resources according to the used hardware (CLOCK,
GPIO, DMA and NVIC). You may update this function to change RNG configuration.

After startup, user is asked to press USER push-button.
The 8-entry array aRandom32bit[] is filled up by 32-bit long random numbers
at each USER push-button press.

Each random number generation triggers an interruption which is handled by
HAL_RNG_ReadyDataCallback() API to retrieve and store the random number.

The random numbers can be displayed on the debugger in aRandom32bit variable.

In case of error, LD3 is toggling at a frequency of 1Hz.

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

Analog, RNG, Random, FIPS PUB 140-2, Analog Random number generator, Entropy, Period, interrupt

### <b>Directory contents</b>

File | Description
 --- | ---
  RNG/RNG_MultiRNG_IT/Appli/Inc/stm32h7rsxx_nucleo_conf.h         |  BSP configuration file
  RNG/RNG_MultiRNG_IT/Appli/Inc/stm32h7rsxx_hal_conf.h            |  HAL configuration file
  RNG/RNG_MultiRNG_IT/Appli/Inc/stm32h7rsxx_it.h                  |  Interrupt handlers header file
  RNG/RNG_MultiRNG_IT/Appli/Inc/main.h                            |  Header for main.c module
  RNG/RNG_MultiRNG_IT/Appli/Src/stm32h7rsxx_it.c                  |  Interrupt handlers
  RNG/RNG_MultiRNG_IT/Appli/Src/main.c                            |  Main program
  RNG/RNG_MultiRNG_IT/Appli/Src/stm32h7rsxx_hal_msp.c             |  HAL MSP module
  RNG/RNG_MultiRNG_IT/Appli/Src/system_stm32h7rsxx.c              |  STM32H7RSxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.
  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

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

