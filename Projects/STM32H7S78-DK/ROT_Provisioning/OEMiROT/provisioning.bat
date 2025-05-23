@ECHO OFF

:: Getting the Trusted Package Creator and STM32CubeProgammer CLI path
call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

set isGeneratedByCubeMX=%PROJECT_GENERATED_BY_CUBEMX%

:: Environment variable for AppliCfg
set project_dir=%~dp0

if "%isGeneratedByCubeMX%" == "true" (
  set appli_dir=%oemirot_boot_path_project%
  set flash_layout="%cube_fw_path%\Projects\STM32H7S78-DK\Applications\ROT\OEMiROT_Boot\Inc\flash_layout.h"
) else (
  set appli_dir=..\..\..\%oemirot_boot_path_project%
  set flash_layout="%project_dir%..\..\Applications\ROT\OEMiROT_Boot\Inc\flash_layout.h"
)

:: Default product state variables
set default_state_config=".\Config\default_state.txt"
set default_product_state_str="default_product_state"
set default_dbgauth_str="default_dbgauth"
set product_state_list=OPEN CLOSED LOCKED
set dbgauth_list=CERTIFICATE PASSWORD

:start
goto exe:
goto py:
:exe
::called if we want to use AppliCfg executable
set AppliCfg="%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\dist\AppliCfg.exe"
set "python="
if exist %AppliCfg% (
    goto update
)
:py
::called if we just want to use AppliCfg python (think to comment "goto exe:")
set AppliCfg="%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"
set "python=python "

:update
set "AppliCfg=%python%%AppliCfg%"

:: External scripts
set ob_flash_programming="ob_flash_programming.bat"
set ob_key_provisioning="obkey_provisioning.bat"
set ob_key_provisioning_open="obkey_provisioning_open.bat"

:: Variables for image xml configuration
set fw_in_bin="Firmware binary input file"
set fw_out_bin="Image output file"
set app_bin="%appli_dir%\Binary\rot_app.bin"
set app_bin=%app_bin:\=/%
set app_enc_sign_hex="%appli_dir%\Binary\rot_app_enc_sign.hex"
set app_enc_sign_hex=%app_enc_sign_hex:\=/%
set oemirot_config_file="%project_dir%Config\OEMiROT_Config.xml"
set code_image_file="%project_dir%Images\OEMiROT_Code_Image.xml"

:provisioning
set ob_flash_log="ob_flash_programming.log"
set ob_key_provisioning_log="obkey_provisioning.log"
set provisioning_log="provisioning.log"
if exist %ob_flash_log% (
    del %ob_flash_log%
)
if exist %provisioning_log% (
    del %provisioning_log%
)

:: Initial configuration
set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=UR
:: Define principal log file
set current_log_file=%provisioning_log%

echo =====
echo ===== Provisioning of OEMiROT boot path
echo ===== Application selected through env.bat: %appli_dir%
echo ===== Product state must be Open. Execute  \ROT_Provisioning\DA\regression.bat if not the case.
echo ===== DTCM/ITCM_AXI_SHARE is going to be forced to 0 for the provisioning steps
echo ===== (if needed uncomment DTCM/ITCM_AXI_SHARE lines with the wished value in the ob_flash_programming script).
echo =====
echo.

