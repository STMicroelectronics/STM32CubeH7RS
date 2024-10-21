#!/bin/bash

# Getting the Trusted Package Creator and STM32CubeProgammer CLI path
source ../env.sh

isGeneratedByCubeMX=$PROJECT_GENERATED_BY_CUBEMX

if [ $# -ge 1 ]; then mode=$1; else mode=MANUAL; fi

# Environment variable for AppliCfg
project_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ "$isGeneratedByCubeMX" == "true" ]; then
  appli_dir="$oemirot_boot_path_project"
  stirot_oemurot_boot_path_project="$cube_fw_path/Projects/STM32H7S78-DK/Applications/ROT/OEMiROT_Boot"
  flash_layout="$cube_fw_path/Projects/STM32H7S78-DK/Applications/ROT/OEMiROT_Boot/Inc/flash_layout.h"
  boot_hal_cfg="$cube_fw_path/Projects/STM32H7S78-DK/Applications/ROT/OEMiROT_Boot/Inc/boot_hal_cfg.h"
else
  appli_dir="../../$oemirot_boot_path_project"
  stirot_oemurot_boot_path_project="$project_dir/../../Applications/ROT/OEMiROT_Boot"
  flash_layout="$stirot_oemurot_boot_path_project/Inc/flash_layout.h"
  boot_hal_cfg="$stirot_oemurot_boot_path_project/Inc/boot_hal_cfg.h"
fi

# Environment variables
stirot_config="$project_dir/Config/STiROT_Config.xml"
stirot_oemurot_appli="rot_enc_sign.hex"

# Default product state variables
default_state_config="./Config/default_state.txt"
default_product_state_str="default_product_state"
default_dbgauth_str="default_dbgauth"
product_state_list="CLOSED LOCKED"
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
update_ob_setup="./update_ob_setup.sh"
update_STiROT_iLoader_setup="./update_stirot_iloader_setup.sh"
update_oemurot_appli_setup="./update_oemurot_appli_setup.sh"

# Provisioning
ob_flash_log="ob_flash_programming.log"
ob_key_provisioning_log="obkey_provisioning.log"
ob_update_ob_log="update_ob_setup.log"
update_STiROT_iLoader_log="update_stirot_iloader_setup.log"
update_oemurot_appli_log="update_oemurot_appli_setup.log"
provisioning_log="provisioning.log"

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
  if [[ "${product_state^^}" == "CLOSED" || "${product_state^^}" == "LOCKED" ]]; then
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

  if [[ "${product_state_selected^^}" == "LOCKED" ]]; then
    ps_update
  elif [[ "${product_state_selected^^}" == "CLOSED" ]]; then
    dbg_auth_update
  else
    echo "       WRONG product state selected"
    step_error
    return 1
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
        fi
      else
        read -p "       [ CERTIFICATE | PASSWORD ]: " dbgauth
      fi
      dbgauth=$(echo $dbgauth | tr '[:lower:]' '[:upper:]')
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
  if [[ "${product_state_selected^^}" == "LOCKED" ]]; then
    da_file=DA_ConfigLock
  elif [[ "${product_state_selected^^}" == "CLOSED" ]]; then
    if [[ "${dbgauth_selected^^}" == "CERTIFICATE" ]]; then
      da_file=DA_Config
    elif [[ "${dbgauth_selected^^}" == "PASSWORD" ]]; then
      da_file=DA_ConfigWithPassword
    else
      echo "       WRONG debug authentication selected"
      step_error
      return 1
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
  if [ $? -ne 0 ]; then step_error; return 1; fi
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
  source "$ob_key_provisioning" AUTO > $ob_key_provisioning_log 2>&1
  obkey_prog_error=$?

  if [ $obkey_prog_error -ne 0 ]; then step_error; return 1; fi

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

  if [ $? -ne 0 ]; then step_error; return 1; fi

  echo "       Successful option bytes programming and images flashing"
  echo "       (see $ob_flash_log for details)"
  echo

  action="Setting the iROT_SELECT to STiROT"
  echo "   * $action"
  ob_add=0x5200220c
  ob_mask=0xff000000
  ob_pos=0x18
  ob_value=0xb4
  "$stm32programmercli" $connect_no_reset -obrss $ob_add $ob_mask $ob_value $ob_pos > $current_log_file
  ob_sel_error=$?
  if [ $ob_sel_error -ne 0 ]; then step_error; return 1; fi

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
echo "===== Provisioning of STiROT_OEMuROT boot path"
echo "===== Application selected through env.sh: $appli_dir"
echo "===== Product state must be Open. Execute  /ROT_Provisioning/DA/regression.sh if not the case."
echo "===== DTCM/ITCM_AXI_SHARE is going to be forced to 0 for the provisioning steps"
echo "===== (if needed uncomment DTCM/ITCM_AXI_SHARE lines with the wished value in the ob_flash_programming script)."
echo "====="
echo

