## <b>STiROT_Appli application Description</b>

This project provides a STiROT boot path application example. Boot is performed through STiROT boot path after authenticity and integrity checks of the project firmware image.

This project is targeted to build an <u> application</u>. When the application is started the MPU
is already configured (by STiROT) to limit the execution area to the project firmware execution slot. This is done in order to avoid
any malicious code execution from an unauthorised area (RAM, out of execution slot in user flash ...). Once started, it is up to the
application to adapt the security configuration to its needs.

### <b>First steps</b>

  - from Firmware/Projects/STM32H7S78-DK/Applications/ROT/STiROT_Appli, copy and paste all folders into current folder
  - from Firmware/Projects/STM32H7S78-DK, copy and paste ROT_Provisioning folder into Firmware/Projects/NUCLEO-H7S3L8 if is not already done. Follow Firmware/Projects/STM32H7S78-DK/Applications/ROT/OEMiROT_Appli/readme to update it.
  - ideally, in toolchains files of current project (IAR, KEIL and STM32CubeIde), update project name from "STM32H7S78-DK" to "NUCLEO-H7S3L8"
  - in toolchains files of current project (IAR, KEIL and STM32CubeIde), update device name from "STM32H7S7xx" to "STM32H7S3xx" (for more details, see the steps described on OEMiROT_Boot readme)

### <b>Keywords</b>

STiROT, Boot path, Root Of Trust, MPU

### <b>Directory contents</b>

File | Description
 --- | ---
  ROT/STiROT_Appli/Src/com.c                       |  UART low level interface
  ROT/STiROT_Appli/Src/main.c                      |  Main program
  ROT/STiROT_Appli/Src/startup_stm32h7s7xx.s       |  Startup file
  ROT/STiROT_Appli/Src/stm32h7rsxx_it.c            |  Interrupt handlers
  ROT/STiROT_Appli/Src/system_stm32h7rsxx.c        |  STM32H7Sxx system source file
  ROT/STiROT_Appli/Src/test_versions.c             |  Print application version
  ROT/STiROT_Appli/Inc/com.h                       |  Header file for com.c
  ROT/STiROT_Appli/Inc/main.h                      |  Header for main.c module
  ROT/STiROT_Appli/Inc/stm32h7rsxx_hal_conf.h      |  HAL configuration file
  ROT/STiROT_Appli/Inc/stm32h7rsxx_it.h            |  Interrupt handlers header file
  ROT/STiROT_Appli/Inc/test_versions.h             |  Header for test_versions.c file
  ROT/STiROT_Appli/Binary                          |  Application firmware image (application binary + MCUBoot header and metadata)

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7Sxx devices.
  - This example has been tested with STMicroelectronics NUCLEO-H7S3L8 (MB1737)
    board and can be easily tailored to any other supported device
    and development board.
  - To print the application menu in your UART console you have to configure it using these parameters:
    Speed: 115200, Data: 8bits, Parity: None, stop bits: 1, Flow control: none.

### <b>How to use it ?</b>

This project is targeted to boot through <b>STiROT boot path</b>.

<u>Before compiling the project, you should first:</u>

- <u>update the environment variables</u> with your own paths (located in ../../../ROT_Provisioning/env.bat)
- <u>start the provisioning process</u>. During the provisioning process, the linker files
as well as the postbuild command of the project will be automatically updated.

The <b>provisioning process</b> (ROT_Provisioning/STiROT/provisioning.bat) is divided in 3 major steps:

  - Step 0 : Preliminary stage

     - OEMiLoader firmware generation:
        - prebuild automatically updates:
          - env.bat: to be usable then by the provisioning script
          - stm32h7s7xx_flash.icf: to align size and offset on H7RS sector constraint (0x2000)

  - Step 1 : Configuration management

     - STiROT_Config.obk generation
        - Pre-update with OEMiLoader dependencies
        - From TrustedPackageCreator (OBkey tab in Security panel)
     - DA_Config.obk generation with TrustedPackageCreator (OBkey tab in Security panel)
     - Automatic update of the option bytes flash programming script with regards of the defined configuration
     - Automatic update of the project files (.icf, .ewp) with regards of the defined configuration

  - Step 2 : Image generation

     - Code firmware image generation: automatically generated at the end of the project compilation (postbuild command of Project)

  - Step 3 : Provisioning

     - Programming the OBkeys
     - Programming the option bytes
     - Flashing the images (code, immutable Loader)
     - Setting the product state

If the product state is set to CLOSED, it is still possible to open the debug or to execute a full regression
with the Debug Authentication feature. To do it, scripts (regression.bat & dbg_auth.bat) are available in the ROT_provisioning/DA folder.

For more details, refer to Wiki article available here : https://wiki.st.com/stm32mcu/wiki/Category:Security

After application startup, check in your "UART console" the menu is well displayed:
```
=================== Main Menu =============================
   Start < Function A > -------------------------------------------------- 1
   Start < Function B > -------------------------------------------------- 2
   Selection :
```

#### <b>Notes:</b>

  1. The most efficient way to develop and debug an application is to boot directly on user flash in the Open product state by setting with
     STM32CubeProgrammer the PRODUCT_STATE to OPEN.

  2. Two versions of ROT_AppliConfig are available: windows executable and python version. By default, the windows executable is selected. It
     is possible to switch to python version by:
        - installing python (Python 3.10 or newer) with the required modules listed in requirements.txt.
        ```
        pip install -r requirements.txt
        ```
        - having python in execution path variable
        - deleting main.exe in Utilities\PC_Software\ROT_AppliConfig\dist



