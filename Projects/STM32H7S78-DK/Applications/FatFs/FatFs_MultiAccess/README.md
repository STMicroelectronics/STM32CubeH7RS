## <b>FatFs_MultiAccess FatFs with uSD card drive application Description</b>

This application provides a description on how to use STM32Cube firmware with FatFs
middleware component as a generic FAT file system module, in order to develop an
application exploiting FatFs offered features with microSD drive in RTOS mode
configuration.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals and initialize the Flash interface.
The HAL initializes the TIM6 to generate an interrupt each 1ms, it will be used as time base for the HAL drivers.
This application uses FreeRTOS, the RTOS initializes the systick to generate an interrupt each 1ms.
The systick is then used for FreeRTOS time base.

The SystemCoreClockUpdate() function is used to configure the system clock for STM32H7RSxx Devices :
The CPU at 600 MHz.

The transfer clock is set at 50Mhz (SDMMC_CLK / Clock_DIV * 4) and configured within the hsd_sdmmc1 init parameters.

The application is based on creating multithreaded environment for starting two concurrent threads that handle file operations :
writing and reading back a text file from a drive,
and it's performed using FatFs APIs to access the FAT volume as described in the following steps:

 - Link the FatFs disk I/O driver;
 - Register the file system object (mount) to the FatFs module for the uSD drive;
 - Create and Open new text file object with write access;
 - Write data to the text file;
 - Close the open text file;
 - ConcurrentThread executing steps below:
 - Create and Open new text file object with write access;
 - Write data to the text file;
 - Close the open text file;
 - Unlink the FatFs disk I/O driver.

The application manages any error occurred during the
access to FAT volume, when using FatFs APIs. Otherwise, user can check if the
written text files are available on the uSD card.

It is possible to fine tune needed FatFs features by modifying defines values
in FatFs configuration file "ffconf.h" available under the project includes
directory, in a way to fit the application requirements.

It is possible to fine tune needed FatFs features by modifying defines values
in FatFs configuration file "ffconf.h" available under the project includes
directory, in a way to fit the application requirements.

It is possible to dynamically hot plug/unplug the uSD card, as the application will
handle the plug/unplug events.

STM32H7S78-DK's LED can be used to monitor the application status:
- BLUE LED is ON when the application runs successfully.
- RED LED is ON when SD card is unplugged.
- RED LED is ON when any error occurs.

#### <b>Expected success behavior</b>

- Two files named STM32_1.TXT and STM32_2.TXT should be visible in the root directory of the SD card.
- A BLUE LED light marks the success of the file operations.
- A RED LED light is ON when SD card is unplugged before the application start.

#### <b>Expected error behavior</b>

- On failure, the RED LED is ON while the BLUE LED is switched OFF.
- Error handler is called at the spot where the error occurred.

#### <b>Assumptions if any</b>

- Hot plug is not supported for this application, the SD card is expected to be inserted before application start.
- The board must be reset in case the SD Card is plugged after application start.

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

4. It is recommended to enable the cache and maintain its coherence.
   Depending on the use case it is also possible to configure the cache attributes using the MPU.
   Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
   Please refer to the AN4839 "Level 1 cache on STM32F7 Series and STM32H7 Series"

### <b>Keywords</b>

FatFs, SDMMC, SD Card, File system, FAT Volume, Format, Mount, Read, Write


### <b>Hardware and Software environment</b>

  - This application runs on STM32H7RSxx devices

  - This application has been tested with STM32H7S78-DK boards Revision MB1736-H7S7L8-C01
    and can be easily tailored to any other supported device and development board.

  STM32H7S78-DK Set-up :

  - Connect the uSD Card to the Micro SD connector (CN13).


### <b>How to use it ?</b>

This application runs from the external Flash memory (FatFs_MultiAccess Appli).
It boots from internal Flash (Template_XIP_Boot) then jumps to the application code in external Flash.

To configure STM32CubeIDE Debug Configuration, you must do the following :

    1. Upload the template XIP
    2. Add the adequate external loader (MX66UW1G45G_STM32H7S78-DK.stldr file) in Project->Debugger Configuration
    3. Add in the startup the template_XIP_Boot in Project->Debugger Configuration
    4. Move up the application in the startup

In order to make the program work, you must do the following :

 - Insert a microSD card in the board appropriate slot
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application

