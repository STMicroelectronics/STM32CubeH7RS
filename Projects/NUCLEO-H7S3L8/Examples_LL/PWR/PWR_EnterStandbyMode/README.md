## <b>PWR_EnterStandbyMode Example Description</b>

How to enter the Standby mode and wake up from this mode by using an external
reset or a wakeup pin.

In the associated software, the system clock is set to 600 MHz.

An EXTI line is connected to the USER push-button through PC.13 and configured to generate an
interrupt on Falling edge upon key press.

Request to enter in standby mode:

When a Falling edge is detected on the EXTI line, an interrupt is generated.

In the EXTI handler routine, the wake-up pin LL_PWR_WAKEUP_PIN3 is enabled and the
corresponding wake-up flag cleared. Then, the system enters Standby mode causing
LD1 to stop toggling.

Exit from Standby mode:

The user can wake-up the system in pressing the USER push-button which is
connected to the wake-up pin LL_PWR_WAKEUP_PIN3.

A Falling edge on WKUP pin will wake-up the system from Standby.
Alternatively, an external reset (reset button on board) will wake-up system from Standby
as well as wake-up pin.

After wake-up from Standby mode, program execution restarts in the same way as
after a reset and LD1 restarts toggling.

These steps are repeated in an infinite loop.

LD1 is used to monitor the system state as follows:

 - LD1 fast toggling (each 200ms): system in Run mode from reset (power on reset or reset pin while system was in Run mode)
 - LD1 slow toggling (each 500ms): system in Run mode after exiting from Standby mode
 - LD1 off: system in Standby mode

#### <b>Notes</b>

 1. This example is not meant to reflect typical product power consumption values but shows how to use the software to switch from power modes.

 2. This example can not be used in DEBUG mode due to the fact
    that the Cortex-M7 core is no longer clocked during low power mode
    so debugging features are disabled.

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

Power, PWR, Standby mode, Interrupt, EXTI, Wakeup, Low Power, External reset,

### <b>Directory contents</b>

  - PWR/PWR_EnterStandbyMode/Boot/Inc/stm32h7rsxx_it.h          Interrupt handlers header file
  - PWR/PWR_EnterStandbyMode/Boot/Inc/main.h                    Header for main.c module
  - PWR/PWR_EnterStandbyMode/Boot/Inc/stm32_assert.h            Template file to include assert_failed function
  - PWR/PWR_EnterStandbyMode/Boot/Src/stm32h7rsxx_it.c          Interrupt handlers
  - PWR/PWR_EnterStandbyMode/Boot/Src/main.c                    Main program
  - PWR/PWR_EnterStandbyMode/Boot/Src/system_stm32h7rsxx.c      STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with STMicroelectronics NUCLEO-H7S3L8
    board and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-H7S3L8 Set-up
    - LD1 connected to PD.10 pin
    - User push-button connected to pin PC.13 (External line 13)
    - WakeUp Pin LL_PWR_WAKEUP_PIN3 connected to PC.13

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