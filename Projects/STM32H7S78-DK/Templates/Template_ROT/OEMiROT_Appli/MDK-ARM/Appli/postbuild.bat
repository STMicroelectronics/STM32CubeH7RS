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

set rot_update_script="%project_dir%\..\..\..\..\..\Applications\ROT\OEMiROT_Boot\auto_rot_update.bat"
call %rot_update_script%

echo Creating %project% image  >> %current_log_file% 2>&1
%stm32tpccli% -pb "%provisioning_dir%\%bootpath%\Images\%project%_Code_Image.xml" >> %current_log_file% 2>&1
IF !errorlevel! NEQ 0 goto :error
exit 0

:error
echo.
echo =====
echo ===== Error occurred.
echo ===== See %current_log_file% for details. Then try again.
echo =====
exit 1
