@ECHO OFF
:: Getting the Trusted Package Creator CLI path
set project_dir=%~dp0
pushd "%project_dir%\..\..\..\..\..\ROT_Provisioning"
set provisioningdir=%cd%
popd
call "%provisioningdir%\env.bat"

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Environment variable for log file
set current_log_file="%project_dir%\postbuild.log"
echo. > %current_log_file%

::=============================================================================================
::image xml configuration files
::=============================================================================================
set code_image_file="%provisioningdir%\STiROT\Image\STiROT_Code_Image.xml"

::=================================================================================================
:: Variables for image xml configuration(ROT_Provisioning\STiROT\Images)
:: relative path from ROT_Provisioning\STiROT\Images directory to retrieve binary files
::=================================================================================================
set bin_path_xml_field="..\..\..\Templates\Template_ROT\STiROT_Appli\Appli\Binary"
set fw_in_bin_xml_field="Firmware binary input file"
set fw_out_bin_xml_field="Image output file"
set stirot_app_bin_xml_field="%bin_path_xml_field%\appli.bin"
set stirot_app_hex_xml_field="%bin_path_xml_field%\appli_enc_sign.hex"

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

:postbuild
echo Creating STiROT image  >> %current_log_file% 2>&1

::update xml file : input file
%python%%AppliCfg% xmlval --name %fw_in_bin_xml_field% --value %stirot_app_bin_xml_field% --string --vb %code_image_file% --vb >> %current_log_file% 2>>&1
IF !errorlevel! NEQ 0 goto :error

::update xml file : output file
%python%%AppliCfg% xmlval --name %fw_out_bin_xml_field% --value %stirot_app_hex_xml_field% --string --vb %code_image_file% >> %current_log_file% 2>>&1
IF !errorlevel! NEQ 0 goto :error

:: Create final image with Trust Package Creator
%stm32tpccli% -pb "%provisioningdir%\STiROT\Image\STiROT_Code_Image.xml" >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error
exit 0

:error
echo.
echo =====
echo ===== Error occurred.
echo ===== See %current_log_file% for details. Then try again.
echo =====
exit 1
