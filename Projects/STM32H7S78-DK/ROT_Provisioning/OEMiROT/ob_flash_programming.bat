@echo off
call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Set the selected product state
set "selected_product_state=NOT_OPEN"
IF /i [%1] EQU [OPEN] set "selected_product_state=OPEN"
IF /i [%2] EQU [OPEN] set "selected_product_state=OPEN"
echo %1 %2 !selected_product_state!

:: Data updated with the postbuild of OEMiROT-Boot
set wrps0=0
set wrps1=0
set wrps2=0
set wrps3=0
set wrps4=0
set wrps5=0
set wrps6=0
set wrps7=0
set /A "wrps=(%wrps0%<<0)|(%wrps1%<<1)|(%wrps2%<<2)|(%wrps3%<<3)|(%wrps4%<<4)|(%wrps5%<<5)|(%wrps6%<<6)|(%wrps7%<<7)"
set bootaddress=0x8000000
set hdp_start=1
set hdp_end=0
set /A "hdps=(%hdp_start%<<0)|(%hdp_end%<<16)"
set app_image_number=1
set code_image=%oemirot_appli%

:: CubeProgrammer connection
set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=UR
if /i "!selected_product_state!" == "OPEN" (
    set connect_normal=-c port=SWD speed=fast ap=1 mode=normal
) else (
    set connect_normal=-c port=%com_port% br=921600
)

if "%isGeneratedByCubeMX%" == "true" (
    set appli_dir=%oemirot_appli_path_project%
    set oemirot_code="%cube_fw_path%\Projects\STM32H7S78-DK\Applications\ROT\OEMiROT_Boot\Binary\OEMiROT_Boot.bin"
) else (
    set appli_dir=..\..\%oemirot_appli_path_project%
    set oemirot_code="%~dp0..\..\Applications\ROT\OEMiROT_Boot\Binary\OEMiROT_Boot.bin"
)

:: =============================================== Remove protections and erase the user flash ===============================================

set remove_protect_open=-ob WRPS0=1 WRPS1=1 WRPS2=1 WRPS3=1 WRPS4=1 WRPS5=1 WRPS6=1 WRPS7=1 HDP_AREA_START=1 HDP_AREA_END=0
set erase_all=-e all
set wrps_disable=0xff
set hdp_area_start_disabled=0x0ff
set hdp_area_end_disabled=0x000
set /A "hdps_disabled=(%hdp_area_start_disabled%<<0)|(%hdp_area_end_disabled%<<16)"

:: =============================================== Configure Option Bytes ====================================================================

set "action=Remove Protection and erase All"
echo %action%
if /i "!selected_product_state!" == "OPEN" (
    set "command=%stm32programmercli% %connect_reset% %remove_protect_open% %erase_all%"
    echo !command!
    !command!
    IF !errorlevel! NEQ 0 goto :error
) else (
    :: Disable WRP protections
    %stm32programmercli% %connect_reset% >> nul
    set ob_add=0x5200221c
    set ob_mask=0x000000ff
    set ob_pos=0x00
    set "command=%stm32programmercli% %connect_no_reset% -obrss !ob_add! !ob_mask! !wrps_disable! !ob_pos!"
    echo !command!
    !command!
    IF !errorlevel! NEQ 0 goto :error

    :: Disable HDP protection on user flash
    %stm32programmercli% %connect_reset% >> nul
    set ob_add=0x52002234
    set ob_mask=0x03ff03ff
    set ob_pos=0x00
    set "command=%stm32programmercli% %connect_no_reset% -obrss !ob_add! !ob_mask! %hdps_disabled% !ob_pos!"
    echo !command!
    !command!
    IF !errorlevel! NEQ 0 goto :error

    %stm32programmercli% %connect_reset% >> nul
    %stm32programmercli% %connect_no_reset% %erase_all%
    IF !errorlevel! NEQ 0 goto :error
)

