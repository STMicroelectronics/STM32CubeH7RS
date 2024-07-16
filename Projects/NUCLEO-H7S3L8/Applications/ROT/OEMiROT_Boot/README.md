## <b>OEMiROT_Boot application Description</b>

This project provides an OEMiROT example. OEMiROT boot path performs authenticity and integrity checks of the project firmware image.

The core function of this application relies on the mcuboot middleware.

This project shows how to implement an OEMiROT.

To ease the development process, a prebuild command and postbuild command are integrated in the project.
The prebuild command is in charge of preparing the OEMiROT_Boot scatter file according to flash layout description.
The postbuild command is in charge of preparing the provisioning scripts and the application image
(firmware application managed by OEMiROT) configuration files in ROT_Provisioning/OEMiROT folder,
according to the flash layout and OEMiROT configuration.

The system clock frequency is set to 380 MHz.
The maximum system clock frequency at 600Mhz can only be used if OEMIROT_USER_SRAM_ECC is not defined.

### <b>First steps</b>

  - from Firmware/Projects/STM32H7S78-DK/Applications/ROT/OEMiROT_Boot, copy and paste all folders into current folder
  - from Firmware/Projects/STM32H7S78-DK, copy and paste ROT_Provisioning folder into Firmware/Projects/NUCLEO-H7S3L8 if is not already done. Follow Firmware/Projects/STM32H7S78-DK/Applications/ROT/OEMiROT_Appli/readme to update it.
  - ideally, in toolchains files of current project (IAR, KEIL and STM32CubeIde), update project name from "STM32H7S78-DK" to "NUCLEO-H7S3L8" or "NUCLEO-H7R3L8" according to your board
  - in toolchains files of current project (IAR, KEIL and STM32CubeIde), update device name from "STM32H7S7xx" to "STM32H7S3xx" or "STM32H7R3xx" according to your device by following these steps:

    For IAR IDE:

    ```
    1- Go to "Options" and select "General Options" then in the "Target" field Select "device" to choose your device.

    2- Go to "Options" and select "C/C++ Compiler" then "Preprocessor" tab and change the compilation symbol (STM32H7R3xx or STM32H7S3xx).

    3- Open the "prebuild.cmd" file (located in "Firmware/Projects/NUCLEO-H7S3L8/Applications/ROT/OEMiROT_Boot/EWARM") and change the compilation symbol (-DSTM32H7R3xx or -DSTM32H7S3xx).
    ```

    For STM32CubeIDE:

    ```
    1- Go to "Properties" and select "C/C++ Build" tab then "Settings" and in the "Build Steps" tab change the command in "Pre-build steps" and change the compilation symbol (STM32H7S3xx).

    2- Go to "Properties" and select "C/C++ General" tab then "Paths an Symbols" and in the "Symbols" tab select symbols for "GNU C" and change the compilation flag (STM32H7S3xx)
    ```

    For Keil IDE:

    ```
    1- Go to "Options" and select "Device" tab to choose your device.

    2- Go to "Options" and select "C/C++ (AC6)" tab then in the "Define" field in "Preprocessor Symbols" and change the compilation symbol (STM32H7R3xx or STM32H7S3xx).

    3- Go to "Options" and in the "User" tab change "Run #2" command in the "Before Build/Rebuild" field and add the define of your platform ("-DSTM32H7R3xx" or "-DSTM32H7S3xx").
    ```


### <b>Keywords</b>

OEMiROT, Boot path, Root Of Trust, Security, mcuboot

### <b>Directory contents</b>

