@ECHO OFF

:: Getting the Trusted Package Creator and STM32CubeProgammer CLI path
call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

set isGeneratedByCubeMX=%PROJECT_GENERATED_BY_CUBEMX%

:: Environment variable for AppliCfg
set "project_dir=%~dp0"

if "%isGeneratedByCubeMX%" == "true" (
  set appli_dir=%oemirot_appli_path_project%
  set flash_layout="%cube_fw_path%\Projects\STM32H7S78-DK\Applications\ROT\OEMiROT_Boot\Inc\flash_layout.h"
) else (
  set appli_dir=../../%oemirot_appli_path_project%
  set flash_layout="%project_dir%..\..\Applications\ROT\OEMiROT_Boot\Inc\flash_layout.h"
)

:: Environment variables
set stirot_config=".\Config\STiROT_Config.xml"
set stirot_oemurot_appli="rot_enc_sign.hex"

:: Default product state variables
set default_state_config=".\Config\default_state.txt"
set default_product_state_str="default_product_state"
set default_dbgauth_str="default_dbgauth"
set product_state_list=CLOSED LOCKED
set dbgauth_list=CERTIFICATE PASSWORD

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

:update
set "AppliCfg=%python%%AppliCfg%"

:: External scripts
set ob_flash_programming="ob_flash_programming.bat"
set ob_key_provisioning="obkey_provisioning.bat"
set update_ob_setup="update_ob_setup.bat"
set update_STiROT_iLoader_setup="update_stirot_iloader_setup.bat"
set update_oemurot_appli_setup="update_oemurot_appli_setup.bat"

:provisioning
set ob_flash_log="ob_flash_programming.log"
set ob_key_provisioning_log="obkey_provisioning.log"
set ob_update_ob_log="update_ob_setup.log"
set update_STiROT_iLoader_log="update_stirot_iloader_setup.log"
set update_oemurot_appli_log="update_oemurot_appli_setup.log"
set provisioning_log="provisioning.log"

:: Initial configuration
set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=UR
:: Define principal log file
set current_log_file=%provisioning_log%

echo =====
echo ===== Provisioning of STiROT_OEMuROT boot path
echo ===== Application selected through env.bat: %appli_dir%
echo ===== Product state must be Open. Execute  \ROT_Provisioning\DA\regression.bat if not the case.
echo ===== DTCM/ITCM_AXI_SHARE is going to be forced to 0 for the provisioning steps
echo ===== (if needed uncomment DTCM/ITCM_AXI_SHARE lines with the wished value in the ob_flash_programming script).
echo =====
echo;

:: ====================================================== STM32H7S product preparation =========================================
:: ================================================ Final product state selection ==============================================
echo Step 0: Preliminary stage
echo;
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
        if /i "!default_product_state!" == "CLOSED" (
            set /p "product_state=%BS%       [ CLOSED* | LOCKED ]: "
        )
        if /i "!default_product_state!" == "LOCKED" (
            set /p "product_state=%BS%       [ CLOSED | LOCKED* ]: "
        )
        goto :ps_selection
    )
    set /p "product_state=%BS%       [ CLOSED | LOCKED ]: "
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

:: =============================================== Step to configure the STiROT_iLoader =======================================
echo;
echo    * STiROT_iLoader firmware generation:
echo        Open the STiROT_iLoader project with your preferred toolchain (Default path is ..\Applications\ROT\STiROT_iLoader)
echo        Rebuild all files.
echo        Press any key to continue...
if [%1] neq [AUTO] pause >nul
echo;

:: ====================================== Step to create the STiROT_Config.obk file ===========================================
echo Step 1: Configuration management
echo;
set current_log_file=%update_STiROT_iLoader_log%
echo    * STiROT_Config.obk generation:
echo        * Pre-update with STiROT_iLoader dependencies
set "command=start /w /b call %update_STiROT_iLoader_setup% AUTO"
%command% > %current_log_file% 2>&1

