#!/bin/bash

source ../env.sh

if [ $# -ge 1 ]; then script_mode=$1; else mode=MANUAL; fi

# CubeProgammer connection
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst"

# =============================================== Configure OB Keys ========================================================
# Verify if obk file exists
if [ ! -f ./Binary/$da_file.obk ]; then
    echo -e [31m     Error: $da_file.obk does not exist! use TPC to generate it[0m > /dev/tty
    error
    return 1
fi

# Provisioning the obk file
action="Provisioning the obk file"
echo $action
"$stm32programmercli" $connect_reset
"$stm32programmercli" $connect_no_reset -sdp ./Binary/$da_file.obk
if [ $? -ne 0 ]; then error; return 1; fi

# Reset the board after provisioning
action="Reset the board after provisioning"
echo $action
"$stm32programmercli" $connect_reset
if [ $? -ne 0 ]; then error; return 1; fi

echo Provisioning success
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
return 0

:error
echo      Error when trying to "$action" > /dev/tty
echo      Provisioning aborted > /dev/tty
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
return 1
