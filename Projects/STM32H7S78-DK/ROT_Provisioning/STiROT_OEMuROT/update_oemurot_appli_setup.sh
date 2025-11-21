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

if [ $isGeneratedByCubeMX == "true" ]; then
   appli_dir="$oemirot_appli_path_project"
else
   appli_dir="../../../$oemirot_appli_path_project"
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

# ================================================ Updating test Application files ========================================================

echo "OEMuROT_Code_Image.xml successfully updated"
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi

