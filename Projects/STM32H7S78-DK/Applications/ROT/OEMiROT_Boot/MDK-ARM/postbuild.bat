@ECHO OFF
:: Getting the Trusted Package Creator and STM32CubeProgammer CLI path
set project_dir=%~dp0
pushd "%project_dir%\..\..\..\..\ROT_Provisioning"
set provisioning_dir=%cd%
popd
call "%provisioning_dir%\env.bat"

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Environment variable for log file
set current_log_file="%project_dir%\postbuild.log"
echo. > %current_log_file%

:start
goto exe:
goto py:
:exe
::line for window executable
set applicfg="%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\dist\AppliCfg.exe"
set "python="
if exist %applicfg% (
    echo run config Appli with windows executable
    goto postbuild
)
:py
::line for python
echo run config Appli with python script
set applicfg="%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"
set "python= "

:postbuild
set auto_rot_update="%project_dir%\..\auto_rot_update.bat"
set preprocess_bl2_file="%project_dir%\image_macros_preprocessed_bl2.c"
set "appli_dir=..\..\..\..\%oemirot_boot_path_project%"
set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b oemurot_enable -m RE_OEMUROT_ENABLE --decimal %auto_rot_update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error
call %auto_rot_update%
set update="%provisioning_dir%\%bootpath%\ob_flash_programming.bat"

:: Environment variable for AppliCfg
set appli_flash_layout="%appli_dir%\Inc\appli_flash_layout.h"
REM Check the configuration of the application: Applications or Templates
if exist "%appli_dir%\MDK-ARM\stm32h7rsxx_RAMxspi2_ROMxspi1.sct" (
    REM Project configuration "Applications"
    set appli_linker_file="%appli_dir%\MDK-ARM\stm32h7rsxx_RAMxspi2_ROMxspi1.sct"
) else (
    if exist "%appli_dir%\..\MDK-ARM\Appli\stm32h7rsxx_RAMxspi2_ROMxspi1.sct" (
        REM Project configuration "Templates"
        set appli_linker_file="%appli_dir%\..\MDK-ARM\Appli\stm32h7rsxx_RAMxspi2_ROMxspi1.sct"
    ) else (
        echo No valid sct file found
        goto :error
    )
)
set map_properties="%project_dir%\..\..\OEMiROT_Boot\map.properties"
set code_xml="%provisioning_dir%\%bootpath%\Images\%project%_code_image.xml"
set code_size="Firmware area size"
set scratch_sector_number="Number of scratch sectors"
set code_address="Firmware area address (in internal RAM)"
set oemurot_code_size="Firmware area size"
set oemurot_code_address="Firmware installation area address (in external flash)"
set oemurot_download_code_address="Firmware download area address (in external flash)"
set stirot_config_xml="%provisioning_dir%\%bootpath%\Config\STiROT_Config.xml"

set "command=%python%%applicfg% modifyfilevalue --variable bootPath --delimiter = --value %project% %map_properties% --str --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b MCUBOOT_OVERWRITE_ONLY -m RE_OVER_WRITE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

if "%project%" == "OEMuROT" goto :oemurot_rot_regions

:oemirot_rot_regions
:: Write zero as there is no dedicated slot with OEMiROT
set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b RUN_ROT_REGION_START -m RE_OEMUROT_ENABLE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:: Write zero as there is no dedicated slot with OEMiROT
set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b RUN_ROT_REGION_SIZE -m RE_OEMUROT_ENABLE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b ROT_REGION_START -m RE_RUN_ROT_START %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b ROT_REGION_SIZE -m RE_RUN_ROT_SIZE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:: Write zero as this slot does not exist with OEMiROT
set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b DOWNLOAD_ROT_REGION_START -m RE_OEMUROT_ENABLE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:: Write zero as this slot does not exist with OEMiROT
set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b DOWNLOAD_ROT_REGION_SIZE -m RE_OEMUROT_ENABLE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

goto :common_rot_regions

