## <b>STiROT_Appli Template Description</b>

This project provides a STiROT boot path reference template. Boot is performed through STiROT boot path after authenticity and integrity checks of the project firmware image.

This project template is based on the STM32Cube HAL API.

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals and initialize the systick used as 1ms HAL timebase.

The SystemClock_Config() function is used to configure the system clock (SYSCLK).

The system clock frequency is the one configured by STiROT.

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

Reference, Template, STiROT, Boot path, Root Of Trust, MPU

### <b>Directory contents</b>

#### <b>Sub-project STiROT_Appli</b>

File | Description
 --- | ---
  Templates/Template_ROT/STiROT_Appli/Appli/Inc/main.h                      |  Header for main.c module
  Templates/Template_ROT/STiROT_Appli/Appli/Inc/stm32h7rsxx_hal_conf.h      |  HAL Configuration file
  Templates/Template_ROT/STiROT_Appli/Appli/Inc/stm32h7rsxx_it.h            |  Interrupt handlers header file
  Templates/Template_ROT/STiROT_Appli/Appli/Inc/stm32h7s78_discovery_conf.h |  BSP Configuration file
  Templates/Template_ROT/STiROT_Appli/Appli/Src/main.c                      |  Main program
  Templates/Template_ROT/STiROT_Appli/Appli/Src/stm32h7rsxx_hal_msp.c       |  HAL MSP module
  Templates/Template_ROT/STiROT_Appli/Appli/Src/stm32h7rsxx_it.c            |  Interrupt handlers
  Templates/Template_ROT/STiROT_Appli/Appli/Src/system_stm32h7rsxx.c        |  STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S7xx devices.

  - This example has been tested with STMicroelectronics STM32H7S78-DK
    boards and can be easily tailored to any other supported device
    and development board.

  - On STM32H7S78-DK board, the BOOT0 mechanical slide switch must be set to SW1.

### <b>How to use it ?</b>

This project is targeted to boot through <b>STiROT boot path</b>.

<u>Before compiling the project, you should first start the provisioning process</u>. During the provisioning process, the linker file
as well as the postbuild command will be automatically updated.

Before starting the provisioning process, select the application project to use (application example or template),
through stirot_boot_path_project variable in ROT_Provisioning/env.bat or env.sh.
Then start provisioning process by running ROT_Provisioning/STiROT/provisioning.bat or provisioning.sh, and follow its instructions.

If the product state is set to CLOSED, it is still possible to execute a regression
with the Debug Authentication feature. To do it, scripts (regression.bat) are available in the ROT_provisioning/DA folder.

For more details, refer to STM32H7RS Wiki articles:

  - [STiRoT for STM32H7S](https://wiki.st.com/stm32mcu/wiki/Security:STiRoT_for_STM32H7S).
  - [How to start with STiRoT on STM32H7S](https://wiki.st.com/stm32mcu/wiki/Security:How_to_start_with_STiRoT_on_STM32H7S).

#### <b>Notes:</b>

  1. The most efficient way to develop and debug an application is to boot directly on user flash in the Open product state by setting with
     STM32CubeProgrammer the PRODUCT_STATE to OPEN.

  2. Two versions of ROT_AppliConfig are available: windows executable and python version. By default, the windows executable is selected. It
     is possible to switch to python version by:
        - installing python (Python 3.10 or newer) with the required modules listed in requirements.txt.
        ```
        pip install -r requirements.txt
        ```
        - having python in execution path variable
        - deleting main.exe in Utilities\PC_Software\ROT_AppliConfig\dist