# ================================================= STM32H7S product preparation ==============================================
# ================================================= Final product state selection =============================================
echo "Step 0: Preliminary stage"
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
    if [[ "${default_product_state^^}" == "CLOSED" ]]; then
      read -p "       [ CLOSED* | LOCKED ]: " product_state
      product_state=${product_state}
    elif [[ "${default_product_state^^}" == "LOCKED" ]]; then
      read -p "       [ CLOSED | LOCKED* ]: " product_state
      product_state=${product_state}
    else
      read -p "       [ CLOSED | LOCKED ]: " product_state
    fi
  else
    read -p "       [ CLOSED | LOCKED ]: " product_state
  fi
  product_state=$(echo $product_state | tr '[:lower:]' '[:upper:]')
else
  product_state=""
fi
ps_selection

# =============================================== Steps to configure the STiROT_iLoader =======================================
echo
echo "   * STiROT_iLoader firmware generation:"
echo "       Open the STiROT_iLoader project with your preferred toolchain (Default path is ../Applications/ROT/STiROT_iLoader)"
echo "       Rebuild all files."
echo "       Press any key to continue..."
if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
echo

# ====================================== Steps to create the STiROT_Config.obk file ===========================================
echo "Step 1: Configuration management"
echo
current_log_file=$update_STiROT_iLoader_log
echo "   * STiROT_Config.obk generation:"
echo "       * Pre-update with STiROT_iLoader dependencies"
command="source $update_STiROT_iLoader_setup AUTO"
$command > $current_log_file 2>&1
ob_update_STiROT_iLoader_error=$?
if [ $ob_update_STiROT_iLoader_error -ne 0 ]; then step_error; return 1; fi
"$stm32tpccli" -obk "$stirot_config" >> $current_log_file

echo "         Option bytes successfully updated according to STiROT_iLoader dependencies"
echo "         (see $current_log_file for details)"
echo

# bypass this step when Generated By CubeMX
if [ "$isGeneratedByCubeMX" == "false" ]; then
  echo "       * From TrustedPackageCreator (OBkey tab in Security panel)"
  echo "         Select STiROT_Config.xml (Default path is (Default path is /ROT_Provisioning/STiROT_OEMuROT/Config/STiROT_Config.xml)"
  echo "         Warning: Default keys must NOT be used in a product. Make sure to regenerate your own keys!"
  echo "         Update the configuration (if/as needed) then generate STiROT_Config.obk file"
  echo "         Press any key to continue..."
  if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi

  # =========================================== Steps to create the DA_Config.obk file =========================================
  echo
  echo "   * DA_Config.obk generation:"
  echo "       From TrustedPackageCreator (OBkey tab in Security panel)"
  echo "       Select DA_Config.xml(Default path is /ROT_Provisioning/DA/Config/$da_file.xml)"
  echo "       Update the configuration (if/as needed) then generate $da_file.obk file"
  echo "       Press any key to continue..."
  if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi

