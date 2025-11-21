#!/bin/bash -
#=================================================================================================
# Managing HOST OS diversity : begin
#=================================================================================================
OS=$(uname)

echo ${OS} | grep -i -e windows -e mingw >/dev/null
if [ $? == 0 ]; then
  echo "=================================="
  echo "HOST OS : Windows detected"
  echo ""
  echo ">>> Running ../prebuild.bat $@"
  echo ""
  # Enable : exit immediately if any commands returns a non-zero status
  set -e
  cd ../
  cmd.exe /C prebuild.bat $@
  # Return OK if no error detected during .bat script
  exit 0
fi

if [ "$OS" == "Linux" ]; then
  echo "HOST OS : Linux detected"
elif [ "$OS" == "Darwin" ]; then
  echo "HOST OS : MacOS detected"
else
  echo "!!!HOST OS not supported : >$OS<!!!"
  exit 1
fi

#=================================================================================================
# Managing HOST OS diversity : end
#=================================================================================================
echo "=================================="
echo ">>> Running $0 $@"
echo ""

# constants for the prebuild script
SCRIPT=$(readlink -f $0)

project_dir=`dirname "$SCRIPT"`
env_script="$project_dir/../../../../ROT_Provisioning/env.sh"
source "$env_script" "$project_dir"

# Update the stirot_iloader reference in env.sh
stirot_iloader_boot_path_project_str="$project_dir"

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
applicfg="$cube_fw_path/../../../Utilities/PC_Software/ROT_AppliConfig/AppliCfg.py"

# Update the stirot_iloader linker file
code_align="0x2000"
ld_file="$project_dir/STM32H7S7L8HXH_FLASH.ld"

offset_macro=CODE_OFFSET
offset_pattern='CODE_OFFSET'

found=$(grep -nE "^$offset_pattern" "$ld_file" | cut -d: -f1)

# get the offset value
read_offset=$(sed -n "$found s/$offset_pattern//p" "$ld_file" | grep -oE "0x[0-9a-fA-F]+")

# Align the Offset on code_align value
dec_offset=$((read_offset / code_align * code_align))

# Convert offset value in hexa
hexa_offset=$(printf "0x%x" "$dec_offset")
offset_value_str="#define $offset_macro $hexa_offset"
echo "$offset_value_str" > "$project_dir/tmp.h"

# get the stirot_iloader size
size_macro=CODE_SIZE
size_pattern='CODE_SIZE'

size_found=$(grep -nE "^$size_pattern" "$ld_file" | cut -d: -f1)

# get the size value
read_size=$(sed -n "$size_found s/$size_pattern//p" "$ld_file" | grep -oE "0x[0-9a-fA-F]+")

# Align the Offset on code_align value
valid_size=$((read_size % code_align))

if [ "$valid_size" -eq 0 ]; then
  dec_size=$((read_size / code_align * code_align))
else
  dec_size=$(((read_size / code_align + 1) * code_align))
fi

# Convert offset value in hexa
hexa_size=$(printf "0x%x\n" "$dec_size")

offset_value_str="#define $size_macro $hexa_size"
echo "$offset_value_str" >> "$project_dir/tmp.h"

$python "$applicfg" linker -l "$project_dir/tmp.h" -m "$offset_macro" -n "$offset_macro" "$ld_file" > "$project_dir/output.txt" 2>&1

$python "$applicfg" linker -l "$project_dir/tmp.h" -m "$size_macro" -n "$size_macro" "$ld_file" > "$project_dir/output.txt" 2>&1

rm "$project_dir/tmp.h"

if [ "$1" != "AUTO" ]; then
  exit 0
fi

# error
echo "$command : failed" >> "$project_dir/output.txt"
exit 1
