#!/bin/bash -
source ../env.sh

# Environment variable for AppliCfg
project_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ "$isGeneratedByCubeMX" == "true" ]; then
  iloader_dir="$cube_fw_path/Projects/STM32H7S78-DK/Applications/ROT/STiROT_iLoader"
  echo $iloader_dir
else
  iloader_dir="../../$stirot_iloader_boot_path_project"
fi

if [ $# -ge 1 ]; then script_mode=$1; else script_mode=MANUAL; fi

# Path and input files
ob_flash_programming="$project_dir/ob_flash_programming.sh"
obk_cfg_file="$project_dir/Config/STiROT_Config.xml"

# xml item nem contents
# This will allow to recover information from xml file
STiROT_iLoader_offset="STiROT_iLoader offset"
STiROT_iLoader_size="STiROT_iLoader size"
code_size="Firmware area size"
data_size="Data download slot size"
data_image_en="Number of images managed"
sram_ecc="SRAM ECC management activation"

# STM32CubeIDE linker
if [ "$isGeneratedByCubeMX" == "true" ]; then
  STiROT_iLoader_ld_file="$iloader_dir/STM32CubeIDE/STM32H7S7L8HXH_FLASH.ld"
else
  STiROT_iLoader_ld_file="$project_dir/$iloader_dir/STM32CubeIDE/STM32H7S7L8HXH_FLASH.ld"
fi

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

# ======================================================= Updating the Option bytes =======================================================
echo Updating the ob_flash_programming script...

action="Update Write protection values"
echo "$python "$AppliCfg" setob --layout "$STiROT_iLoader_ld_file" -b wrps -ms "CODE_OFFSET" -me "CODE_SIZE" -msec "CODE_SIZE" -d "0x2000" --protected_area "0x10000" "$ob_flash_programming" --vb"
$python "$AppliCfg" setob --layout "$STiROT_iLoader_ld_file" -b wrps -ms "CODE_OFFSET" -me "CODE_SIZE" -msec "CODE_SIZE" -d "0x2000" --protected_area "0x10000" "$ob_flash_programming" --vb
if [ $? -ne 0 ]; then error; return 1; fi

action="Update SRAM_ECC value"
echo "$python "$AppliCfg" obscript --xml "$obk_cfg_file" -ob "ECC_ON_SRAM" -sp "$sram_ecc" --vb "$ob_flash_programming""
$python "$AppliCfg" obscript --xml "$obk_cfg_file" -ob "ECC_ON_SRAM" -sp "$sram_ecc" --vb "$ob_flash_programming"
if [ $? -ne 0 ]; then error; return 1; fi

echo Script successfully updated according to STiROT_Config.obk
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
return 0

error()
{
  echo "       Error when trying to \"$action\"" > /dev/tty
  echo "       Update script aborted" > /dev/tty
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}
