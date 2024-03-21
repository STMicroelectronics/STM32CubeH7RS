@echo off

if [%1] neq [AUTO] call ../env.bat

:: CubeProgammer connection
set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst

:: =============================================== Configure OB Keys ========================================================
:provisioning
:: Verify if obk file exists
if not exist "%~dp0\Binary\%da_obk_file%" (
    @echo [31m     Error: %da_obk_file% does not exist! use TPC to generate it[0m >CON
    goto :error
)

:: Provisioning the obk file 
set "action=Provisioning the obk file"
echo %action%
%stm32programmercli% %connect_reset%
%stm32programmercli% %connect_no_reset% -sdp .\Binary\%da_file%.obk
if %errorlevel% neq 0 (goto :error)

:: Reset the board after provisioning
set "action=Reset the board after provisioning"
echo %action%
%stm32programmercli% %connect_reset%
if %errorlevel% neq 0 goto :error

echo Provisioning success
if [%1] neq [AUTO] cmd /k
exit 0

:error
echo      Error when trying to "%action%" >CON
echo      Provisioning aborted >CON
if [%1] neq [AUTO] cmd /k
exit 1
