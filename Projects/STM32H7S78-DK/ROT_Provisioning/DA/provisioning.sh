#!/bin/bash

# Getting the CubeProgrammer_cli path
source ../env.sh

if [ $# -ge 1 ]; then script_mode=$1; else mode=MANUAL; fi

# CubeProgammer path and input files
ob_programming="./ob_programming.sh"
obk_provisioning="./obk_provisioning.sh"
# Log Files
ob_programming_log="ob_programming.log"
obk_provisioning_log="obk_provisioning.log"
provisioning="provisioning.log"
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst"

# ============================================== Script functions ============================================================

# Error when external script is executed
step_error()
{
  echo
  echo "====="
  echo "===== Error while executing \"$action\"."
  echo "===== See $current_log_file for details. Then try again."
  echo "====="
  $SHELL
  exit 1
}

dbg_auth_update()
{
  echo
  echo "   * Define Debug Authentication method"
  da_file=""

  if [ "$script_mode" != "AUTO" ]; then
    read -p "dbgauth=       [ CERTIFICATE | PASSWORD ]: " dbgauth
    dbgauth=$(echo $dbgauth | tr '[:lower:]' '[:upper:]')
  else
    dbgauth=""
  fi

  if [[ "${dbgauth^^}" == "CERTIFICATE" ]]; then
    echo
    dbgauth_selected="CERTIFICATE"
    da_file="DA_Config"
    update_da
  elif [[ "${dbgauth^^}" == "PASSWORD" ]]; then
    echo
    dbgauth_selected="PASSWORD"
    da_file="DA_ConfigWithPassword"
    update_da
  else
    echo        WRONG dbg auth selected
    step_error
    exit 1
  fi

  return
}

# Steps to create the obk file
update_da()
{
  echo "   * $da_file.obk generation:"
  echo "       From TrustedPackageCreator ^(OBkey tab in Security panel^)."
  echo "       Select $da_file.xml (Default path is \ROT_Provisioning\DA\Config\$da_file.xml)"
  echo "       Update the configuration (if/as needed) then generate $da_file.xml file"
  echo "       Press any key to continue..."
  echo
  read -p "" -n1 -s

  return
}

echo
echo "====="
echo "===== Provisioning of Legacy DA"
echo "===== Product state must be Open. Execute  \ROT_Provisioning\DA\regression.sh if not the case."
echo "===== DTCM/ITCM_AXI_SHARE is going to be forced to 0 for the provisioning steps"
echo "===== (if needed uncomment DTCM/ITCM_AXI_SHARE lines with the wished value in the ob_programming script)."
echo "====="
echo

# ======================================================== Define board configuration ======================================================
echo "Step 1 : Configuration management"
echo

if [ "$script_mode" != "AUTO" ]; then
  read -p "product_state=       [ CLOSED | LOCKED ]: " product_state
  product_state=$(echo $product_state | tr '[:lower:]' '[:upper:]')
else
  product_state=""
fi

if [[ "${product_state^^}" == "CLOSED" ]]; then
  product_state_selected="$product_state"
  psrss_value="0x72"
  dbg_auth_update

elif [[ "${product_state^^}" == "LOCKED" ]]; then
  product_state_selected="$product_state"
  psrss_value="0x5C"
  da_file="DA_ConfigLock"
  update_da
else
  echo "       WRONG product state selected"
  echo
  step_error
  exit 1
fi


# ========================================================= Board provisioning steps =======================================================
# ================================================== Provisioning the obk file ===========================================================
echo "Step 2 : Provisioning"
# Set DTCM/ITCM_AXI_SHARE to 0
current_log_file="$provisioning"
echo "\"$stm32programmercli\" $connect_no_reset -ob DTCM_AXI_SHARE=0 ITCM_AXI_SHARE=0" > $current_log_file
"$stm32programmercli" $connect_no_reset -ob DTCM_AXI_SHARE=0 ITCM_AXI_SHARE=0 >> $current_log_file 2>&1

# Set the product state "Provisioning". This will allow to execute the provisioning step
action="Setting the product state PROVISIONING"
current_log_file="$provisioning"
echo "   * $action"
echo

echo "\"$stm32programmercli\" $connect_no_reset -psrss 0x17" >> $current_log_file
"$stm32programmercli" $connect_no_reset -psrss 0x17 >> $current_log_file 2>&1
if [ $? -ne 0 ]; then step_error; exit 1; fi

# Provisioning the obk file step
current_log_file="$obk_provisioning_log"
action="Provisionning the .obk file ..."
echo "   * $action"
source "$obk_provisioning" AUTO >> $obk_provisioning_log
if [ $? -ne 0 ]; then step_error; exit 1; fi

echo "       Successful obk provisioning"
echo "       (see $obk_provisioning_log for details)"
echo

# ====================================================== Option Bytes programming ==========================================================
action="Programming the option bytes  ..."
current_log_file=$ob_programming_log
echo "   * $action"
source "$ob_programming" AUTO > $ob_programming_log
if [ $? -ne 0 ]; then step_error; exit 1; fi

echo "       Successful option bytes programming"
echo "       (see $ob_programming_log for details)"
echo

# ========================================================= image generation steps ========================================================
echo "Step 3 : Image flashing"
echo "   * At this step, you have to flash your application with your preferred toolchain"
echo "     The connection, with the toolchain, must be done in Hotplug mode (not on a reset)"
echo "       Press any key to continue..."
echo
read -p "" -n1 -s


# Set the final product state of the STM32H7RS product
action="Setting the final product state $product_state"
current_log_file="$provisioning"
echo "   * $action"
echo "\"$stm32programmercli\" $connect_no_reset -psrss  $psrss_value" >> $provisioning
"$stm32programmercli" $connect_no_reset -psrss  $psrss_value >> $provisioning
# In the final product state, the connection with the board is lost and the return value of the command cannot be verified
echo

# ============================================================= End functions =============================================================
# All the steps to set the STM32H7RS product were executed correctly
echo "====="
echo "===== The board is correctly configured."
echo "====="
$SHELL
exit 0

