## <b>HID_Standalone Application Description</b>

This application provides an example of FreeRTOS on NUCLEO-H7S3L8 board,
it shows how to develop USB Device Human Interface HID mouse based application.

The application is designed to emulate an USB HID mouse device, the code provides all required device descriptors framework
and associated Class descriptor report to build a compliant USB HID mouse device.

####  <b>Expected success behavior</b>

When plugged to PC host, the NUCLEO-H7S3L8 must be properly enumerated as an USB HID mouse device.
During the enumeration phase, device provides host with the requested descriptors (Device, configuration, string).
Those descriptors are used by host driver to identify the device capabilities.
Once the NUCLEO-H7S3L8 USB device successfully completed the enumeration phase, the device sends a HID report after a user button press.
Each report sent should move the mouse cursor by one step on host side.

#### <b>Error behaviors</b>

Host PC shows that USB device does not operate as designed (Mouse enumeration failed, the mouse pointer doesn't move).

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 Universal Serial BUS Specification and HID class Specification.

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

 - This example runs on NUCLEO-H7S3L8 revision B01.

 - This example has been tested with STMicroelectronics NUCLEO-H7S3L8
    board revision B01 and can be easily tailored to any other supported device
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
  5. Run the example


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
  5. Run the example


#### <b>STM32CubeIDE</b>

To configure STM32CubeIDE Debug Configuration, you must do the following :

For "HID_Standalone_Boot" project :
    Add the adequate external loader (MX25UW25645G_STM32H7R38-NUCLEO.stldr file) in Project->Debugger Configuration

For "HID_Standalone_Appli" project :
    1. Add the adequate external loader (MX25UW25645G_STM32H7R38-NUCLEO.stldr file) in Project->Debugger Configuration
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
  6. Run the example

<b>Note</b>

   The application uses the external HSE clock as system clock source.