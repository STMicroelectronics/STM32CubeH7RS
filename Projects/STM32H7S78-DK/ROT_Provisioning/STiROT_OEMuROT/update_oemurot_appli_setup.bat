@ECHO OFF

call ../env.bat

:: Environment variable for setting postbuild command with AppliCfg
set project_dir=%~dp0

:: Enable delayed expansion
setlocal EnableDelayedExpansion

set code_image="%project_dir%Images\OEMuROT_Code_Image.xml"

:: General section need
set fw_in_bin="Firmware binary input file"
set fw_out_bin="Image output file"

if "%isGeneratedByCubeMX%" == "true" (
set appli_dir=%oemirot_boot_path_project%
) else (
set appli_dir=..\..\..\%oemirot_boot_path_project%
)

::Path adapted to IAR postbuild command
set oemurot_app_bin="%appli_dir%\Binary\rot_app.bin"
set oemurot_app_bin=%oemurot_app_bin:\=/%
set oemurot_app_hex="%appli_dir%\Binary\%oemirot_appli%"
set oemurot_app_hex=%oemurot_app_hex:\=/%

:start
goto exe:
goto py:
:exe
::line for Windows executable
set AppliCfg="%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\dist\AppliCfg.exe"
set "python="
if exist %AppliCfg% (
    echo run config Appli with Windows executable
    goto update
)
:py
::called if we just want to use AppliCfg python (think to comment "goto exe:")
set AppliCfg="%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"
set "python=python "

:update
set "AppliCfg=%python% %AppliCfg%"

:: ================================================ Updating test Application files ========================================================

set "action=Configure the Firmware binary input file in OEMuROT_Code_Image.xml..."
%AppliCfg% xmlval --name %fw_in_bin% --value %oemurot_app_bin% --string --vb %code_image%
if !errorlevel! neq 0 goto :error

set "action=Configure the Image output encrypted/signed hexadecimal file in OEMuROT_Code_Image.xml..."
%AppliCfg% xmlval --name %fw_out_bin% --value %oemurot_app_hex% --string --vb %code_image%
if !errorlevel! neq 0 goto :error

echo OEMuROT_Code_Image.xml successfully updated
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo        Error when trying to "%action%" >CON
echo        Update script aborted >CON
IF [%1] NEQ [AUTO] cmd /k
exit 1
