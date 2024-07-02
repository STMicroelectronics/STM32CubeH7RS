## <b>Templates_XIP Example Description</b>

- This project provides a reference template based on the STM32Cube HAL API that can be used
to build any firmware application to execute from external Flash (Sub-project Appli). It boots from internal Flash
and jumps to the application code in external Flash (Sub-project Boot).
- This project is targeted to run on STM32H7S3xx device on NUCLEO-H7S3L8 board from STMicroelectronics.
At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals and initialize the systick used as 1ms HAL timebase.
- This project runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [regions 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.
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

Reference, Template, Boot, Loader, XiP

### <b>Directory contents</b>

#### <b>Sub-project Boot</b>

File | Description
 --- | ---
  Templates/Template_XIP/Boot/Inc/main.h                       |  Header for main.c module
  Templates/Template_XIP/Boot/Inc/extmem_manager.h             |  Header for extmem_manager.c module
  Templates/Template_XIP/Boot/Inc/stm32h7rsxx_hal_conf.h       |  HAL Configuration file
  Templates/Template_XIP/Boot/Inc/stm32h7rsxx_it.h             |  Interrupt handlers header file
  Templates/Template_XIP/Boot/Inc/stm32h7rsxx_nucleo_conf.h    |  BSP Configuration file
  Templates/Template_XIP/Boot/Inc/stm32_extmem_conf.h          |  External memory manager Configuration file
  Templates/Template_XIP/Boot/Src/main.c                       |  Main program
  Templates/Template_XIP/Boot/Src/extmem_manager.c             |  code to initialize external memory
  Templates/Template_XIP/Boot/Src/stm32h7rsxx_hal_msp.c        |  HAL MSP module
  Templates/Template_XIP/Boot/Src/stm32h7rsxx_it.c             |  Interrupt handlers
  Templates/Template_XIP/Boot/Src/system_stm32h7rsxx.c         |  STM32H7RSxx system source file

#### <b>Sub-project Appli</b>

File | Description
 --- | ---
  Templates/Template_XIP/Appli/Inc/main.h                      |  Header for main.c module
  Templates/Template_XIP/Appli/Inc/stm32h7rsxx_hal_conf.h      |  HAL Configuration file
  Templates/Template_XIP/Appli/Inc/stm32h7rsxx_it.h            |  Interrupt handlers header file
  Templates/Template_XIP/Appli/Inc/stm32h7rsxx_nucleo_conf.h   |  BSP Configuration file
  Templates/Template_XIP/Appli/Src/main.c                      |  Main program
  Templates/Template_XIP/Appli/Src/stm32h7rsxx_hal_msp.c       |  HAL MSP module
  Templates/Template_XIP/Appli/Src/stm32h7rsxx_it.c            |  Interrupt handlers
  Templates/Template_XIP/Appli/Src/system_stm32h7rsxx.c        |  STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This template runs on STM32H7S3xx devices.

  - This template has been tested with STMicroelectronics NUCLEO-H7S3L8
    boards and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool):
    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

#### <b>IAR</b>
  1. Open your toolchain
  2. Open Multi-projects workspace file Project.eww

> <b>Optional:</b>
>
> - Select first "Template_XIP_Boot" workspace
> - Rebuild all files from sub-project Boot
> - If no modification is done on boot project, this step could be skipped.

  3. Select then "Template_XIP_Appli" workspace
  4. Rebuild all files from sub-project Appli and load your images into memories: This sub-project
     will first load the Appli part in External memory available on NUCLEO-H7S3L8 board, then load
     the Template_XIP_Boot.hex in internal Flash.
  5. Run the example

NB: For this target, following switches are set in C/C++ preprocessor settings:

 - STM32_EXTMEMLOADER_EWARMTARGET
 - STM32_EXTMEMLOADER_STM32CUBETARGET_NA
 - STM32_EXTMEMLOADER_STM32CUBEOPENBLTARGET_NA

#### <b>MDK-ARM</b>
  1. Open your toolchain
  2. Open Multi-projects workspace file Project.uvmpw

> <b>Optional:</b>
>
> - Select first "Template_XIP_Boot" workspace
> - Rebuild all files from sub-project Boot
> - If no modification is done on boot project, this step could be skipped.

  3. Select then "Template_XIP_Appli" workspace
  4. Rebuild all files from sub-project Appli and load your images into memories: This sub-project
     will first load the Appli part in External memory available on NUCLEO-H7S3L8 board, then load
     the Boot_XIP.hex in internal Flash.
  5. Run the example

NB: For this target, following switches are set in C/C++
preprocessor settings:

 - STM32_EXTMEMLOADER_MDKARMTARGET
 - STM32_EXTMEMLOADER_STM32CUBETARGET_NA
 - STM32_EXTMEMLOADER_STM32CUBEOPENBLTARGET_NA

#### <b>STM32CubeIDE</b>
  1. Open your toolchain
  2. Open Multi-projects workspace file .project

> <b>Optional:</b>
>
> - Select the "Template_XIP_Boot" project
> - Build the project
> - If the project is not compiled, Appli debugging will manage its compilation
>   for debugging
> - With the debug icon select the configuration “Template_XIP_Boot Debug”.
>   This operation loads the boot in internal Flash.

  3. Select the "Template_XIP_Appli" project
  4. Build the project
  5. With the Debug icon select the configuration “Template_XIP_Appli Debug”. This sub-project
     will first load the Boot binary in internal Flash, then load the Appli binary in
     External memory available on NUCLEO-H7S3L8 board
  6. Run the example


#### <b>STM32CubeProgrammer</b>

> The ExtMemLoader project uses an environment variable to copy the generated stdlr file inside
> the cube programmer tool.
> This variable is defined by the tool during its installation but if the version used does not
> include this functionality, the environment variable can be declared manually.
>
> STM32_PRG_PATH=C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin

When targeting to build ExtMemLoader to be used in Cubepogrammer, please ensure
the following switches are set in C/C++ preprocessor settings:

 - STM32_EXTMEMLOADER_EWARMTARGET_NA
 - STM32_EXTMEMLOADER_STM32CUBETARGET
 - STM32_EXTMEMLOADER_STM32CUBEOPENBLTARGET_NA


#### <b>External memory programming using BootLoader Interfaces</b>

> When using a bootloader interface such as USB or USART, the standard
ExtMemLoader cannot be used.
>Instead, a specific ExtMemLoader must be built for use with OpenBootloader. To
accomplish this, you should carry out the following steps:
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
> #if !defined STM32_EXTMEMLOADER_STM32CUBEOPENBLTARGET)
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
>      HAL_SBS_EnableIOSpeedOptimize(SBS_IO_XSPI2_HSLV);
>
>      #endif