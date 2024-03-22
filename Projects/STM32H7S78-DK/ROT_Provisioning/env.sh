#!/bin/bash -

# Absolute path to this script
if [ $# -ge 1 ] && [ -d "$1" ]; then
    script_path="$1"
else
    script_path=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
fi
#==============================================================================
#                              General
#==============================================================================
#Configure tools installation path
if [ "$OS" == "Windows_NT" ]; then
    stm32programmercli_path="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin"
    stm32programmercli="$stm32programmercli_path\STM32_Programmer_CLI.exe"
    stm32tpccli="$stm32programmercli_path\STM32TrustedPackageCreator_CLI.exe"
    stm32ExtLoaderFlash="-elbl $stm32programmercli_path/ExternalLoader/MX66UW1G45G_STM32H7S78-DK_XSPIM1-SFIx.stldr"
    stm32ExtLoaderFlashOpen="-el $stm32programmercli_path/ExternalLoader/MX66UW1G45G_STM32H7S78-DK_XSPIM1.stldr"
else
    stm32programmercli_path=~/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/
    PATH=$stm32programmercli_path:$PATH
    stm32programmercli="STM32_Programmer_CLI"
    stm32tpccli="STM32TrustedPackageCreator_CLI"
    stm32ExtLoaderFlash="-elbl $stm32programmercli_path/ExternalLoader/MX66UW1G45G_STM32H7S78-DK_XSPIM1-SFIx.stldr"
    stm32ExtLoaderFlashOpen="-el $stm32programmercli_path/ExternalLoader/MX66UW1G45G_STM32H7S78-DK_XSPIM1.stldr"
fi

# Configure Virtual Com Port
com_port="COM19"

#==============================================================================
#                           STiRoT bootpath
#==============================================================================
#Select application project below
# stirot_boot_path_project=Templates/Template_ROT/STiROT_Appli/Appli
stirot_boot_path_project=Applications/ROT/STiROT_Appli

# Select iloader project below
stirot_iloader_boot_path_project=Applications/ROT/STiROT_iLoader
#==============================================================================

#==============================================================================
#                           OEMiROT bootpath
#==============================================================================
#Select application project below
#oemirot_boot_path_project=Templates/Template_ROT/OEMiROT_Appli/Appli
oemirot_boot_path_project=Applications/ROT/OEMiROT_Appli
#==============================================================================

#==============================================================================
#                        STiROT_OEMuROT boot path
#==============================================================================
#Application is the same as for OEMiROT boot path

#==============================================================================
#              !!!! DOT NOT EDIT --- UPDATED AUTOMATICALLY !!!!
#==============================================================================
PROJECT_GENERATED_BY_CUBEMX=false
cube_fw_path="$script_path/../../../"
stirot_appli='appli_enc_sign.hex'
oemirot_appli='rot_app_enc_sign.hex'
