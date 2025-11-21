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
  echo ">>> Running ../postbuild.bat $@"
  echo ""
  # Enable : exit immediately if any commands returns a non-zero status
  set -e
  cd ../
  cmd.exe /C postbuild.bat $@
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

# Getting the Trusted Package Creator CLI path
SCRIPT=$(readlink -f $0)
project_dir=`dirname "$SCRIPT"`
provisioning_dir="$project_dir/../../../../ROT_Provisioning"

env_script="$provisioning_dir/env.sh"
source "$env_script" "$provisioning_dir" "$project_dir"

# arg1 is the config type (Debug, Release)
config=$1

# Environment variable for log file
current_log_file="$project_dir/postbuild.log"
echo "" > "$current_log_file"

code_bin="$project_dir/$config/STM32H7S78-DK_STiROT_Appli.bin"
mkdir -p "$project_dir/../Binary"
bin_dest="$project_dir/../Binary/appli.bin"

error()
{
    echo ""
    echo "====="
    echo "===== Error occurred."
    echo "===== See $current_log_file for details. Then try again."
    echo "====="
    exit 1
}

#=============================================================================================
#image xml configuration files
#=============================================================================================
code_image_file="$provisioning_dir/STiROT/Image/STiROT_Code_Image.xml"

#=================================================================================================
# Variables for image xml configuration(ROT_Provisioning\STiROT\Images)
# relative path from ROT_Provisioning\STiROT\Images directory to retrieve binary files
#=================================================================================================
bin_path_xml_field="../../../Applications/ROT/STiROT_Appli/Binary"
fw_in_bin_xml_field="Firmware binary input file"
fw_out_bin_xml_field="Image output file"
stirot_app_bin_xml_field="$bin_path_xml_field/appli.bin"
stirot_app_hex_xml_field="$bin_path_xml_field/appli_enc_sign.hex"

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

# Create final image with Trust Package Creator
echo "Copy binary to Binary location" >> "$current_log_file" 2>&1
cp "$code_bin" "$bin_dest" >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

echo "Creating STiROT image" >> "$current_log_file" 2>&1

# update xml file : input file
$python"$AppliCfg" xmlval --name "$fw_in_bin_xml_field" --value "$stirot_app_bin_xml_field" --string --vb "$code_image_file" >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

# update xml file : output file
$python"$AppliCfg" xmlval --name "$fw_out_bin_xml_field" --value "$stirot_app_hex_xml_field" --string --vb "$code_image_file" >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi

"$stm32tpccli" -pb "$provisioning_dir/STiROT/Image/STiROT_Code_Image.xml" >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi
exit 0