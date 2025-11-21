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
cd "$project_dir/../../../../../ROT_Provisioning"
provisioningdir=$(pwd)
cd "$project_dir"
source "$provisioningdir/env.sh" "$provisioningdir"

# arg1 is the config type (Debug, Release)
config=$1

error()
{
    echo ""
    echo "====="
    echo "===== Error occurred."
    echo "===== See $current_log_file for details. Then try again."
    echo "====="
    exit 1
}

# Environment variable for log file
current_log_file="$project_dir/postbuild.log"
echo "" > "$current_log_file"

#=============================================================================================
#Variables updated by OEMiROT_Boot postbuild
#=============================================================================================
#flag to switch between OEMiROT and OEMuROT
oemurot_enable=0

if [ "$oemurot_enable" == "1" ]; then
    project=OEMuROT
    bootpath=STiROT_OEMuROT
fi

if [ "$oemurot_enable" == "0" ]; then
    project=OEMiROT
    bootpath=OEMiROT
fi

#=============================================================================================
#image xml configuration files
#=============================================================================================
code_image_file="$provisioningdir/$bootpath/Images/${project}_Code_Image.xml"

#=================================================================================================
# Variables for image xml configuration(ROT_Provisioning\%bootpath%\Images)
# relative path from ROT_Provisioning\%bootpath%\Images directory to retrieve binary files
#=================================================================================================
bin_path_xml_field="../../../Templates/Template_ROT/OEMiROT_Appli/Appli/Binary"
fw_in_bin_xml_field="Firmware binary input file"
fw_out_bin_xml_field="Image output file"
app_bin_xml_field="$bin_path_xml_field/rot_app.bin"
app_enc_sign_hex_xml_field="$bin_path_xml_field/rot_app_enc_sign.hex"

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

# Copy application build in Binary location
bin_location="$project_dir/../../Appli/Binary"
mkdir -p "$bin_location"
cp "$project_dir/$config/STM32H7S78-DK_Template_OEMiROT_Appli.bin" "$bin_location/rot_app.bin"
if [ $? != 0 ]; then error; fi

echo "Creating $project image" >> "$current_log_file"

# update xml file : input file
$python$AppliCfg xmlval -v "$app_bin_xml_field" --string -n "$fw_in_bin_xml_field" "$code_image_file" >> "$current_log_file"
if [ $? != 0 ]; then error; fi

# update xml file : output file
$python$AppliCfg xmlval -v "$app_enc_sign_hex_xml_field" --string -n "$fw_out_bin_xml_field" "$code_image_file" >> "$current_log_file"
if [ $? != 0 ]; then error; fi

"$stm32tpccli" -pb "$provisioningdir/$bootpath/Images/${project}_Code_Image.xml" >> "$current_log_file"
if [ $? != 0 ]; then error; fi

exit 0
