#!/bin/bash -
source ../env.sh

# Environment variable for setting postbuild command with AppliCfg
project_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ $# -ge 1 ]; then script_mode=$1; else script_mode=MANUAL; fi

script_error_file="error"

# Environment variable for setting postbuild command with AppliCfg
if [ "$isGeneratedByCubeMX" == "true" ]; then
  iloader_dir="$cube_fw_path/Projects/STM32H7S78-DK/Applications/ROT/STiROT_iLoader"
else
  iloader_dir="../../$stirot_iloader_boot_path_project"
fi

error()
{
  echo "        Error when trying to $action" >> $script_error_file
  echo "        Programming aborted" >> $script_error_file
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}

# External script
ob_flash_programming="$project_dir/ob_flash_programming.sh"

obk_cfg_file="$project_dir/Config/STiROT_Config.xml"

# General section need
STiROT_iLoader_offset_str='STiROT_iLoader offset'
code_align=8192
if [ "$isGeneratedByCubeMX" == "true" ]; then
  STiROT_iLoader_bin="$iloader_dir/Binary/STiROT_iLoader.bin"
else
  STiROT_iLoader_bin="$project_dir/$iloader_dir/Binary/STiROT_iLoader.bin"
fi

# STM32CubeIDE linker
if [ "$isGeneratedByCubeMX" == "true" ]; then
  STiROT_iLoader_ld_file="$iloader_dir/STM32CubeIDE/STM32H7S7L8HXH_FLASH.ld"
else
  STiROT_iLoader_ld_file="$project_dir/$iloader_dir/STM32CubeIDE/STM32H7S7L8HXH_FLASH.ld"
fi

# Get the IDE configuration
active_ide="$(head -n 1 "$iloader_dir/Binary/_ide")"

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

# ================================================ Updating STiROT_iLoader files ========================================================
echo "Updating the STiROT_iLoader files..."
action="Update the STiROT_iLoader files"
if [ ! -f "$STiROT_iLoader_bin" ]; then
    echo "Error: STiROT_iLoader binary does not exist! use TPC to generate it"
    error
    return 1
fi

STiROT_iLoader_bin_size="$(stat -c %s "$STiROT_iLoader_bin")"

action="Update STiROT_Config.xml input binary file"

# get the STiROT_iLoader size aligned on the sector size
if [ $((STiROT_iLoader_bin_size % code_align)) -ne 0 ]; then
    STiROT_iLoader_bin_size=$(( (STiROT_iLoader_bin_size / code_align + 1) * code_align ))
fi

# Update the STiROT_iLoader size in the STiROT configuration
$python"$AppliCfg" xmlval -nxml_el_idx "STiROT configuration" -n "STiROT_iLoader size" --value $STiROT_iLoader_bin_size --vb "$obk_cfg_file"
if [ $? -ne 0 ]; then error; return 1; fi

if [ "$active_ide" != "STM32CubeIDE" ]; then
    echo "The current toolchain is not supported: $active_ide"
    error
    return 1
fi

action="Update STiRoT configuration related to the STiROT_iLoader"
# Update the STiROT_iLoader size in the STiROT configuration
$python"$AppliCfg" xmlval -l "$STiROT_iLoader_ld_file" -m "CODE_OFFSET" -n "STiROT_iLoader offset" --vb "$obk_cfg_file"
if [ $? -ne 0 ]; then error; return 1; fi


# Update the STiROT_iLoader address in ob flash programming script
action="Update STiROT_Config.xml"
$python"$AppliCfg" setob -l "$STiROT_iLoader_ld_file" -b "OEMiLoader_address" -ms "CODE_OFFSET" -me "CODE_SIZE" -msec "CODE_SIZE" -d "0x2000" --vb "$ob_flash_programming"
if [ $? -ne 0 ]; then error; return 1; fi

echo "STiROT_Config.xml, ob_flash_programming.sh and STM32H7S7L8HXH_FLASH.ld successfully updated"
return 0
