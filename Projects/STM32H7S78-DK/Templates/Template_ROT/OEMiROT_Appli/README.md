## <b>OEMiROT_Appli Template Description</b>

This project provides a OEMiROT boot path application example. Boot is performed through OEMiROT bootpath after authenticity and integrity checks of the project firmware image.

This project template is based on the STM32Cube HAL API. It highlights the ROT specificies:

  - Specific memory regions, defines automatically updated in the linker file.
  - Postbuild script (postbuild.bat (.sh)) called from the project files.
  - MPU configuration.
  - Watchdog reloading.

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals and initialize the systick used as 1ms HAL timebase.

The SystemClock_Config() function is used to configure the system clock (SYSCLK).

The system clock frequency is the one configured by OEMiROT / OEMuROT.

### <b>Keywords</b>

Reference, Template, OEMiROT, Boot path, Root Of Trust, MPU

### <b>Directory contents</b>

#### <b>Sub-project OEMiROT_Appli</b>

File | Description
 --- | ---
  Templates/Template_ROT/OEMiROT_Appli/Appli/Inc/main.h                      |  Header for main.c module
  Templates/Template_ROT/OEMiROT_Appli/Appli/Inc/stm32h7rsxx_hal_conf.h      |  HAL Configuration file
  Templates/Template_ROT/OEMiROT_Appli/Appli/Inc/stm32h7rsxx_it.h            |  Interrupt handlers header file
  Templates/Template_ROT/OEMiROT_Appli/Appli/Inc/stm32h7s78_discovery_conf.h |  BSP Configuration file
  Templates/Template_ROT/OEMiROT_Appli/Appli/Src/main.c                      |  Main program
  Templates/Template_ROT/OEMiROT_Appli/Appli/Src/stm32h7rsxx_hal_msp.c       |  HAL MSP module
  Templates/Template_ROT/OEMiROT_Appli/Appli/Src/stm32h7rsxx_it.c            |  Interrupt handlers
  Templates/Template_ROT/OEMiROT_Appli/Appli/Src/system_stm32h7rsxx.c        |  STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S7xx devices.

  - This example has been tested with STMicroelectronics STM32H7S78-DK (MB1736) board and can be easily tailored for
    STM32H7R7xx devices by following these steps
    (more details in the wiki article [<b>How to create ROT examples for STM32H7RS</b>](https://wiki.st.com/stm32mcu/wiki/Security:How_to_create_ROT_examples_for_STM32H7RS)).

### <b>How to use it ?</b>

<u>Before compiling the project, you should first start the provisioning process</u>. During the provisioning process, the linker files
will be automatically updated.

Before starting the provisioning process, select the application project to use (application example or template),
through oemirot_appli_path_project variable in ROT_Provisioning/env.bat or env.sh.
Then start provisioning process by running provisioning.bat (.sh) from ROT_Provisioning/OEMiROT or ROT_Provisioning/STiROT_OEMuROT,
and follow its instructions. Refer to ROT_Provisioning/OEMiROT or ROT_Provisioning/STiROT_OEMuROT readme for more information on
the provisioning process for OEMiROT boot path or STiROT_OEMuROT boot path.

If the product state is set to PROVISIONING or CLOSED, it is still possible to open the debug or to execute a regression
with the Debug Authentication feature. To do it, scripts (regression.bat (.sh) & dbg_auth.bat (.sh)) are available in the ROT_provisioning/DA folder.

For more details, refer to STM32H7RS Wiki articles:

  - [OEMiRoT OEMuRoT for STM32H7S](https://wiki.st.com/stm32mcu/wiki/Security:OEMiRoT_OEMuRoT_for_STM32H7S).
  - [How to start with OEMiRoT on STM32H7S](https://wiki.st.com/stm32mcu/wiki/Security:How_to_start_with_OEMiRoT_on_STM32H7S).
  - [How to start with STiRoT OEMuRoT on STM32H7S](https://wiki.st.com/stm32mcu/wiki/Security:How_to_start_with_STiRoT_OEMuRoT_on_STM32H7S).

#### <b>Notes:</b>

Be aware that a default MPU configuration is already applied by OEMiRoT when jumping in your application. To ensure the security of the
device, the MPU is configured to allow only the user application code area to be executed, minimizing the risk of unauthorized code execution.
It is the user application responsibility to reconfigure the MPU to fit with its security needs.