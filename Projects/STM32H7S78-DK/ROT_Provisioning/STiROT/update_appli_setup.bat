@ECHO OFF
call ../env.bat

:: Environment variable for setting postbuild command with AppliCfg
set project_dir=%~dp0

:: Enable delayed expansion
setlocal EnableDelayedExpansion

if "%isGeneratedByCubeMX%" == "true" (
    set appli_dir=%stirot_boot_path_project%
) else (
    set appli_dir=../../%stirot_boot_path_project%
)

:: check if Application or template is used
if exist "%appli_dir%\EWARM\stm32h7s7xx_flash.icf" (
    :: Project configuration "Applications"
    set "appli_subdir="
    set "appli_proj_dir=%appli_dir%
) else (
    if exist "%appli_dir%\..\EWARM\Appli\stm32h7s7xx_flash.icf" (
        :: Project configuration "Templates"
        set "appli_subdir=Appli\"
        set "appli_proj_dir=%appli_dir%\..
    ) else (
        echo No valid linker file found
        goto :error
    )
)

:: CubeIDE project Appli Full Secure files
set ld_file="%appli_proj_dir%\STM32CubeIDE\%appli_subdir%STM32H7S7L8HXH_FLASH.ld"

:: IAR project Appli Full Secure files
set icf_file="%appli_proj_dir%\EWARM\%appli_subdir%stm32h7s7xx_flash.icf"

:: KEIL project Appli Full Secure files
set sct_file="%appli_proj_dir%\MDK-ARM\%appli_subdir%stm32h7sxx_app.sct"

set main_h="%appli_dir%\Inc\main.h"

:: STiRoT xml files
set obk_cfg_file="%project_dir%Config\STiROT_Config.xml"
set code_image="%project_dir%Image\STiROT_Code_Image.xml"

:: General section need
set code_size="Firmware area size"
set code_address="Firmware area address (in internal RAM)"
set fw_in_bin="Firmware binary input file"
set fw_out_bin="Image output file"
set secure_code_size="Size of the secure area"

::Path adapted to IAR postbuild command
set stirot_app_bin="..\%appli_dir%\Binary\appli.bin"
set stirot_app_bin=%stirot_app_bin:\=/%
set stirot_app_hex="..\%appli_dir%\Binary\appli_enc_sign.hex"
set stirot_app_hex=%stirot_app_hex:\=/%

:start
goto exe:
goto py:
:exe
::line for Windows executable
set applicfg="%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\dist\AppliCfg.exe"
set "python="
if exist %applicfg% (
echo run config Appli with Windows executable
goto update
)
:py
::called if we just want to use AppliCfg python (think to comment "goto exe:")
set applicfg="%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"
set "python=python ".\..\..\..\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"
set "python=python "

:update
set "AppliCfg=%python% %applicfg%"

:: ================================================ Updating test Application files ========================================================

set "action=Update STiROT_Code_Image.xml input binary file"
echo !action!
%AppliCfg% xmlval --name %fw_in_bin% --value %stirot_app_bin% --string --vb %code_image%
if !errorlevel! neq 0 goto :error

set "action=Update STiROT_Code_Image.xml output encrypted/signed hexadecimal file"
echo !action!
%AppliCfg% xmlval --name %fw_out_bin% --value %stirot_app_hex% --string --vb %code_image%
if !errorlevel! neq 0 goto :error

if exist "%icf_file%" (
    set "action=Updating Linker .icf file"
    echo !action!
    echo ICF %icf_file%

    %AppliCfg% linker -xml %obk_cfg_file% -nxml %code_size% -n CODE_SIZE --vb %icf_file%
    if !errorlevel! neq 0 goto :error

    %AppliCfg% linker -xml %obk_cfg_file% -nxml %code_address% -n CODE_OFFSET --vb %icf_file%
    if !errorlevel! neq 0 goto :error
)

if exist "%ld_file%" (
    set "action=Updating Linker .ld file"
    echo !action!
    echo LD %ld_file%

    %AppliCfg% linker -xml %obk_cfg_file% -nxml %code_size% -n CODE_SIZE --vb %ld_file%
    if !errorlevel! neq 0 goto :error

    %AppliCfg% linker -xml %obk_cfg_file% -nxml %code_address% -n CODE_OFFSET --vb %ld_file%
    if !errorlevel! neq 0 goto :error
)

if exist "%sct_file%" (
    set "action=Updating Linker .sct file"
    echo !action!
    echo SCT %sct_file%

    %AppliCfg% linker -xml %obk_cfg_file% -nxml %code_size% -n CODE_SIZE --vb %sct_file%
    if !errorlevel! neq 0 goto :error

    %AppliCfg% linker -xml %obk_cfg_file% -nxml %code_address% -n CODE_OFFSET --vb %sct_file%
    if !errorlevel! neq 0 goto :error
)

if "!appli_subdir!" == "" (
    set "action=Updating Data Image Define Flag"
    echo !action!
    %AppliCfg% definevalue -xml %obk_cfg_file% -nxml %code_size% -n CODE_SIZE --vb %main_h%
    if !errorlevel! neq 0 goto :error

    %AppliCfg% definevalue -xml %obk_cfg_file% -nxml %code_address% -n CODE_OFFSET --vb %main_h%
    if !errorlevel! neq 0 goto :error
)

echo Linker files and main.h successfully updated according to STiROT_Config.obk
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo        Error when trying to "%action%" >CON
echo        Update script aborted >CON
IF [%1] NEQ [AUTO] cmd /k
exit 1
