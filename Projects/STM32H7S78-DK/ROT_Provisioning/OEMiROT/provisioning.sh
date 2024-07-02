#!/bin/bash

# Getting the Trusted Package Creator and STM32CubeProgammer CLI path
source ../env.sh

# Select device type (H7S or H7R)
device_type="H7S"

isGeneratedByCubeMX=$PROJECT_GENERATED_BY_CUBEMX

if [ $# -ge 1 ]; then mode=$1; else mode=MANUAL; fi

# Environment variable for AppliCfg
project_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ "$isGeneratedByCubeMX" == "true" ]; then
  appli_dir=$oemirot_boot_path_project
  flash_layout="$cube_fw_path/Projects/STM32H7S78-DK/Applications/ROT/OEMiROT_Boot/Inc/flash_layout.h"
  boot_hal_cfg="$cube_fw_path/Projects/STM32H7S78-DK/Applications/ROT/OEMiROT_Boot/Inc/boot_hal_cfg.h"
else
  appli_dir="../../../$oemirot_boot_path_project"
  flash_layout="$project_dir/../../Applications/ROT/OEMiROT_Boot/Inc/flash_layout.h"
  boot_hal_cfg="$project_dir/../../Applications/ROT/OEMiROT_Boot/Inc/boot_hal_cfg.h"
fi


# Default product state variables
default_state_config="./Config/default_state.txt"
default_product_state_str="default_product_state"
default_dbgauth_str="default_dbgauth"
product_state_list="OPEN CLOSED LOCKED"
dbgauth_list="CERTIFICATE PASSWORD"

# AppliCfg path
AppliCfg="$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/dist/AppliCfg.exe"
uname | grep -i -e windows -e mingw
if [ $? == 0 ] && [ -e "$AppliCfg" ]; then
  #line for window executable
  echo "AppliCfg with windows executable"
  python=""
else
  #line for python
  echo "AppliCfg with python script"
  AppliCfg="$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/AppliCfg.py"
  #determine/check python version command
  python="python3 "
fi

# External scripts
ob_flash_programming="./ob_flash_programming.sh"
ob_key_provisioning="./obkey_provisioning.sh"
ob_key_provisioning_open="./obkey_provisioning_open.sh"

# Variables for image xml configuration
fw_in_bin="Firmware binary input file"
fw_out_bin="Image output file"
app_bin="$appli_dir/Binary/rot_app.bin"
app_bin="${app_bin//\\/\/}"
app_enc_sign_hex="$appli_dir/Binary/rot_app_enc_sign.hex"
app_enc_sign_hex="${app_enc_sign_hex//\\/\/}"
oemirot_config_file="$project_dir/Config/OEMiROT_Config.xml"
code_image_file="$project_dir/Images/OEMiROT_Code_Image.xml"

# Provisioning
ob_flash_log="ob_flash_programming.log"
ob_key_provisioning_log="obkey_provisioning.log"
provisioning_log="provisioning.log"
if [ -f $ob_flash_log ]; then
  rm $ob_flash_log
fi
if [ -f $provisioning_log ]; then
  rm $provisioning_log
fi

# Initial configuration
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=UR"
# Define principal log file
current_log_file=$provisioning_log

# ============================================== Script functions ============================================================
# ===================================== Final product state selection ========================================================

ps_selection()
{
  # Select product state

  if [[ "${product_state^^}" == "OPEN" || "${product_state^^}" == "CLOSED" || "${product_state^^}" == "LOCKED" ]]; then
    product_state_selected=$product_state
  else
    product_state_selected=$default_product_state
    echo "       Default product state is going to be applied"
  fi

  for i in $product_state_list; do
    if [ "${i^^}" = "${product_state^^}" ]; then
        product_state_selected="$product_state"
        break
    fi
  done

  if [[ "${product_state_selected^^}" == "OPEN" || "${product_state_selected^^}" == "LOCKED" ]]; then
    ps_update
  elif [[ "${product_state_selected^^}" == "CLOSED" ]]; then
    dbg_auth_update
  else
    echo "       WRONG product state selected"
    step_error
  fi
  return
}

