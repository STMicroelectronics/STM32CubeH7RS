## <b>STiROT_iLoader Application Description</b>

This project provides an immutable loader example required for <b>STiROT boot path</b> or <b>STiROT_OEMuROT boot path</b>.

STiRoT, which is an immutable code, is designed to be independent from external flash memory. Therefore,
the control of the authenticity and integrity of the user application is done by STiRoT from the internal RAM.
STiRoT runs in isolation level 1 (HDPL1).
STiROT_iLoader project is provided to do the interface between the internal RAM and the external flash memory. This
project can be adapted to the external flash memory selected by the customer. The iLoader binary is programmed
in user flash memory during the provisioning process.
STiROT_iLoader runs in isolation level 3 (HDPL3) and has therefore no access to the STiRoT configuration data
(including the encryption and authentication keys) stored in HDPL1 OBKeys.

Depending on its state machine, STiRoT requests STiROT_iLoader:

- To copy the user application code image from the external flash memory to the internal RAM. The purpose
is to detect if a new user application image must be installed or to verify if an existing user application
image can be executed.
- To copy the user application code from the internal RAM memory to the external flash memory at the end
of the installation process. During the copy, the user application code image is reencrypted by MCE
peripheral. The encryption key is configured and locked inside MCE by STiRoT.

A reset is generated after each STiROT_iLoader action to continue the secure firmware update and the secure boot
process.

From security point of view STiROT_iLoader project must be trusted and is therefore under flash memory write
protection (immutability).

The communication between STiROT and STiLoader is performed trough two channels:

- RSSCMD register: write channel, STiROT_iLoader provides STiROT with a status on the last operation performed
- shared RAM: read channel, STiROT notifies STiROT_iLoader:
  - the operation to be performed (load or save)
  - the configuration of the memory slots

XSPI and DMA IPs are activated to access external flash memory.

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

<u>Before compiling the project, you should first start the provisioning process</u>. STiROT_iLoader compilation, flash programming
and memory protection configuration are part of the provisioning process.

Before starting the provisioning process, select the application project to use (application example or template),
through STiROT_boot_path_project variable in ROT_Provisioning/env.bat or env.sh.
Then start provisioning process by running provisioning.bat (.sh) from ROT_Provisioning/STiROT,
and follow its instructions. Refer to ROT_Provisioning/STiROT readme for more information on the provisioning process for STiROT boot path.

If the product state is set to CLOSED, it is still possible to open the debug or to execute a full regression
with the Debug Authentication feature. To do it, scripts (regression.bat & dbg_auth.bat) are available in the ROT_provisioning/DA folder.

For more details, refer to STM32H7RS Wiki articles:

  - [STiRoT for STM32H7S](https://wiki.st.com/stm32mcu/wiki/Security:STiRoT_for_STM32H7S).
  - [How to start with STiRoT on STM32H7S](https://wiki.st.com/stm32mcu/wiki/Security:How_to_start_with_STiRoT_on_STM32H7S).
