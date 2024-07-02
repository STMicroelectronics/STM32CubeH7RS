## <b>LTDC_ColorKeying Example Description</b>

  This example describe how to enable and use the LTDC color keying functionality.

  At the beginning of the main program the HAL_Init() function is called to reset 
  all the peripherals, initialize the Flash interface and the systick.
  
 This example runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
 configuration (caches, MPU regions [regions 0, 1], system clock at 600 MHz and external memory interface at the highest speed).

 Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

 This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
 the Layer 1 Core Instruction and Data Caches.
 
 After LCD initialization, LCD layer 1 and color keying configuration, an image 
 of size 480x272 in ARGB4444 format featuring ST logo on light blue background (color key ARGB4444==0xFFFF)
 is displayed on LCD.
 Color keying is enabled/disabled on the image when the tamper button is pressed. 
 The effect is that at each button press the light blue background is made transparent or not thanks
 to color keying. At time when the color key is enabled, the background of the logo is the background color
 configured for layer 1, which is all zeroes (black color).
 
 Note that user button press is scanned in polling in the main.c, no use of EXTI in this case.

<b>Color Keying</b>:

 If the Color Keying is enabled, all pixels of the image are compared to that color key pixel value
 (0xFFFF), practically only the background of the logo correspond to that color key in the image. 
 For the pixels that match the color key RGB value, all channels (ARGB) of those pixels 
 are set to 0 (those pixels are then becoming transparent and does not appear on LCD screen, the background
 of image : black is seen instead).

STM32 DK board's LED can be used to monitor the transfer status:

 - LED3 is ON when there is an error in Init process.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
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

Graphics, LTDC, ARGB4444, Pixel format, ColorKeying, Layer, Display

### <b>Directory contents</b>

File | Description
 --- | ---
  LTDC/LTDC_ColorKeying/Appli/Inc/stm32h7rsxx_hal_conf.h  | HAL configuration file
  LTDC/LTDC_ColorKeying/Appli/Inc/stm32h7rsxx_it.h        | Interrupt handlers header file
  LTDC/LTDC_ColorKeying/Appli/Inc/main.h                  | Main configuration file
  LTDC/LTDC_ColorKeying/Appli/Src/stm32h7rsxx_it.c        | Interrupt handlers
  LTDC/LTDC_ColorKeying/Appli/Src/main.c                  | Main program 
  LTDC/LTDC_ColorKeying/Appli/Src/stm32h7rsxx_hal_msp.c   | HAL MSP module	
  LTDC/LTDC_ColorKeying/Appli/Src/system_stm32h7rsxx.c    | STM32H7RSxx system clock configuration file
  LTDC/LTDC_ColorKeying/Appli/Inc/ARGB4444_480x272.h      | Image (single layer 1) of size 480x272 and format ARGB4444 to be displayed on LCD

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S7L8HxH devices.

  - This example has been tested with STM32H7S78-DK board and can be
    easily tailored to any other supported device and development board.    
	
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