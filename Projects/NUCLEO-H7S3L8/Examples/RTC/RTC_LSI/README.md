## <b>RTC_LSI Example Description</b>

Use of the LSI clock source autocalibration to get a precise RTC clock.

In the associated software, the system clock is set to 600 MHz.

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals and initialize the systick used as 1ms HAL timebase.

This example runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).

Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

As an application example, it demonstrates how to configure the TIM12 timer
internally connected to LSI clock output, in order to adjust the RTC prescaler.

The Low Speed Internal (LSI) clock is used as RTC clock source.

After reset, the RTC prescaler is set with the default LSI frequency (32 kHz).

The RTC WakeUp is configured to generate an interrupt each 1s.

A key value is written in backup data register 1 to indicate if the wakeup timer has
already been enabled. If so the wakeup timer is disabled prior configuring the RTC
automatic wakeup.

LD1 is toggled inside the RTC WakeUp interrupt each 1s.

The inaccuracy of the LSI clock causes the RTC WakeUp Interrupt to be inaccurate.

The RTC CK_SPRE signal can be monitored by LD1 which is toggled into the RTC
Wakeup interrupt service routine.

The program waits until USER push-button is pressed to begin the auto calibration.

Procedure:

 - Configure the TIM12 to remap internally the TIM12 TIM_CHANNEL_1
   Input Capture to the LSI clock output.

 - Enable the TIM12 Input Capture interrupt: after one cycle of LSI clock, the
   period value is stored in a variable and compared to the timer kernel clock
   (tim_ker_ck) to get its real value.

 - The RTC prescaler is adjusted with this LSI frequency value so that the RTC
   CK_SPRE value become more accurate.

 - When calibration is done the variable uwLsiFreq is visualized into the debugger
   to indicate the end of this operation.

 - The LSI value is updated and displayed on the debugger in uwLsiFreq variable.

LD3 is On: This indicates that the system generates an error.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate
    delay (in milliseconds) based on variable incremented in SysTick ISR. This
    implies that if HAL_Delay() is called from a peripheral ISR process, then
    the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
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

System, RTC, LSI, Internal time stamp, Wake up, interrupt

### <b>Directory contents</b>

  - RTC/RTC_LSI/Appli/Inc/stm32h7rsxx_nucleo_conf.h                BSP configuration file
  - RTC/RTC_LSI/Appli/Inc/stm32h7rsxx_hal_conf.h                   HAL configuration file
  - RTC/RTC_LSI/Appli/Inc/main.h                                   Header file for main.c
  - RTC/RTC_LSI/Appli/Inc/stm32h7rsxx_it.h                         Header for stm32h7rsxx_it.c
  - RTC/RTC_LSI/Appli/Src/system_stm32h7rsxx.c                     STM32H7RSxx system clock configuration file
  - RTC/RTC_LSI/Appli/Src/stm32h7rsxx_it.c                         Interrupt handlers
  - RTC/RTC_LSI/Appli/Src/main.c                                   Main program
  - RTC/RTC_LSI/Appli/Src/stm32h7rsxx_hal_msp.c                    HAL MSP module

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8  board and can be
    easily tailored to any other supported device and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

  - NUCLEO-H7S3L8 Set-up

    - Use LD1 connected to PD.10 pin (pin 12 in CN7 connector).
    - Use the USER push-button connected to pin PC.13 (External line 13)

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
