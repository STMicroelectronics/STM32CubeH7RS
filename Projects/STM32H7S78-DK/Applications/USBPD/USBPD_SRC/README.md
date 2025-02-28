
## <b>USBPD_SRC Application Description</b>

This application is a USBPD type C Provider using FreeRTOS.
USBPD Provider: This application initialize the type C port 1 in source mode with only one PDO at 5V.

####  <b>Expected success behavior</b>
Connect UCPD cube Monitor on the VCP associated to our board (only available if USB cable is connected)
The UCPD Cube Monitor will be used to trace all the messages exchange between the ports partner.

#### <b>Error behaviors</b>

Errors are detected and the corresponding message is displayed on the UCPD Cube Monitor.

#### <b>Assumptions if any</b>

User is familiar with USB-PD 3.1 "Universal Serial BUS Power Delivery" Specification.

#### <b>Known limitations</b>
None.

### <b>Keywords</b>

Connectivity, USBPD, FreeRTOS, UART/USART, USB_PD_Lib, UCPD, Type C

### <b>Hardware and Software environment</b>

  - This application runs on STM32H7S78-DK.

  - This application has been tested with STMicroelectronics STM32H7S78-DK boards Revision MB1736-H7S7L8-D01
    and can be easily tailored to any other supported device
    and development board.

  - Connect ST-Link cable to the PC USB port to display data on the CubeMx Monitor.

### <b>How to use it ?</b>

This application runs from the external Flash memory (USBPD_SRC Appli).
It boots from internal Flash (Template_XIP_Boot) then jumps to the application code in external Flash.

To configure STM32CubeIDE Debug Configuration, you must do the following :

    1. Upload the template XIP
    2. Add the adequate external loader (MX66UW1G45G_STM32H7S78-DK.stldr file) in Project->Debugger Configuration
    3. Add in the startup the template_XIP_Boot in Project->Debugger Configuration
    4. Move up the application in the startup

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
 - Run CubeMx Monitor and open trace tools
 - Connect a type C sink device on the Type-C connector 1

<b>Note</b>

   This application runs from the external Flash memory. It is launched from a first boot stage and inherits from this
   boot project configuration (caches, MPU regions, system clock at 600 MHz and external memory interface at the
   highest speed).

   Note that the boot part is automatically downloaded from the IDE environment via the board
   project Templates/Template_XIP/Binary/Boot_XIP.hex file.

   This project uses custom linker settings to place the HEAP in a non-cacheable section of RAM. This is due to
   the dynamic allocation of DMA buffers in the HEAP that must not be cached for DMA to access the data directly.
