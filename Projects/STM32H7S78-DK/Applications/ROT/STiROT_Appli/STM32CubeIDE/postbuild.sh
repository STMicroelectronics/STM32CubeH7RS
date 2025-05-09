#!/bin/bash -

# Getting the Trusted Package Creator CLI path
SCRIPT=$(readlink -f $0)
project_dir=`dirname "$SCRIPT"`
provisioning_dir="$project_dir/../../../../ROT_Provisioning"

env_script="$provisioning_dir/env.sh"
source "$env_script" "$project_dir"

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

# Create final image with Trust Package Creator
echo "Copy binary to Binary location" >> "$current_log_file" 2>&1
cp "$code_bin" "$bin_dest" >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi
echo "Creating STiROT image" >> "$current_log_file" 2>&1
"$stm32tpccli" -pb "$provisioning_dir/STiROT/Image/STiROT_Code_Image.xml" >> "$current_log_file" 2>&1
if [ $? != 0 ]; then error; fi
exit 0