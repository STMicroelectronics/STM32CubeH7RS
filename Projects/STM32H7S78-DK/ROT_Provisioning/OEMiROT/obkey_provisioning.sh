#!/bin/bash -
source ../env.sh

# Select device type (H7S or H7R)
device_type="H7S"

script_error_file="error"
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst"
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
"$stm32programmercli" $connect_reset
"$stm32programmercli" $connect_no_reset -sdp ../DA/Binary/$da_file.obk
if [ $? -ne 0 ]; then error; return 1; fi

action="Configure OBKeys HDPL1-OEMiROT config area"
echo "$action"
"$stm32programmercli" $connect_reset
"$stm32programmercli" $connect_no_reset -sdp Binary/OEMiROT_Config.obk
if [ $? -ne 0 ]; then error; return 1; fi

action="Configure OBKeys HDPL1-OEMiROT data area"
echo "$action"
"$stm32programmercli" $connect_reset
if [[ "$device_type" == "H7S" ]]; then
    "$stm32programmercli" $connect_no_reset -sdp Binary/OEMiROT_Data.obk
else
    # Use a non-encrypted obk file for H7R (DoEncryption = 0)
    "$stm32programmercli" $connect_no_reset -sdp Binary/OEMiROT_Data_OPEN.obk
fi
if [ $? -ne 0 ]; then error; return 1; fi

echo "Provisioning success"
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi

return
