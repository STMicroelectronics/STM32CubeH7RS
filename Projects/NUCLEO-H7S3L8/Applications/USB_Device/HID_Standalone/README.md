## <b>HID_Standalone Application Description</b>

This application is a part of the USB Device Library package using STM32Cube firmware.
It shows how to develop USB Device application based on Human Interface HID mouse on NUCLEO-H7S3L8.

This is a typical application on how to use the STM32H7S3xx USB OTG Device peripheral where the STM32 MCU is
enumerated as a HID device using the native PC Host HID driver to which the NUCLEO-H7S3L8
board is connected, in order to emulate the Mouse directions using User push-button mounted on the
NUCLEO-H7S3L8 board.

####  <b>Expected success behavior</b>

When plugged to PC host, the NUCLEO-H7S3L8 must be properly enumerated as an USB HID mouse device.
During the enumeration phase, device provides host with the requested descriptors (device, configuration, string).
Those descriptors are used by host driver to identify the device capabilities.
Once the NUCLEO-H7S3L8 USB device successfully completed the enumeration phase, the device sends a HID report after a user button press.
Each report sent should move the mouse cursor by one step on host side.

#### <b>Error behaviors</b>

Host PC shows that USB device does not operate as designed (mouse enumeration failed, the mouse pointer doesn't move).

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 Universal Serial BUS specification and HID class specification.

#### <b>Known limitations</b>

The remote wakeup feature is not yet implemented (used to bring the USB suspended bus back to the active condition).

### <b>Notes</b>

 - It is mandatory to check that the jumper below is connected:
 - JP4  (USB-PD).
 - Connect the NUCLEO-H7S3L8 board to the PC through TYPE-C-to-Standard-A cable.

#### <b>USB usage hints</b>

 - The DTCM (0x20000000) memory region should not be used by application in case USB DMA is enabled
 - Should make sure to configure the USB pool memory region with attribute Non-Cacheable to ensure coherency between CPU and USB DMA

### <b>Keywords</b>

USB_Device, USB_OTG, HIGH Speed, HID, Mouse

### <b>Hardware and Software environment</b>

 - This application runs on STM32H7S3xx devices.

 - This application has been tested with STMicroelectronics NUCLEO-H7S3L8
   board revision MB1737-H7S3L8-B02 and can be easily tailored to any other supported device
   and development board.

### <b>How to use it ?</b>

This application runs from the external Flash memory (HID_Standalone Appli).
It boots from internal Flash (HID_Standalone Boot) then jumps to the application code in external Flash.

In order to make the program work, you must do the following :

#### <b>IAR</b>

  1. Open your toolchain
  2. Open HID_Standalone workspace file Project.eww

 <b>Optional:</b>

- Select first "HID_Standalone_Boot" workspace
- Rebuild all files from HID_Standalone_Boot project
 - If no modification is done on boot project, this step could be skipped.

  3. Select then "HID_Standalone_Appli" workspace
  4. Rebuild all files from HID_Standalone Appli and load your images into memories: First, load the HID_Standalone_Boot.hex in internal Flash, then, load
     the Appli part in External memory available on NUCLEO-H7S3L8 board.
  5. Run the application

#### <b>MDK-ARM</b>

  1. Open your toolchain
  2. Open HID_Standalone workspace file Project.uvmpw

<b>Optional:</b>

- Select first "HID_Standalone_Boot" workspace
- Rebuild all files from HID_Standalone_Boot project
- If no modification is done on boot project, this step could be skipped.

  3. Select then "HID_Standalone_Appli" workspace
  4. Rebuild all files from HID_Standalone Appli and load your images into memories: First, load the HID_Standalone_Boot.hex in internal Flash, then, load
     the Appli part in External memory available on NUCLEO-H7S3L8 board.
  5. Run the application

#### <b>STM32CubeIDE</b>

To configure STM32CubeIDE Debug Configuration, you must do the following :

For "HID_Standalone_Boot" project :
    Add the adequate external loader (MX25UW25645G_STM32H7S3L8-NUCLEO.stldr file) in Project->Debugger Configuration

For "HID_Standalone_Appli" project :
    1. Add the adequate external loader (MX25UW25645G_STM32H7S3L8-NUCLEO.stldr file) in Project->Debugger Configuration
    2. Add in the startup the HID_Standalone_Boot in Project->Debugger Configuration
    3. Move up the application in the startup

  1. Open your toolchain
  2. Open HID_Standalone workspace file .project

<b>Optional:</b>

- Select the "HID_Standalone_Boot" project
- Build the project
- If the project is not compiled, Appli debugging will manage its compilation
   for debugging
 - With the debug icon select the configuration “HID_Standalone_Boot Debug”.
   This operation loads the boot in internal Flash.

  3. Select the "HID_Standalone_Appli" project
  4. Build the project
  5. With the Debug icon select the configuration “HID_Standalone_Appli Debug”. First, load the Boot binary in internal Flash, then, load the Appli binary in
     External memory available on NUCLEO-H7S3L8 board
  6. Run the application

<b>Note</b>

 - The application uses the external HSE clock as system clock source.