:oemurot_rot_regions
set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b RUN_ROT_REGION_START -m RE_RUN_ROT_START %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b RUN_ROT_REGION_SIZE -m RE_RUN_ROT_SIZE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --xml %stirot_config_xml% -b ROT_REGION_START -n %oemurot_code_address% %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --xml %stirot_config_xml% -b ROT_REGION_SIZE -n %oemurot_code_size% %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --xml %stirot_config_xml% -b DOWNLOAD_ROT_REGION_START -n %oemurot_download_code_address% %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --xml %stirot_config_xml% -b DOWNLOAD_ROT_REGION_SIZE -n %oemurot_code_size% %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

goto :common_rot_regions

:common_rot_regions
set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b SCRATCH_REGION_START -m RE_FLASH_AREA_SCRATCH_START %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b SCRATCH_REGION_SIZE -m RE_FLASH_AREA_SCRATCH_SIZE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b RUN_CODE_REGION_START -m RE_RUN_START %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b RUN_CODE_REGION_SIZE -m RE_AREA_0_SIZE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b CODE_REGION_START -m RE_CODE_START %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b CODE_REGION_SIZE -m RE_IMAGE_FLASH_SIZE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b DOWNLOAD_CODE_REGION_START -m RE_IMAGE_DOWNLOAD_ADDRESS %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b DOWNLOAD_CODE_REGION_SIZE -m RE_IMAGE_FLASH_SIZE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b APP_RAM_START -m RE_APP_RAM_START %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b APP_RAM_SIZE -m RE_APP_RAM_SIZE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b RAMECC_HANDLE_START -m RE_RAMECC_HANDLE_START %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b RAMECC_HANDLE_SIZE -m RE_RAMECC_HANDLE_SIZE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b CONF_FLAG_START -m RE_CONF_FLAG_START %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b CONF_FLAG_SIZE -m RE_CONF_FLAG_SIZE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_PARTITION_START -n PARTITION_START %appli_linker_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_AREA_0_OFFSET -n CODE_OFFSET %appli_linker_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_AREA_0_SIZE -n CODE_SIZE %appli_linker_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

REM appli_flash_layout.h may not exist in a template application
REM Initialize the variable "command" outside of the parentheses
set "command=%python%%applicfg% setdefine --layout %preprocess_bl2_file% -m RE_OVER_WRITE -n MCUBOOT_OVERWRITE_ONLY -v 1 %appli_flash_layout% --vb >> %current_log_file% 2>&1"
if exist %appli_flash_layout% (
  %command%
  IF !errorlevel! NEQ 0 goto :error
)


set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SIZE -c S %code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_DOWNLOAD_ADDRESS -c x %code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout  %preprocess_bl2_file% -m RE_OVER_WRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" %code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_FLASH_AREA_SCRATCH_SIZE -n %scratch_sector_number% --decimal %code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval -xml %code_xml% -nxml %code_size% -nxml %scratch_sector_number% --decimal -e (((val1+1)/val2)+1) -cond val2 -c M %code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

if "%project%" == "OEMuROT" goto :oemurot

set "command=%python%%applicfg% setob --layout %preprocess_bl2_file% -b wrps -ms RE_BL2_WRP_OFFSET -me RE_BL2_WRP_SIZE -msec RE_BL2_WRP_SIZE -d 0x2000 --protected_area 0x10000 %update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% setob --layout %preprocess_bl2_file% -b hdp_start -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_HDP_BLOCK_NBR -d 0x100 %update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% setob --layout %preprocess_bl2_file% -b hdp_end -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_HDP_BLOCK_NBR -d 0x100 %update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file%  -b bootaddress -m RE_BL2_BOOT_ADDRESS %update% >> "%project_dir%\output.txt" 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:: Bypass configuration of OEMuROT in case of OEMiROT
if "%project%" == "OEMiROT" goto :oemirot

:: Create final OEMuROT image with Trust Package Creator
:oemurot
set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_BL2_RAM_SIZE -n %code_size% %stirot_config_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_BL2_RAM_BASE -n %code_address% %stirot_config_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

echo Creating OEMuROT image  >> %current_log_file% 2>&1
%stm32tpccli% -pb "%provisioning_dir%\%bootpath%\Images\STiROT_Code_Image.xml" >> %current_log_file% 2>&1
IF !errorlevel! NEQ 0 goto :error

:oemirot
exit 0

:error
echo.
echo =====
echo ===== Error occurred.
echo ===== See %current_log_file% for details. Then try again.
echo =====
exit 1