dbg_auth_update()
{
  if [[ "${product_state_selected^^}" == "CLOSED" ]]; then
    echo
    echo "   * Define Debug Authentication method (* default choice)"
    if [ "$mode" != "AUTO" ]; then
      if [ $default_ps_enabled == 1 ]; then
        if [[ "${default_dbgauth^^}" == "CERTIFICATE" ]]; then
          read -p "       [ CERTIFICATE* | PASSWORD ]: " dbgauth
          dbgauth=${dbgauth}
        elif [[ "${default_dbgauth^^}" == "PASSWORD" ]]; then
          read -p "       [ CERTIFICATE | PASSWORD* ]: " dbgauth
          dbgauth=${dbgauth}
        else
          read -p "       [ CERTIFICATE | PASSWORD ]: " dbgauth
          dbgauth=${dbgauth}
        fi
      else
        read -p "       [ CERTIFICATE | PASSWORD ]: " dbgauth
        dbgauth=${dbgauth}
      fi
      dbgauth=$(echo $dbgauth | tr '[:lower:]' '[:upper:]')
      dbgauth=$(echo "$dbgauth" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')
    else
      dbgauth=""
    fi
    dbgauth_selection
  fi
  return
}

dbgauth_selection()
{
  if [[ "${product_state_selected^^}" == "CLOSED" ]]; then
    if [[ "${dbgauth^^}" == "CERTIFICATE" ]]; then
      dbgauth_selected="CERTIFICATE"
    elif [[ "${dbgauth^^}" == "PASSWORD" ]]; then
      dbgauth_selected="PASSWORD"
    else
      dbgauth_selected=$default_dbgauth
      echo "       Default Debug Authentication is going to be applied"
    fi
    ps_update
  fi
  return
}

# =============================================== Updating the Product state =================================================
ps_update()
{
  if [[ "${product_state_selected^^}" == "OPEN" || "${product_state_selected^^}" == "LOCKED" ]]; then
    da_file=DA_ConfigLock
  elif [[ "${product_state_selected^^}" == "CLOSED" ]]; then
    if [[ "${dbgauth_selected^^}" == "CERTIFICATE" ]]; then
      da_file=DA_Config
    elif [[ "${dbgauth_selected^^}" == "PASSWORD" ]]; then
      da_file=DA_ConfigWithPassword
    else
      echo "       WRONG debug authentication selected"
      step_error
    fi
  fi
  return
}

# =================================== Product State configuration and Provisioning steps =====================================
# Provisioning execution
set_provisionning_ps()
{
  current_log_file=$provisioning_log
  action="Set the product state PROVISIONING"
  echo "   * $action"
  echo
  "$stm32programmercli" $connect_no_reset -psrss 0x17 >> $provisioning_log
  if [ $? -ne 0 ]; then step_error; fi
  provisioning_step
  return
}

# Set the final product state of the STM32H7S product
set_final_ps()
{
  current_log_file=$provisioning_log
  action="Set the final product state $product_state_selected"
  echo "   * $action"
  echo
  "$stm32programmercli" $connect_reset > /dev/null
  "$stm32programmercli" $connect_no_reset -psrss  $psrss_value >> $provisioning_log
  echo
  final_execution
  return
}

# Provisioning the obk files step
provisioning_step()
{
  current_log_file=$ob_key_provisioning_log
  action="Provision the .obk files ..."
  echo "   * $action"
  if [[ "${product_state_selected^^}" == "OPEN" ]]; then
    source "./$ob_key_provisioning_open" AUTO > $ob_key_provisioning_log 2>&1
    obkey_prog_error=$?
  else
    source "./$ob_key_provisioning" AUTO > $ob_key_provisioning_log 2>&1
    obkey_prog_error=$?
  fi
  if [ $obkey_prog_error -ne 0 ]; then step_error; fi

  echo "       Successful obk provisioning"
  echo "       (see $ob_key_provisioning_log for details)"
  echo
  ob_flash_programming
  return
}

# =========================================== Option Bytes and flash programming ==============================================
ob_flash_programming()
{
  action="Program the option bytes and flashing the image ..."
  current_log_file=$ob_flash_log
  echo "   * $action"
  source "$ob_flash_programming" AUTO $product_state_selected > $ob_flash_log

  if [ $? -ne 0 ]; then step_error; fi

  echo "       Successful option bytes programming and images flashing"
  echo "       (see $ob_flash_log for details)"
  echo

  if [[ "${product_state_selected^^}" == "OPEN" ]]; then final_execution; fi
  set_final_ps
  return
}

# ========================================================= End functions ====================================================
# All the steps to set the STM32H7S product were executed correctly
final_execution()
{
  echo "====="
  echo "===== The board is correctly configured."
  if [ "$isGeneratedByCubeMX" == "false" ]; then
    echo "===== Connect UART console (115200 baudrate) to get application menu."
  fi
  echo "===== Reset the board to start the application."
  echo "====="
  if [ "$mode" != "AUTO" ]; then $SHELL; fi
  exit 0
}

# Error when external script is executed
step_error()
{
  #log error file
  if [ -e "error" ]; then
    cat error
    rm error
  fi
  echo
  echo "====="
  echo "===== Error while executing \"$action\"."
  echo "===== See $current_log_file for details. Then try again."
  echo "====="
  if [ "$mode" != "AUTO" ]; then $SHELL; fi
  exit 1
}

# =========================================================== Script start  ==================================================

# ====================================================== Initial instructions ================================================
echo "====="
echo "===== Provisioning of OEMiROT boot path"
echo "===== Application selected through env.sh: $oemirot_boot_path_project"
echo "===== Product state must be Open. Execute  /ROT_Provisioning/DA/regression.sh if not the case."
echo "===== DTCM/ITCM_AXI_SHARE is going to be forced to 0 for the provisioning steps"
echo "===== (if needed uncomment DTCM/ITCM_AXI_SHARE lines with the wished value in the ob_flash_programming script)."
echo "====="
echo

# bypass this step when Generated By CubeMX
if [ "$isGeneratedByCubeMX" == "false" ]; then
  # Verify the project name configuration
  if [[ ! "${oemirot_boot_path_project,,}" == *oemirot_appli* ]]; then
    echo "====="
    echo "===== Wrong Boot path: $oemirot_boot_path_project"
    echo "===== please modify the env.sh to set the right path"
    step_error
  fi
fi

# ================================================= STM32H7S product preparation ==============================================
# ================================================= Final product state selection =============================================
echo "Step 1: Preliminary stage"
echo
# get the default product state & debug authentication value from the default state file
if [ -f $default_state_config ]; then
  default_ps_enabled=1

  # READ default product state configuration file
  default_product_state=$(grep -i "$default_product_state_str" "$default_state_config" | cut -d'=' -f2)
  default_dbgauth=$(grep -i "$default_dbgauth_str" "$default_state_config" | cut -d'=' -f2)
else
  default_ps_enabled=0
fi
echo "   * Define product state value (* default choice)"
if [ "$mode" != "AUTO" ]; then
  if [ $default_ps_enabled == 1 ]; then
    if [[ "${default_product_state^^}" == "OPEN" ]]; then
      read -p "       [ OPEN* | CLOSED | LOCKED ]: " product_state
      product_state=${product_state}
    elif [[ "${default_product_state^^}" == "CLOSED" ]]; then
      read -p "       [ OPEN | CLOSED* | LOCKED ]: " product_state
      product_state=${product_state}
    elif [[ "${default_product_state^^}" == "LOCKED" ]]; then
      read -p "       [ OPEN | CLOSED | LOCKED* ]: " product_state
      product_state=${product_state}
    else
      read -p "       [ OPEN | CLOSED | LOCKED ]: " product_state
    fi
  else
    read -p "       [ OPEN | CLOSED | LOCKED ]: " product_state
  fi
  product_state=$(echo $product_state | tr '[:lower:]' '[:upper:]')
else
  product_state=""
fi
ps_selection

# =============================================== Steps to create the OEMiROT_Config.obk file ==============================================
echo
echo "Step 2 : Configuration management"
echo

# bypass this step when Generated By CubeMX
if [ "$isGeneratedByCubeMX" == "false" ]; then
  echo "   * OEMiROT_Config.obk generation:"
  echo "       From TrustedPackageCreator ^(OBkey tab in Security panel^)."
  echo "       Select OEMiROT_Config.xml^(Default path is /ROT_Provisioning/OEMiROT/Config/OEMiROT_Config.xml^)"
  echo "       Warning: Default keys must NOT be used in a product. Make sure to regenerate your own keys!"
  echo "       Run Keygen script to randomly regenerate your own keys (Keys/*.pem)"
  echo "       Update the configuration (if/as needed) then generate OEMiROT_Config.obk file"
  echo "       Press any key to continue..."
  echo
  if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
fi

if [[ "$device_type" == "H7S" ]]; then
    if [[ "${product_state_selected^^}" == "OPEN" ]]; then
        action="Configure OEMiROT_Config.xml for OPEN"
        echo "   * $action"
        $python"$AppliCfg" xmlval --value 0 --decimal -txml DoEncryption "$oemirot_config_file" >> $provisioning_log 2>&1
        if [ $? -ne 0 ]; then step_error; fi
    else
        action="Configure OEMiROT_Config.xml for CLOSED or LOCKED"
        echo "   * $action"
        $python"$AppliCfg" xmlval --value 1 --decimal -txml DoEncryption "$oemirot_config_file" >> $provisioning_log 2>&1
        if [ $? -ne 0 ]; then step_error; fi
    fi
else
    $python"$AppliCfg" xmlval --value 0 --decimal -txml DoEncryption "$oemirot_config_file" >> $provisioning_log 2>&1
fi
if [ $? -ne 0 ]; then step_error; fi

action="Force the update of OEMiROT_Config.obk"
echo "   * $action"
echo
"$stm32tpccli" -obk "$oemirot_config_file" AUTO >> $provisioning_log 2>&1
if [ $? -ne 0 ]; then step_error; fi

# bypass this step when Generated By CubeMX
if [ "$isGeneratedByCubeMX" == "false" ]; then
  # =============================================== Steps to create the DA_Config.obk file ======================================
  if [[ "${product_state_selected^^}" == "CLOSED" ]]; then
    echo
    echo "   * $da_file.obk generation:"
    echo "       From TrustedPackageCreator ^(OBkey tab in Security panel^)"
    echo "       Select $da_file.xml^(Default path is /ROT_Provisioning/DA/Config/$da_file.xml^)"
    echo "       Update the configuration ^(if/as needed^) then generate $da_file.obk file"
    echo "       Press any key to continue..."
    if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
  fi
fi

# ========================================================= Images generation steps ========================================================
echo
echo "Step 3: Images generation"
echo

# Configure OEMIROT_Boot project as OEMIROT
action="Configure OEMIROT_Boot project as OEMIROT..."
echo "   * $action"
$python"$AppliCfg" setdefine -a comment -n OEMUROT_ENABLE "$flash_layout" >> $provisioning_log 2>&1
if [ $? -ne 0 ]; then step_error; fi

echo
echo "   * Boot firmware image generation"
echo "       Open the OEMiROT_Boot project with preferred toolchain and rebuild all files."
echo "       At this step the project is configured for OEMiROT boot path."
echo "       Press any key to continue..."
echo
if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
echo

# bypass this step when Generated By CubeMX
if [ "$isGeneratedByCubeMX" == "false" ]; then
  action="Configure the Firmware binary input file in OEMiROT_Code_Image.xml..."
  echo "   * $action"
  $python"$AppliCfg" xmlval -v "$app_bin" --string -n "$fw_in_bin" "$code_image_file" >> $provisioning_log
  if [ $? -ne 0 ]; then step_error; fi

  action="Configure the Image output file in OEMiROT_Code_Image.xml..."
  echo "   * $action"
  $python"$AppliCfg" xmlval -v "$app_enc_sign_hex" --string -n "$fw_out_bin" "$code_image_file" >> $provisioning_log
  if [ $? -ne 0 ]; then step_error; fi
fi

echo "   * Code firmware image generation:"
if [ "$isGeneratedByCubeMX" == "false" ]; then
  echo "       Open the OEMiROT_Appli project with your preferred toolchain"
  echo "       Rebuild all files. The appli_enc_sign.hex file is generated with the postbuild command"
else
  echo "       If the configuration of OEMiROT_Boot project has been updated, reload and regenerate STM32CubeMX application project."
  echo "       Open the regenerated application project with preferred toolchain and rebuild all files."
fi
echo "       Press any key to continue..."
if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
echo

# ================================================== Force DTCM/ITCM_AXI_SHARE to 0 =================================================
action="Force DTCM/ITCM_AXI_SHARE to 0..."
current_log_file=$provisioning_log
echo "   * $action"
"$stm32programmercli" $connect_no_reset -ob DTCM_AXI_SHARE=0 ITCM_AXI_SHARE=0 > $current_log_file 2>&1
if [ $? -ne 0 ]; then step_error; return 1; fi
echo "       Successful"
echo

# ================================================ Final product state selection =========================================================
if [[ "${product_state_selected^^}" == "OPEN" ]]; then
  provisioning_step

elif [[ "${product_state_selected^^}" == "CLOSED" ]]; then
  psrss_value=0x72
  set_provisionning_ps

elif [[ "${product_state_selected^^}" == "LOCKED" ]]; then
  psrss_value=0x5C
  set_provisionning_ps

else
  echo "       WRONG product state selected"
  step_error
fi

exit 0
