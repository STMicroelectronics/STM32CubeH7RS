## <b>MSC_RTOS Application Description</b>

This application is an example implementation of a USB MSC (Mass Storage Class) host with FreeRTOS, supporting high-speed (HS).
The code provides the necessary requests to properly enumerate MSC devices, MSC Class APIs to communicate with mass storage devices like USB flash drives or external hard drives, and display data on UART HyperTerminal.
This is a typical application on how to use the STM32H7S78-DK USB OTG Host peripheral to operate with an USB flash disk using the Bulk Only Transfer (BOT) and Small Computer System Interface (SCSI) transparent commands combined with a file system FatFs (Middleware component).

####  <b>Expected success behavior</b>

When an MSC (Mass Storage Class) device is plugged into the STM32H7S78-DK board, a message will be displayed on the UART HyperTerminal showing the Vendor ID and Product ID of the attached device. After the enumeration phase, a message will indicate that the device is ready for use. The host must be able to properly communicate with the mass storage device and display relevant information on the HyperTerminal.

The host can use the received data to perform operations on the mass storage device, such as reading or writing files. The MSC class APIs provided in the code will assist in communicating with the mass storage device, and the data can be displayed on the HyperTerminal.

#### <b>Error behaviors</b>

Errors are detected such as (Unsupported device, Enumeration Fail) and the corresponding message is displayed on the HyperTerminal.

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 “Universal Serial BUS” Specification and MSC class Specification.

#### <b>Known limitations</b>
None.

### <b>Notes</b>
None.

#### <b>USB usage hints</b>

- The DTCM (0x20000000) memory region should not be used by application in case USB DMA is enabled
- Should make sure to configure the USB pool memory region with attribute "Non-Cacheable" to ensure coherency between CPU and USB DMA

### <b>Keywords</b>

Connectivity, USBPD, Thread, UART/USART, USB_NOPD_Lib, UCPD, Type C, USB, MSC

### <b>Hardware and Software environment</b>

  - This application has been tested with STMicroelectronics STM32H7S78-DK boards Revision MB1736-H7S7L8-C01 and can be easily tailored to any other supported device and development board.

- STM32H7S78-DK Set-up
    - Plug the USB MSC device into the STM32H7S78-DK board through 'Type C  to A-Female' cable to the connector:
      - CN18 : to use USB OTG IP in high speed (HS)
    - Connect ST-Link cable to the PC USB port to display data on the HyperTerminal.

    A virtual COM port will then appear in the HyperTerminal:
     - Hyperterminal configuration
       - Data Length = 8 Bits
       - One Stop Bit
       - No parity
       - BaudRate = 115200 baud
       - Flow control: None

### <b>How to use it ?</b>

This application runs from the external Flash memory (MSC_RTOS Appli).
It boots from internal Flash (MSC_RTOS Boot) then jumps to the application code in external Flash.

In order to make the program work, you must do the following :

#### <b>IAR</b>
  1. Open your toolchain
  2. Open MSC_RTOS workspace file Project.eww

<b>Optional:</b>

- Select first "MSC_RTOS" workspace
- Rebuild all files from MSC_RTOS project
- If no modification is done on boot project, this step could be skipped.

  3. Select then "MSC_RTOS_Appli" workspace
  4. Rebuild all files from MSC_RTOS Appli and load your images into memories: First load the MSC_RTOS.hex in internal Flash , then load
     the Appli part in External memory available on STM32H7S78-DK board.
  5. Run the example


#### <b>MDK-ARM</b>
  1. Open your toolchain
  2. Open MSC_RTOS workspace file Project.uvmpw

<b>Optional:</b>

- Select first "MSC_RTOS" workspace
- Rebuild all files from MSC_RTOS project
- If no modification is done on boot project, this step could be skipped.

  3. Select then "MSC_RTOS_Appli" workspace
  4. Rebuild all files from MSC_RTOS Appli and load your images into memories: First load the MSC_RTOS.hex in internal Flash , then load
     the Appli part in External memory available on STM32H7S78-DK board.
  5. Run the example


#### <b>STM32CubeIDE</b>
  1. Open your toolchain
  2. Open MSC_RTOS workspace file .project

<b>Optional:</b>

- Select the "MSC_RTOS" project
- Build the project
- If the project is not compiled, Appli debugging will manage its compilation
   for debugging
 - With the debug icon select the configuration “MSC_RTOS_Boot Debug”.
   This operation loads the boot in internal Flash.

  3. Select the "MSC_RTOS_Appli" project
  4. Build the project
  5. With the Debug icon select the configuration “MSC_RTOS_Appli Debug”. First load the Boot binary in internal Flash, then load the Appli binary in
     External memory available on STM32H7S78-DK board
  6. Run the example

To configure STM32CubeIDE Debug Configuration, you must do the following :

For "MSC_RTOS_Boot" project :
    Add the adequate external loader (MX66UW1G45G_STM32H7S78-DK.stldr file) in Project->Debugger Configuration

For "MSC_RTOS_Appli" project :
    1. Add the adequate external loader (MX66UW1G45G_STM32H7S78-DK.stldr file) in Project->Debugger Configuration
    2. Add in the startup the MSC_RTOS_Boot in Project->Debugger Configuration
    3. Move up the application in the startup

<b>Note</b>

The user has to check the list of the COM ports in Device Manager to find out the number of the COM ports that have been assigned (by OS) to the Stlink VCP.
The application uses the external HSE clock as USB source clock.
