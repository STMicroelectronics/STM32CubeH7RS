@echo off

call ..\env.bat

:: Environment variable for setting postbuild command with AppliCfg
set project_dir=%~dp0

:: Enable delayed expansion
setlocal EnableDelayedExpansion

if "%isGeneratedByCubeMX%" == "true" (
    set iloader_dir=%cube_fw_path%\Projects\STM32H7S78-DK\Applications\ROT\STiROT_iLoader
) else (
    set iloader_dir=..\..\%stirot_iloader_boot_path_project%
)

:: External script
set ob_flash_programming="%project_dir%\ob_flash_programming.bat"

:: IAR project STiROT_iLoader files
set obk_cfg_file="%project_dir%\Config\STiROT_Config.xml"

set code_align=8192

::Path adapted to IAR postbuild command
if "%isGeneratedByCubeMX%" == "true" (
    set STiROT_iLoader_bin=%iloader_dir%\Binary\STiROT_iLoader.bin
) else (
    set STiROT_iLoader_bin=%project_dir%\%iloader_dir%\Binary\STiROT_iLoader.bin
)
set STiROT_iLoader_bin="%STiROT_iLoader_bin:\=/%"

:: IAR linker
if "%isGeneratedByCubeMX%" == "true" (
    set STiROT_iLoader_icf_file=%iloader_dir%\EWARM\stm32h7s7xx_flash.icf
) else (
    set STiROT_iLoader_icf_file=%project_dir%\%iloader_dir%\EWARM\stm32h7s7xx_flash.icf
)
set STiROT_iLoader_icf_file="%STiROT_iLoader_icf_file:\=/%"

:: STM32CubeIDE linker
if "%isGeneratedByCubeMX%" == "true" (
    set STiROT_iLoader_ld_file=%iloader_dir%\STM32CubeIDE\STM32H7S7L8HXH_FLASH.ld
) else (
    set STiROT_iLoader_ld_file=%project_dir%\%iloader_dir%\STM32CubeIDE\STM32H7S7L8HXH_FLASH.ld
)
set STiROT_iLoader_ld_file="%STiROT_iLoader_ld_file:\=/%"

:: Keil linker
if "%isGeneratedByCubeMX%" == "true" (
    set STiROT_iLoader_sct_file=%iloader_dir%\MDK-ARM\stm32h7sxx_iloader.sct
) else (
    set STiROT_iLoader_sct_file=%project_dir%\%iloader_dir%\MDK-ARM\stm32h7sxx_iloader.sct
)
set STiROT_iLoader_sct_file="%STiROT_iLoader_sct_file:\=/%"

:: Get the IDE configuration
set /p active_ide=<"%iloader_dir%\Binary\_ide"

:start
goto exe:
goto py:
:exe
::line for Windows executable
set Applicfg="%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\dist\AppliCfg.exe"
set "python="
if exist %Applicfg% (
    echo run config Appli with Windows executable
    goto :update
)
:py
::called if we just want to use AppliCfg python (think to comment "goto exe:")
set Applicfg="%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"
set "python=python "
goto :update

:update
set "AppliCfg=%python% %Applicfg%"

:: ================================================ Updating STiROT_iLoader files ========================================================
echo Updating the STiROT_iLoader files ...
set "action=Updating the STiROT_iLoader files"
call :getBinSize STiROT_iLoader_bin_size %STiROT_iLoader_bin%

:getBinSize
echo %STiROT_iLoader_bin%
set "action=Getting iLoader size"
IF not exist %STiROT_iLoader_bin% (
    @echo [31mError: STiROT_iLoader binary does not exist! %STiROT_iLoader_bin% [0m
    goto :error
)
set %~1=%~z2

set "action=Update STiROT_Config.xml input binary file"
set /a aligned=STiROT_iLoader_bin_size%%code_align
if %aligned% neq 0 (
    :: get the STiROT_iLoader size aligned on the sector size
    set /a "STiROT_iLoader_bin_size=(%STiROT_iLoader_bin_size%/%code_align%+1)*%code_align%"
)

:: Update the STiROT_iLoader size in the STiROT configuration
%AppliCfg% xmlval -nxml_el_idx "STiROT configuration" -n "STiROT_iLoader size" --value %STiROT_iLoader_bin_size% --vb %obk_cfg_file%

if !errorlevel! neq 0 goto :error

if "%active_ide%" == "IAR" goto :iar
if "%active_ide%" == "STM32CubeIDE" goto :stm32cubeide
if "%active_ide%" == "KEIL" goto :keil

echo The selected IDE (%active_ide%) is not supported yet
goto :error

:iar
set "action=Update STiROT_Config.xml icf"
:: Update the STiROT_iLoader size in the STiROT configuration
%AppliCfg% xmlval -l %STiROT_iLoader_icf_file% -m CODE_OFFSET -n "STiROT_iLoader offset" --vb %obk_cfg_file%
if !errorlevel! neq 0 goto :error

:: Update the STiROT_iLoader address in ob flash programming script
%AppliCfg% setob -l %STiROT_iLoader_icf_file% -b OEMiLoader_address -ms CODE_OFFSET -me CODE_SIZE -msec CODE_SIZE -d 0x2000 --vb %ob_flash_programming%
if !errorlevel! neq 0 goto :error
goto :end

:stm32cubeide
set "action=Update STiROT_Config.xml ld"
:: Update the STiROT_iLoader size in the STiROT configuration
%AppliCfg% xmlval -l %STiROT_iLoader_ld_file% -m CODE_OFFSET -n "STiROT_iLoader offset" --vb %obk_cfg_file%
if !errorlevel! neq 0 goto :error

:: Update the STiROT_iLoader address in ob flash programming script
set "action=Update STiROT_Config.xml sct"
%AppliCfg% setob -l %STiROT_iLoader_ld_file% -b OEMiLoader_address -ms CODE_OFFSET -me CODE_SIZE -msec CODE_SIZE -d 0x2000 --vb %ob_flash_programming%
if !errorlevel! neq 0 goto :error
goto :end

:keil
:: Update the STiROT_iLoader size in the STiROT configuration
%AppliCfg% xmlval -l %STiROT_iLoader_sct_file% -m CODE_OFFSET -n "STiROT_iLoader offset" --vb %obk_cfg_file%
if !errorlevel! neq 0 goto :error
%AppliCfg% setob -l %STiROT_iLoader_sct_file% -b OEMiLoader_address -ms CODE_OFFSET -me CODE_SIZE -msec CODE_SIZE -d 0x2000 --vb %ob_flash_programming%
if !errorlevel! neq 0 goto :error
goto :end

:end
exit 0

:error
echo ===== Error when trying to "%action%" >CON
echo ===== See %current_log_file% for details. Then try again.
echo ===== Update script aborted >CON
IF [%1] NEQ [AUTO] cmd /k
exit 1
