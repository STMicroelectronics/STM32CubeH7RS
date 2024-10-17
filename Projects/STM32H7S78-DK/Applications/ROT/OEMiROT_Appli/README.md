## <b>OEMiROT_Appli application Description</b>

This project provides an OEMiROT or STiROT_OEMuROT boot path application example. Boot is performed through <b>OEMiROT boot path</b> or <b>STiROT_OEMuROT boot path</b>
after authenticity and integrity checks of the application firmware image.

This application will display a menu on the console explaining how to download through bootloader a new version of the application firmware. At next reset,
the OEMiROT will decrypt and install this new version after successful check of the authenticity and the integrity of the image.

The system clock frequency is the one configured by OEMiROT.

### <b>Keywords</b>

OEMiROT, boot path, Root Of Trust, MPU

### <b>Directory contents</b>

File | Description
 --- | ---
  OEMiROT_Appli/Src/com.c                       |  Communication
  OEMiROT_Appli/Src/main.c                      |  Main program
  OEMiROT_Appli/Src/stm32h7rsxx_it.c            |  Interrupt handlers
  OEMiROT_Appli/Src/system_stm32h7rsxx.c        |  System Init file
  OEMiROT_Appli/Src/appli_flash_layout.h        |  Flash layout header file
  OEMiROT_Appli/Src/com.h                       |  Communication header file
  OEMiROT_Appli/Inc/main.h                      |  Main program header file
  OEMiROT_Appli/Inc/stm32h7rsxx_hal_conf.h      |  HAL Configuration file
  OEMiROT_Appli/Inc/stm32h7rsxx_it.h            |  Interrupt handlers header file
  OEMiROT_Appli/Binary                          |  Application firmware image (application binary + MCUBoot header and metadata)

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S7xx devices.

  - This example has been tested with STMicroelectronics STM32H7S78-DK (MB1736) board and can be easily tailored for
    STM32H7R7xx devices by following these steps
    (more details in the wiki article [<b>How to create ROT examples for STM32H7RS</b>](https://wiki.st.com/stm32mcu/wiki/Security:How_to_create_ROT_examples_for_STM32H7RS)).

  - To print the application menu in your UART console you have to configure it using these parameters:
    Speed: 115200, Data: 8bits, Parity: None, stop bits: 1, Flow control: none.

### <b>How to use it ?</b>

<u>Before compiling the project, you should first start the provisioning process</u>. During the provisioning process, the linker files
will be automatically updated.

Before starting the provisioning process, select the application project to use (application example or template),
through oemirot_boot_path_project variable in ROT_Provisioning/env.bat or env.sh.
Then start provisioning process by running provisioning.bat (.sh) from ROT_Provisioning/OEMiROT or ROT_Provisioning/STiROT_OEMuROT,
and follow its instructions. Refer to ROT_Provisioning/OEMiROT or ROT_Provisioning/STiROT_OEMuROT readme for more information on
the provisioning process for OEMiROT boot path or STiROT_OEMuROT boot path.

If the product state is set to PROVISIONING or CLOSED, it is still possible to open the debug or to execute a regression
with the Debug Authentication feature. To do it, scripts (regression.bat (.sh) & dbg_auth.bat (.sh)) are available in the ROT_provisioning/DA folder.

After application startup, check in your "UART console" the menu is well displayed:
```
=================== Main Menu =============================
    Start BootLoader -------------------------------------- 1
    Selection :
```

To update the application firmware version, you must:

  - Select the function "Start Bootloader"
  - If the product state is CLOSED, use the script dbg_auth.bat (.sh) in the ROT_provisioning/DA folder to perform a "Forced download"
  - Connect STM32CubeProgrammer through:
      - USART if the product state is CLOSED.
      - ST-LINK if the product state is OPEN.
  - Download the new image
  - Reset the board
  - After authenticity and integrity checks, the new image is decrypted and installed.

For more details, refer to STM32H7RS Wiki articles:

  - [OEMiRoT OEMuRoT for STM32H7S](https://wiki.st.com/stm32mcu/wiki/Security:OEMiRoT_OEMuRoT_for_STM32H7S).
  - [How to start with OEMiRoT on STM32H7S](https://wiki.st.com/stm32mcu/wiki/Security:How_to_start_with_OEMiRoT_on_STM32H7S).
  - [How to start with STiRoT OEMuRoT on STM32H7S](https://wiki.st.com/stm32mcu/wiki/Security:How_to_start_with_STiRoT_OEMuRoT_on_STM32H7S).


#### <b>Note:</b>

Be aware that a default MPU configuration is already applied by OEMiRoT when jumping in your application. To ensure the security of the
device, the MPU is configured to allow only the user application code area to be executed, minimizing the risk of unauthorized code execution.
It is the user application responsibility to reconfigure the MPU to fit with its security needs.