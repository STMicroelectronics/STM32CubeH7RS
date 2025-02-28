## <b>Templates_LRUN Example Description</b>

- This project provides a reference template based on the STM32Cube HAL API that can be used
to build any firmware application to execute application stored on NOR Flash (Sub-project Appli). It boots
from internal Flash, copies the application from the external NOR Flash to the external PSRAM and jumps to
the application code in external PSRAM (Sub-project Boot).
- This project is targeted to run on STM32H7S7xx device on STM32H7S78-DK board from STMicroelectronics.
At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals and initialize the systick used as 1ms HAL timebase.
- The SystemClock_Config() function is used to configure the system clock (SYSCLK) to run at 600 MHz.
- The template project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches. This is provided as template implementation that the User may
integrate in his application in order to enhance the performance.

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
    This is true also for any other data buffers accessed by the CPU and other masters (DMA2D, LTDC).
    The addresses and the size of cacheable buffers (shared between CPU and other masters)
    must be properly defined to be aligned to data cache line size (32 bytes) and of a size of being multiple
    of this cache line size.
    Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs".
    Please refer to the AN4839 "Level 1 cache on STM32F7 Series".

### <b>Keywords</b>

Reference, Template, Boot, Loader, LRUN

### <b>Directory contents</b>

#### <b>Sub-project Boot</b>

File | Description
 --- | ---
  Templates/Template_LRUN/Boot/Inc/main.h                       |  Header for main.c module
  Templates/Template_LRUN/Boot/Inc/extmem_manager.h             |  Header for extmem_manager.c module
  Templates/Template_LRUN/Boot/Inc/stm32h7rsxx_hal_conf.h       |  HAL Configuration file
  Templates/Template_LRUN/Boot/Inc/stm32h7rsxx_it.h             |  Interrupt handlers header file
  Templates/Template_LRUN/Boot/Inc/stm32h7s78_discovery_conf.h  |  BSP Configuration file
  Templates/Template_LRUN/Boot/Inc/stm32_extmem_conf.h          |  External memory manager Configuration file
  Templates/Template_LRUN/Boot/Src/main.c                       |  Main program
  Templates/Template_LRUN/Boot/Src/extmem_manager.c             |  code to initialize external memory
  Templates/Template_LRUN/Boot/Src/stm32h7rsxx_hal_msp.c        |  HAL MSP module
  Templates/Template_LRUN/Boot/Src/stm32h7rsxx_it.c             |  Interrupt handlers
  Templates/Template_LRUN/Boot/Src/system_stm32h7rsxx.c         |  STM32H7RSxx system source file

#### <b>Sub-project Appli</b>

File | Description
 --- | ---
  Templates/Template_LRUN/Appli/Inc/main.h                      |  Header for main.c module
  Templates/Template_LRUN/Appli/Inc/stm32h7rsxx_hal_conf.h      |  HAL Configuration file
  Templates/Template_LRUN/Appli/Inc/stm32h7rsxx_it.h            |  Interrupt handlers header file
  Templates/Template_LRUN/Appli/Inc/stm32h7s78_discovery_conf.h |  BSP Configuration file
  Templates/Template_LRUN/Appli/Src/main.c                      |  Main program
  Templates/Template_LRUN/Appli/Src/stm32h7rsxx_hal_msp.c       |  HAL MSP module
  Templates/Template_LRUN/Appli/Src/stm32h7rsxx_it.c            |  Interrupt handlers
  Templates/Template_LRUN/Appli/Src/system_stm32h7rsxx.c        |  STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This template runs on STM32H7S7xx devices.

  - This template has been tested with STMicroelectronics STM32H7S78-DK
    boards and can be easily tailored to any other supported device
    and development board.

  - On STM32H7S78-DK board, the BOOT0 mechanical slide switch must be set to SW1.

  - User Option Bytes requirement (with STM32CubeProgrammer tool):

    - XSPI1_HSLV=1     I/O XSPIM_P1 High speed option enabled
    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled
    - VDDIO_HSLV=0     I/O speed optimized to operate at 3.3V

### <b>How to use it ?</b>

In order to make the program work, you must do the following:
 - Ensure BOOT0 switch is set to SW1

The output application is a raw binary and the IDEs does not offer the ability to handle
the download in an easy way. The following procedure must be applied to load and debug
the program :
 - Use a programming tool such as STM32CubeProgrammer to download the generated binary at the load address
   (here the external loader corresponding to NOR flash and load address is 0x70000000)
 - Reset the board
 - follow the procedure corresponding to your IDE to debug the project:
    - IAR     : menu Projet->Attach to running target
    - KEIL    : memu Debug->Start stop debug session (thanks to debug.ini/load.ini config)
    - CubeIDE : use the template Appli LRUN launch file provided

  1. Open your toolchain

  2. Open Multi-projects workspace file
     The contents of the workspace is the same for all IDEs
     - 'Boot' subproject: its role is to prepare the launch of the application by configuring the 
       system and configuring the external memories, to manage the LRUN operation by copying the application 
       and starting its execution. 
     - 'Appli' subproject: this is the user application using a linker file aligned with the external 
       memory usage.

  3. Build & Load  application
     To build the application you must compile all the projects and each one will produce a binary file:
     - 'Boot' subproject: an hex file that can be loaded by the toolchain or a programming tool such as
       STM32CubeProgrammer
     - 'Appli' subproject: in LRUN case the output is a raw binary because the execution and storage addresses differ, 
       you must download it with a programming tool such as STM32CubeProgrammer

  4. Debug the application 
     - 'Boot' subproject: directly from the toolchain you can load/debug this part of the code.
     - 'Appli' subproject: as it is a raw binary file, it is not possible to load the application from the toolchain,
       so for debug, you must attach the debugger to the running target. 
     
     The procedure to debug is as follows:
     - Use STM32CubeProgrammer to download the generated binary at the load address 
       (here the external loader of the board corresponding to NOR flash and load address is 0x70000000)
       - Reset the board
       - Attachment to the running target depending on the IDE:
         - IAR: menu Projet->Attach to running target
         - KEIL: memu Debug->Start stop debug session (thanks to debug.ini/load.ini config)
         - STM32CubeIDE : use the launch file provided (its contains the debug procedure)
       - Then you can break execution and debug the application.
