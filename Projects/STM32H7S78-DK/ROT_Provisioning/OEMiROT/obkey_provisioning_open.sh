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
if [[ "$device_type" == "H7S" ]]; then
    action="Configure OBKeys HDPL1 dummy AHK"
    echo "$action"
    "$stm32programmercli" $connect_no_reset
    sleep 3
    "$stm32programmercli" $connect_no_reset -vb 3 -sdp Binary/OEMiROT_AHK_OPEN.obk
    if [ $? -ne 0 ]; then error; return 1; fi
else
    # do nothing
    echo
fi

action="Configure OBKeys HDPL1-OEMiROT config area"
echo "$action"
"$stm32programmercli" $connect_no_reset
sleep 3
"$stm32programmercli" $connect_no_reset -vb 3 -sdp Binary/OEMiROT_Config.obk
if [ $? -ne 0 ]; then error; return 1; fi

action="Configure OBKeys HDPL1-OEMiROT data area"
echo "$action"
"$stm32programmercli" $connect_no_reset
sleep 3
"$stm32programmercli" $connect_no_reset -vb 3 -sdp Binary/OEMiROT_Data_OPEN.obk
if [ $? -ne 0 ]; then error; return 1; fi

echo "Provisioning success"
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi

return
