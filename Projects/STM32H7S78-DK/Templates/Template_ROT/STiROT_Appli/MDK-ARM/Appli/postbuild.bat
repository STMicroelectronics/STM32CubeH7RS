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

:: Create final image with Trust Package Creator
echo Creating STiROT image  >> %current_log_file% 2>&1
%stm32tpccli% -pb "%provisioningdir%\STiROT\Image\STiROT_Code_Image.xml" >> %current_log_file% 2>&1
if !errorlevel! neq 0 goto :error
exit 0

:error
echo.
echo =====
echo ===== Error occurred.
echo ===== See %current_log_file% for details. Then try again.
echo =====
exit 1
