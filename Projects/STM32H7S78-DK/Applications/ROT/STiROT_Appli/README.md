## <b>STiROT_Appli application Description</b>

This project provides a STiROT boot path application example. Boot is performed through <b>STiROT boot path</b> after authenticity and integrity checks of the application firmware image.

As example, this application will display a menu when started.

The system clock is set to 380 MHz to be functional with all hardware configurations (ECC_ON_SRAM enabled, no internal regulator)
and with the full range of temperature (Tj up to 125°). In other circumstances, the system clock can be increased up to 600 Mhz.


### <b>Keywords</b>

STiROT, Boot path, Root Of Trust, MPU

### <b>Directory contents</b>

File | Description
 --- | ---
  ROT/STiROT_Appli/Src/com.c                       |  UART low level interface
  ROT/STiROT_Appli/Src/main.c                      |  Main program
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

  - This example has been tested with STMicroelectronics STM32H7S78-DK (MB1736)
    board and can be easily tailored to any other supported device
    and development board.

  - To print the application menu in your UART console you have to configure it using these parameters:
    Speed: 115200, Data: 8bits, Parity: None, stop bits: 1, Flow control: none.

### <b>How to use it ?</b>

<u>Before compiling the project, you should first start the provisioning process</u>. During the provisioning process, the linker files
will be automatically updated.

Before starting the provisioning process, select the application project to use (application example or template),
through stirot_boot_path_project variable in ROT_Provisioning/env.bat or env.sh.
Then start provisioning process by running provisioning.bat (.sh) from ROT_Provisioning/STiROT,
and follow its instructions. Refer to ROT_Provisioning/STiROT readme for more information on the provisioning process for STiROT boot path.

If the product state is set to CLOSED, it is still possible to open the debug or to execute a full regression
with the Debug Authentication feature. To do it, scripts (regression.bat & dbg_auth.bat) are available in the ROT_provisioning/DA folder.

After application startup, check in your "UART console" the menu is well displayed:
```
=================== Main Menu =============================
   Start < Function A > -------------------------------------------------- 1
   Start < Function B > -------------------------------------------------- 2
   Selection :
```

For more details, refer to STM32H7S Wiki articles:

  - [STiRoT for STM32H7S](https://wiki.st.com/stm32mcu/wiki/Security:STiRoT_for_STM32H7S).
  - [How to start with STiRoT on STM32H7S](https://wiki.st.com/stm32mcu/wiki/Security:How_to_start_with_STiRoT_on_STM32H7S).


#### <b>Notes:</b>

Be aware that a default MPU configuration is already applied by STiROT when jumping in your application. To ensure the security of the
device, the MPU is configured to allow only the user application code area to be executed, minimizing the risk of unauthorized code execution.
It is the user application responsibility to reconfigure the MPU to fit with its security needs.



