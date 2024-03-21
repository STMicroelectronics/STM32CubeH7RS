## <b>RCC_CRS_Synchronization_Polling example Description</b>

Configuration of the clock recovery system (CRS) in Polling mode, using the RCC HAL API.

### <b>Notes</b>

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 600 MHz.

Then, HSI48 and LSE oscillators are enabled. In this example CRS will use LSE
oscillator to calibrate HSI48 frequency.
When HSI48 and LSE are started, automatic calibration starts and program call the
polling function to wait for synchronization status.

In this test, there are results which could occur:

 - few loops occurs up to SYNC OK event (if SYNC WARN, HSI48 trimming value will be
 automatically modified)
 - in case of SYNC MISS or SYNC ERROR, there is an issue with synchronization input
 parameters. In this case, user need to apply new parameters after checking synchronization
 information and restart the synchronization.
 - LD3 power on means that a timeout occurred during polling procedure. It may
 due to an issue with LSE synchronization frequency.
 - LD1 will finally power on if SYNC OK is returned by CRS.

Note: HSI48 frequency can be checked with oscilloscope using MCO PIN


### <b>Keywords</b>

RCC, System, Clock Configuration, CRS, System clock, HSI48, LSE, Oscillator, Polling

### <b>Directory contents</b>

      - RCC/RCC_CRS_Synchronization_Polling/Appli/Inc/stm32h7rsxx_nucleo_conf.h     BSP configuration file
      - RCC/RCC_CRS_Synchronization_Polling/Appli/Inc/stm32h7rsxx_hal_conf.h    HAL configuration file
      - RCC/RCC_CRS_Synchronization_Polling/Appli/Inc/stm32h7rsxx_it.h          Interrupt handlers header file
      - RCC/RCC_CRS_Synchronization_Polling/Appli/Inc/main.h                  Header for main.c module
      - RCC/RCC_CRS_Synchronization_Polling/Appli/Src/stm32h7rsxx_it.c          Interrupt handlers
      - RCC/RCC_CRS_Synchronization_Polling/Appli/Src/main.c                  Main program
      - RCC/RCC_CRS_Synchronization_Polling/Appli/Src/system_stm32h7rsxx.c      STM32H7RSxx system source file
      - RCC/RCC_CRS_Synchronization_Polling/Appli/Src/stm32h7rsxx_hal_msp.c     HAL MSP module

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

  - NUCLEO-H7S3L8 Set-up
    - Connect the MCO pin to an oscilloscope to monitor HSI48 frequency:
      - PA.08: connected to pin 23 of CN12 connector for Nucleo-144 (MB1737)

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
