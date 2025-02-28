## <b>FatFs_uSD_RTOS application Description</b>

This application provides a description on how to use STM32Cube firmware with FatFs
middleware component as a generic FAT file system module, in order to develop an
application exploiting FatFs offered features with microSD drive in RTOS mode
configuration.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals and initialize the Flash interface.
The HAL initializes the TIM6 to generate an interrupt each 1ms, it will be used as time base for the HAL drivers.
This application uses FreeRTOS, the RTOS initializes the systick to generate an interrupt each 1ms.
The systick is then used for FreeRTOS time base.

The transfer clock is set at 50Mhz (SDMMC_CLK / Clock_DIV * 4) and configured within the hsd1 init parameters.
The application is based on writing and reading back a text file from a drive,
and it's performed using FatFs APIs to access the FAT volume as described in the following steps:

 - Link the uSD/DMA disk I/O driver;
 - Register the file system object (mount) to the FatFs module for the uSD drive;
 - Create a FAT file system (format) on the uSD drive;
 - Create and Open new text file object with write access;
 - Write data to the text file;
 - Close the open text file;
 - Open text file object with read access;
 - Read back data from the text file;
 - Check on read data from text file;
 - Close the open text file;
 - Unlink the uSD disk I/O driver.

The application manages any error occurred during the
access to FAT volume, when using FatFs APIs. Otherwise, user can check if the
written text file is available on the uSD card.

It is possible to fine tune needed FatFs features by modifying defines values
in FatFs configuration file "ffconf.h" available under the project includes
directory, in a way to fit the application requirements.

It is possible to dynamically hotplug/unplug the uSD, as the application is handling
the insert/remove events.

#### <b>Expected success behavior</b>

- A file named STM32.TXT should be visible in the root directory of the SD card.
- A BLUE LED light marks the success of the file operations.
- A RED LED light is ON when SD card is unplugged.

#### <b>Expected error behavior</b>

- On failure, the RED LED is ON while the BLUE LED is switched OFF.
- Error handler is called at the spot where the error occurred.

### <b>Notes</b>

1. for some uSD's, replacing it  while the application is running makes the application fail.
   It is recommended to reset the board using the "Reset button" after replacing the uSD.

2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
   based on variable incremented in HAL time base ISR. This implies that if HAL_Delay() is called from
   a peripheral ISR process, then the HAL time base interrupt must have higher priority (numerically lower)
   than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
   To change the HAL time base interrupt priority you have to use HAL_NVIC_SetPriority() function.

3. The application needs to ensure that the HAL time base is always set to 1 millisecond
   to have correct HAL operation.

4. The FreeRTOS heap size "configTOTAL_HEAP_SIZE" defined in FreeRTOSConfig.h is set according to
   the OS resources memory requirements of the application with +10% margin and rounded to the
   upper Kbyte boundary.

5. If the  application is using the DTCM/ITCM memories (@0x20000000/ 0x0000000: not cacheable and only accessible
   by the Cortex M7 and the MDMA), no need for cache maintenance when the Cortex M7 and the MDMA access these RAMs.
If the application needs to use DMA(or other masters) based access or requires more RAM, then the user has to:
    - Use a non TCM SRAM. (example : D1 AXI-SRAM @ 0x24000000)
    - Add a cache maintenance mechanism to ensure the cache coherence between CPU and other masters(DMAs,DMA2D,LTDC,MDMA).
    - The addresses and the size of cacheable buffers (shared between CPU and other masters)
        must be properly defined to be aligned to L1-CACHE line size (32 bytes).

6. It is recommended to enable the cache and maintain its coherence.
   Depending on the use case it is also possible to configure the cache attributes using the MPU.
      Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
      Please refer to the AN4839 "Level 1 cache on STM32F7 Series and STM32H7 Series"

### <b>Keywords</b>

FatFs, SDMMC, SD Card, File system, FAT Volume, Format, Mount, Read, Write

### <b>Hardware and Software environment</b>

  - This application runs on STM32H7RSxx devices

  - This application has been tested with STM32H7S78-DK boards Revision MB1736-H7S7L8-D01 and can be
    easily tailored to any other supported device and development board.

  STM32H7S78-DK Set-up :

  - Connect the uSD Card to the Micro SD connector (CN13).


### <b>How to use it ?</b>

This application runs from the external Flash memory (FatFs_uSD_RTOS Appli).
It boots from internal Flash (Template_XIP_Boot) then jumps to the application code in external Flash.

To configure STM32CubeIDE Debug Configuration, you must do the following :

    1. Upload the template XIP
    2. Add the adequate external loader (MX66UW1G45G_STM32H7S78-DK.stldr file) in Project->Debugger Configuration
    3. Add in the startup the template_XIP_Boot in Project->Debugger Configuration
    4. Move up the application in the startup

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Plug the SD Card in the STM32H7S78-DK
 - Run the application

