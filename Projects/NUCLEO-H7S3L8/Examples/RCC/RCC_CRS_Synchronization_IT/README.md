## <b>RCC_CRS_Synchronization_IT example Description</b>

Configuration of the clock recovery system (CRS) in Interrupt mode, using the RCC HAL API.

### <b>Notes</b>

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 600 MHz.

Then, HSI48 and LSE oscillators are enabled.
In this example, as a first step a synchronization miss is raised when using a wrong synchronization
source (GPIO instead of LSE which is not configured). In a 2nd step, a reconfiguration is done to
use LSE and the correct synchronization.

In this test, this is the scenario:

 1. set the source to GPIO and then enable IT

 2. to provoke a SYNC event, call the software event generation function

 3. Interrupt is raised to indicate a SYNC MISS and then power on LD3

 4. in main program, wait for 1 sec and reconfigure source to LSE

 5. LD2 is power on when SYNC WARN is detected

 6. Increase tolerance (FELIM value) when SYNC WARN is detected

 7. At the end of the test LD1 should stay ON

Note: HSI48 frequency can be checked with oscilloscope using MCO PIN PA.08

### <b>Keywords</b>

RCC, System, Clock Configuration, CRS, System clock, HSI48, LSE, Oscillator, Interrupt

### <b>Directory contents</b>

      - RCC/RCC_CRS_Synchronization_IT/Appli/Inc/stm32h7rsxx_nucleo_conf.h     BSP configuration file
      - RCC/RCC_CRS_Synchronization_IT/Appli/Inc/stm32h7rsxx_hal_conf.h    HAL configuration file
      - RCC/RCC_CRS_Synchronization_IT/Appli/Inc/stm32h7rsxx_it.h          Interrupt handlers header file
      - RCC/RCC_CRS_Synchronization_IT/Appli/Inc/main.h                  Header for main.c module
      - RCC/RCC_CRS_Synchronization_IT/Appli/Src/stm32h7rsxx_it.c          Interrupt handlers
      - RCC/RCC_CRS_Synchronization_IT/Appli/Src/main.c                  Main program
      - RCC/RCC_CRS_Synchronization_IT/Appli/Src/system_stm32h7rsxx.c      STM32H7RSxx system source file
      - RCC/RCC_CRS_Synchronization_IT/Appli/Src/stm32h7rsxx_hal_msp.c     HAL MSP module

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
