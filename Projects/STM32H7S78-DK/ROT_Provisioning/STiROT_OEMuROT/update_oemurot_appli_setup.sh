#!/bin/bash -
source ../env.sh

# Environment variable for setting postbuild command with AppliCfg
project_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ $# -ge 1 ]; then script_mode=$1; else script_mode=MANUAL; fi

error()
{
  echo "        Error when trying to \"$action\"" > /dev/tty
  echo "        Programming aborted" > /dev/tty
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}


code_image="$project_dir/Images/OEMuROT_Code_Image.xml"

# General section need
fw_in_bin="Firmware binary input file"
fw_out_bin="Image output file"

if [ $isGeneratedByCubeMX == "true" ]; then
   appli_dir="$oemirot_boot_path_project"
else
   appli_dir="../../../$oemirot_boot_path_project"
fi

#Path adapted to postbuild command
oemurot_app_bin="$appli_dir/Binary/rot_app.bin"
oemurot_app_bin="${oemurot_app_bin//\\/\/}"
oemurot_app_hex="$appli_dir/Binary/$oemirot_appli"
oemurot_app_hex="${oemurot_app_hex//\\/\/}"

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

# ================================================ Updating test Application files ========================================================

action="Configure the Firmware binary input file in OEMuROT_Code_Image.xml..."
echo "$action"
$python"$AppliCfg" xmlval --name "$fw_in_bin" --value "$oemurot_app_bin" --string --vb "$code_image"
if [ $? -ne 0 ]; then error; return 1; fi

action="Update OEMuROT_Code_Image.xml output encrypted/signed hexadecimal file"
echo "$action"
echo $python"$AppliCfg" xmlval --name "$fw_out_bin" --value "$oemurot_app_hex" --string --vb "$code_image"
$python"$AppliCfg" xmlval --name "$fw_out_bin" --value "$oemurot_app_hex" --string --vb "$code_image"
if [ $? -ne 0 ]; then error; return 1; fi

echo "OEMuROT_Code_Image.xml successfully updated"
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi

