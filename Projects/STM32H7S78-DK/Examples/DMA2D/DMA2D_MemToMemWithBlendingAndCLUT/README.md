## <b>DMA2D_MemToMemWithBlendingAndCLUT Example Description</b>

This example provides a description of how to configure DMA2D peripheral in 
Memory_to_Memory blending transfer mode with indexed 256 color images (L8).

- This project provides a reference template based on the STM32Cube HAL API that can be used
to build any firmware application to execute from external Flash (Sub-project Appli). It boots from internal Flash and jumps
to the application code in external Flash (Sub-project Boot).
- This project is targeted to run on STM32H7S7xx device on STM32H7S78-DK board from STMicroelectronics.
At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals and initialize the systick used as 1ms HAL timebase.
- This project runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [regions 0, 1 and 2 here], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.
- The project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

The following example performs an infinite loop, executing the following actions repeatedly:

  - Configure the LTDC driver using MX_LTDCConfig to display the content of the LTDC frame buffer in one layer
  - Configure the foreground layer of the DMA2D in Memory_to_Memory mode with Pixel Format Conversion from L8 to ARGB8888
  - Load the L8_CLUT_1 CLUT (color index table for image L8_Image1) from to DMA2D Foreground CLUT and wait for CLUT loading ends.
  - Start the DMA2D transfer where source address is L8_Image1 and destination address is the LTDC display frame buffer.
  - Wait for 2 sec before next stage
  - Load the L8_CLUT_0 CLUT (color index table for image L8_Image0) from flash to DMA2D Foreground CLUT and wait for CLUT loading ends.
  - Start the DMA2D transfer where source address is L8_Image1 and destination address is the LTDC display frame buffer.
  - Wait for 2 sec before next stage
  - Configure the foreground and background layers of the DMA2D in Memory_to_Memory_with_blending mode
  - Configure the output color format as ARGB8888 and the input pixel formats as L8.
  - Set the Transparency factor for the foreground layer and background layers to 0x7F (semi-transparent)
  - Load the L8_CLUT_1 CLUT (color index table for image L8_Image1) from flash to DMA2D Foreground CLUT and wait for CLUT loading ends.
  - Load the L8_CLUT_0 CLUT (color index table for image L8_Image) from flash to DMA2D Background CLUT and wait for CLUT loading ends.
  - Start the DMA2D transfer with blending where:
     - source address 1 : for foreground is L8_Image1
     - source address 2 : for background is L8_Image0      
     - destination address is the LTDC display frame buffer.
  - Wait for 2 sec before restarting the loop

The DMA2D blends the two sources pixels from FLASH to internal FIFOs to compute:

- The resulting pixel in dedicated output buffer (using foregound and background CLUTs to convert L8 source images to ARGB8888 blend image). 
- The transferred data to the output memory address is the result of the blending operation between background and foreground.
  
  Files L8_Image1.h and L8_Image0.h provides the definition of the L8 images where:
  
  - L8_CLUT_x is the 256 colors CLUT for L8_Imagex : color palette (Color index tables).
  - L8_Imagex is the 256 colors image itself. each pixel is coded on one byte.

STM32H7S78-DK board's LEDs can be used to monitor the transfer status:

 - [**LD1**]{style="color: GREEN"} is ON when the DMA2D transfer is complete.
 - [**LD2**]{style="color: ORANGE"} is ON when there is a DMA2D transfer error.
 - [**LD3**]{style="color: RED"} is ON when there is an error in LTDC transfer/Init process.


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
   Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs".
   Please refer to the AN4839 "Level 1 cache on STM32F7 Series".

### <b>Keywords</b>

Graphics, DMA2D, GPIO, LTDC, LCD Glass

### <b>Directory contents</b>

File                                                                                  | Description
 ---                                                                                  | ---
  DMA2D/DMA2D_BlendingWithAlphaInversion/Appli/Src/main.c                             | Main program
  DMA2D/DMA2D_BlendingWithAlphaInversion/Appli/Src/system_stm32h7rsxx.c               | STM32H7RSxx system clock configuration file
  DMA2D/DMA2D_BlendingWithAlphaInversion/Appli/Src/stm32h7rsxx_it.c                   | Interrupt handlers
  DMA2D/DMA2D_BlendingWithAlphaInversion/Appli/Src/stm32h7rsxx_hal_msp.c              | HAL MSP module
  DMA2D/DMA2D_BlendingWithAlphaInversion/Appli/Inc/main.h                             | Main program header file
  DMA2D/DMA2D_BlendingWithAlphaInversion/Appli/Inc/stm32h7s78_discovery_conf.h        | BSP Configuration file
  DMA2D/DMA2D_BlendingWithAlphaInversion/Appli/Inc/stm32h7rsxx_hal_conf.h             | HAL Configuration file
  DMA2D/DMA2D_BlendingWithAlphaInversion/Appli/Inc/stm32h7rsxx_it.h                   | Interrupt handlers header file
  DMA2D/DMA2D_BlendingWithAlphaInversion/Appli/Inc/L8_Image1.h                        | L8_Image1 definition
  DMA2D/DMA2D_BlendingWithAlphaInversion/Appli/Inc/L8_Image0.h                        | L8_Image0 definition

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S7xx devices.

  - This example has been tested with STMicroelectronics STM32H7S78-DK
    boards and can be easily tailored to any other supported device
    and development board.

  - On STM32H7S78-DK board, the **SW1** mechanical slide switch must be set to **BOOT1**.

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