set ob_update_STiROT_iLoader_error=!errorlevel!
if %ob_update_STiROT_iLoader_error% neq 0 goto :step_error
call %stm32tpccli% -obk %stirot_config% >> %current_log_file% 2>&1

echo          Option bytes successfully updated according to STiROT_iLoader dependencies
echo          (see %current_log_file% for details)

:: bypass this step when Generated By CubeMX
if "%isGeneratedByCubeMX%" == "true" goto :cubemx1
echo;
echo        * From TrustedPackageCreator (OBkey tab in Security panel)
echo          Select STiROT_Config.xml (Default path is (Default path is \ROT_Provisioning\STiROT_OEMuROT\Config\STiROT_Config.xml)
echo          Warning: Default keys must NOT be used in a product. Make sure to regenerate your own keys!"
echo          Update the configuration (if/as needed) then generate STiROT_Config.obk file
echo          Press any key to continue...
if [%1] neq [AUTO] pause >nul

:: =============================================== Step to create the DA_Config.obk file ======================================
if /i "!product_state_selected!" == "CLOSED" (
    echo;
    echo    * %da_file%.obk generation:
    echo        From TrustedPackageCreator ^(OBkey tab in Security panel^)
    echo        Select %da_file%.xml^(Default path is \ROT_Provisioning\DA\Config\%da_file%.xml^)
    echo        Update the configuration ^(if/as needed^) then generate %da_file%.obk file
    echo        Press any key to continue...
    if [%1] neq [AUTO] pause >nul
)

:cubemx1
:: ======================================================= Updating the STiROT Option bytes ====================================
echo;
set current_log_file=%ob_update_ob_log%
set "action=ob_flash_programming script update..."
set "command=start /w /b call %update_ob_setup% AUTO"
echo    * %action%
%command% > %ob_update_ob_log% 2>&1

set ob_update_ob_error=!errorlevel!
if %ob_update_ob_error% neq 0 goto :step_error
echo        Option bytes successfully updated according to STiROT_Config.xml
echo        (see %ob_update_ob_log% for details)
echo;

:: =============================================== Step to configure the OEMuROT ===============================================
echo Step 2: Image generations
:: bypass this step when Generated By CubeMX
if "%isGeneratedByCubeMX%" == "true" goto :cubemx2
:: Create the OEMuROT_Config.obk file
echo;
echo    * OEMuROT_Config.obk generation:
echo        From TrustedPackageCreator ^(OBkey tab in Security panel^).
echo        Select OEMuROT_Config.xml^(Default path is \ROT_Provisioning\STiROT_OEMuROT\Config\OEMuROT_Config.xml^)
echo        Warning: Default keys must NOT be used in a product. Make sure to regenerate your own keys!
echo        Run Keygen script to randomly regenerate your own keys (Keys/*.pem)
echo        Update the configuration (if/as needed) then generate OEMuROT_Config.obk file
echo        Press any key to continue...
if [%1] neq [AUTO] pause >nul
:cubemx2
echo;
:: Configure OEMiROT_Boot project as OEMuROT
set "action=Configure OEMIROT_Boot project as OEMuROT..."
set "command=%AppliCfg% setdefine -a uncomment -n OEMUROT_ENABLE -v 1 %flash_layout%"
echo    * %action%
%command% >> %provisioning_log% 2>&1
IF !errorlevel! NEQ 0 goto :step_error
echo;

echo    * Boot firmware image generation:
echo        Open the OEMiROT_Boot project with your preferred toolchain
echo        Rebuild all files. The rot_enc_sign.hex file is generated with the postbuild command.
echo        STiROT_Config.xml and OEMuROT_Code_Image.xml are updated with OEMuROT dependencies with the postbuild command
echo        Press any key to continue...
if [%1] neq [AUTO] pause >nul

:: ================================================ Updating OEMuROT application files ========================================================
echo;
set current_log_file=%update_oemurot_appli_log%
set "command=start /w /b call %update_oemurot_appli_setup% AUTO"
::must be bypassed when generated by CubeMX
if "%isGeneratedByCubeMX%" == "false" (
    set "action=OEMuROT_Code_Image.xml update..."
    echo    * !action!

    %command% > %update_oemurot_appli_log% 2>&1
    if !errorlevel! neq 0 goto :step_error

    echo        ^(OEMuROT_Code_Image.xml successfully updated^)

    echo        ^(see %update_oemurot_appli_log% for details^)
)

:: ============================================= OEMuROT application image generation step =============================================
echo;
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
echo;

:: =============================================== Setup of the final product state  ===========================================
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
echo;
exit 1

:: ====================================================== Board provisioning step =============================================
:set_provisionning_ps
echo Step 3: Provisioning
echo;

:: ====================================================== Force DTCM/ITCM_AXI_SHARE to 0 =======================================
set current_log_file=%provisioning_log%
set "command=%stm32programmercli% %connect_no_reset% -ob DTCM_AXI_SHARE=0 ITCM_AXI_SHARE=0"
echo %command% > %provisioning_log%
%command% > %current_log_file% 2>&1

:: ========================================= Product State configuration and Provisioning steps ================================
:: Provisioning execution

set current_log_file=%provisioning_log%
set "action=Setting the product state PROVISIONING"
echo    * %action%
set "command=%stm32programmercli% %connect_no_reset% -psrss 0x17"
echo %command% >> %provisioning_log%
echo;
%command% >> %provisioning_log%
if !errorlevel! neq 0 goto :step_error

:: ======================================================= OB Keys =============================================================
:: Provisioning the obk files step
set current_log_file=%ob_key_provisioning_log%
set "action=Provisionning the .obk files..."
echo    * %action%
set "command=start /w /b call %ob_key_provisioning% AUTO"
%command% > %ob_key_provisioning_log%
set obkey_prog_error=!errorlevel!
if %obkey_prog_error% neq 0 goto :step_error

echo        Successful obk provisioning
echo        (see %ob_key_provisioning_log% for details)
echo;

:: ============================================= Option Byte and flash programming ============================================
set current_log_file=%ob_flash_log%
set "action=Programming the option bytes and flashing the images..."
set "command=start /w /b call %ob_flash_programming% AUTO"
echo    * %action%
%command% > %ob_flash_log%

set ob_flash_error=!errorlevel!
::type %ob_flash_log%
if %ob_flash_error% neq 0 goto :step_error

echo        Successful option byte programming and image flashing
echo        (see %ob_flash_log% for details)
echo;

:: Boot on STiROT
:: ================================================ IROT_SELECT Option Bytes ===================================================
set "action=Setting the iROT_SELECT to STiROT"
echo    * %action%
set ob_add=0x5200220c
set ob_mask=0xff000000
set ob_pos=0x18
set ob_value=0xb4
set "command=%stm32programmercli% %connect_no_reset% -obrss %ob_add% %ob_mask% %ob_value% %ob_pos%"
echo %command% >> %provisioning_log%
%command% >> %provisioning_log%
echo;
if !errorlevel! neq 0 goto :step_error

:: ============================================================= End functions =================================================
:: All the steps to set the STM32H7S product were executed correctly
:: Set the final product state of the STM32H7S product
:: In the final product state, the connection with the board is lost and the return value of the command cannot be verified
:set_final_ps
set current_log_file=%provisioning_log%
set "action=Setting the final product state !product_state_selected! "
echo    * %action%
set "command=%stm32programmercli% %connect_no_reset% -psrss  %psrss_value%"
echo %command% >> %provisioning_log%
echo;
%stm32programmercli% %connect_reset% >> nul
%command% >> %provisioning_log%
echo.

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
echo;
echo =====
echo ===== Error while executing "%action%".
echo ===== See %current_log_file% for details. Then try again.
echo =====
if [%1] neq [AUTO] cmd /k
exit 1