fi

# ============================================= Updating the STiROT Option bytes =============================================
echo
current_log_file=$ob_update_ob_log
action="update of ob_flash_programming script ..."
command="source $update_ob_setup AUTO"
echo "   * $action"
$command > $ob_update_ob_log 2>&1
ob_update_ob_error=$?
if [ $ob_update_ob_error -ne 0 ]; then step_error; return 1; fi

echo "       Option bytes successfully updated according to STiROT_Config.xml"
echo "       (see $ob_update_ob_log for details)"

# =============================================== Steps to configure the OEMuROT ===============================================
echo
echo "Step 2: Image generations"
# bypass this step when Generated By CubeMX
if [ "$isGeneratedByCubeMX" == "false" ]; then
  # Create the OEMuROT_Config.obk file
  echo
  echo "   * OEMuROT_Config.obk generation:"
  echo "       From TrustedPackageCreator ^(OBkey tab in Security panel^)."
  echo "       Select OEMuROT_Config.xml^(Default path is /ROT_Provisioning/STiROT_OEMuROT/Config/OEMuROT_Config.xml^)"
  echo "       Warning: Default keys must NOT be used in a product. Make sure to regenerate your own keys!"
  echo "       Run Keygen script to randomly regenerate your own keys (Keys/*.pem)"
  echo "       Update the configuration (if/as needed) then generate OEMuROT_Config.obk file"
  echo "       Press any key to continue..."
  if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
fi

echo
action="Configure OEMIROT_Boot project as OEMuROT..."
echo "   * $action"
$python"$AppliCfg" setdefine -a uncomment -n OEMUROT_ENABLE -v 1 "$flash_layout" >> $provisioning_log 2>&1
if [ $? -ne 0 ]; then step_error; return 1; fi
echo

echo "   * Boot firmware image generation:"
echo "       Open the OEMiROT_Boot project with your preferred toolchain"
echo "       Rebuild all files. The rot_enc_sign.hex file is generated with the postbuild command."
echo "       STiROT_Config.xml and OEMuROT_Code_Image.xml are updated with OEMuROT dependencies with the postbuild command"
echo "       Press any key to continue..."
if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi

# ============================================= Updating OEMuROT Application files ==============================================
echo
current_log_file=$update_oemurot_appli_log
command="source $update_oemurot_appli_setup AUTO"
# must be bypassed when generated by CubeMX
if [ "$isGeneratedByCubeMX" == "false" ]; then
  action="OEMuROT_Code_Image.xml update ..."
  echo "   * $action"

  $command > $update_oemurot_appli_log 2>&1
  if [ $? -ne 0 ]; then step_error; return 1; fi

  echo "       ^(OEMuROT_Code_Image.xml successfully updated^)"
  echo "       ^(see $update_oemurot_appli_log for details^)"
fi

# ============================================= OEMuROT application image generation step =====================================
echo
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

# ====================================================== Board provisioning steps ==============================================
echo "Step 3: Provisioning"
echo

# ================================================== Force DTCM_AXI_SHARE to 0 =================================================
action="Force DTCM/ITCM_AXI_SHARE to 0..."
current_log_file=$provisioning_log
echo "   * $action"
"$stm32programmercli" $connect_no_reset -ob DTCM_AXI_SHARE=0 ITCM_AXI_SHARE=0 > $current_log_file
if [ $? -ne 0 ]; then step_error; return 1; fi
echo "       Successful"
echo

# ================================================ Final product state selection =========================================================
if [[ "${product_state_selected^^}" == "CLOSED" ]]; then
  psrss_value=0x72
  set_provisionning_ps

elif [[ "${product_state_selected^^}" == "LOCKED" ]]; then
  psrss_value=0x5C
  set_provisionning_ps

else
  echo "       WRONG product state selected"
  step_error
  return 1
fi

exit 0
