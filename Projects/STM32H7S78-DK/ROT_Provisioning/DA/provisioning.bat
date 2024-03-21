@ECHO OFF
:: Getting the CubeProgammer_cli path
call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: CubeProgammer path and input files
set ob_programming="ob_programming.bat"
set obk_provisioning="obk_provisioning.bat"
:: Log Files
set ob_programming_log="ob_programming.log"
set obk_provisioning_log="obk_provisioning.log"
set provisioning="provisioning.log"
set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst

echo.
echo =====
echo ===== Provisioning of Legacy DA
echo ===== Product state must be Open. Execute  \ROT_Provisioning\DA\regression.bat if not the case.
echo ===== DTCM/ITCM_AXI_SHARE is going to be forced to 0 for the provisioning steps
echo ===== (if needed uncomment DTCM/ITCM_AXI_SHARE lines with the wished value in the ob_programming script).
echo =====
echo.

:: ======================================================== Define board configuration ======================================================
echo Step 1 : Configuration management
echo;

:product_state_choice
for /f %%A in ('"prompt $H & echo on & for %%B in (1) do rem"') do set "BS=%%A"
if [%1] neq [AUTO] (
    set /p "product_state=%BS%       [ CLOSED | LOCKED ]: "
) else (
    set "product_state=%BS%"
)

:ps_selection
if /i "%product_state%" == "CLOSED" (
    set "product_state_selected=%product_state%"
    set psrss_value=0x72
    goto dbg_auth_update
)

if /i "%product_state%" == "LOCKED" (
    set "product_state_selected=%product_state%"
    set psrss_value=0x5C
    set da_file=DA_ConfigLock
    goto update_da
)

echo        WRONG product state selected
set current_log_file="./*.log files "
echo;
goto product_state_choice

:dbg_auth_update
echo;
echo    * Define Debug Authentication method
set "da_file=""
for /f %%A in ('"prompt $H & echo on & for %%B in (1) do rem"') do set "BS=%%A"
if [%1] neq [AUTO] (
    set /p "dbgauth=%BS%       [ CERTIFICATE | PASSWORD ]: "
) else (
    set "dbgauth=%BS%"
)

if /i "!dbgauth!" == "CERTIFICATE" (
    echo;
    set "dbgauth_selected=CERTIFICATE"
    set "da_file=DA_Config"
    goto update_da
)

if /i "!dbgauth!" == "PASSWORD" (
    echo;
    set "dbgauth_selected=PASSWORD"
    set "da_file=DA_ConfigWithPassword"
    goto update_da
)

echo        WRONG dbg auth selected
set current_log_file="./*.log files "
echo;
goto dbg_auth_update

:: =============================================== Steps to create the obk file ==============================================
:update_da

echo    * !da_file!.obk generation:
echo        From TrustedPackageCreator ^(OBkey tab in Security panel^).
echo        Select !da_file!.xml (Default path is \ROT_Provisioning\DA\Config\!da_file!.xml)
echo        Update the configuration (if/as needed) then generate !da_file!.xml file
echo        Press any key to continue...
echo.
pause >nul

:: ========================================================= Board provisioning steps =======================================================
:: ================================================== Provisioning the obk file ===========================================================
echo Step 2 : Provisioning
:: Set DTCM/ITCM_AXI_SHARE to 0
set current_log_file=%provisioning%
set "command=%stm32programmercli% %connect_no_reset% -ob DTCM_AXI_SHARE=0 ITCM_AXI_SHARE=0"
echo %command% > %current_log_file%
%command% >> %current_log_file% 2>&1

:: Set the product state "Provisioning". This will allow to execute the provisioning step
:set_provisionning_ps
set "action=Setting the product state PROVISIONING"
set current_log_file=%provisioning%
echo    * %action%
echo.

set "command=%stm32programmercli% %connect_no_reset% -psrss 0x17"
%command% >> %current_log_file%
if %errorlevel% neq 0 goto :step_error
goto provisioning_step

:: Provisioning the obk file step
:provisioning_step
set current_log_file=%obk_provisioning_log%
set "action=Provisionning the .obk file ..."
set "command=start /w /b call %obk_provisioning% AUTO"
echo    * %action%
%command% >> %obk_provisioning_log%
if %errorlevel% neq 0 goto :step_error

echo        Successful obk provisioning
echo        (see %obk_provisioning_log% for details)
echo.

:: ====================================================== Option Bytes programming ==========================================================
set "action=Programming the option bytes  ..."
set current_log_file=%ob_programming_log%
set "command=start /w /b call %ob_programming% AUTO"
echo    * %action%
%command% > %ob_programming_log%
if %errorlevel% neq 0 goto :step_error

echo        Successful option bytes programming
echo        (see %ob_programming_log% for details)
echo.

:: ========================================================= image generation steps ========================================================
echo Step 3 : Image flashing
echo    * At this step, you have to flash your application with your preferred toolchain
echo      The connection, with the toolchain, must be done in Hotplug mode (not on a reset)
echo        Press any key to continue...
echo.
pause >nul


: Set the final product state of the STM32H7RS product
:set_final_ps
set "action=Setting the final product state %product_state% "
set current_log_file=%provisioning%
echo    * %action%
set "command=%stm32programmercli% %connect_no_reset% -psrss  %psrss_value%"
echo %command% >> %provisioning%
%command% >> %provisioning%
:: In the final product state, the connection with the board is lost and the return value of the command cannot be verified
echo.
goto final_execution

:: ============================================================= End functions =============================================================
:: All the steps to set the STM32H7RS product were executed correctly
:final_execution
echo =====
echo ===== The board is correctly configured.
echo =====
cmd /k
exit 0

:: Error when external script is executed
:step_error
echo.
echo =====
echo ===== Error while executing "%action%".
echo ===== See %current_log_file% for details. Then try again.
echo =====
cmd /k
