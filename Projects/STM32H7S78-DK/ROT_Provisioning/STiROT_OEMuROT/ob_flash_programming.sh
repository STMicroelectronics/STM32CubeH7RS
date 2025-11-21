#!/bin/bash

# Call env.sh script to set up environment variables
source ../env.sh

project_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

script_error_file="error"

# Handle the arguments
if [ $# -ge 1 ]; then

  # Get the arguments
  arg1=$1
  if [ $# -ge 2 ]; then arg2=$2; else arg2="NA"; fi

  # Set script_mode
  if [ "$arg1" == "AUTO" ] || [ "$arg2" == "AUTO" ]; then
    script_mode="AUTO"
  else
    script_mode="MANUAL"
  fi

  # Set selected_product_state
  if [ "$arg1" == "OPEN" ] || [ "$arg2" == "OPEN" ]; then
    selected_product_state="OPEN"
  else
    selected_product_state="NOT_OPEN"
  fi

else
  script_mode="MANUAL"
  selected_product_state="NOT_OPEN"
fi


# Set connection settings for programmer tool
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=UR"

# Set directory where OEMuROT code is located
if [ "$isGeneratedByCubeMX" == "true" ]; then
   appli_dir=$oemirot_appli_path_project
   iloader_dir="$stirot_iloader_boot_path_project"
   # Set path to STiROT_iLoader.bin file
   STiROT_iLoader_bin="$stirot_iloader_boot_path_project/Binary/STiROT_iLoader.bin"
   OEMuROT_hex="$cube_fw_path/Projects/STM32H7S78-DK/Applications/ROT/OEMiROT_Boot/Binary/OEMiROT_Boot_enc_sign.hex"
else
   appli_dir="../../$oemirot_appli_path_project"
   iloader_dir="../../$stirot_iloader_boot_path_project"
   # Set path to STiROT_iLoader.bin file
   STiROT_iLoader_bin="$project_dir/$iloader_dir/Binary/STiROT_iLoader.bin"
   OEMuROT_hex="$project_dir/../../Applications/ROT/OEMiROT_Boot/Binary/OEMiROT_Boot_enc_sign.hex"
fi

oemurot_appli_hex="$appli_dir/Binary/$oemirot_appli"

# ============================================================= Error handling ==============================================================

error()
{
  echo "        Error when trying to $action" >> $script_error_file
  echo "        Programming aborted" >> $script_error_file
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}

# ============================================ Remove protections and erase the user flash ==================================
erase_all="-e all"
wrps_disable="0xff"
hdp_area_start="0x0ff"
hdp_area_end="0x000"
hdps=$((($hdp_area_start<<0)|($hdp_area_end<<16)))

# =============================================== Configure Option Bytes ====================================================
action="Remove Protection and erase All"
echo "* $action"

# Disable WRP protections
# WRPS
"$stm32programmercli" $connect_reset > /dev/null
ob_add="0x5200221c"
ob_mask="0x000000ff"
ob_pos="0x00"
command="\"$stm32programmercli\" $connect_no_reset -obrss $ob_add $ob_mask $wrps_disable $ob_pos"
echo "$command"
eval "$command"
if [ $? -ne 0 ]; then error; return 1; fi

# Disable HDP protection on user flash
"$stm32programmercli" $connect_reset >> /dev/null
ob_add=0x52002234
ob_mask=0x03ff03ff
ob_pos=0x00
command="\"$stm32programmercli\" $connect_no_reset -obrss $ob_add $ob_mask $hdps $ob_pos"
echo "$command"
eval "$command"

action="Set XSPI1_HSLV configuration (needed to use external memories)"
echo $action
"$stm32programmercli" $connect_no_reset -ob XSPI1_HSLV=1
if [ $? -ne 0 ]; then error; return 1; fi

action="Set XSPI2_HSLV configuration (needed to use external memories)"
echo $action
"$stm32programmercli" $connect_no_reset -ob XSPI2_HSLV=1
if [ $? -ne 0 ]; then error; return 1; fi

"$stm32programmercli" $connect_reset > /dev/null
"$stm32programmercli" $connect_no_reset $erase_all
if [ $? -ne 0 ]; then error; return 1; fi

action="Erase external non volatile primary code slot"
echo "$action"
ext_nvm_primary_code_start=0
ext_nvm_primary_code_stop=1
echo "\"$stm32programmercli\" -c port=$com_port br=921600 $stm32ExtLoaderFlash -e [$ext_nvm_primary_code_start $ext_nvm_primary_code_stop]"
"$stm32programmercli" -c port=$com_port br=921600 $stm32ExtLoaderFlash -e [$ext_nvm_primary_code_start $ext_nvm_primary_code_stop]
if [ $? -ne 0 ]; then error; return 1; fi

action="Set SRAM ECC configuration"
echo "$action"
# Recommended configuration for secure boot is :
#   - SRAM ECC activated. Hack tentative detection enabled ==> ECC_ON_SRAM=1
"$stm32programmercli" $connect_no_reset -ob ECC_ON_SRAM=1
if [ $? -ne 0 ]; then error; return 1; fi

# Download STiROT_iLoader
action="Download STiROT_iLoader"
echo "$action"
if [ ! -f "$STiROT_iLoader_bin" ]; then
    echo "Error: $STiROT_iLoader_bin does not exist!"
    error
    return 1
fi
OEMiLoader_address=0x8000000
command="\"$stm32programmercli\" $connect_no_reset -d \"$STiROT_iLoader_bin\" $OEMiLoader_address --verify"
echo "$command"
eval "$command"
if [ $? -ne 0 ]; then error; return 1; fi

action="Define write protected area"
echo "$action"
# This configuration depends on user mapping but the watermarks should cover at least the STiROT iloader.
wrps0=0
wrps1=0
wrps2=0
wrps3=1
wrps4=1
wrps5=1
wrps6=1
wrps7=1
wrps=$((wrps0<<0|wrps1<<1|wrps2<<2|wrps3<<3|wrps4<<4|wrps5<<5|wrps6<<6|wrps7<<7))

action="Set WRP on STiROT-iLoader"
echo "* $action"

"$stm32programmercli" $connect_reset > /dev/null

# WRPS
ob_add="0x5200221c"
ob_mask="0x000000ff"
ob_pos="0x00"
command="\"$stm32programmercli\" $connect_no_reset -obrss $ob_add $ob_mask $wrps $ob_pos"
echo "$command"
eval "$command"
if [ $? -ne 0 ]; then error; return 1; fi

"$stm32programmercli" $connect_reset > /dev/null


# ==================================================== Download OEMuROT ====================================================================
action="Program STiROT OEMuROT image in download slot"
echo $action

if [ ! -e "$OEMuROT_hex" ]; then
    echo -e [31mError: OEMiROT_Boot_enc_sign.hex does not exist! use TPC to generate it [0m
    error
    return 1
fi

echo "\"$stm32programmercli\" -c port=$com_port br=921600 $stm32ExtLoaderFlash -d \"$OEMuROT_hex\" --verify"
"$stm32programmercli" -c port=$com_port br=921600 $stm32ExtLoaderFlash -d "$OEMuROT_hex" --verify
if [ $? -ne 0 ]; then error; return 1; fi

# ==================================================== Download images ====================================================================
action="OEMuROT application image programming in download slots"
echo $action

action="Write OEMiROT_Appli Code"
echo $action

if [ ! -e "$oemurot_appli_hex" ]; then
    echo -e [31mError: $oemurot_appli_hex does not exist! use TPC to generate it[0m
    error
    return 1
fi
echo "\"$stm32programmercli\" -c port=$com_port br=921600 $stm32ExtLoaderFlash -d \"$oemurot_appli_hex\" --verify"
"$stm32programmercli" -c port=$com_port br=921600 $stm32ExtLoaderFlash -d "$oemurot_appli_hex" --verify
if [ $? -ne 0 ]; then error; return 1; fi

echo "Appli Written"

# ==================================================== DTCM/ITCM_AXI_SHARE ====================================================================

# Place to change DTCM/ITCM_AXI_SHARE configurations if there is a need
#"$stm32programmercli" $connect_no_reset -ob DTCM_AXI_SHARE=0 ITCM_AXI_SHARE=0

echo "Programming success"
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
return 0