#!/bin/bash -

# Getting the Trusted Package Creator CLI path
SCRIPT=$(readlink -f $0)
project_dir=`dirname "$SCRIPT"`
cd "$project_dir/../../../../ROT_Provisioning"
provisioning_dir=$(pwd)
cd "$project_dir"
source "$provisioning_dir/env.sh"

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

rot_update_script="$project_dir/../../OEMiROT_Boot/auto_rot_update.sh"
source "$rot_update_script"

# Copy application build in Binary location
cp "$project_dir/$config/STM32H7S78-DK_OEMiROT_Appli.bin" "$project_dir/../Binary/rot_app.bin"
if [ $? != 0 ]; then error; fi

echo "Creating $project image" > "$current_log_file"
"$stm32tpccli" -pb "$provisioning_dir/$bootpath/Images/${project}_Code_Image.xml" >> "$current_log_file"
if [ $? != 0 ]; then error; fi

exit 0