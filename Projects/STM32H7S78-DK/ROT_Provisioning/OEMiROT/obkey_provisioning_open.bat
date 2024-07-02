@echo off

:: Enable delayed expansion
setlocal EnableDelayedExpansion

call ../env.bat

:: Select device type (H7S or H7R)
set device_type=H7S

:: Getting the CubeProgammer_cli path
set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst

:: =============================================== Configure OB Keys =========================================================================
if  "%device_type%" == "H7S" (
    set "action=Configure OBKeys HDPL1 dummy AHK"
    echo %action%
    %stm32programmercli% %connect_reset%
    timeout /t 3
    %stm32programmercli% %connect_no_reset% -vb 3 -sdp ./Binary/OEMiROT_AHK_OPEN.obk
    IF !errorlevel! NEQ 0 goto :error
) else (
    :: do nothing
    goto :next
)

:next
set "action=Configure OBKeys HDPL1-OEMiROT config area"
echo %action%
%stm32programmercli% %connect_reset%
timeout /t 3
%stm32programmercli% %connect_no_reset% -vb 3 -sdp ./Binary/OEMiROT_Config.obk
IF !errorlevel! NEQ 0 goto :error

set "action=Configure OBKeys HDPL1-OEMiROT data area"
echo %action%
%stm32programmercli% %connect_reset%
timeout /t 3
%stm32programmercli% %connect_no_reset% -vb 3 -sdp ./Binary/OEMiROT_Data_OPEN.obk
IF !errorlevel! NEQ 0 goto :error

echo Successful obkey provisioning
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo        Error when trying to "%action%" >CON
echo        Provisioning aborted >CON
IF [%1] NEQ [AUTO] cmd /k
exit 1
