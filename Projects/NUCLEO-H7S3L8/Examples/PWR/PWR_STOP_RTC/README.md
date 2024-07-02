## <b>PWR_STOP_RTC Example Description</b>

How to enter the Stop mode and wake-up from this mode by using an external
reset or the RTC wakeup timer.

The RTC wakeup timer event is connected to EXTI_Line19.

This example runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

Two leds LD1 and LD3 are used to monitor the system state as follows:

 - LD1 toggling: system in Run mode
 - LD1 off : system in Stop mode
 - LD3 on: configuration failed (system in an infinite loop)

In the associated software:

  - The Low Speed External (LSE) clock is used as RTC clock source by default.

  - The EXTI_Line19 connected internally to the RTC Wakeup event is configured
    to generate an interrupt on rising edge each 25s.

  - The SysTick is programmed to generate an interrupt each 1 ms.

The system automatically enters Stop mode 5s after start-up. 
The RTC wake-up is configured to generate an interrupt on rising edge 20s afterwards. 
After the system woken up from Stop, the clock system is reconfigured because the default clock
after wake up is the HSI.
The software then comes back in run mode for 5s before automatically entering Stop mode again.

Note: To measure the current consumption in stop mode, remove JP2 jumper
      and connect an amperemeter to JP2 to measure IDD current.

The above scenario is repeated in an infinite loop.

    - Stop Mode with RTC clocked by LSE
    ===================================
            - RTC Clocked by LSE
            - Regulator in LP mode
            - VREFINT OFF with fast wakeup enabled
            - HSI as SysClk after Wake Up
            - No IWDG
            - Automatic Wakeup using RTC on LSE each 25s (with 5s in run mode and 20s in Stop mode)

@note This example can not be used in DEBUG mode due to the fact
      that the Cortex-M7 core is no longer clocked during low power mode
      so debugging features are disabled.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

 3. Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select
    the RTC clock source; in this case the Backup domain will be reset in
    order to modify the RTC Clock source, as consequence RTC registers (including
    the backup registers) and RCC_CSR register are set to their reset values.

### <b>Keywords</b>

Power, PWR, Stop mode, Interrupt, EXTI, Wakeup, Low Power, RTC, External reset, LSE.

### <b>Directory contents</b>

  - PWR/PWR_STOP_RTC/Appli/Inc/stm32h7rsxx_conf.h         HAL Configuration file
  - PWR/PWR_STOP_RTC/Appli/Inc/stm32h7rsxx_nucleo_conf.h  BSP configuration file
  - PWR/PWR_STOP_RTC/Appli/Inc/stm32h7rsxx_it.h           Header for stm32h7rsxx_it.c
  - PWR/PWR_STOP_RTC/Appli/Inc/main.h                     Header file for main.c
  - PWR/PWR_STOP_RTC/Appli/Src/system_stm32h7rsxx.c       STM32H7RSxx system clock configuration file
  - PWR/PWR_STOP_RTC/Appli/Src/stm32h7rsxx_it.c           Interrupt handlers
  - PWR/PWR_STOP_RTC/Appli/Src/main.c                     Main program
  - PWR/PWR_STOP_RTC/Appli/Src/stm32h7rsxx_hal_msp.c      HAL MSP module

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices

  - This example has been tested with STMicroelectronics NUCLEO-H7S3L8
    board and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

  - NUCLEO-H7S3L8 Set-up
    - Use LD1 and LD3 connected respectively to PD.10 and PB.07 pins

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
 - Unplug then Plug STLINK connection to perform a power-on-reset
