#!/bin/bash -
source ../env.sh

# Environment variable for setting postbuild command with AppliCfg
project_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ $# -ge 1 ]; then script_mode=$1; else script_mode=MANUAL; fi

script_error_file="error"

error()
{
  echo "        Error when trying to $action" > $script_error_file
  echo "        Programming aborted" >> $script_error_file
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}

# Environment variable for setting postbuild command with AppliCfg
if [ $isGeneratedByCubeMX == "true" ]; then
   appli_dir="$stirot_boot_path_project"
else
   appli_dir="../../$stirot_boot_path_project"
fi

if [ -f "$appli_dir/EWARM/stm32h7s7xx_flash.icf" ]; then
    # Project configuration "Applications"
    appli_subdir=""
    appli_proj_dir="$appli_dir"
else
    if [ -f "$appli_dir/../EWARM/Appli/stm32h7s7xx_flash.icf" ]; then
        # Project configuration "Templates"
        appli_subdir="Appli/"
        appli_proj_dir="$appli_dir/.."
    else
        echo "No valid linker file found"
        error
        exit 1
    fi
fi

# CubeIDE project Appli Full Secure files
ld_file="$appli_proj_dir/STM32CubeIDE/$appli_subdir/STM32H7S7L8HXH_FLASH.ld"

main_h="$appli_dir/Inc/main.h"

# STiRoT xml files
obk_cfg_file="$project_dir/Config/STiROT_Config.xml"

# General section need
code_size="Firmware area size"
code_address="Firmware area address (in internal RAM)"
secure_code_size="Size of the secure area"

# Environment variable for AppliCfg
# Check if Python is installed
python3 --version >/dev/null 2>&1
if [ $? -ne 0 ]; then
  python --version >/dev/null 2>&1
  if [ $? -ne 0 ]; then
  echo "Python installation missing. Refer to Utilities/PC_Software/ROT_AppliConfig/README.md"
  exit 1
  fi
  python="python "
else
  python="python3 "
fi

# Environment variable for AppliCfg
AppliCfg="$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/AppliCfg.py"

# ================================================ Updating test Application files ========================================================

action="Update Linker .ld file"
echo "$action"

$python"$AppliCfg" linker -xml "$obk_cfg_file" -nxml "$code_size" -n "CODE_SIZE" --vb "$ld_file"
if [ $? -ne 0 ]; then error; return 1; fi

$python"$AppliCfg" linker -xml "$obk_cfg_file" -nxml "$code_address" -n "CODE_OFFSET" --vb "$ld_file"
if [ $? -ne 0 ]; then error; return 1; fi

if [ "$appli_subdir" == "" ]; then
    action="Update Code size Define Flag"
    echo "$action"
    $python"$AppliCfg" definevalue -xml "$obk_cfg_file" -nxml "$code_size" -n "CODE_SIZE" --vb "$main_h"
    if [ $? -ne 0 ]; then error; return 1; fi

    $python"$AppliCfg" definevalue -xml "$obk_cfg_file" -nxml "$code_address" -n "CODE_OFFSET" --vb "$main_h"
    if [ $? -ne 0 ]; then error; return 1; fi
fi

#!/bin/bash -
echo "Linker files and main.h successfully updated according to STiROT_Config.obk"