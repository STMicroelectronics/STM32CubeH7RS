## <b>LTDC_Display_2Layers Example Description</b>

This example describes how to configure the LTDC peripheral to display two Layers  
at the same time.

The goal of this example is to explain how to blend two layers and how to display 
an image with L8 (8bits per pixels indexed) as pixel format. 

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.

This example runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [regions 0, 1], system clock at 600 MHz and external memory interface at the highest speed).

Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

After LCD initialization, the LCD layer 1 and Layer 2 are configured as following :

  - Layer 1 is configured to display an image loaded from flash memory with direct
    color (RGB565) as pixel format and 320x240 (QVGA) size.
  - Layer 2 is configured to display an image loaded from flash memory with indirect
    color (L8) as pixel format and 320x240 (QVGA) size.
    To display an image with an indirect color as pixel format a color lookup table (CLUT) 
    is loaded, then every byte from image data (L8_320x240) is considered as 
    a position in color lookup table (CLUT).(see example below)  

The blending is always active and the two layers can be blended following 
the configured blending factors and the constant alpha.

In this example the constant alpha for layer 2 is decreased to see the layer 1
in the intersection zone.

  Example :
  
|Image data | position   CLUT(RGB Value) | Alpha | Output (ARGB value) |
| --- | --- | --- | --- |
| 00 10 FF 25 |  0   -----> 0xFD10EA | 0xFF | 0xFFFD10EA |
|             |  .                   |      | 0xFF1548AD |
|             |  .                   |      | 0xFFAE6547 |
|             |  .                   |      | 0xFFDA14EA |
|             |  .                   |      | . |
|             |  16  -----> 0x1548AD |      | . |
|             |  .                   |      | . |
|             |  .                   |      | . |
|             |  37  -----> 0xDA14EA |      | . |
|             |  .                   |      | . |
|             |  .                   |      | . |
|             |  255 -----> 0xAE6547 |      | . |


The images change unlike their positions on LCD in an infinite loop.

STM32 Discovery board's LED can be used to monitor the transfer status:

 - LED3 is ON when there is an error in Init process.

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
	(a proposed dma_buffer section is available from CMSIS Device linker template file and its size must
    be adapted to the application requirements)
    - or to ensure cache maintenance operations to ensure the cache coherence between the CPU and the DMAs.
    This is true also for any other data buffers accessed by the CPU and other masters (DMA2D, LTDC)
    The addresses and the size of cacheable buffers (shared between CPU and other masters)
    must be properly defined to be aligned to data cache line size (32 bytes) and of a size of being multiple
    of this cache line size.
    Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
    Please refer to the AN4839 "Level 1 cache on STM32F7 Series"

### <b>Keywords</b>

Graphics, LTDC, RGB565, L8, Pixel format, Layer1, Layer2, CLUT, Display

### <b>Directory contents</b>

    - LTDC/LTDC_Display_2Layers/Appli/Inc/stm32h7rsxx_hal_conf.h   HAL configuration file
    - LTDC/LTDC_Display_2Layers/Appli/Inc/stm32h7rsxx_it.h         Interrupt handlers header file
    - LTDC/LTDC_Display_2Layers/Appli/Inc/main.h                   Main configuration file
    - LTDC/LTDC_Display_2Layers/Appli/Src/stm32h7rsxx_it.c         Interrupt handlers
    - LTDC/LTDC_Display_2Layers/Appli/Src/main.c                   Main program 
    - LTDC/LTDC_Display_2Layers/Appli/Src/stm32h7rsxx_hal_msp.c    HAL MSP module	
    - LTDC/LTDC_Display_2Layers/Appli/Src/system_stm32h7rsxx.c     STM32H7RSxx system clock configuration file
    - LTDC/LTDC_Display_2Layers/Appli/Inc/RGB565_320x240.h         Image layer 1 to be displayed on LCD : QVGA (320x240) in RGB565
    - LTDC/LTDC_Display_2Layers/Appli/Inc/L8_320x240.h             Image layer 2 to be displayed on LCD QVGA (320x240) with CLUT8 format.

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