File | Description
 --- | ---
  OEMiROT_Boot/Inc/mcuboot_config/mcuboot_config.h |  Mcuboot configuration file
  OEMiROT_Boot/Inc/boot_hal_cfg.h                  |  Platform configuration file for OEMiROT_Boot
  OEMiROT_Boot/Inc/boot_hal_flowcontrol.h          |  Header file for flow control code in boot_hal.c
  OEMiROT_Boot/Inc/boot_hal_hash_ref.h             |  Header file for hash reference code in boot_hal.c
  OEMiROT_Boot/Inc/boot_hal_imagevalid.h           |  Header file for image validation code in boot_hal.c
  OEMiROT_Boot/Inc/boot_hal_mce.h                  |  Header file for MCE code in boot_hal.c
  OEMiROT_Boot/Inc/cmsis.h                         |  Header file for CMSIS
  OEMiROT_Boot/Inc/config-boot.h                   |  Mbed-crypto configuration file
  OEMiROT_Boot/Inc/flash_layout.h                  |  Flash mapping
  OEMiROT_Boot/Inc/low_level_ext_flash.h           |  Header file for low_level_ext_flash.c
  OEMiROT_Boot/Inc/low_level_ext_ram.h             |  Header file for low_level_ext_ram.c
  OEMiROT_Boot/Inc/low_level_flash.h               |  Header file for low_level_flash.c
  OEMiROT_Boot/Inc/low_level_mce.h                 |  Header file for low_level_mce.c
  OEMiROT_Boot/Inc/low_level_obkeys.h              |  Header file for low_level_obkeys.c
  OEMiROT_Boot/Inc/low_level_ramecc.h              |  Header file for low_level_ramecc.c
  OEMiROT_Boot/Inc/low_level_rng.h                 |  Header file for low_level_rng.c
  OEMiROT_Boot/Inc/low_level_security.h            |  Header file for low_level_security.c
  OEMiROT_Boot/Inc/region_defs.h                   |  RAM and FLASH regions definitions
  OEMiROT_Boot/Inc/stm32_extmem_conf.h             |  STM32_ExtMem_Manager configuration file
  OEMiROT_Boot/Inc/stm32h7rsxx_hal_conf.h          |  HAL driver configuration file
  OEMiROT_Boot/Inc/target_cfg.h                    |  Header file for target start up
  OEMiROT_Boot/Src/bl2_nv_services.c               |  Non Volatile services for OEMiROT_Boot
  OEMiROT_Boot/Src/boot_hal.c                      |  Platform initialization
  OEMiROT_Boot/Src/image_macros_to_preprocess_bl2.c |  Images definitions to preprocess for bl2
  OEMiROT_Boot/Src/keys_map.c                      |  keys indirection to access keys in OBKeys area
  OEMiROT_Boot/Src/low_level_com.c                 |  UART low level interface
  OEMiROT_Boot/Src/low_level_device.c              |  Flash Low level device configuration
  OEMiROT_Boot/Src/low_level_ext_flash.c           |  External Flash Low level interface
  OEMiROT_Boot/Src/low_level_ext_ram.c             |  External RAM Low level interface
  OEMiROT_Boot/Src/low_level_extmem_device.c       |  External Memory Low level device configuration
  OEMiROT_Boot/Src/low_level_flash.c               |  Flash Low level interface
  OEMiROT_Boot/Src/low_level_mce.c                 |  MCE Low level interface
  OEMiROT_Boot/Src/low_level_obkeys.c              |  OBKEY Low level interface
  OEMiROT_Boot/Src/low_level_obkeys_device.c       |  OBKEY Low level device configuration
  OEMiROT_Boot/Src/low_level_ramecc.c              |  RAMECC Low level interface
  OEMiROT_Boot/Src/low_level_rng.c                 |  Random generator interface
  OEMiROT_Boot/Src/low_level_security.c            |  Security Low level services
  OEMiROT_Boot/Src/startup_stm32h7rsxx.c           |  Startup file in c
  OEMiROT_Boot/Src/stm32h7rsxx_hal_msp.c           |  HAL MSP module
  OEMiROT_Boot/Src/system_stm32h7rsxx.c            |  System Init file
  OEMiROT_Boot/Src/tick.c                          |  HAL Tick implementation

### <b>Hardware and Software environment</b>

  - This template runs on STM32H7S3xx devices.
  - It is possible to tailor it for STM32H7R3xx devices by following steps described above
  - This example has been tested with STMicroelectronics NUCLEO-H7S3L8 (MB1737)
    board and can be easily tailored to any other supported device
    and development board.
  - To get debug print in your UART console you have to configure it using these parameters:
    Speed: 115200, Data: 8bits, Parity: None, stop bits: 1, Flow control: none.


### <b>How to use it ?</b>

In order to build the OEMiROT_Boot project, you must do the following:

 - Open your preferred toolchain
 - Rebuild the project

Then refer to OEMiROT_Appli readme for example of application booted through OEMiROT boot path (or through STiROT_OEMuROT boot path).

### <b>Notes</b>

1. This template can be used when using H7S board in STM32CubeIde

2. For STM32H7R3xx devices, this template is not supported with STM32CubeIDE

3. For STM32H7S3xx and STM32H7R3xx devices, with all toolchains, OEMIROT_LOAD_AND_RUN should be set to NO_LOAD_AND_RUN or LOAD_AND_RUN_INT_RAM. Update it in Inc\\flash_layout.h:

    ```
    #define OEMIROT_LOAD_AND_RUN NO_LOAD_AND_RUN /*!< Flag configuring the feature "Load and Run" */
    #define OEMIROT_LOAD_AND_RUN LOAD_AND_RUN_INT_RAM /*!< Flag configuring the feature "Load and Run" */
    ```

4. By default the anti-tamper is enabled for internal tamper events only. It is possible to change this configuration with
   OEMIROT_TAMPER_ENABLE define in Inc\\boot_hal_cfg.h.

   ```
   #define NO_TAMPER            (0)                   /*!< No tamper activated */
   #define INTERNAL_TAMPER_ONLY (1)                   /*!< Only Internal tamper activated */
   #define ALL_TAMPER           (2)                   /*!< Internal and External tamper activated */
   #define OEMIROT_TAMPER_ENABLE INTERNAL_TAMPER_ONLY /*!< TAMPER configuration flag  */
   ```

   If OEMIROT_TAMPER_ENABLE is changed to ALL_TAMPER, the anti-tamper protection is enabled with active tamper pins usage.
   It is needed to connect TAMP_IN2 (PB9 on CN7 pin 4) and TAMP_OUT8 (PC1 on CN9 pin 20) on the NUCLEO-H7S3L8 board (unplug/plug of the board may be needed),
   to allow the application to run. In case the tamper pins are opened or shorted, then the application is reset and blocked.

   If the product state is set to CLOSED or LOCKED, even with OEMIROT_DEV_MODE enabled, there will be no warning traces about
   anti-tamper detection. In case the tamper pins are opened or shorted, the application is reset and blocked at RSS level.
