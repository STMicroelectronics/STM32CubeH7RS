
## <b>USBPD_SNK Application Description</b>

This application is a USBPD type C Consumer using FreeRTOS.
USBPD Consumer: This application initialize the type C port 1 in sink mode with only one PDO at 5V.

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

  - This example runs on NUCLEO-H7S3L8.

  - This example has been tested with STMicroelectronics NUCLEO-H7S3L8 (MB1737)
    board and can be easily tailored to any other supported device
    and development board.

  - Connect ST-Link cable to the PC USB port to display data on the CubeMx Monitor.
	
### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
 - Run CubeMx Monitor and open trace tools
 - Connect a type C source device on the Type-C connector 1

<b>Note</b>

   This example runs from the external Flash memory. It is launched from a first boot stage and inherits from this
   boot project configuration (caches, MPU regions, system clock at 600 MHz and external memory interface at the 
   highest speed). 

   Note that the boot part is automatically downloaded from the IDE environment via the board 
   project Templates/Template_XIP/Binary/Boot_XIP.hex file. 

   This project uses custom linker settings to place the HEAP in a non-cacheable section of RAM. This is due to
   the dynamic allocation of DMA buffers in the HEAP that must not be cached for DMA to access the data directly.
