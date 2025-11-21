@echo off

:: constants for the prebuild script
set "project_dir=%~dp0"
set "env_script=%project_dir%..\..\..\..\ROT_Provisioning\env.bat"
call "%env_script%"

:: ----------------------- Update the stirot_iloader reference in env.bat -----------------------
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

:: ----------------------- Update the stirot_iloader icf file -----------------------
set code_align=0x2000
set icf_file="%project_dir%stm32h7s7xx_flash.icf"

set offset_macro=CODE_OFFSET
set offset_pattern="define symbol CODE_OFFSET"

for /f "delims=:" %%a in ('findstr /c:%offset_pattern% %icf_file%') do (
	set found=%%a
)
:: get the offset value
set hexa=0x\d*
for %%a in (%found%) do (
    echo %%a | FindStr /R %hexa%  > nul 2>&1 && set read_offset=%%a
)

setlocal enabledelayedexpansion
::Align the Offset on code_align value
set /a "dec_offset=(%read_offset%/%code_align%)*%code_align%"

:: Convert offset value in hexa
cmd /C exit !dec_offset!
set "hexa_offset=%=ExitCode%"
for /F "tokens=* delims=0" %%Z in ("!hexa_offset!") do set "hexa_offset=%%Z"
if not defined hexa_offset set "hexa_offset=0"
set hexa_offset=0x!hexa_offset!
set offset_value_str=#define %offset_macro% !hexa_offset!
echo !offset_value_str! > "%project_dir%tmp.h"
endlocal

:: get the stirot_iloader size
set size_macro=CODE_SIZE
set size_pattern="define symbol CODE_SIZE"
for /f "delims=:" %%a in ('findstr /c:%size_pattern% %icf_file%') do (
	set size_found=%%a
)
:: get the size value
for %%a in (%size_found%) do (
    echo %%a | FindStr /R %hexa%  > nul 2>&1 && set read_size=%%a
)

setlocal enabledelayedexpansion
::Align the Offset on code_align value
set /a valid_size=%read_size% %% %code_align%
if  %valid_size%==0 (
	set /a "dec_size=(%read_size%/%code_align%)*%code_align%"
) else (
	set /a "dec_size=(%read_size%/%code_align%+1)*%code_align%"
)

:: Convert offset value in hexa
cmd /C exit !dec_size!
set "hexa_size=%=ExitCode%"
for /F "tokens=* delims=0" %%Z in ("!hexa_size!") do set "hexa_size=%%Z"
if not defined hexa_size set "hexa_size=0"
set hexa_size=0x!hexa_size!
set offset_value_str=#define %size_macro% !hexa_size!
echo !offset_value_str! >> "%project_dir%tmp.h"
endlocal


set "command=%python%%applicfg% linker -l "%project_dir%tmp.h" -m %offset_macro% -n %offset_macro% %icf_file% > "%project_dir%output.txt" 2>&1"
%command%

set "command=%python%%applicfg% linker -l "%project_dir%tmp.h" -m %size_macro% -n %size_macro% %icf_file% > "%project_dir%output.txt" 2>&1"
%command%

call del "%project_dir%tmp.h" > nul

rem iccarm.exe --cpu=Cortex-M7 -DBL2  -I%1\..\..\..\..\..\..\..\__INTERNAL__RSS\st_irot\Linker  %1\stm32h7s7xx_flash.icf  --silent --preprocess=ns %1\stm32h7s7xx_flash.icf.i > %1\output.txt 2>&1

IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo "%command% : failed" >> %project_dir%output.txt
exit 1