:: ====================================================== STM32H7S product preparation =========================================
:: ================================================ Final product state selection ==============================================
echo Step 0: Preliminary stage
echo.
:: get the default product state & debug authentication value from the default state file
for /f %%A in ('"prompt $H & echo on & for %%B in (1) do rem"') do set "BS=%%A"
if exist %default_state_config% (
    set default_ps_enabled=1
    set "product_state="
    set "dbgauth="

    :: READ default product state configuration file
    for /f "tokens=1,2,3 delims=^=" %%a IN ('findstr %default_product_state_str% %default_state_config%') do set "default_product_state=%%b"
    for /f "tokens=1,2,3 delims=^=" %%a IN ('findstr %default_dbgauth_str% %default_state_config%') do set "default_dbgauth=%%b"
)
echo    * Define product state value ^(* default choice^)
if [%1] neq [AUTO] (
    if "%default_ps_enabled%" neq "" (
        for %%i in (%product_state_list%) do (
            if /i "%%i"=="!default_product_state!" (
                goto :valid_default_ps
            )
            echo %%i
        )
        :: Invalid default product state go into error
        echo "WRONG default ps !default_product_state!"
        goto :step_error

:valid_default_ps
        if /i "!default_product_state!" == "OPEN" (
            set /p "product_state=%BS%       [ OPEN* | CLOSED | LOCKED ]: "
        )
        if /i "!default_product_state!" == "CLOSED" (
            set /p "product_state=%BS%       [ OPEN | CLOSED* | LOCKED ]: "
        )
        if /i "!default_product_state!" == "LOCKED" (
            set /p "product_state=%BS%       [ OPEN | CLOSED | LOCKED* ]: "
        )
        goto ps_selection
    )
    set /p "product_state=%BS%       [ OPEN | CLOSED | LOCKED ]: "
) else (
    set "product_state=%BS%"
)

:ps_selection
for %%i in (%product_state_list%) do (
    if /i "%%i"=="!product_state!" (
       set product_state_selected=!product_state!
       goto :ps_selection_next
    )
)

set product_state_selected=!default_product_state!

:: No valid product state is provided, use the default one
set product_state_selected=!default_product_state!
echo        Default product state is going to be applied

:ps_selection_next
:: Check the input product state
if /i "!product_state_selected!" == "LOCKED" goto :ps_update
if /i "!product_state_selected!" == "OPEN" goto :ps_update

:: Update of the dbg authentication method
for /f %%A in ('"prompt $H & echo on & for %%B in (1) do rem"') do set "BS=%%A"

echo;
echo    * Define Debug Authentication method ^(* default choice^)
if [%1] neq [AUTO] (
    if "%default_ps_enabled%" neq "" (
        for %%i in (%dbgauth_list%) do (
            if /i "%%i"=="!default_dbgauth!" (
                goto :valid_default_dbgauth
            )
        )
        :: Invalid default product state go into error
        echo "WRONG default dbgauth !default_product_state!"
        goto :step_error

:valid_default_dbgauth
        if /i "%default_dbgauth%" == "CERTIFICATE" (
            set /p "dbgauth=%BS%       [ CERTIFICATE* | PASSWORD ]: "
        )
        if /i "%default_dbgauth%" == "PASSWORD" (
            set /p "dbgauth=%BS%       [ CERTIFICATE | PASSWORD* ]: "
        )
        goto dbgauth_selection
    )
    set /p "dbgauth=%BS%       [ CERTIFICATE | PASSWORD ]: "
) else (
    set "dbgauth=%BS%"
)

:dbgauth_selection

for %%i in (%dbgauth_list%) do (
    if /i "%%i"=="!dbgauth!" (
       echo;
       set dbgauth_selected=%%i
       goto :ps_update
    )
)

set dbgauth_selected=%default_dbgauth%
echo        Default Debug Authentication is going to be applied
goto :ps_update

:: ======================================================= Updating the Product state ==========================================
:ps_update

if /i "!product_state_selected!" == "OPEN" (
    set da_file=DA_ConfigLock
)

if /i "!product_state_selected!" == "CLOSED" (
    if /i "!dbgauth_selected!" == "CERTIFICATE" (
        set da_file=DA_Config
    )

    if /i "!dbgauth_selected!" == "PASSWORD" (
        set da_file=DA_ConfigWithPassword
    )
)

if /i "!product_state_selected!" == "LOCKED" (
    set da_file=DA_ConfigLock
)

:: =============================================== Step to create the OEMiROT_Config.obk file ==============================================
echo.
echo Step 1: Configuration management
echo.

:: bypass this step when Generated By CubeMX
if "%isGeneratedByCubeMX%" == "true" goto :cubemx1

