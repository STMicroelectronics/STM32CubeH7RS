@ECHO OFF

call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Environment variable for setting postbuild command with AppliCfg
if "%isGeneratedByCubeMX%" == "true" (
  set iloader_dir=%cube_fw_path%\Projects\STM32H7S78-DK\Applications\ROT\STiROT_iLoader
  echo %iloader_dir%
) else (
  set iloader_dir=../../%stirot_iloader_boot_path_project%
)

:: Get the IDE configuration
set /p active_ide=<"%iloader_dir%\Binary\_ide"

:: Path and input files
set ob_flash_programming="ob_flash_programming.bat"
set obk_cfg_file="%~dp0./Config/STiROT_Config.xml"

set project_dir=%~dp0
:: xml item nem contents
:: This will allow to recover information from xml file
set STiROT_iLoader_offset="STiROT_iLoader offset"
set STiROT_iLoader_size="STiROT_iLoader size"
set primary_code_nvm_add="Firmware installation area address (in external flash)"
set code_size="Firmware area size"
set data_size="Data download slot size"
set data_image_en="Number of images managed"
set sram_ecc="SRAM ECC management activation"

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
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"

:update
set "AppliCfg=%python% %applicfg%"
:: ======================================================= Updating the Option bytes =======================================================
echo Updating the ob_flash_programming script...

set "action=Update Write protection values from %active_ide%"
echo %action%
if "%active_ide%" == "IAR" goto :iar
if "%active_ide%" == "STM32CubeIDE" goto :stm32cubeide
if "%active_ide%" == "KEIL" goto :keil

echo The selected IDE is not supported yet
goto :error

:iar
%AppliCfg% setob --layout %STiROT_iLoader_icf_file% -b wrps -ms CODE_OFFSET -me CODE_SIZE -msec CODE_SIZE -d 0x2000 --protected_area 0x10000  %ob_flash_programming% --vb
goto :check_status

:stm32cubeide
%AppliCfg% setob --layout %STiROT_iLoader_ld_file% -b wrps -ms CODE_OFFSET -me CODE_SIZE -msec CODE_SIZE -d 0x2000 --protected_area 0x10000  %ob_flash_programming% --vb
goto :check_status

:keil
%AppliCfg% setob --layout %STiROT_iLoader_sct_file% -b wrps -ms CODE_OFFSET -me CODE_SIZE -msec CODE_SIZE -d 0x2000 --protected_area 0x10000  %ob_flash_programming% --vb
goto :check_status

:check_status
if !errorlevel! neq 0 goto :error

set "action=Update SRAM_ECC value"
%AppliCfg% obscript --xml %obk_cfg_file% -ob ECC_ON_SRAM -sp %sram_ecc% --vb %ob_flash_programming%
if !errorlevel! neq 0 goto :error

set "action=Updating NVM primary code slot to erase"
%AppliCfg% sectorerase -xml %obk_cfg_file% -s primary_code -bxml %primary_code_nvm_add% -sxml %code_size% -m ext_nvm -d 0x10000 --vb %ob_flash_programming%
if !errorlevel! neq 0 goto :error

echo Script successfully updated according to STiROT_Config.obk
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo        Error when trying to "%action%" >CON
echo        Update script aborted >CON
IF [%1] NEQ [AUTO] cmd /k
exit 1
