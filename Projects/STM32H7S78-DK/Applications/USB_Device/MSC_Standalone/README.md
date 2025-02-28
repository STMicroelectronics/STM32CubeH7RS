## <b>MSC_Standalone Application Description</b>

- This application is a part of the USB Device Library package using STM32Cube firmware. It describes how to
use USB device application based on the Mass Storage Class (MSC) on the STM32H7rsxx devices.

- This is a typical application on how to use the STM32H7S78-DK USB OTG Device peripheral to communicate with a PC
Host using the Bulk Only Transfer (BOT) and Small Computer System Interface (SCSI) transparent commands,
while the microSD card is used as storage media. The STM32 MCU is enumerated as a MSC device using the
native PC Host MSC driver to which the STM32H7S78-DK board is connected.

####  <b>Expected success behavior</b>

- When plugged to PC host, the STM32H7S78-DK should enumerate as an USB MSC device. During the enumeration phase, device must provide host with the requested descriptors (Device descriptor, configuration descriptor, string descriptors).

Those descriptors are used by the host driver to identify the device capabilities.

- Once the STM32H7S78-DK USB device successfully completed the enumeration phase, a new removable drive appears in the system window and write/read/format operations can be performed as with any other removable drive.

#### <b>Error behaviors</b>

Host PC shows that USB device does not operate as designed (Mouse enumeration failed, the mouse pointer doesn't move).

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 "Universal Serial BUS" Specification and MSC class Specification.

#### <b>Known limitations</b>

The remote wakeup feature is not yet implemented (used to bring the USB suspended bus back to the active condition).

### <b>Notes</b>

- It is mandatory to check that the jumper below is connected for USB_HS:
    JP2

#### <b>USB usage hints</b>

- The DTCM (0x20000000) memory region should not be used by application in case USB DMA is enabled
- Should make sure to configure the USB pool memory region with attribute "Non-Cacheable" to ensure coherency between CPU and USB DMA
- STM32H7S78-DK  Set-up
  - Connect the STM32H7S78-DK board to the PC through USB cable to the connector:
    - CN18 : to use USB High Speed (HS)

### <b>Keywords</b>

Connectivity, USB_Device, USB, SDMMC, MSC, Mass Storage, SD Card, Full Speed, High Speed, Removable drive, Write, Read,

### <b>Hardware and Software environment</b>

  - This application runs on STM32H7S78-DK.

  - This application has been tested with STMicroelectronics STM32H7S78-DK boards Revision MB1736-H7S7L8-D01
    and can be easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

This application runs from the external Flash memory (MSC_Standalone Appli).
It boots from internal Flash (MSC_Standalone Boot) then jumps to the application code in external Flash.

In order to make the program work, you must do the following :

#### <b>IAR</b>
  1. Open your toolchain
  2. Open MSC_Standalone workspace file Project.eww

<b>Optional:</b>

- Select first "MSC_Standalone" workspace
- Rebuild all files from MSC_Standalone project
- If no modification is done on boot project, this step could be skipped.

  3. Select then "MSC_Standalone_Appli" workspace
  4. Rebuild all files from MSC_Standalone Appli and load your images into memories: First load the MSC_Standalone.hex in internal Flash , then load
     the Appli part in External memory available on STM32H7S78-DK board.
  5. Run the application

#### <b>MDK-ARM</b>
  1. Open your toolchain
  2. Open MSC_Standalone workspace file Project.uvmpw

<b>Optional:</b>

- Select first "MSC_Standalone" workspace
- Rebuild all files from MSC_Standalone project
- If no modification is done on boot project, this step could be skipped.

  3. Select then "MSC_Standalone_Appli" workspace
  4. Rebuild all files from MSC_Standalone Appli and load your images into memories: First load the MSC_Standalone.hex in internal Flash , then load
     the Appli part in External memory available on STM32H7S78-DK board.
  5. Run the application

#### <b>STM32CubeIDE</b>
  1. Open your toolchain
  2. Open MSC_Standalone workspace file .project

<b>Optional:</b>

- Select the "MSC_Standalone" project
- Build the project
- If the project is not compiled, Appli debugging will manage its compilation
   for debugging
 - With the debug icon select the configuration “MSC_Standalone_Boot Debug”.
   This operation loads the boot in internal Flash.

  3. Select the "MSC_Standalone_Appli" project
  4. Build the project
  5. With the Debug icon select the configuration “MSC_Standalone_Appli Debug”. First load the Boot binary in internal Flash, then load the Appli binary in
     External memory available on STM32H7S78-DK board
  6. Run the application

To configure STM32CubeIDE Debug Configuration, you must do the following :

For "MSC_Standalone_Boot" project :
    Add the adequate external loader (MX66UW1G45G_STM32H7S78-DK.stldr file) in Project->Debugger Configuration

For "MSC_Standalone_Appli" project :
    1. Add the adequate external loader (MX66UW1G45G_STM32H7S78-DK.stldr file) in Project->Debugger Configuration
    2. Add in the startup the MSC_Standalone_Boot in Project->Debugger Configuration
    3. Move up the application in the startup

<b>Note</b>

 - The application uses the internal HSE clock as system clock source.
