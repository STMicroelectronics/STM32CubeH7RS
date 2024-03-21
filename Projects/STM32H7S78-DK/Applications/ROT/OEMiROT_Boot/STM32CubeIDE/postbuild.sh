#!/bin/bash -
# Getting the Trusted Package Creator CLI path
SCRIPT=$(readlink -f $0)
project_dir=`dirname "$SCRIPT"`
echo "$project_dir"
cd "$project_dir/../../../../ROT_Provisioning"
provisioning_dir="$(pwd)"
cd "$project_dir"
source "$provisioning_dir/env.sh" "$provisioning_dir"

# Environment variable for log file
current_log_file="$project_dir/postbuild.log"
echo "" > "$current_log_file"

# arg1 is the config type (Debug, Release)
config=$1

applicfg="$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/dist/AppliCfg.exe"
uname | grep -i -e windows -e mingw
if [ $? == 0 ] && [ -e "$applicfg" ]; then
  #line for window executable
  echo "AppliCfg with windows executable"
  python=""
else
  #line for python
  echo "AppliCfg with python script"
  applicfg="$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/AppliCfg.py"
  #determine/check python version command
  python="python3 "
fi

auto_rot_update="$project_dir/../auto_rot_update.sh"
map_properties="$project_dir/../map.properties"
preprocess_bl2_file="$project_dir/image_macros_preprocessed_bl2.c"
appli_dir="../../../../$oemirot_boot_path_project"

appli_flash_layout="$appli_dir/Inc/appli_flash_layout.h"
appli_postbuild="$appli_dir/STM32CubeIDE/postbuild.sh"

$python"$applicfg" flash --layout "$preprocess_bl2_file" -b oemurot_enable -m RE_OEMUROT_ENABLE --decimal "$auto_rot_update" --vb >> "$current_log_file"

error()
{
    echo ""
    echo "====="
    echo "===== Error occurred."
    echo "===== See $current_log_file for details. Then try again."
    echo "====="
    exit 1
}

if [ $? != 0 ]; then error; fi

source "$auto_rot_update"

update_sh="$provisioning_dir/$bootpath/ob_flash_programming.sh"
update_bat="$provisioning_dir/$bootpath/ob_flash_programming.bat"
code_xml="$provisioning_dir/$bootpath/Images/${project}_Code_Image.xml"
code_size="Firmware area size"
code_address="Firmware area address (in internal RAM)"
scratch_sector_number="Number of scratch sectors"
oemurot_code_size="Firmware area size"
oemurot_code_address="Firmware installation area address (in external flash)"
oemurot_download_code_address="Firmware download area address (in external flash)"
stirot_config_xml="$provisioning_dir/$bootpath/Config/STiROT_Config.xml"
# Check the configuration of the application: Applications or Templates
if [ -f "$appli_dir/STM32CubeIDE/STM32H7S7L8HXH_FLASH.ld" ]; then
    ld_file="$appli_dir/STM32CubeIDE/STM32H7S7L8HXH_FLASH.ld"
elif [ -f "$appli_dir/../STM32CubeIDE/Appli/STM32H7S7L8HXH_FLASH.ld" ]; then
    ld_file="$appli_dir/../STM32CubeIDE/Appli/STM32H7S7L8HXH_FLASH.ld"
else
    echo No valid ld file found
    error
fi

$python"$applicfg" modifyfilevalue --variable bootPath --delimiter = --value $project "$map_properties" --str --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" flash --layout "$preprocess_bl2_file" -b MCUBOOT_OVERWRITE_ONLY -m RE_OVER_WRITE "$map_properties" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

if [ "$oemurot_enable" == "0" ]; then
  # Write zero as there is no dedicated slot with OEMiROT
  $python"$applicfg" flash --layout "$preprocess_bl2_file" -b RUN_ROT_REGION_START -m RE_OEMUROT_ENABLE "$map_properties" --vb >> "$current_log_file"
  if [ $? != 0 ]; then error; fi

  # Write zero as there is no dedicated slot with OEMiROT
  $python"$applicfg" flash --layout "$preprocess_bl2_file" -b RUN_ROT_REGION_SIZE -m RE_OEMUROT_ENABLE "$map_properties" --vb >> "$current_log_file"
  if [ $? != 0 ]; then error; fi

  $python"$applicfg" flash --layout "$preprocess_bl2_file" -b ROT_REGION_START -m RE_RUN_ROT_START "$map_properties" --vb >> "$current_log_file"
  if [ $? != 0 ]; then error; fi

  $python"$applicfg" flash --layout "$preprocess_bl2_file" -b ROT_REGION_SIZE -m RE_RUN_ROT_SIZE "$map_properties" --vb >> "$current_log_file"
  if [ $? != 0 ]; then error; fi

  # Write zero as this slot does not exist with OEMiROT
  $python"$applicfg" flash --layout "$preprocess_bl2_file" -b DOWNLOAD_ROT_REGION_START -m RE_OEMUROT_ENABLE "$map_properties" --vb >> "$current_log_file"
  if [ $? != 0 ]; then error; fi

  # Write zero as this slot does not exist with OEMiROT
  $python"$applicfg" flash --layout "$preprocess_bl2_file" -b DOWNLOAD_ROT_REGION_SIZE -m RE_OEMUROT_ENABLE "$map_properties" --vb >> "$current_log_file"
  if [ $? != 0 ]; then error; fi
