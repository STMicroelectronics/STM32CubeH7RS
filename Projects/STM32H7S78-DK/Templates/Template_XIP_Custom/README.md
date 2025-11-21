## <b>Templates XIP Custom (eXecute In Place with External Memory Manager Custom driver) Description</b>

This project provides a reference XIP template based on the STM32Cube HAL API and the External Memory Manager (EMM) Middleware. It can be used to build any firmware application that executes in external Flash (sub-project Appli).
The main difference from the existing Template_XIP project is that the Custom driver of the External Memory Manager is used (instead of the Nor Flash driver in Template_XIP).

The External Memory Manager middleware now includes a new driver type called “Custom,” in addition to the previously existing Nor_Flash and PsRAM drivers.

The goal of this new driver is to offer a new way to configure and use external memory through EMM APIs (with no changes to the EMM APIs compared to previous EMM versions).
This driver is based on a configuration file that contains memory characteristics and details. This file is processed by the Custom driver to configure memory registers and access methods.
In this template, the Nor Flash memory present on the STM32H7S78-DK board is a Macronix MX66UW1G45G. The EMM Custom Configurations file for this reference is provided in the stm32_mx66uw1g45g.h file, whose content can be built based on the memory datasheet.

This project is targeted to run on STM32H7S7xx device on STM32H7S78-DK board from STMicroelectronics.
This project consists of three sub-projects:

- Boot: System configuration, flash access configuration, boots from internal Flash and jumps to the application code in external Flash
- Appli: Application code stored in external flash
- ExtmemLoader: Loader generation environment

