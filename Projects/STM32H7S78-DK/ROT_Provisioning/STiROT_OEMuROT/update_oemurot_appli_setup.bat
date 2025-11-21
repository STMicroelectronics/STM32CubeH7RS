@ECHO OFF

call ../env.bat

:: Environment variable for setting postbuild command with AppliCfg
set project_dir=%~dp0

:: Enable delayed expansion
setlocal EnableDelayedExpansion

if "%isGeneratedByCubeMX%" == "true" (
set appli_dir=%oemirot_appli_path_project%
) else (
set appli_dir=..\..\..\%oemirot_appli_path_project%
)

:start
:: Check if Python is installed
python3 --version >nul 2>&1
if %errorlevel% neq 0 (
 python --version >nul 2>&1
 if !errorlevel! neq 0 (
    echo Python installation missing. Refer to Utilities\PC_Software\ROT_AppliConfig\README.md
    goto :error
 )
  set "python=python "
) else (
  set "python=python3 "
)

:: Environment variable for AppliCfg
set AppliCfg="%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"

:update
set "AppliCfg=%python% %AppliCfg%"

:: ================================================ Updating test Application files ========================================================

echo OEMuROT_Code_Image.xml successfully updated
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo        Error when trying to "%action%" >CON
echo        Update script aborted >CON
IF [%1] NEQ [AUTO] cmd /k
exit 1
