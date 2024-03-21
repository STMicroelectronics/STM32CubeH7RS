## <b>STiROT_iLoader Description</b>

This project provides a STiROT boot path immutable loader example. Boot is performed through STiROT boot path after
write-protection check on the flash.

This project is targeted to build an immutable loader. This immutable loader is interfaced between
the external non-volatile memory and the internal volatile memory.  
The immutable loader is used by STiROT:

- on a boot sequence, to load a firmware from the external non-volatile memory to the internal volatile memory
- on an update sequence, to save a valid update in the external non-volatile memory and/or erase a bad request.

This loader is interfaced with STiROT via two communication channels:

- RSSCMD register: write channel, it notifies STiROT on the load status in the internal volatile memory
- shared RAM: read channel, STiROT writes in it to share configuration with the loader:
  - the operation to be performed by the immutable loader (load or save)
  - the definition of the memory slots

The external non-volatile memory is used with the MCE to encrypt/decrypt its content. The MCE configuration is done by STiROT.  
The immutable loader must not configure it.  
The immutable loader is interfaced with XSPI to the external non-volatile memory. To write in the external non-volatile memory
the DMA write must be used.  

### <b>Keywords</b>

STiROT, Boot path, Root Of Trust, MCE, XSPI, DMA

### <b>Directory contents</b>

File | Description
 --- | ---
  ROT/STiROT_iLoader/Inc/boot_hal_iloader.h          |  Header for iloader life-cycle
  ROT/STiROT_iLoader/Inc/com.h                       |  Header for com.c module
  ROT/STiROT_iLoader/Inc/loader.h                    |  Header for loader.c module
  ROT/STiROT_iLoader/Inc/low_level_flash_ext.h       |  Header for low_level_flash_ext.c module
  ROT/STiROT_iLoader/Inc/low_level_shared_mem.h      |  Header for low_level_shared_mem.c module
  ROT/STiROT_iLoader/Inc/main.h                      |  Header for main.c module
  ROT/STiROT_iLoader/Inc/region_defs.h               |  Memory region configuration file
  ROT/STiROT_iLoader/Inc/stm32extmem_conf.h          |  STM32_ExtMem Configuration file
  ROT/STiROT_iLoader/Inc/stm32h7rsxx_hal_conf.h      |  HAL Configuration file
  ROT/STiROT_iLoader/Inc/stm32h7rsxx_it.h            |  Interrupt handlers header file
  ROT/STiROT_iLoader/Inc/stm32h7s78_discovery_conf.h |  BSP Configuration file
  ROT/STiROT_iLoader/Src/com.c                       |  Uart low level interface
  ROT/STiROT_iLoader/Src/loader.c                    |  Loader services
  ROT/STiROT_iLoader/Src/low_level_flash_ext.c       |  External Flash Low level services
  ROT/STiROT_iLoader/Inc/low_level_shared_mem.c      |  Shared memory Low level services
  ROT/STiROT_iLoader/Src/main.c                      |  Main program
  ROT/STiROT_iLoader/Src/stm32h7rsxx_hal_msp.c       |  HAL MSP module
  ROT/STiROT_iLoader/Src/stm32h7rsxx_it.c            |  Interrupt handlers
  ROT/STiROT_iLoader/Src/system_stm32h7rsxx.c        |  STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This template runs on STM32H7S7xx devices.

  - This template has been tested with STMicroelectronics STM32H7S78-DK
    boards and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

This project is targeted to boot through <b>STiROT boot path</b>.

<u>Before compiling the project, you should first start the provisioning process</u>. During the provisioning process, the linker files
as well as the postbuild command of the project will be automatically updated.

The <b>provisioning process</b> (ROT_Provisioning/STiROT/provisioning.bat) is divided in 3 majors steps:

  - Step 0 : Preliminary stage

     - OEMiLoader firmware generation:
        - prebuild automatically updates:
          - env.bat: to be usable then by the provisioning script
          - stm32h7s7xx_flash.icf: to align size and offset on H7RS sector constraint (0x2000)

  - Step 1 : Configuration management

     - STiROT_Config.obk generation
        - Pre-update with OEMiLoader dependencies
        - From TrustedPackageCreator (tab H7S-OBkey)
     - DA_Config.obk generation with TrustedPackageCreator (tab H7S-OBkey)
     - Automatic update of the option bytes flash programming script with regards of the defined configuration
     - Automatic update of the project files (.icf, .ewp) with regards of the defined configuration

  - Step 2 : Image generation

     - Code firmware image generation: automatically generated at the end of the project compilation (postbuild command of Project)

  - Step 3 : Provisioning

     - Programming the OBkeys
     - Programming the option bytes
     - Flashing the images (code, immutable Loader)
     - Setting the product state

As soon as the immutable loader is flashed, its memory area becomes write-protected.

If the product state is set to CLOSED, it is still possible to open the debug or to execute a regression
with the Debug Authentication feature. To do it, scripts (regression.bat & dbg_auth.bat) are available in the ROT_provisioning/DA folder.

For more details, refer to Wiki article available here : https://wiki.st.com/stm32mcu/wiki/Category:Security

To update the immutable loader firmware, you must:

  - Perform a regression
  - Repeat the provisioning script

#### <b>Notes:</b>

  1. Two versions of ROT_AppliConfig are available: windows executable and python version. By default, the windows executable is selected. It
     is possible to switch to python version by:
        - installing python (Python 3.10 or newer) with the required modules listed in requirements.txt.
        ```
        pip install -r requirements.txt
        ```
        - having python in execution path variable
        - deleting main.exe in Utilities\PC_Software\ROT_AppliConfig\dist