set "action=Set SRAM ECC configuration"
echo %action%
:: Recommended configuration for secure boot is :
::   - SRAM ECC activated. Hack tentative detection enabled ==> ECC_ON_SRAM=1
set "command=%stm32programmercli% %connect_no_reset% -ob ECC_ON_SRAM=1"
echo !command!
!command!
IF !errorlevel! NEQ 0 goto :error

set "action=Set XSPI1_HSLV configuration (needed to use external memories)"
echo %action%
set "command=%stm32programmercli% %connect_no_reset% -ob XSPI1_HSLV=1"
echo !command!
!command!
IF !errorlevel! NEQ 0 goto :error

set "action=Set XSPI2_HSLV configuration (needed to use external memories)"
echo %action%
set "command=%stm32programmercli% %connect_no_reset% -ob XSPI2_HSLV=1"
echo !command!
!command!
IF !errorlevel! NEQ 0 goto :error

:: ================================================== Download OEMiROT_Boot ==================================================================

set "action=Write OEMiROT_Boot"
echo %action%
set "command=%stm32programmercli% %connect_no_reset% -d %oemirot_code% %bootaddress% -v"
echo !command!
!command!
IF !errorlevel! NEQ 0 goto :error

set "action=OEMiROT_Boot Written"
echo %action%

:: ======================================================= Extra board protections =========================================================

set "action=Define write protected area and hide protected area"
echo %action%
if /i "!selected_product_state!" == "OPEN" (
    set "command=%stm32programmercli% %connect_no_reset% -ob WRPS0=%wrps0% WRPS1=%wrps1% WRPS2=%wrps2% WRPS3=%wrps3% WRPS4=%wrps4% WRPS5=%wrps5% WRPS6=%wrps6% WRPS7=%wrps7% HDP_AREA_START=%hdp_start% HDP_AREA_END=%hdp_end%"
    echo !command!
    !command!
    IF !errorlevel! NEQ 0 goto :error
) else (
    :: Configure WRP protection on user flash
    %stm32programmercli% %connect_reset% >> nul
    set ob_add=0x5200221c
    set ob_mask=0x000000ff
    set ob_pos=0x00
    set "command=%stm32programmercli% %connect_no_reset% -obrss !ob_add! !ob_mask! %wrps% !ob_pos!"
    echo !command!
    !command!
    IF !errorlevel! NEQ 0 goto :error

    :: Configure HDP protection on user flash
    %stm32programmercli% %connect_reset% >> nul
    set ob_add=0x52002234
    set ob_mask=0x03ff03ff
    set ob_pos=0x00
    set "command=%stm32programmercli% %connect_no_reset% -obrss !ob_add! !ob_mask! %hdps% !ob_pos!"
    echo !command!
    !command!
    IF !errorlevel! NEQ 0 goto :error
)

:: ==================================================== Download images ====================================================================
set "action=Application image programming in download slots"
echo %action%

if  "%app_image_number%" == "1" (
    set "action=Write OEMiROT_Appli Code"
    echo %action%
    if /i "!selected_product_state!" == "OPEN" (
        set "command=%stm32programmercli% !connect_normal! %stm32ExtLoaderFlashOpen% -d %appli_dir%\Binary\%code_image% --verify"
        echo !command!
        !command!
        IF !errorlevel! NEQ 0 goto :error
    ) else (
        set "command=%stm32programmercli% !connect_normal! %stm32ExtLoaderFlash% -d %appli_dir%\Binary\%code_image% --verify"
        echo !command!
        !command!
        IF !errorlevel! NEQ 0 goto :error
    )

    echo "Appli Written"
)

:: ==================================================== DTCM/ITCM_AXI_SHARE ====================================================================

:: Place to change DTCM/ITCM_AXI_SHARE configurations if there is a need
::set "command=%stm32programmercli% %connect_no_reset% -ob DTCM_AXI_SHARE=0 ITCM_AXI_SHARE=0"
::echo !command!
::!command!

echo Programming success
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo      Error when trying to "%action%" >CON
echo      Programming aborted >CON
IF [%1] NEQ [AUTO] cmd /k
exit 1
