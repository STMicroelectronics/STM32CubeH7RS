## <b>CDC_Standalone Application Description</b>

This application is a part of the USB Device Library package using STM32Cube firmware. It describes how to
use USB device application based on the Device Communication Class (CDC) following the PSTN subprotocol
in the NUCLEO-H7S3L8 devices using the OTG-USB and UART peripherals.

This is a typical application on how to use the NUCLEO-H7S3L8 USB OTG Device peripheral where the STM32 MCU
behaves as a USB-to-RS232 bridge following the Virtual COM Port (VCP) implementation.

 - On one side, the STM32 exchanges data with a PC host through USB interface in Device mode.

 - On the other side, the STM32 exchanges data with other devices (same host, other host, other devices..) through the UART interface (RS232).

When the VCP application is started, the STM32 MCU is enumerated as serial communication port and is
configured in the same way (baudrate, data format, parity, stop bit) as it would configure a standard
COM port. The 7-bit data length with no parity control is NOT supported.

During enumeration phase, three communication pipes "endpoints" are declared in the CDC class
implementation (PSTN sub-class):

 - 1 x Bulk IN endpoint for receiving data from STM32 device to PC host:
   When data are received over UART they are saved in the buffer "UserTxBuffer". Periodically, in a
   timer callback the state of the buffer "UserTxBuffer" is checked. If there are available data, they
   are transmitted in response to IN token otherwise it is NAKed.
   The polling period depends on "CDC_POLLING_INTERVAL" value.

 - 1 x Bulk OUT endpoint for transmitting data from PC host to STM32 device:
   When data are received through this endpoint they are saved in the buffer "UserRxBuffer" then they
   are transmitted over UART using interrupt mode and in meanwhile the OUT endpoint is NAKed.
   Once the transmission is over, the OUT endpoint is prepared to receive next packet in
   HAL_UART_TxCpltCallback().

 - 1 x Interrupt IN endpoint for setting and getting serial-port parameters:
   When control setup is received, the corresponding request is executed in CDC_Itf_Control().
   In this application, two requests are implemented:
    - Set line: Set the bit rate, number of Stop bits, parity, and number of data bits
    - Get line: Get the bit rate, number of Stop bits, parity, and number of data bits
   The other requests (send break, control line state) are not implemented.

####  <b>Expected success behavior</b>

When plugged to PC host, the NUCLEO-H7S3L8 must be properly enumerated as an USB Serial device and an STlink Com port.
During the enumeration phase, the device must provide host with the requested descriptors (Device descriptor, configuration descriptor, string descriptors).
Those descriptors are used by host driver to identify the device capabilities. Once NUCLEO-H7S3L8 USB device successfully completed the enumeration phase,
Open two hyperterminals (USB com port and UART com port(USB STLink VCP)) to send/receive data to/from host from/to device.

#### <b>Error behaviors</b>

Host PC shows that USB device does not operate as designed (CDC Device enumeration failed, PC and Device can not communicate over VCP ports).

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 "Universal Serial BUS" Specification and CDC_ACM class Specification.

#### <b>Known limitations</b>

None.

### <b>Notes</b>

> - The reception of data via UART is managed by interrupt while the transmission is managed by DMA, thus allowing the application to receive data at the same time as it transmits other data (full-duplex functionality).
> - The user has to check the list of the COM ports in Device Manager to find out the number of the COM ports that have been assigned (by OS) to the VCP interface.
> - It is mandatory to check that the jumper below is connected:
> -    JP4  (USB-PD).
> - Connect the NUCLEO-H7S3L8 board to the PC through TYPE-C to Standard A cable.

#### <b>USB usage hints</b>

- This application uses UART as a communication interface. The UART instance and associated resources (GPIO, NVIC) can be tailored in "usbd_cdc_interface.h" header file according to your hardware  configuration. Moreover, this application can be customized to communicate with interfaces other than UART.
For that purpose a template CDC interface is provided in: Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src directory.
- The DTCM (0x20000000) memory region should not be used by application in case USB DMA is enabled
- Should make sure to configure the USB pool memory region with attribute "Non-Cacheable" to ensure coherency between CPU and USB DMA