echo    * OEMiROT_Config.obk generation:
echo        From TrustedPackageCreator ^(OBkey tab in Security panel^).
echo        Select OEMiROT_Config.xml^(Default path is \ROT_Provisioning\OEMiROT\Config\OEMiROT_Config.xml^)
echo        Warning: Default keys must NOT be used in a product. Make sure to regenerate your own keys!
echo        Run Keygen script to randomly regenerate your own keys (Keys/*.pem)
echo        Update the configuration (if/as needed) then generate OEMiROT_Config.obk file
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul

:cubemx1
if  "%device_type%" == "H7S" (
    if /i "!product_state_selected!" == "OPEN" (
        set "action=Configure OEMiROT_Config.xml for OPEN"
        set "command=%AppliCfg% xmlval --value 0 --decimal -txml DoEncryption %oemirot_config_file%"
    ) else (
        set "action=Configure OEMiROT_Config.xml for CLOSED or LOCKED"
        set "command=%AppliCfg% xmlval --value 1 --decimal -txml DoEncryption %oemirot_config_file%"
)) else (
    set "command=%AppliCfg% xmlval --value 0 --decimal -txml DoEncryption %oemirot_config_file%"
)
echo    * %action%
%command% >> %provisioning_log% 2>&1
IF !errorlevel! NEQ 0 goto :step_error

set "action=Force the update of OEMiROT_Config.obk"
set "command=%stm32tpccli% -obk %oemirot_config_file% AUTO"
echo    * %action%
echo.
%command% >> %provisioning_log% 2>&1
IF !errorlevel! NEQ 0 goto :step_error

:: =============================================== Step to create the DA_Config.obk file ==================================================

:: bypass this step when Generated By CubeMX
if "%isGeneratedByCubeMX%" == "true" goto :cubemx2

if /i "!product_state_selected!" == "CLOSED" (
    echo.
    echo    * %da_file%.obk generation:
    echo        From TrustedPackageCreator ^(OBkey tab in Security panel^).
    echo        Select %da_file%.xml^(Default path is \ROT_Provisioning\DA\Config\%da_file%.xml^)
    echo        Update the configuration ^(if/as needed^) then generate %da_file%.obk file
    echo        Press any key to continue...
    if [%1] neq [AUTO] pause >nul
)

:: ========================================================= Image generation step ========================================================
:cubemx2
echo.
echo Step 2: Image generations
echo.

:: Configure OEMIROT_Boot project as OEMIROT
set "action=Configure OEMIROT_Boot project as OEMIROT..."
set "command=%AppliCfg% setdefine -a comment -n OEMUROT_ENABLE %flash_layout%"
echo    * %action%
%command% >> %provisioning_log% 2>&1
IF !errorlevel! NEQ 0 goto :step_error

echo.
echo    * Boot firmware image generation
echo        Open the OEMiROT_Boot project with preferred toolchain and rebuild all files.
echo        At this step the project is configured for OEMiROT boot path.
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul
echo.

:: bypass this step when Generated By CubeMX
if "%isGeneratedByCubeMX%" == "true" goto :cubemx3

set "action=Configure the Firmware binary input file in OEMiROT_Code_Image.xml..."
set "command=%AppliCfg% xmlval -v %app_bin% --string -n %fw_in_bin% %code_image_file%"
echo    * %action%
echo %command% >> %provisioning_log%
%command% >> %provisioning_log%
IF !errorlevel! NEQ 0 goto :step_error

set "action=Configure the Image output file in OEMiROT_Code_Image.xml..."
set "command=%AppliCfg% xmlval -v %app_enc_sign_hex% --string -n %fw_out_bin% %code_image_file%"
echo    * %action%
echo %command% >> %provisioning_log%
%command% >> %provisioning_log%
IF !errorlevel! NEQ 0 goto :step_error

:cubemx3
echo    * Code firmware image generation:
if "%isGeneratedByCubeMX%" == "true" (
    echo        If the configuration of OEMiROT_Boot project has been updated, reload and regenerate STM32CubeMX application project.
    echo        Open the regenerated application project with preferred toolchain and rebuild all files.
) else (
    echo        Open the OEMiROT_Appli project with your preferred toolchain
    echo        Rebuild all files. The appli_enc_sign.hex file is generated with the postbuild command
)
echo        Press any key to continue...
if [%1] neq [AUTO] pause >nul
echo.

:: ================================================== Force DTCM/ITCM_AXI_SHARE to 0 =================================================
echo Step 3: Provisioning
echo.
set "action=Force DTCM/ITCM_AXI_SHARE to 0..."
set current_log_file=%provisioning_log%
echo    * %action%
set "command=%stm32programmercli% %connect_no_reset% -ob DTCM_AXI_SHARE=0 ITCM_AXI_SHARE=0"
echo %command% >> %provisioning_log%
%command% > %current_log_file% 2>&1

:: ================================================ Final product state selection =========================================================
if /i "!product_state_selected!" == "OPEN" (
    goto provisioning_step
)

if /i "!product_state_selected!" == "CLOSED" (
    set psrss_value=0x72
    goto set_provisionning_ps
)

if /i "!product_state_selected!" == "LOCKED" (
    set psrss_value=0x5C
    goto set_provisionning_ps
)

echo        WRONG product state selected
set current_log_file="./*.log files "
echo.
exit 1

:: ========================================= Product State configuration and Provisioning steps ==========================================
:: Provisioning execution
:set_provisionning_ps
set current_log_file=%provisioning_log%
set "action=Setting the product state PROVISIONING"
echo    * %action%
set "command=%stm32programmercli% %connect_no_reset% -psrss 0x17"
echo %command% >> %provisioning_log%
echo.
%command% >> %provisioning_log%
if !errorlevel! neq 0 goto :step_error
goto provisioning_step

:: Set the final product state of the STM32H7S product
:set_final_ps
set current_log_file=%provisioning_log%
set "action=Setting the final product state !product_state_selected! "
echo    * %action%
set "command=%stm32programmercli% %connect_no_reset% -psrss  %psrss_value%"
echo %command% >> %provisioning_log%
echo.
%stm32programmercli% %connect_reset% >> nul
%command% >> %provisioning_log%
echo.
goto final_execution

:: Provisioning the obk files step
:provisioning_step
set current_log_file=%ob_key_provisioning_log%
set "action=Provisionning the .obk files..."
echo    * %action%
if /i "!product_state_selected!" == "OPEN" (
    set "command=start /w /b call %ob_key_provisioning_open% AUTO"
) else (
    set "command=start /w /b call %ob_key_provisioning% AUTO"
)
!command! > %ob_key_provisioning_log% 2>&1
set obkey_prog_error=!errorlevel!
if %obkey_prog_error% neq 0 goto :step_error

echo        Successful obk provisioning
echo        (see %ob_key_provisioning_log% for details)
echo.
goto ob_flash_programming


:: ================================================= Option Byte and flash programming =====================================================
:ob_flash_programming
set "action=Programming the option bytes and flashing the images..."
set current_log_file=%ob_flash_log%
set "command=start /w /b call %ob_flash_programming% AUTO !product_state_selected!"
echo    * %action%
%command% > %ob_flash_log%

if !errorlevel! neq 0 goto :step_error

echo        Successful option byte programming and image flashing
echo        (see %ob_flash_log% for details)
echo.

if /i "!product_state_selected!" == "OPEN" goto :final_execution
goto set_final_ps

:: ============================================================= End functions =============================================================
:: All the steps to set the STM32H7S product were executed correctly
:final_execution

echo =====
echo ===== The board is correctly configured.
if "%isGeneratedByCubeMX%" == "true" goto :no_menu
echo ===== Connect UART console (115200 baudrate) to get application menu.

:no_menu
echo ===== Reset the board to start the application.
echo =====
if [%1] neq [AUTO] cmd /k
exit 0

:: Error when external script is executed
:step_error
echo.
echo =====
echo ===== Error while executing "%action%".
echo ===== See %current_log_file% for details. Then try again.
echo =====
if [%1] neq [AUTO] cmd /k
exit 1