At the beginning of the main program, the HAL_Init() function is called to reset all the peripherals and initialize the systick used as 1ms HAL timebase.
- This project runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project configuration (caches, MPU regions [regions 0, 1 and 2 here], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP_Custom/Binary/Boot_XIP.hex file.
- The template project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable the Layer 1 Core Instruction and Data Caches. This is provided as template implementation that the User may integrate in his application in order to enhance the performance.

Once the application runs, the green LED (GPIO PO.01) toggles in an infinite loop with a 250 ms period.

The ExtMemLoader is a subproject used to generate a binary library capable of downloading an application to external memory.
This binary is called a "Loader" which can be used by the IDE or CubeProgrammer.

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

Reference, Template, Boot, Loader, XiP, External Memory Manager

### <b>Directory contents</b>

#### <b>Sub-project Boot</b>

File | Description
 --- | ---
  Templates/Template_XIP_Custom/Boot/Inc/main.h                       |  Header for main.c module
  Templates/Template_XIP_Custom/Boot/Inc/extmem_manager.h             |  Header for extmem_manager.c module
  Templates/Template_XIP_Custom/Boot/Inc/stm32h7rsxx_hal_conf.h       |  HAL Configuration file
  Templates/Template_XIP_Custom/Boot/Inc/stm32h7rsxx_it.h             |  Interrupt handlers header file
  Templates/Template_XIP_Custom/Boot/Inc/stm32_extmem_conf.h          |  External memory manager Configuration file
  Templates/Template_XIP_Custom/Boot/Inc/stm32_mx66uw1g45g.h          |  EMM Custom driver Configuration file for Macronix MX66UW1G45G Nor Flash
  Templates/Template_XIP_Custom/Boot/Inc/stm32_aps256xx.h             |  EMM Custom driver Configuration file for APMemory APS256XXOBR PSRAM
  Templates/Template_XIP_Custom/Boot/Src/main.c                       |  Main program
  Templates/Template_XIP_Custom/Boot/Src/extmem_manager.c             |  code to initialize external memory
  Templates/Template_XIP_Custom/Boot/Src/stm32h7rsxx_hal_msp.c        |  HAL MSP module
  Templates/Template_XIP_Custom/Boot/Src/stm32h7rsxx_it.c             |  Interrupt handlers
  Templates/Template_XIP_Custom/Boot/Src/system_stm32h7rsxx.c         |  STM32H7RSxx system source file

#### <b>Sub-project Appli</b>

File | Description
 --- | ---
  Templates/Template_XIP_Custom/Appli/Inc/main.h                      |  Header for main.c module
  Templates/Template_XIP_Custom/Appli/Inc/stm32h7rsxx_hal_conf.h      |  HAL Configuration file
  Templates/Template_XIP_Custom/Appli/Inc/stm32h7rsxx_it.h            |  Interrupt handlers header file
  Templates/Template_XIP_Custom/Appli/Inc/stm32h7s78_discovery_conf.h |  BSP Configuration file
  Templates/Template_XIP_Custom/Appli/Src/main.c                      |  Main program
  Templates/Template_XIP_Custom/Appli/Src/stm32h7rsxx_hal_msp.c       |  HAL MSP module
  Templates/Template_XIP_Custom/Appli/Src/stm32h7rsxx_it.c            |  Interrupt handlers
  Templates/Template_XIP_Custom/Appli/Src/system_stm32h7rsxx.c        |  STM32H7RSxx system source file

#### <b>Sub-project ExtMemLoader</b>

File | Description
 --- | ---
  Templates/Template_XIP_Custom/ExtMemLoader/Inc/extmem_manager.h             |  Header for extmem_manager.c module
  Templates/Template_XIP_Custom/ExtMemLoader/Inc/extmemloader_init.h          |  Header for system initialization
  Templates/Template_XIP_Custom/ExtMemLoader/Inc/stm32_extmem_conf.h          |  External memory manager Configuration file
  Templates/Template_XIP_Custom/ExtMemLoader/Inc/stm32_extmemloader_conf.h    |  Header with externam memory device information
  Templates/Template_XIP_Custom/ExtMemLoader/Inc/stm32h7rsxx_hal_conf.h       |  HAL Configuration file
  Templates/Template_XIP_Custom/ExtMemLoader/Inc/stm32_mx66uw1g45g.h          |  EMM Custom driver Configuration file for Macronix MX66UW1G45G Nor Flash
  Templates/Template_XIP_Custom/ExtMemLoader/Src/extmem_manager.c             |  Code to initialize external memory
  Templates/Template_XIP_Custom/ExtMemLoader/Src/extmemloader_init.c          |  Code to perform system initialization
  Templates/Template_XIP_Custom/ExtMemLoader/Src/stm32h7rsxx_hal_msp.c        |  HAL MSP module
  Templates/Template_XIP_Custom/ExtMemLoader/Src/system_stm32h7rsxx.c         |  STM32H7RSxx system source file
  Templates/Template_XIP_Custom/ExtMemLoader/Config/extmemloader.board        |  Configuration file
  Templates/Template_XIP_Custom/ExtMemLoader/Config/extmemloader.flash        |  Configuration file
  Templates/Template_XIP_Custom/ExtMemLoader/Config/extmemloader.mac          |  Configuration file

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

#### <b>IAR</b>
  1. Open your toolchain
  2. Open Multi-projects workspace file Project.eww

> <b>Optional:</b>
>
> - Select first "Template_XIP_Custom_Boot" workspace
> - Rebuild all files from sub-project Boot
> - If no modification is done on boot project, this step could be skipped.

  3. Select then "Template_XIP_Custom_Appli" workspace
  4. Rebuild all files from sub-project Appli and load your images into memories: This sub-project
     will first load the Appli part in External memory available on STM32H7S78-DK board, then load
     the Boot_XIP_Custom.hex in internal Flash.
  5. Run the example

#### <b>MDK-ARM</b>
  1. Open your toolchain
  2. Open Multi-projects workspace file Project.uvmpw

> <b>Optional:</b>
>
> - Select first "Template_XIP_Custom_Boot" workspace
> - Rebuild all files from sub-project Boot
> - If no modification is done on boot project, this step could be skipped.

  3. Select then "Template_XIP_Custom_Appli" workspace
  4. Rebuild all files from sub-project Appli and load your images into memories: This sub-project
     will first load the Appli part in External memory available on STM32H7S78-DK board, then load
     the Boot_XIP_Custom.hex in internal Flash.
  5. Run the example

#### <b>STM32CubeIDE</b>
  1. Open your toolchain
  2. Open Multi-projects workspace file .project

> <b>Optional:</b>
>
> - Select the "Template_XIP_Custom_Boot" project
> - Build the project
> - If the project is not compiled, Appli debugging will manage its compilation
>   for debugging
> - With the debug icon select the configuration "Template_XIP_Boot Debug"�.
>   This operation loads the boot in internal Flash.

  3. Select the "Template_XIP_Appli" project
  4. Build the project
  5. With the Debug icon select the configuration "Template_XIP_Appli Debug". This sub-project
     will first load the Boot binary in internal Flash, then load the Appli binary in
     External memory available on STM32H7S78-DK board
  6. Run the example

#### <b>External Memory Manager trace system</b>
  1. EMM middleware Custom driver allows to enable an internal trace system by activataing the DEBUG_TRACE compilation switch in the Template_XIP_Custom_Boot" workspace.
  2. When this compilation switch is enabled, all EMM debug/trace prints are redirected to STM32H7S78-DK board Virtual COM Port (UART4, PD1/PD0).
  3. During EMM execution, traces could be collected on a PC opening a Serial Port connection on VCP (115200 bauds, 8bit data, no parity, 1 stop bit). 

#### <b>STM32CubeProgrammer</b>

> The ExtMemLoader project uses an environment variable to copy the generated stdlr file inside
> the cube programmer tool.
> This variable is defined by the tool during its installation but if the version used does not
> include this functionality, the environment variable can be declared manually.
>
> STM32_PRG_PATH=C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin

When targeting to build ExtMemLoader to be used in Cubepogrammer, please ensure the following switches are set in C/C++ preprocessor settings:

 - STM32_EXTMEMLOADER_EWARMTARGET_NA
 - STM32_EXTMEMLOADER_STM32CUBETARGET
 - STM32_EXTMEMLOADER_STM32CUBEOPENBLTARGET_NA


#### <b>External memory programming using BootLoader Interfaces</b>

> When using a bootloader interface such as USB or USART, the standard ExtMemLoader cannot be used.
>Instead, a specific ExtMemLoader must be built for use with OpenBootloader. To accomplish this, you should carry out the following steps:
>
>- Modify the target switches in the C/C++ preprocessor settings to:
>
>   - STM32_EXTMEMLOADER_EWARMTARGET_NA
>   - STM32_EXTMEMLOADER_STM32CUBETARGET
>   - STM32_EXTMEMLOADER_STM32CUBEOPENBLTARGET
>- Choose the appropriate file for your linker script based on the file extension (.sct, .ld, or .icf). The file should be renamed to match the following pattern, depending on the extension:
>   * stm32h7rsxx_extmemloader_stm32cubeopenbl.sct
>   * stm32h7rsxx_extmemloader_stm32cubeopenbl.ld
>   * stm32h7rsxx_extmemloader_stm32cubeopenbl.icf
>- The initialization section of extmemloader_init() should be updated as follows:
>
>      #if !defined(STM32_EXTMEMLOADER_STM32CUBEOPENBLTARGET)
>
>      /* Init system */
>      SystemInit();
>
>      /* disable all the IRQ */
>
>      __disable_irq();
>
>      /* MCU Configuration--------------------------------------------------------*/
>
>      /* Enable the CPU Cache */
>
>      /* Enable I-Cache---------------------------------------------------------*/
>
>      SCB_EnableICache();
>
>      /* Enable D-Cache---------------------------------------------------------*/
>
>      SCB_EnableDCache();
>
>      /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
>
>      HAL_Init();
>
>      /* USER CODE BEGIN Init */
>
>      /* USER CODE END Init */
>
>      /* Configure the system clock  */
>
>      SystemClock_Config();
>
>      #else
>      /* Reset of all peripherals, Initializes the Flash interface. */
>
>      __HAL_RCC_SBS_CLK_ENABLE();
>
>      /* System interrupt init*/
>
>      /* Enable the XSPIM_P2 interface */
>
>      HAL_PWREx_EnableXSPIM2();
>
>      /* high speed low voltage config */
>
>      HAL_SBS_EnableIOSpeedOptimize(SBS_IO_XSPI1_HSLV);
>
>      HAL_SBS_EnableIOSpeedOptimize(SBS_IO_XSPI2_HSLV);
>
>      #endif