else
  $python"$applicfg" flash --layout "$preprocess_bl2_file" -b RUN_ROT_REGION_START -m RE_RUN_ROT_START "$map_properties" --vb >> "$current_log_file"
  if [ $? != 0 ]; then error; fi

  $python"$applicfg" flash --layout "$preprocess_bl2_file" -b RUN_ROT_REGION_SIZE -m RE_RUN_ROT_SIZE "$map_properties" --vb >> "$current_log_file"
  if [ $? != 0 ]; then error; fi

  $python"$applicfg" flash --xml "$stirot_config_xml" -b ROT_REGION_START -n "$oemurot_code_address" "$map_properties" --vb >> "$current_log_file"
  if [ $? != 0 ]; then error; fi

  $python"$applicfg" flash --xml "$stirot_config_xml" -b ROT_REGION_SIZE -n "$oemurot_code_size" "$map_properties" --vb >> "$current_log_file"
  if [ $? != 0 ]; then error; fi

  $python"$applicfg" flash --xml "$stirot_config_xml" -b DOWNLOAD_ROT_REGION_START -n "$oemurot_download_code_address" "$map_properties" --vb >> "$current_log_file"
  if [ $? != 0 ]; then error; fi

  $python"$applicfg" flash --xml "$stirot_config_xml" -b DOWNLOAD_ROT_REGION_SIZE -n "$oemurot_code_size" "$map_properties" --vb >> "$current_log_file"
  if [ $? != 0 ]; then error; fi
fi

$python"$applicfg" flash --layout "$preprocess_bl2_file" -b SCRATCH_REGION_START -m RE_FLASH_AREA_SCRATCH_START "$map_properties" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" flash --layout "$preprocess_bl2_file" -b SCRATCH_REGION_SIZE -m RE_FLASH_AREA_SCRATCH_SIZE "$map_properties" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" flash --layout "$preprocess_bl2_file" -b RUN_CODE_REGION_START -m RE_RUN_START "$map_properties" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" flash --layout "$preprocess_bl2_file" -b RUN_CODE_REGION_SIZE -m RE_AREA_0_SIZE "$map_properties" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" flash --layout "$preprocess_bl2_file" -b CODE_REGION_START -m RE_CODE_START "$map_properties" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" flash --layout "$preprocess_bl2_file" -b CODE_REGION_SIZE -m RE_IMAGE_FLASH_SIZE "$map_properties" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" flash --layout "$preprocess_bl2_file" -b DOWNLOAD_CODE_REGION_START -m RE_IMAGE_DOWNLOAD_ADDRESS "$map_properties" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" flash --layout "$preprocess_bl2_file" -b DOWNLOAD_CODE_REGION_SIZE -m RE_IMAGE_FLASH_SIZE "$map_properties" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" flash --layout "$preprocess_bl2_file" -b APP_RAM_START -m RE_APP_RAM_START "$map_properties" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" flash --layout "$preprocess_bl2_file" -b APP_RAM_SIZE -m RE_APP_RAM_SIZE "$map_properties" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" flash --layout "$preprocess_bl2_file" -b RAMECC_HANDLE_START -m RE_RAMECC_HANDLE_START "$map_properties" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" flash --layout "$preprocess_bl2_file" -b RAMECC_HANDLE_SIZE -m RE_RAMECC_HANDLE_SIZE "$map_properties" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" flash --layout "$preprocess_bl2_file" -b CONF_FLAG_START -m RE_CONF_FLAG_START "$map_properties" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" flash --layout "$preprocess_bl2_file" -b CONF_FLAG_SIZE -m RE_CONF_FLAG_SIZE "$map_properties" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" linker --layout "$preprocess_bl2_file" -m RE_PARTITION_START -n PARTITION_START "$ld_file" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" linker --layout "$preprocess_bl2_file" -m RE_AREA_0_OFFSET -n CODE_OFFSET "$ld_file" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" linker --layout "$preprocess_bl2_file" -m RE_AREA_0_SIZE -n CODE_SIZE "$ld_file" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

