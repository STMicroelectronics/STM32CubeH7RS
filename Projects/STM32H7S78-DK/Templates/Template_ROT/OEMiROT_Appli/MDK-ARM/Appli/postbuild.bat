@ECHO OFF
:: Getting the Trusted Package Creator CLI path
set "project_dir=%~dp0"
pushd %project_dir%\..\..\..\..\..\ROT_Provisioning
set provisioning_dir=%cd%
popd
call "%provisioning_dir%\env.bat"

:: Enable delayed expansion
setlocal EnableDelayedExpansion
:: Environment variable for log file
set current_log_file="%project_dir%\postbuild.log"
echo. > %current_log_file%

::=============================================================================================
::Variables updated by OEMiROT_Boot postbuild
::=============================================================================================
:: flag to switch between OEMiROT and OEMuROT
set oemurot_enable=0

if %oemurot_enable% == 1 (
set project=OEMuROT
set bootpath=STiROT_OEMuROT)

if %oemurot_enable% == 0 (
set project=OEMiROT
set bootpath=OEMiROT)

::=============================================================================================
::image xml configuration files
::=============================================================================================
set code_image_file="%provisioning_dir%\%bootpath%\Images\%project%_Code_Image.xml"

::=================================================================================================
:: Variables for image xml configuration(ROT_Provisioning\%bootpath%\Images)
:: relative path from ROT_Provisioning\%bootpath%\Images directory to retrieve binary files
::=================================================================================================
set bin_path_xml_field="..\..\..\Templates\Template_ROT\OEMiROT_Appli\Appli\Binary"
set fw_in_bin_xml_field="Firmware binary input file"
set fw_out_bin_xml_field="Image output file"
set app_bin_xml_field="%bin_path_xml_field%\rot_app.bin"
set app_enc_sign_hex_xml_field="%bin_path_xml_field%\rot_app_enc_sign.hex"

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
echo Creating %project% image  >> %current_log_file% 2>>&1

::update xml file : input file
%python%%AppliCfg% xmlval -v %app_bin_xml_field% --string -n %fw_in_bin_xml_field% %code_image_file% --vb >> %current_log_file% 2>>&1
IF !errorlevel! NEQ 0 goto :error

::update xml file : output file
%python%%AppliCfg% xmlval -v %app_enc_sign_hex_xml_field% --string -n %fw_out_bin_xml_field% %code_image_file% >> %current_log_file% 2>>&1
IF !errorlevel! NEQ 0 goto :error

%stm32tpccli% -pb "%provisioning_dir%\%bootpath%\Images\%project%_Code_Image.xml" >> %current_log_file% 2>>&1
IF !errorlevel! NEQ 0 goto :error
exit 0

:error
echo.
echo =====
echo ===== Error occurred.
echo ===== See %current_log_file% for details. Then try again.
echo =====
exit 1
