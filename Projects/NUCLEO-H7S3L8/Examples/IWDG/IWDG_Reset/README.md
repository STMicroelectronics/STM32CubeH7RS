## <b>IWDG_Reset Example Description</b>

How to handle the IWDG reload counter and simulate a software fault that generates
an MCU IWDG reset after a preset laps of time.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 600 MHz.

This project runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

The IWDG timeout is set to 1 second.

First, the TIM15 timer is configured to measure the LSI frequency as the
LSI is internally connected to TIM15 CH1, in order to adjust the IWDG clock.

The LSI measurement using the TIM15 is described below:

  - Configure the TIM15 to remap internally the TIM15 CH1 Input Capture to the LSI
    clock output.
  - Enable the TIM15 Input Capture interrupt: after one cycle of LSI clock, the
    period value is stored in a variable and compared to the HCLK clock to get
    its real value.

Then, the IWDG reload counter is configured as below to obtain 1 second according
to the measured LSI frequency after setting the prescaler value:

    IWDG counter clock Frequency = LSI Frequency / Prescaler value

The IWDG reload counter is refreshed each 990 ms in the main program infinite
loop to prevent a IWDG reset.

LD2 is also toggling each 990 ms indicating that the program is running.

An EXTI Line is connected to a GPIO pin, configured to generate an interrupt
when the USER push-button (PC.13) is pressed.

The EXTI Line is used to simulate a software failure: once the EXTI Line event
occurs by pressing the USER push-button (PC.13), the corresponding interrupt is served.

In the ISR, a write to invalid address generates a Hardfault exception
containing an infinite loop and preventing to return to main program (the IWDG
reload counter is not refreshed).
As a result, when the IWDG counter reaches 0, the IWDG reset occurs.

If the IWDG reset is generated, after the system resumes from reset, LD1 turns on for 4 seconds approximately 3 seconds after the USER push-button is pressed.
If the EXTI Line event does not occur, the IWDG counter is indefinitely refreshed in the main
program infinite loop, and there is no IWDG reset.

LD3 will turn on if any error occurs.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate
    delay (in milliseconds) based on variable incremented in SysTick ISR. This
    implies that if HAL_Delay() is called from a peripheral ISR process, then
    the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
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
    - Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
    - Please refer to the AN4839 "Level 1 cache on STM32F7 Series"

### <b>Keywords</b>

System, IWDG, reload counter, MCU Reset, Timeout, Software fault

### <b>Directory contents</b>

  - IWDG/IWDG_Reset/Appli/Inc/stm32h7rsxx_nucleo_conf.h     BSP configuration file
  - IWDG/IWDG_Reset/Appli/Inc/stm32h7rsxx_hal_conf.h    HAL configuration file
  - IWDG/IWDG_Reset/Appli/Inc/stm32h7rsxx_it.h          Interrupt handlers header file
  - IWDG/IWDG_Reset/Appli/Inc/main.h                  Header for main.c module
  - IWDG/IWDG_Reset/Appli/Src/stm32h7rsxx_it.c          Interrupt handlers
  - IWDG/IWDG_Reset/Appli/Src/main.c                  Main program
  - IWDG/IWDG_Reset/Appli/Src/stm32h7rsxx_hal_msp.c     HAL MSP file
  - IWDG/IWDG_Reset/Appli/Src/system_stm32h7rsxx.c      STM32H7RSxx system source file


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
