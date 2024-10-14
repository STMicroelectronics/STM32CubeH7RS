## <b>ROT_Provisioning STiROT Description</b>

This section provides an overview of the available scripts for STiROT boot path.

STiROT stands for "ST Immutable Root of Trust" and it provides two services:

  - Secure Boot: Verification of the integrity, and authenticity of the application code before any execution.
  - Secure Firmware Update: If a new firmware image is available on detection, check of its authenticity.
                            Once the firmware is decrypted, check of the integrity of the code before installing it.

### <b>Keywords</b>

STiROT, boot path, Root Of Trust, Security

### <b>Directory contents</b>

<b>Sub-directories</b>

- STiROT/Binary                               Output binaries and generated images.
- STiROT/Config                               STiROT configuration files.
- STiROT/Image                                Image configuration files.
- STiROT/Keys                                 Keys for firmware image authentication and encryption.

<b>Scripts</b>

- STiROT/ob_flash_programming.bat/.sh         Programs option bytes and firmware image on the device.
- STiROT/obkey_provisioning.bat/.sh           Programs OBKeys on the device.
- STiROT/provisioning.bat/.sh                 Performs device provisioning process.
- STiROT/update_appli_setup.bat/.sh           Updates application files according to STiRoT configuration.
- STiROT/update_ob_setup.bat/.sh              Updates the programming scripts according to STiRoT configuration.
- STiROT/update_stirot_iloader_setup.bat/.sh  Updates iLoader files according to STiRoT configuration.

### <b>Hardware and Software environment</b>

- This example has been tested with STMicroelectronics STM32H7S78-DK (MB1736)
  board and can be easily tailored to any other supported device and development board.

### <b>How to use it?</b>

To use STiRoT bootpath, you should first configure ROT_Provisioning/env.bat/.sh script
(tools path, application path and COM port configuration).<br>
The .bat scripts are designed for Windows, whereas the .sh scripts are designed for Linux and Mac-OS.

Then you should run the provisioning script (provisioning.bat/.sh).<br>
During the **provisioning process**, the programming scripts and the application files will
be automatically updated according to STiRoT configuration, and user answers.

The **provisioning process** (STiROT/provisioning.bat/.sh) is divided into 3 majors steps:

- Step 0: Preliminary stage
- Step 1: Configuration management
- Step 2: STiROT application image generation
- Step 3: Provisioning

The provisioning script is relying on ob_flash_programming, obkey_provisioning, update_appli_setup,
update_ob_setup and update_stirot_iloader_setup scripts.

For more details, refer to STM32H7S Wiki articles:

  - [STiRoT for STM32H7S](https://wiki.st.com/stm32mcu/wiki/Security:STiRoT_for_STM32H7S)
  - [How to start with STiRoT on STM32H7S](https://wiki.st.com/stm32mcu/wiki/Security:How_to_start_with_STiRoT_on_STM32H7S)

#### <b>Notes</b>

Two versions of AppliCfg (used during **provisioning process**) are available: windows executable and python version.<br>
On Windows environment, the executable is used. On other environment, the python version is automatically used.<br>
To use Python version, it is needed to:

- have python installed (Python 3.6 or newer) and available in execution path variable
- have required python modules installed (Utilities/PC_Software/ROT_AppliConfig/requirements.txt):

        pip install -r requirements.txt
