## <b>ROT_Provisioning OEMiROT Description</b>

This section provides an overview of the available scripts for OEMiROT boot path.

OEMiROT stands for "OEM Immutable Root of Trust" and it provides two services:

  - Secure Boot: Verification of the integrity, and authenticity of the application code before any execution.
  - Secure Firmware Update: If a new firmware image is available on detection, check of its authenticity.
                            Once the firmware is decrypted, check of the integrity of the code before installing it.

### <b>Keywords</b>

OEMiROT, boot path, Root Of Trust, Security, mcuboot

### <b>Directory contents</b>

<b>Sub-directories</b>

- OEMiROT/Binary                               Output binaries and generated images.
- OEMiROT/Config                               OEMiROT configuration files.
- OEMiROT/Image                                Image configuration files.
- OEMiROT/Keys                                 Keys for firmware image authentication and encryption.

<b>Scripts</b>

- OEMiROT/ob_flash_programming.bat/.sh         Programs option bytes and firmware image on the device.
- OEMiROT/obkey_provisioning.bat/.sh           Programs OBKeys on the device.
- OEMiROT/obkey_provisioning_open.bat/.sh      Programs OBKeys on the device in OPEN product state.
- OEMiROT/provisioning.bat/.sh                 Performs device provisioning process.

### <b>Hardware and Software environment</b>

- This example has been tested with STMicroelectronics STM32H7S78-DK (MB1736)
  board and can be easily tailored to any other supported device and development board.


### <b>How to use it ?</b>

To use OEMiRoT bootpath, you should first configure ROT_Provisioning/env.bat/.sh script
(tools path, application path and COM port configuration).<br>
The .bat scripts are designed for Windows, whereas the .sh scripts are designed for Linux and Mac-OS.

Then you should run the provisioning script (provisioning.bat/.sh).<br>
During the **provisioning process**, the programming scripts and the application files will
be automatically updated according to OEMiRoT configuration, and user answers.

The **provisioning process** (OEMiROT/provisioning.bat/.sh) is divided into 3 majors steps:

- Step 0: Preliminary stage
- Step 1: Configuration management
- Step 2: Image generations
- Step 3: Provisioning

The provisioning script is relying on ob_flash_programming, obkey_provisioning and obkey_provisioning_open scripts.

For more details, refer to STM32H7S Wiki articles:

  - [OEMiRoT OEMuRoT for STM32H7S](https://wiki.st.com/stm32mcu/wiki/Security:OEMiRoT_OEMuRoT_for_STM32H7S)
  - [How to start with OEMiRoT on STM32H7S](https://wiki.st.com/stm32mcu/wiki/Security:How_to_start_with_OEMiRoT_on_STM32H7S)

### <b>Notes</b>

Two versions of AppliCfg (used during **provisioning process**) are available: windows executable and python version.<br>
On Windows environment, the executable is used. On other environment, the python version is automatically used.<br>
To use Python version, it is needed to:

- have python installed (Python 3.6 or newer) and available in execution path variable
- have required python modules installed (Utilities/PC_Software/ROT_AppliConfig/requirements.txt):

        pip install -r requirements.txt