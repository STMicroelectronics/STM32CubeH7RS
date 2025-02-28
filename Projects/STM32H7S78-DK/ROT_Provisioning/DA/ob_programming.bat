@echo off

:: Enable delayed expansion
setlocal EnableDelayedExpansion

if [%1] neq [AUTO] call ../env.bat

:: CubeProgammer connection
set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=UR

:: =============================================== Remove protections and initialize Option Bytes ===========================================
set "wrps_disable=0xff"
set "hdp_area_start=0x0ff"
set "hdp_area_end=0x000"

:: =============================================== Configure Option Bytes ====================================================================
set "action=Configure Initial OB"
echo %action%

:: Disable WRP protections
:: WRPS
%stm32programmercli% %connect_reset% >> nul
set ob_add=0x5200221c
set ob_mask=0x000000ff
set ob_pos=0x00
set "command=%stm32programmercli% %connect_no_reset% -obrss %ob_add% %ob_mask% %wrps_disable% %ob_pos%"
echo !command!
!command!
IF %errorlevel% NEQ 0 goto :error

:: Disable HDP protection on user flash
:: HDP_AREA_START
%stm32programmercli% %connect_reset% >> nul
set ob_add=0x52002230
set ob_mask=0x000003ff
set ob_pos=0x00
set "command=%stm32programmercli% %connect_no_reset% -obrss %ob_add% %ob_mask% %hdp_area_start% %ob_pos%"
echo !command!
!command!
IF %errorlevel% NEQ 0 goto :error

:: HDP_AREA_END
%stm32programmercli% %connect_reset% >> nul
set ob_add=0x52002230
set ob_mask=0x000003ff
set ob_pos=0x10
set "command=%stm32programmercli% %connect_no_reset% -obrss %ob_add% %ob_mask% %hdp_area_end% %ob_pos%"
echo !command!
!command!
IF %errorlevel% NEQ 0 goto :error

:: ==================================================== DTCM/ITCM_AXI_SHARE ====================================================================

:: Place to change DTCM/ITCM_AXI_SHARE configurations if there is a need
::set "command=%stm32programmercli% %connect_no_reset% -ob DTCM_AXI_SHARE=0 ITCM_AXI_SHARE=0
::echo !command!
::!command!

:end_programming
echo Programming success
if [%1] neq [AUTO] cmd /k
exit 0

:error
echo        Error when trying to "%action%" >CON
echo        Programming aborted >CON
if [%1] neq [AUTO] cmd /k
exit 1
