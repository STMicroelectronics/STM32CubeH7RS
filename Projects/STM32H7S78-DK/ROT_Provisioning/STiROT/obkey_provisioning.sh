#!/bin/bash -
source ../env.sh

script_error_file="error"
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"

if [ $# -ge 1 ]; then script_mode=$1; else script_mode=MANUAL; fi

error()
{
  echo "        Error when trying to \"$action\"" >$script_error_file
  echo "        Provisioning aborted" >> $script_error_file
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}

# =============================================== Configure OB Keys ===============================================
action="Configure OBKeys HDPL1-DA config area"
echo "$action"
"$stm32programmercli" $connect_no_reset
"$stm32programmercli" $connect_no_reset -sdp ../DA/Binary/$da_file.obk
if [ $? -ne 0 ]; then error; return 1; fi

action="Configure OBKeys HDPL1-STiROT config area"
echo "$action"
"$stm32programmercli" $connect_no_reset
"$stm32programmercli" $connect_no_reset -sdp Binary/STiROT_Config.obk
if [ $? -ne 0 ]; then error; return 1; fi

action="Configure OBKeys HDPL1-STiROT area"
echo "$action"
"$stm32programmercli" $connect_no_reset
"$stm32programmercli" $connect_no_reset -sdp Binary/STiROT_Data.obk
if [ $? -ne 0 ]; then error; return 1; fi

echo "Provisioning success"
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi

return
