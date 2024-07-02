## <b>AUDIO_Standalone Application Description</b>

- This application is a part of the USB Host Library package using STM32Cube firmware. It describes how to
use USB Host application based on the AUDIO Class implementation of an audio streaming Wav files from sd card capability on the STM32H7RSxx Hosts.

- It follows the "Universal Serial Bus Host Class Definition for Audio Hosts Release 1.0 March 18,
1998" defined by the USB Implementers Forum for reprogramming an application through USB-FS-Host.
Following this specification, it is possible to manage only Full Speed USB mode (High Speed is not supported).
This class is natively supported by most Operating Systems: no need for specific driver setup.

- This is a typical application on how to use the STM32H7RSxx USB OTG Host peripheral to
stream audio data from SD Card to USB audio device implemented on the STM32H7S78-DK board.


####  <b>Expected success behavior</b>

- When plugged to PC host, the STM32H7S78-DK should enumerate as an USB AUDIO Host. A Message will be displayed on the uart HyperTerminal showing the Vendor ID and Product ID of the attached device. After enumeration phase, a message will indicates the list of wav files founded in SD card and  then the device is ready for use.

- The user button should be pressed to start streamingn audio files from SD card.

#### <b>Error behaviors</b>

- Host PC shows that USB Host does not operate as designed.
- No Wav file found on SD card or no SD card is plugged in.

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 "Universal Serial BUS" Specification and AUDIO class Specification.

#### <b>Known limitations</b>

### <b>Notes</b>

- It is mandatory to check that the jumper below is connected for USB_FS:
    JP1  (USB2).

- An SD card must be plugged in with containing wav files.

#### <b>USB usage hints</b>

- STM32H7S78-DK  Set-up
  - Connect the STM32H7S78-DK board to the PC through USB cable to the connector CN17 to use USB Full Speed (FS)

### <b>Keywords</b>

Connectivity, USB_Host, USB, Audio, Full speed

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S78-DK MB1736 revision C01.

  - This example has been tested with STMicroelectronics STM32H7S78-DK
    MB1736 revision C01 board and can be easily tailored to any other supported device
    and development board.
  - User Option Bytes requirement (with STM32CubeProgrammer tool):

    - XSPI1_HSLV=1     I/O XSPIM_P1 High speed option enabled
    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled
    - VDDIO_HSLV=0     I/O speed optimized to operate at 3.3V

   - Set the EXTI GPIO Pull to Pull-up.

### <b>How to use it ?</b>

This application runs from the external Flash memory (AUDIO_Standalone Appli).
It boots from internal Flash (AUDIO_Standalone Boot) then jumps to the application code in external Flash.

In order to make the program work, you must do the following :

#### <b>IAR</b>
  1. Open your toolchain
  2. Open AUDIO_Standalone workspace file Project.eww

<b>Optional:</b>

- Select first "AUDIO_Standalone_Boot" workspace
- Rebuild all files from AUDIO_Standalone_Boot project
- If no modification is done on boot project, this step could be skipped.

  3. Select then "AUDIO_Standalone_Appli" workspace
  4. Rebuild all files from AUDIO_Standalone Appli and load your images into memories: First, load the AUDIO_Standalone_Boot.hex in internal Flash, then, load
     the Appli part in External memory available on NUCLEO-H7S3L8 board.
  5. Run the example


#### <b>MDK-ARM</b>
  1. Open your toolchain
  2. Open AUDIO_Standalone workspace file Project.uvmpw

<b>Optional:</b>

- Select first "AUDIO_Standalone_Boot" workspace
- Rebuild all files from AUDIO_Standalone_Boot project
- If no modification is done on boot project, this step could be skipped.

  3. Select then "AUDIO_Standalone_Appli" workspace
  4. Rebuild all files from AUDIO_Standalone Appli and load your images into memories: First, load the AUDIO_Standalone_Boot.hex in internal Flash, then, load
     the Appli part in External memory available on NUCLEO-H7S3L8 board.
  5. Run the example


#### <b>STM32CubeIDE</b>

To configure STM32CubeIDE Debug Configuration, you must do the following :

For "AUDIO_Standalone_Boot" project :
    Add the adequate external loader (MX25UW25645G_STM32H7R38-NUCLEO.stldr file) in Project->Debugger Configuration

For "AUDIO_Standalone_Appli" project :
    1. Add the adequate external loader (MX66UW1G45G_STM32H7S78-DK.stldr file) in Project->Debugger Configuration
    2. Add in the startup the AUDIO_Standalone_Boot in Project->Debugger Configuration
    3. Move up the application in the startup
  1. Open your toolchain
  2. Open AUDIO_Standalone workspace file .project

<b>Optional:</b>

- Select the "AUDIO_Standalone_Boot" project
- Build the project
- If the project is not compiled, Appli debugging will manage its compilation
   for debugging
 - With the debug icon select the configuration “AUDIO_Standalone_Boot Debug”.
   This operation loads the boot in internal Flash.

  3. Select the "AUDIO_Standalone_Appli" project
  4. Build the project
  5. With the Debug icon select the configuration “AUDIO_Standalone_Appli Debug”. First, load the Boot binary in internal Flash, then, load the Appli binary in
     External memory available on NUCLEO-H7S3L8 board
  6. Run the example

<b>Note</b>

   The application uses the external HSE clock as system clock source.
