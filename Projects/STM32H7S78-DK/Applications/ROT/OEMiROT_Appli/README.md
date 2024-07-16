## <b>OEMiROT_Appli application Description</b>

This project provides an OEMiROT boot path application example. Boot is performed through OEMiROT boot path after authenticity and integrity checks of the project firmware image.

The application will display a menu on the console explaining how to download through bootloader a new version
of the application firmware.
At next reset, the OEMiROT will decrypt and install this new version after successful check of the authenticity and the integrity of the image.

The system clock frequency is the one configured by OEMiROT.

### <b>Keywords</b>

OEMiROT, boot path, Root Of Trust, MPU

### <b>Directory contents</b>

File | Description
 --- | ---
  OEMiROT_Appli/Src/com.c                       |  Communication
  OEMiROT_Appli/Src/main.c                      |  Main program
  OEMiROT_Appli/Src/startup_stm32h7s7xx.s       |  Startup file
  OEMiROT_Appli/Src/stm32h7rsxx_it.c            |  Interrupt handlers
  OEMiROT_Appli/Src/system_stm32h7rsxx.c        |  STM32H7RSxx system clock configuration file
  OEMiROT_Appli/Src/appli_flash_layout.h        |  Flash layout header file
  OEMiROT_Appli/Src/com.h                       |  Communication header file
  OEMiROT_Appli/Inc/main.h                      |  Main program header file
  OEMiROT_Appli/Inc/stm32h7rsxx_hal_conf.h      |  HAL Configuration file
  OEMiROT_Appli/Inc/stm32h7rsxx_it.h            |  Interrupt handlers header file
  OEMiROT_Appli/Binary                          |  Application firmware image (application binary + MCUBoot header and metadata)

### <b>Hardware and Software environment</b>

  - This template runs on STM32H7S7xx and STM32H7R7xx devices.
  - This example has been tested with STMicroelectronics STM32H7S78-DK (MB1736)
    board and can be easily tailored to any other supported device
    and development board.
  - To print the application menu in your UART console you have to configure it using these parameters:
    Speed: 115200, Data: 8bits, Parity: None, stop bits: 1, Flow control: none.

### <b>How to use it ?</b>

This project is targeted to boot through <b>OEMiROT boot path</b> (for <b>STiROT_OEMuROT boot path</b>, see in the section <b>Notes</b>).

<u>Before compiling the project, you should first:</u>

- <u>update the environment variables</u>
    - for **H7S and H7R**, with your own paths and com port (located in Firmware/Projects/STM32H7S78-DK/ROT_Provisioning/env.bat (.sh))
    - for **H7S and H7R**, with device type (located in Firmware/Projects/STM32H7S78-DK/ROT_Provisioning/OEMiROT/obkey_provisioning.bat, obkey_provisioning_open.bat, provisioning.bat (.sh))
    - for **H7R only**, with these following additional steps:
        - in Firmware/Projects/STM32H7S78-DK/ROT_Provisioning/DA/Config (and OEMiROT/Config), use a text editor to set the field DoEncryption to 0 in the *.xml* files
        - use STM32TrustedPackageCreator to generate matching *.obk* files in the folder Binary
        - in Firmware/Projects/STM32H7S78-DK/ROT_Provisioning/DA/Binary (and OEMiROT/Config), use a binary editor to ensure the field DoEncryption is set to 0 (byte number 8) in *.obk* files
        - in Firmware/Projects/STM32H7S78-DK/ROT_Provisioning/env.bat (.sh):

          replace these following two lines:

          ```
          set stm32ExtLoaderFlash=-elbl %stm32tool_path%/ExternalLoader/MX66UW1G45G_STM32H7S78-DK_XSPIM1-SFIx.stldr
          set stm32ExtLoaderFlashOpen=-el %stm32tool_path%/ExternalLoader/MX66UW1G45G_STM32H7S78-DK_XSPIM1.stldr
          ```

          by these lines:

          ```
          set stm32ExtLoaderFlash=-elbl %stm32tool_path%/ExternalLoader/MX66UW1G45G_STM32H7S78-DK-SFIx.stldr
          set stm32ExtLoaderFlashOpen=-el %stm32tool_path%/ExternalLoader/MX66UW1G45G_STM32H7S78-DK.stldr
          ```

- <u>start the provisioning process</u>. During the provisioning process, the linker files
as well as the postbuild command of the project will be automatically updated.

The <b>provisioning process</b> (ROT_Provisioning/OEMiROT/provisioning.bat (.sh)) is divided in 2 majors steps :

  - Step 1 : Image generation

     - OEMiROT binary: build OEMiROT_Boot project
     - Application firmware: build OEMiROT_Appli project
     - Code firmware image generation: automatically generated at the end of compilation (postbuild command)

  - Step 2 : Provisioning

     - Setting the product state to PROVISIONING
     - Programming the OBkeys
     - Flashing the OEMiROT_Boot image
     - Programming the option bytes
     - Flashing the OEMiROT_Appli image
     - Setting the final product state

If the product state is set to PROVISIONING or CLOSED, it is still possible to open the debug or to execute a regression
with the Debug Authentication feature. To do it, scripts (regression.bat (.sh) & dbg_auth.bat (.sh)) are available in the ROT_provisioning/DA folder.

For more details, refer to Wiki article available here : https://wiki.st.com/stm32mcu/wiki/Category:Security.

After application startup, check in your "UART console" the menu is well displayed:
```
=================== Main Menu =============================
    Start BootLoader -------------------------------------- 1
    Selection :
```

To update the application firmware version, you must:

  - Select the function "Start Bootloader" for more details
  - If the product state is CLOSED, use the script dbg_auth.bat (.sh) in the ROT_provisioning/DA folder to perform a "Forced download"
  - Connect STM32CubeProgrammer through:
      - USART if the product state is CLOSED.
      - ST-LINK if the product state is OPEN.
  - Download the new image
  - Reset the board
  - After authenticity and integrity checks, the new image is decrypted and installed.

#### <b>Notes:</b>

  1. Two versions of ROT_AppliConfig are available: windows executable and python version. By default, the windows executable is selected. It
     is possible to switch to python version by:
        - installing python (Python 3.10 or newer) with the required modules listed in requirements.txt.
        ```
        pip install -r requirements.txt
        ```
        - having python in execution path variable
        - deleting main.exe in Utilities\PC_Software\ROT_AppliConfig\dist

  2. This project can also be used as a <b>STiROT_OEMuROT boot path</b> application example (H7S only). In this case, the provisioning process
     is done with ROT_Provisioning/STiROT_OEMuROT/provisioning.bat (.sh).


