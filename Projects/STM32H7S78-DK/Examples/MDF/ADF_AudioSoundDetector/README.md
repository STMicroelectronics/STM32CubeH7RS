## <b>ADF_AudioSoundDetector Example Description</b>

How to use the MDF HAL API (ADF instance) to use audio sound activity detection.

This project is targeted to run on STM32H7S7xx device mounted on STM32H7S78-DK board.

This project runs from the external Flash memory.
It is launched from a first boot stage and inherits from this boot project configuration
(caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board
project Templates/Template_XIP/Binary/Boot_XIP.hex file.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

For audio sound recording, digital microphone mounted on the board will be used (CN1).
Digital microphone runs with a typical clock frequency around 2.4MHz.
This clock frequency should be delivered by ADF1.
PLL3 will be used and configured to generate a frequency of 49.142MHZ on PLL3P.
PLL3P will be selected as ADF1 kernel clock and ADF1 will be configured to generate a clock of 2.047MHZ
on ADF1_CCK0 (PE2).
ADF1 processing clock divider will be set to 2 and output clock divider will be set to 12 (49.142/(2*12) = 2.047).
Please note that CubeMX doesn't generate PLL clock configuration for project on external memory so PLL3 configuration
will be implemented on user section of HAL_MDF_MspInit function.

ADF1_DATIN0 (PD6) will be used to get PDM audio stream from digital microphone.
ADF1 bitstream will be configured to take data from this input on falling edge.
ADF1 serial interface 0 will be configured to operate in SPI mode, with a threshold of 31 and with ADF1_CCK0 as clock.

ADF filter 0 will be used to perform conversions.
Following programming recommendations on reference manual for sound activaty detection,
SINC5 filter will be used with decimation ratio at 16.
Reshape filter will also be activated with a decimation at 4 in order to have an audio sample frequency
at 32KHz (2.047MHz/(16*4)).
High pass filter will also be activated with a cut-off frequency of 0.00250 x FPCM.

Sound Activity Detector (SAD) will be activated in voice activity detector mode.
SAD will be configured with a frame size of 8 PCM samples, a minimum noise level of 200, hysteresis enable,
a hangover window of 512 frames, 2 learning frames and a signal noise ratio of 6dB.
Sound activity detection interrupt will be rising when SAD will enter in detect mode and green LED will be toggeled.
Data memory transfer to record buffer will occur only when SAD will be on detect state and blue LED will be toggeled
during this period.

STM32H7S78-DK board's LEDs are used to monitor the example status:

  - Green LED toggles as soon as an SAD detection occurs.
  - Blue LED is toggling during memory data record in SAD detect state.
  - Red LED turns on when any error occurs.

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
    must be properly defined to be aligned to data cache line size (32 bytes) and of a size of being multiple
    of this cache line size.
    Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
    Please refer to the AN4839 "Level 1 cache on STM32F7 Series"

### <b>Execution:</b>

The program configures system clocks, GPIO and DMA channel used by ADF.
Then, program performs the following sequence:

  - Initialize ADF.
  - Start ADF conversion in DMA mode (linked list circular mode) with SAD enabled.
  - Make some noise and wait for SAD interrupt.

### <b>Keywords</b>

Audio, MDF, ADF, DMA, SAD

### <b>Directory contents</b>

  - MDF/ADF_AudioSoundDetector/Appli/Src/main.c                      Main program
  - MDF/ADF_AudioSoundDetector/Appli/Src/stm32h7rsxx_hal_msp.c       HAL MSP module
  - MDF/ADF_AudioSoundDetector/Appli/Src/stm32h7rsxx_it.c            Interrupt handlers
  - MDF/ADF_AudioSoundDetector/Appli/Src/system_stm32h7rsxx.c        STM32H7RSxx system source file
  - MDF/ADF_AudioSoundDetector/Appli/Inc/main.h                      Main program header file
  - MDF/ADF_AudioSoundDetector/Appli/Inc/stm32h7s78_discovery_conf.h STM32H7S78-DK board configuration file
  - MDF/ADF_AudioSoundDetector/Appli/Inc/stm32h7rsxx_hal_conf.h      HAL configuration file
  - MDF/ADF_AudioSoundDetector/Appli/Inc/stm32h7rsxx_it.h            Interrupt handlers header file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S7xx devices.

  - This example has been tested with STMicroelectronics STM32H7S78-DK
    boards and can be easily tailored to any other supported device
    and development board.

  - On STM32H7S78-DK board, the BOOT0 mechanical slide switch must be set to SW1.

  - User Option Bytes requirement (with STM32CubeProgrammer tool):

    - XSPI1_HSLV=1     I/O XSPIM_P1 High speed option enabled
    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled
    - VDDIO_HSLV=0     I/O speed optimized to operate at 3.3V

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