# Bypass configuration of appli_flash_layout file if not present
if [ -f "$appli_flash_layout" ]; then
  $python"$applicfg" setdefine --layout "$preprocess_bl2_file" -m RE_OVER_WRITE -n MCUBOOT_OVERWRITE_ONLY -v 1 "$appli_flash_layout" --vb >> "$current_log_file"
  if [ $? != 0 ]; then error; fi
fi

$python"$applicfg" xmlval --layout "$preprocess_bl2_file" -m RE_IMAGE_FLASH_SIZE -c S "$code_xml" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" xmlval --layout "$preprocess_bl2_file" -m RE_IMAGE_DOWNLOAD_ADDRESS -c x "$code_xml" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" xmlparam --layout  "$preprocess_bl2_file" -m RE_OVER_WRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" "$code_xml" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" xmlval --layout "$preprocess_bl2_file" -m RE_FLASH_AREA_SCRATCH_SIZE -n "$scratch_sector_number" --decimal "$code_xml" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

$python"$applicfg" xmlval -xml "$code_xml" -nxml "$code_size" -nxml "$scratch_sector_number" --decimal -e "(((val1+1)/val2)+1)" -cond "val2" -c M "$code_xml" --vb >> "$current_log_file"
if [ $? != 0 ]; then error; fi

if [ "$oemurot_enable" == "0" ]; then
    # Configuration of OEMiROT (sh)
    $python"$applicfg" setob --layout "$preprocess_bl2_file" -b wrps -ms RE_BL2_WRP_OFFSET -me RE_BL2_WRP_SIZE -msec RE_BL2_WRP_SIZE -d 0x2000 --protected_area 0x10000 "$update_sh" --vb >> "$current_log_file"
    if [ $? != 0 ]; then error; fi

    $python"$applicfg" setob --layout "$preprocess_bl2_file" -b hdp_start -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_HDP_BLOCK_NBR -d 0x100 "$update_sh" --vb >> "$current_log_file"
    if [ $? != 0 ]; then error; fi

    $python"$applicfg" setob --layout "$preprocess_bl2_file" -b hdp_end -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_HDP_BLOCK_NBR -d 0x100 "$update_sh" --vb >> "$current_log_file"
    if [ $? != 0 ]; then error; fi

    $python"$applicfg" flash --layout "$preprocess_bl2_file" -b bootaddress -m RE_BL2_BOOT_ADDRESS "$update_sh" >> "$project_dir/output.txt"
    if [ $? != 0 ]; then error; fi

    # Configuration of OEMiROT (bat)
    $python"$applicfg" setob --layout "$preprocess_bl2_file" -b wrps -ms RE_BL2_WRP_OFFSET -me RE_BL2_WRP_SIZE -msec RE_BL2_WRP_SIZE -d 0x2000 --protected_area 0x10000 "$update_bat" --vb >> "$current_log_file"
    if [ $? != 0 ]; then error; fi

    $python"$applicfg" setob --layout "$preprocess_bl2_file" -b hdp_start -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_HDP_BLOCK_NBR -d 0x100 "$update_bat" --vb >> "$current_log_file"
    if [ $? != 0 ]; then error; fi

    $python"$applicfg" setob --layout "$preprocess_bl2_file" -b hdp_end -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_HDP_BLOCK_NBR -d 0x100 "$update_bat" --vb >> "$current_log_file"
    if [ $? != 0 ]; then error; fi

    $python"$applicfg" flash --layout "$preprocess_bl2_file" -b bootaddress -m RE_BL2_BOOT_ADDRESS "$update_bat" >> "$project_dir/output.txt"
    if [ $? != 0 ]; then error; fi
else
    # Configuration of OEMuROT
    $python"$applicfg" xmlval --layout "$preprocess_bl2_file" -m RE_BL2_RAM_SIZE -n "$code_size" "$stirot_config_xml" --vb >> "$current_log_file"
    if [ $? != 0 ]; then error; fi

    $python"$applicfg" xmlval --layout "$preprocess_bl2_file" -m RE_BL2_RAM_BASE -n "$code_address" "$stirot_config_xml" --vb >> "$current_log_file"
    if [ $? != 0 ]; then error; fi
fi
if [ ! -d "$project_dir/../Binary" ]; then
  mkdir "$project_dir/../Binary/"
fi
cp -a "$project_dir/$config/STM32H7S78-DK_OEMiROT_Boot.bin" "$project_dir/../Binary/OEMiROT_Boot.bin" >> "$current_log_file"
if [ $? != 0 ]; then error; fi

if [ "$oemurot_enable" == "1" ]; then
    echo Creating OEMuROT image  >> "$current_log_file"
    "$stm32tpccli" -pb "$provisioning_dir/$bootpath/Images/STiROT_Code_Image.xml" >> "$current_log_file"
    if [ $? != 0 ]; then error; fi
fi

exit 0