### <b>Keywords</b>

USB_Device, USB_OTG, High_Speed, CDC, VCP

### <b>Hardware and Software environment</b>

  - This example runs on NUCLEO-H7S3L8 device
  - This example has been tested with STMicroelectronics NUCLEO-H7S3L8 boards Revision MB1737-H7S3L8-B01 and can be easily tailored to any other supported device and development board.
  - NUCLEO-H7S3L8 Set-up
  - Connect the NUCLEO-H7S3L8 board CN2 to the PC through USB cable.
  - For VCP the configuration is dynamic for example it can be :
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - Stop Bit = 1
    - Parity = None
    - Flow control = None

  - The USART3 interface available on PD8 and PD9 of the microcontroller are
  connected to ST-LINK MCU.
  By default the USART3 communication between the target MCU and ST-LINK MCU is enabled.
  Its configuration is as following:
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - Stop Bit = 1
    - Parity = None
    - Flow control = None

### <b>How to use it ?</b>

This application runs from the external Flash memory (CDC_Standalone Appli).
It boots from internal Flash (CDC_Standalone Boot) then jumps to the application code in external Flash.

In order to make the program work, you must do the following :

#### <b>IAR</b>
  1. Open your toolchain
  2. Open CDC_Standalone workspace file Project.eww

<b>Optional:</b>

- Select first "CDC_Standalone_Boot" workspace
- Rebuild all files from CDC_Standalone_Boot project
- If no modification is done on boot project, this step could be skipped.

  3. Select then "CDC_Standalone_Appli" workspace
  4. Rebuild all files from CDC_Standalone Appli and load your images into memories: First, load the CDC_Standalone_Boot.hex in internal Flash, then, load
     the Appli part in External memory available on NUCLEO-H7S3L8 board.
  5. Run the example


#### <b>MDK-ARM</b>
  1. Open your toolchain
  2. Open CDC_Standalone workspace file Project.uvmpw

<b>Optional:</b>

- Select first "CDC_Standalone_Boot" workspace
- Rebuild all files from CDC_Standalone_Boot project
- If no modification is done on boot project, this step could be skipped.

  3. Select then "CDC_Standalone_Appli" workspace
  4. Rebuild all files from CDC_Standalone Appli and load your images into memories: First, load the CDC_Standalone_Boot.hex in internal Flash, then, load
     the Appli part in External memory available on NUCLEO-H7S3L8 board.
  5. Run the example


#### <b>STM32CubeIDE</b>

To configure STM32CubeIDE Debug Configuration, you must do the following :

For "CDC_Standalone_Boot" project :
    Add the adequate external loader (MX25UW25645G_STM32H7R38-NUCLEO.stldr file) in Project->Debugger Configuration

For "CDC_Standalone_Appli" project :
    1. Add the adequate external loader (MX25UW25645G_STM32H7R38-NUCLEO.stldr file) in Project->Debugger Configuration
    2. Add in the startup the CDC_Standalone_Boot in Project->Debugger Configuration
    3. Move up the application in the startup

  1. Open your toolchain
  2. Open CDC_Standalone workspace file .project

<b>Optional:</b>

- Select the "CDC_Standalone_Boot" project
- Build the project
- If the project is not compiled, Appli debugging will manage its compilation
   for debugging
 - With the debug icon select the configuration “CDC_Standalone_Boot Debug”.
   This operation loads the boot in internal Flash.

  3. Select the "CDC_Standalone_Appli" project
  4. Build the project
  5. With the Debug icon select the configuration “CDC_Standalone_Appli Debug”. First, load the Boot binary in internal Flash, then, load the Appli binary in
     External memory available on NUCLEO-H7S3L8 board
  6. Run the example

<b>Note</b>

   The application uses the external HSE clock as system clock source.
