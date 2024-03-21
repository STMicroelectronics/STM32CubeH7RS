## <b>Video_Standalone Application Description</b>

This application is a part of the USB Device Library package using STM32Cube firmware. It describes how to use USB device application based on the Device video Class in the NUCLEO-H7S3L8 devices using the OTG-USB peripherals.

This is a typical application on how to use the NUCLEO-H7S3L8 USB OTG Device peripheral where the STM32 MCU behaves as an USB VIDEO device implementation.

####  <b>Expected success behavior</b>

When plugged to PC host, the NUCLEO-H7S3L8 must be properly enumerated as an USB VIDEO device.
During the enumeration phase, device provides host with the requested descriptors (Device, configuration, string).
Those descriptors are used by host driver to identify the device capabilities.
Once the NUCLEO-H7S3L8 USB device successfully completed the enumeration phase.

  1. Open a camera application (Camera, ContaCam or VLC media player) then open the capture device and play the video on the PC host.

  2. To test other stream change number of included file "usbd_conf.h":

- if "USBD_UVC_FORMAT_UNCOMPRESSED" is commented:
    The video takes the format of stream1 (compressed): MJPEG and Width = 176, Height = 144.
- if "USBD_UVC_FORMAT_UNCOMPRESSED" is Uncommented:
    The video takes the format of stream2 (uncompressed): UNCOMPRESSED and Width = 320, Height = 236 and the descriptor needs to be updated.

#### <b>Error behaviors</b>

Host PC shows that USB device does not operate as designed (VIDEO Device enumeration failed).

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 "Universal Serial BUS" Specification and VIDEO class Specification.

#### <b>Known limitations</b>

None.

### <b>Notes</b>

> - It is mandatory to check that the jumper below is connected:
> - JP4  (USB-PD).
> - Connect the NUCLEO-H7S3L8 board to the PC through TYPE-C-to-Standard-A cable.

#### <b>USB usage hints</b>

- This application uses UART as a communication interface. The UART instance and associated resources (GPIO, NVIC) can be tailored in "usbd_video_if.h" header file according to your hardware  configuration. Moreover, this application can be customized to communicate with interfaces other than UART.
For that purpose a template VIDEO interface is provided in: Middlewares/ST/STM32_USB_Device_Library/Class/VIDEO/Src directory.
- The DTCM (0x20000000) memory region should not be used by application in case USB DMA is enabled
- Should make sure to configure the USB pool memory region with attribute "Non-Cacheable" to ensure coherency between CPU and USB DMA

### <b>Keywords</b>

USB_Device, USB_OTG, High_Speed, VIDEO

### <b>Hardware and Software environment</b>

  - This example runs on NUCLEO-H7S3L8 device
  - This example has been tested with STMicroelectronics NUCLEO-H7S3L8 boards Revision MB1737-H7S3L8-B01 and can be easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

This application runs from the external Flash memory (Video_Standalone Appli).
It boots from internal Flash (Video_Standalone Boot) then jumps to the application code in external Flash.

In order to make the program work, you must do the following :

#### <b>IAR</b>
  1. Open your toolchain
  2. Open Video_Standalone workspace file Project.eww

<b>Optional:</b>

- Select first "Video_Standalone_Boot" workspace
- Rebuild all files from Video_Standalone_Boot project
- If no modification is done on boot project, this step could be skipped.

  3. Select then "Video_Standalone_Appli" workspace
  4. Rebuild all files from Video_Standalone Appli and load your images into memories: First, load the Video_Standalone_Boot.hex in internal Flash, then, load
     the Appli part in External memory available on NUCLEO-H7S3L8 board.
  5. Run the example


#### <b>MDK-ARM</b>
  1. Open your toolchain
  2. Open Video_Standalone workspace file Project.uvmpw

<b>Optional:</b>

- Select first "Video_Standalone_Boot" workspace
- Rebuild all files from Video_Standalone_Boot project
- If no modification is done on boot project, this step could be skipped.

  3. Select then "Video_Standalone_Appli" workspace
  4. Rebuild all files from Video_Standalone Appli and load your images into memories: First, load the Video_Standalone_Boot.hex in internal Flash, then, load
     the Appli part in External memory available on NUCLEO-H7S3L8 board.
  5. Run the example


#### <b>STM32CubeIDE</b>

To configure STM32CubeIDE Debug Configuration, you must do the following :

For "Video_Standalone_Boot" project :
    Add the adequate external loader (MX25UW25645G_STM32H7R38-NUCLEO.stldr file) in Project->Debugger Configuration

For "Video_Standalone_Appli" project :
    1. Add the adequate external loader (MX25UW25645G_STM32H7R38-NUCLEO.stldr file) in Project->Debugger Configuration
    2. Add in the startup the Video_Standalone_Boot in Project->Debugger Configuration
    3. Move up the application in the startup

  1. Open your toolchain
  2. Open Video_Standalone workspace file .project

<b>Optional:</b>

- Select the "Video_Standalone_Boot" project
- Build the project
- If the project is not compiled, Appli debugging will manage its compilation
   for debugging
 - With the debug icon select the configuration “Video_Standalone_Boot Debug”.
   This operation loads the boot in internal Flash.

  3. Select the "Video_Standalone_Appli" project
  4. Build the project
  5. With the Debug icon select the configuration “Video_Standalone_Appli Debug”. First, load the Boot binary in internal Flash, then, load the Appli binary in
     External memory available on NUCLEO-H7S3L8 board
  6. Run the example

<b>Note</b>

   The application uses the external HSE clock as system clock source.