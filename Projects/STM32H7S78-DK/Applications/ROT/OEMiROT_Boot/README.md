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

  - This template runs on STM32H7S7xx devices.
  - This example has been tested with STMicroelectronics STM32H7S78-DK (MB1736)
    board and can be easily tailored to any other supported device
    and development board.
  - To get debug print in your UART console you have to configure it using these parameters:
    Speed: 115200, Data: 8bits, Parity: None, stop bits: 1, Flow control: none.


### <b>How to use it?</b>

In order to build the OEMiROT_Boot project, you must do the following:

 - Open your preferred toolchain
 - Rebuild the project

Then refer to OEMiROT_Appli readme for example of application booted through OEMiROT boot path (or through STiROT_OEMuROT boot path).

### <b>Notes</b>

1. With STM32CubeIDE, the following configurations are not compatible with the Build Configuration "Debug":
   - OEMUROT_ENABLE undefined, OEMIROT_LOAD_AND_RUN set to NO_LOAD_AND_RUN, MCUBOOT_OVERWRITE_ONLY undefined
   - OEMUROT_ENABLE undefined, OEMIROT_LOAD_AND_RUN set to LOAD_AND_RUN_INT_RAM, MCUBOOT_OVERWRITE_ONLY undefined

   They can be used with the Build Configuration "Release".

2. With STM32CubeIDE, the following configuration isn't supported:
   - OEMUROT_ENABLE undefined, OEMIROT_LOAD_AND_RUN set to LOAD_AND_RUN_EXT_RAM, MCUBOOT_OVERWRITE_ONLY undefined

3. By default the anti-tamper is enabled for internal tamper events only. It is possible to change this configuration with
   OEMIROT_TAMPER_ENABLE define in Inc\\boot_hal_cfg.h.

   ```
   #define NO_TAMPER            (0)                   /*!< No tamper activated */
   #define INTERNAL_TAMPER_ONLY (1)                   /*!< Only Internal tamper activated */
   #define ALL_TAMPER           (2)                   /*!< Internal and External tamper activated */
   #define OEMIROT_TAMPER_ENABLE INTERNAL_TAMPER_ONLY /*!< TAMPER configuration flag  */
   ```

   If OEMIROT_TAMPER_ENABLE is changed to ALL_TAMPER, the anti-tamper protection is enabled with active tamper pins usage.
   It is needed to connect TAMP_IN2 (PB9 on CN3 pin 10) and TAMP_OUT8 (PC1 on CN11 pin 6) on the STM32H7S78-DK board,
   to allow the application to run. In case the tamper pins are opened or shorted, then the application is reset and blocked.

   If the product state is set to CLOSED or LOCKED, even with OEMIROT_DEV_MODE enabled, there will be no warning traces about
   anti-tamper detection. In case the tamper pins are opened or shorted, the application is reset and blocked at RSS level.
