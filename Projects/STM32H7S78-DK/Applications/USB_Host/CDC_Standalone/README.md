## <b>CDC_Standalone Application Description</b>

This application is a part of the USB Host Library package using STM32Cube firmware. It describes how to use
USB host application based on the Communication Class (CDC) on the stm32h7rsxx devices.

This is a typical application on how to use the stm32h7rsxx USB OTG Host peripheral to operate with an USB
CDC device application based on the two CDC transfer directions with a dynamic serial configuration:

 - Transmission:
   by pressing the user button a message "USB_STM32_Host_CDC_ACM" will be sent from the host to a device and this message will be displayed in the hyper terminal of the device in hyperterminal
   The data to be transmitted is stored in the CDC_TX_Buffer and can be viewed via the Hyperterminal.

 - Reception:
   The data entered by the user using the Hyperterminal in ASCII format is transferred by the device board
   to the host board and displayed on the hyperterminal on the host side.
   The CDC_RX_Buffer is the buffer used for receiving data.

####  <b>Expected success behavior</b>

- When plugged to PC host, the STM32H7S78-DK must be properly enumerated as an USB Serial device and an STlink Com port.
During the enumeration phase, the device must provide host with the requested descriptors (Device descriptor, configuration descriptor, string descriptors).

- Those descriptors are used by host driver to identify the device capabilities. Once STM32H7S78-DK USB device successfully completed the enumeration phase,
Open two hyperterminals (USB com port and UART com port(USB STLink VCP)) to send/receive data to/from host from/to device.

#### <b>Error behaviors</b>

- Host PC shows that USB device does not operate as designed (CDC Device enumeration failed, PC and Device can not communicate over VCP ports).

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 “Universal Serial BUS” Specification and CDC class Specification.

#### <b>Known limitations</b>
None.

### <b>Notes</b>

- It is mandatory to check that the jumper below is connected for USB_FS:
    JP1  (USB2).

### <b>Keywords</b>

Connectivity, UART/USART, Type C, USB, CDC

### <b>Hardware and Software environment</b>

  - This application has been tested with STMicroelectronics STM32H7S78-DK boards revision MB1736-H7S7L8-D01 and can be easily tailored to any other supported device and development board.

  - Connect ST-Link cable to the PC USB port to display data on the HyperTerminal.
  - STM32H7S78-DK Set-up
      - Plug the USB CDC device into the STM32H7S78-DK board through 'Type A  to Type C' cable to the connector:
        - CN17 : to use USB OTG IP in full speed (FS)

    A virtual COM port will then appear in the HyperTerminal:
    - Hyperterminal configuration
      - Data Length = 8 Bits
      - One Stop Bit
      - No parity
      - BaudRate = 115200 baud
        Flow control: None

  - The USRT4 interface available on PD0 and PD1 of the microcontroller are connected to ST-LINK MCU.
  - By default the USRT4 communication between the target MCU and ST-LINK MCU is enabled. It's configuration is as following:
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

- Select first "CDC_Standalone" workspace
- Rebuild all files from CDC_Standalone project
- If no modification is done on boot project, this step could be skipped.

  3. Select then "CDC_Standalone_Appli" workspace
  4. Rebuild all files from CDC_Standalone Appli and load your images into memories: First load the CDC_Standalone.hex in internal Flash , then load
     the Appli part in External memory available on STM32H7S78-DK board.
  5. Run the application


#### <b>MDK-ARM</b>
  1. Open your toolchain
  2. Open CDC_Standalone workspace file Project.uvmpw

<b>Optional:</b>

- Select first "CDC_Standalone" workspace
- Rebuild all files from CDC_Standalone project
- If no modification is done on boot project, this step could be skipped.

  3. Select then "CDC_Standalone_Appli" workspace
  4. Rebuild all files from CDC_Standalone Appli and load your images into memories: First load the CDC_Standalone.hex in internal Flash , then load
     the Appli part in External memory available on STM32H7S78-DK board.
  5. Run the application


#### <b>STM32CubeIDE</b>
  1. Open your toolchain
  2. Open CDC_Standalone workspace file .project

<b>Optional:</b>

- Select the "CDC_Standalone" project
- Build the project
- If the project is not compiled, Appli debugging will manage its compilation
   for debugging
 - With the debug icon select the configuration “CDC_Standalone_Boot Debug”.
   This operation loads the boot in internal Flash.

  3. Select the "CDC_Standalone_Appli" project
  4. Build the project
  5. With the Debug icon select the configuration “CDC_Standalone_Appli Debug”. First load the Boot binary in internal Flash, then load the Appli binary in
     External memory available on STM32H7S78-DK board
  6. Run the application

To configure STM32CubeIDE Debug Configuration, you must do the following :

For "CDC_Standalone_Boot" project :
    Add the adequate external loader (MX66UW1G45G_STM32H7S78-DK.stldr file) in Project->Debugger Configuration

For "CDC_Standalone_Appli" project :
    1. Add the adequate external loader (MX66UW1G45G_STM32H7S78-DK.stldr file) in Project->Debugger Configuration
    2. Add in the startup the CDC_Standalone_Boot in Project->Debugger Configuration
    3. Move up the application in the startup

<b>Note</b>

The user has to check the list of the COM ports in Device Manager to find out the number of the COM ports that have been assigned (by OS) to the Stlink VCP.
The application uses the external HSE clock as USB source clock.
