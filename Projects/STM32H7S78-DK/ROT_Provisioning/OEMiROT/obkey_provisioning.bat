@echo off

:: Enable delayed expansion
setlocal EnableDelayedExpansion

call ../env.bat

:: Getting the CubeProgammer_cli path
set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug

:: =============================================== Configure OB Keys =========================================================================
set "action=Configure OBKeys HDPL1-DA config area"
echo %action%
%stm32programmercli% %connect_no_reset%
%stm32programmercli% %connect_no_reset% -sdp ./../DA/Binary/%da_file%.obk
IF !errorlevel! NEQ 0 goto :error

set "action=Configure OBKeys HDPL1-OEMiROT config area"
echo %action%
%stm32programmercli% %connect_no_reset%
%stm32programmercli% %connect_no_reset% -sdp ./Binary/OEMiROT_Config.obk
IF !errorlevel! NEQ 0 goto :error

set "action=Configure OBKeys HDPL1-OEMiROT data area"
echo %action%
%stm32programmercli% %connect_no_reset%
if  "%device_type%" == "H7S" (
    %stm32programmercli% %connect_no_reset% -sdp ./Binary/OEMiROT_Data.obk
) else (
    :: Use a non-encrypted obk file for H7R (DoEncryption = 0)
    %stm32programmercli% %connect_no_reset% -sdp ./Binary/OEMiROT_Data_OPEN.obk
)
IF !errorlevel! NEQ 0 goto :error

echo Successful obkey provisioning
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo        Error when trying to "%action%" >CON
echo        Provisioning aborted >CON
IF [%1] NEQ [AUTO] cmd /k
exit 1
