#!/bin/bash

# Call env.sh script to set up environment variables
source ../env.sh

project_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

script_error_file="error"

if [ $# -ge 1 ]; then script_mode=$1; else script_mode=MANUAL; fi

# Set connection settings for programmer tool
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=UR"

# Set directory where application code is located
if [ "$isGeneratedByCubeMX" == "true" ]; then
    appli_dir="$stirot_boot_path_project"
    iloader_dir="$stirot_iloader_boot_path_project"
    # Set path to STiROT_iLoader.bin file
    STiROT_iLoader_bin="$iloader_dir/Binary/STiROT_iLoader.bin"
else
    appli_dir="../../$stirot_boot_path_project"
    iloader_dir="../../$stirot_iloader_boot_path_project"
    # Set path to STiROT_iLoader.bin file
    STiROT_iLoader_bin="$project_dir/$iloader_dir/Binary/STiROT_iLoader.bin"
fi


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
# HDP_AREA_START
"$stm32programmercli" $connect_reset > /dev/null
ob_add="0x52002230"
ob_mask="0x000003ff"
ob_pos="0x00"
command="\"$stm32programmercli\" $connect_no_reset -obrss $ob_add $ob_mask $hdp_area_start $ob_pos"
echo "$command"
eval "$command"
if [ $? -ne 0 ]; then error; return 1; fi

# HDP_AREA_END
"$stm32programmercli" $connect_reset > /dev/null
ob_add="0x52002230"
ob_mask="0x000003ff"
ob_pos="0x10"
command="\"$stm32programmercli\" $connect_no_reset -obrss $ob_add $ob_mask $hdp_area_end $ob_pos"
echo "$command"
eval "$command"
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


# ================================================= Download image ===========================================================
action="Program STiROT application image in download slot"
echo $action

if [ ! -e "$project_dir/$appli_dir/Binary/$stirot_appli" ]; then
    echo -e [31mError: appli_enc_sign.hex does not exist! use TPC to generate it [0m
    error
    return 1
fi

echo "\"$stm32programmercli\" -c port=$com_port br=921600 $stm32ExtLoaderFlash -d \"$project_dir/$appli_dir/Binary/$stirot_appli\" --verify"
"$stm32programmercli" -c port=$com_port br=921600 $stm32ExtLoaderFlash -d "$project_dir/$appli_dir/Binary/$stirot_appli" --verify
if [ $? -ne 0 ]; then error; return 1; fi

# ==================================================== DTCM/ITCM_AXI_SHARE ====================================================================

# Place to change DTCM/ITCM_AXI_SHARE configurations if there is a need
#"$stm32programmercli" $connect_no_reset -ob DTCM_AXI_SHARE=0 ITCM_AXI_SHARE=0

echo "Programming success"
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
return 0