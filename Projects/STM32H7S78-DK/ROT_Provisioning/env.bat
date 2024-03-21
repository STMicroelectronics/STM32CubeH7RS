@ECHO OFF

:: ==============================================================================
::                               General
:: ==============================================================================
:: Configure tools installation path
set stm32tool_path=C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin
set stm32tool_path=%stm32tool_path:\=/%

set stm32programmercli="%stm32tool_path%/STM32_Programmer_CLI.exe"
set stm32tpccli="%stm32tool_path%/STM32TrustedPackageCreator_CLI.exe"
set stm32ExtLoaderFlash=-elbl %stm32tool_path%/ExternalLoader/MX66UW1G45G_STM32H7S78-DK_XSPIM1-SFIx.stldr
set stm32ExtLoaderFlashOpen=-el %stm32tool_path%/ExternalLoader/MX66UW1G45G_STM32H7S78-DK_XSPIM1.stldr

:: Configure Virtual Com Port
set com_port=COM19

:: ==============================================================================
::                            STiROT bootpath
:: ==============================================================================
:: Select application project below
rem set stirot_boot_path_project=Templates\Template_ROT\STiROT_Appli\Appli
set stirot_boot_path_project=Applications\ROT\STiROT_Appli

:: Select iloader project below
set stirot_iloader_boot_path_project=Applications\ROT\STiROT_iLoader
:: ==============================================================================

:: ==============================================================================
::                            OEMiROT bootpath
:: ==============================================================================
:: Select application project below
rem set oemirot_boot_path_project=Templates\Template_ROT\OEMiROT_Appli\Appli
set oemirot_boot_path_project=Applications\ROT\OEMiROT_Appli
:: ==============================================================================

:: ==============================================================================
::               !!!! DOT NOT EDIT --- UPDATED AUTOMATICALLY !!!!
:: ==============================================================================
set PROJECT_GENERATED_BY_CUBEMX=false
set cube_fw_path=%~dp0..\..\..\
set stirot_appli=appli_enc_sign.hex
set oemirot_appli=rot_app_enc_sign.hex
