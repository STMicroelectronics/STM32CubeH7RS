## <b>HID_RTOS Application Description</b>

This application is an example implementation of a USB HID (Human Interface Device) host with FreeRTOS, supporting both high-speed (HS) and full-speed (FS) USB devices.
The code provides required requests to properly enumerate HID devices , HID Class APIs to decode HID reports received from a mouse or a keyboard and display data on uart HyperTerminal.
The application allows for connecting a USB HID device, such as a mouse or keyboard, and communicating with it through the HID interface.

####  <b>Expected success behavior</b>

When a hid device is plugged to STM32H7S78-DK board, a Message will be displayed on the uart HyperTerminal showing the Vendor ID and Product ID of the attached device. After enumeration phase, a message will indicates that the device is ready for use. The host must be able to properly decode HID reports sent by the corresponding device and display those information on the HyperTerminal.

The received HID reports are used by host to identify:
- In case of a mouse
  - (x,y) mouse position
  - Pressed mouse buttons
- In case of a keyboard
  - Pressed key

#### <b>Error behaviors</b>

Errors are detected such as (Unsupported device, Enumeration Fail) and the corresponding message is displayed on the HyperTerminal.

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 “Universal Serial BUS” Specification and HID class Specification.

#### <b>Known limitations</b>

None.

### <b>Notes</b>

None.

#### <b>USB usage hints</b>

- The DTCM (0x20000000) memory region should not be used by application in case USB DMA is enabled
- Should make sure to configure the USB pool memory region with attribute "Non-Cacheable" to ensure coherency between CPU and USB DMA

### <b>Keywords</b>

Connectivity, USBPD, Thread, UART/USART, USB_NOPD_Lib, UCPD, Type C, USB

### <b>Hardware and Software environment</b>

  - This application runs on STM32H7RSxx devices

  - This application has been tested with STMicroelectronics STM32H7S78-DK MB1736 revision D01 board and can be
    easily tailored to any other supported device and development board.

- STM32H7S78-DK Set-up to run application with HS mode
    - Plug the USB HID device into the STM32H7S78-DK board through 'Type C  to A-Female' cable to the connector:
    - USB1 : to use USB OTG IP in high speed (HS)

- STM32H7S78-DK Set-up to run application with FS mode
    - USB2 : To use USB OTG IP in full speed (FS)
    - Remove IDE project folder(EWARM/ MDK-ARM/ STM32CubeIDE)
    - CubeMX side : Follow the following steps
                  - Uncheck UCPD1 in connectivity of Categories
                  - Uncheck USB_OTG_HS in connectivity of Categories
                  - Check USB_OTG_FS and keep the same configuration as USB_OTG_HS
                  - Use the same old configuration of USB_HOST in Middlewares and Software Packs of Categories
                  - Put USB_HS_REGEN Disable for PWR in Power and Thermal of Categories
    - EWARM side : - Remove "USE_USB_HS" Define from Preprocessor Symbols in C/C++ Option
                   - Keep the same old configuration of the Debugger option
    - MDK-ARM side : - Remove "USE_USB_HS" Define from Preprocessor Symbols in C/C++ Option
                     - Keep the same old configuration of the Target and Debug/Settings options
    - CubeIDE side : - Remove "USE_USB_HS" Define from Paths and Symbols in C/C++ General Settings

    - Connect JP1 jumper on USB2

    - Connect ST-Link cable to the PC USB port to display data on the HyperTerminal.

    A virtual COM port will then appear in the HyperTerminal:
    - Hyperterminal configuration
      - Data Length = 8 Bits
      - One Stop Bit
      - No parity
      - BaudRate = 115200 baud
      - Flow control: None

### <b>How to use it ?</b>

This application runs from the external Flash memory (HID_RTOS Appli).
It boots from internal Flash (HID_RTOS Boot) then jumps to the application code in external Flash.

In order to make the program work, you must do the following :

#### <b>IAR</b>

  1. Open your toolchain
  2. Open HID_RTOS workspace file Project.eww

<b>Optional:</b>

- Select first "HID_RTOS_Boot" workspace
- Rebuild all files from HID_RTOS_Boot project
- If no modification is done on boot project, this step could be skipped.

  3. Select then "HID_RTOS_Appli" workspace
  4. Rebuild all files from HID_RTOS_Appli and load your images into memories: First, load the HID_RTOS_Boot.hex in internal Flash, then, load the Appli part in External memory available on STM32H7S78-DK board.
  5. Run the application

#### <b>MDK-ARM</b>

  1. Open your toolchain
  2. Open HID_RTOS workspace file Project.uvmpw

<b>Optional:</b>

- Select first "HID_RTOS_Boot" workspace
- Rebuild all files from HID_RTOS_Boot project
- If no modification is done on boot project, this step could be skipped.

  3. Select then "HID_RTOS_Appli" workspace
  4. Rebuild all files from HID_RTOS_Appli and load your images into memories: First, load the HID_RTOS_Boot.hex in internal Flash, then, load the Appli part in External memory available on STM32H7S78-DK board.
  5. Run the application

#### <b>STM32CubeIDE</b>

To configure STM32CubeIDE Debug Configuration, you must do the following :

For "HID_RTOS_Boot" project :
    Add the adequate external loader (MX66UW1G45G_STM32H7S78-DK.stldr file) in Project->Debugger Configuration

For "HID_RTOS_Appli" project :
    1. Add the adequate external loader (MX66UW1G45G_STM32H7S78-DK.stldr file) in Project->Debugger Configuration
    2. Add in the startup the HID_RTOS_Boot in Project->Debugger Configuration
    3. Move up the application in the startup

  1. Open your toolchain
  2. Open HID_RTOS workspace file .project

<b>Optional:</b>

- Select the "HID_RTOS_Boot" project
- Build the project
- If the project is not compiled, Appli debugging will manage its compilation for debugging
 - With the debug icon select the configuration “HID_RTOS_Boot Debug”.
   This operation loads the boot in internal Flash.

  3. Select the "HID_RTOS_Appli" project
  4. Build the project
  5. With the Debug icon select the configuration “HID_RTOS_Appli Debug”. First, load the Boot binary in internal Flash, then, load the Appli binary in External memory available on STM32H7S78-DK board
  6. Run the application

<b>Note</b>

The user has to check the list of the COM ports in Device Manager to find out the number of the COM ports that have been assigned (by OS) to the Stlink VCP.
The application uses the external HSE clock as USB source clock.
