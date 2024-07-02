## <b>SPI_ACCEL_ComPolling Example Description</b>

This example describes how to perform SPI transmission/reception in Polling mode
to program an Accelerometer. This examples can uses different SPI Accelerometer
from STMicroelectronics.

       Board: STMicroelectronics: NUCLEO-H7S3L8 (embeds a STM32H7S3L8Hx device)
       - CLK  Pin: PA.05 (D13 on Arduino connector)
       - MISO Pin: PA.06 (D12 on Arduino connector )
       - MOSI Pin: PB.05 (D11 on Arduino connector)
       - CS   Pin: PD.14 (D10 on Arduino connector)
       Board: MICKROE - Arduino UNO Click SHIELD   (https://www.mikroe.com/arduino-uno-click-shield)
       Board: MICKROE - LSM6DSL Click (ST LSM6DSL) (https://www.mikroe.com/lsm6dsl-click)
                  or  - Accel 14 Click (ST IIS3DWB)(https://www.mikroe.com/accel-14-click)

This project runs from the external Flash memory. It is launched from a first boot stage
and inherits from this boot project configuration (caches, MPU regions [region 0 and 1],
system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment
via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

The template project calls also SCB_EnableICache() and SCB_EnableDCache() functions
in order to enable the Layer 1 Core Instruction and Data Caches. This is provided as
template implementation that the User may integrate in his application in order to
enhance the performance.

The SPI peripheral configuration is ensured by the HAL_SPI_Init() function.
This later is calling the HAL_SPI_MspInit() function which core is implementing
the configuration of the needed SPI resources according to the used hardware (CLOCK,
GPIO and NVIC). You may update this function to change SPI configuration.

The SPI/ACCEL communication is then initiated.
The HAL_SPI_Transmit() and the HAL_SPI_Receive() functions allow respectively
the transmission of predefined Data to ACCEL and the data reception from ACCEL
to a buffer.

For this example, the aTxCmd is predefined for TX operation and the aRxStatus
is used for RX operation.

Step 1: The SPI sends a WHO_AM_I command to retrieve Accel ID and verify it.

Step 2: The SPI sends command to set Output DataRate of accelerometer.

Step 3: SPI sends a Read status command to get accelerometer status and read status.

Step 4: SPI retrieves Output X_Y_Z data only if new value is available.

Step 5: Enable/Disable LEDs depending on output

NUCLEO-H7S3L8's LEDs are be used to monitor the X_Y_Z acceleration status:

 - LD1 turns ON when X axis is the most significant. LD2 and LD3 are Off
 - LD2 turns ON when Y axis is the most significant. LD1 and LD3 are Off
 - LD3 turns ON when Z axis is the most significant. LD1 and LD2 are Off
 - LD1 is toggle slowly when there is an error in the initialization and transmission process.

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

### <b>Keywords</b>

Connectivity, Communication, SPI, ACCEL, Accelerometer, Polling, Master, Transmission, Reception, MISO, MOSI, CS, MICKROE, LSM6DSL, IIS3DWB

### <b>Directory contents</b>

  - SPI/SPI_ACCEL_ComPolling/Appli/Inc/stm32h7rsxx_nucleo_conf.h     BSP configuration file
  - SPI/SPI_ACCEL_ComPolling/Appli/Inc/stm32h7rsxx_hal_conf.h        HAL configuration file
  - SPI/SPI_ACCEL_ComPolling/Appli/Inc/stm32h7rsxx_it.h              DMA and SPI interrupt handlers header file
  - SPI/SPI_ACCEL_ComPolling/Appli/Inc/main.h                        Header for main.c module
  - SPI/SPI_ACCEL_ComPolling/Appli/Src/stm32h7rsxx_it.c              DMA and SPI interrupt handlers
  - SPI/SPI_ACCEL_ComPolling/Appli/Src/main.c                        Main program
  - SPI/SPI_ACCEL_ComPolling/Appli/Src/system_stm32h7rsxx.c          STM32H7RSxx system source file
  - SPI/SPI_ACCEL_ComPolling/Appli/Src/stm32h7rsxx_hal_msp.c         HAL MSP file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

  - NUCLEO-H7S3L8 Set-up

    - Plug the MICKROE Arduino UNO click SHIELD on the STMicroelectronics NUCLEO-H7S3L8 (Arduino connector).
    - Plug the MIKROELSM6DSL Click (or Accel 14 Click) board in Slot1 of Arduino UNO click SHIELD (mikro BUS connector).
    - Depending on which board is plugged on Slot 1, uncomment define in main.c file:
        - LSM6DSL_ID for MIKROE LSM6DSL click board
        - IIS3DWB_ID for MIKROE Accel14 click board

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

