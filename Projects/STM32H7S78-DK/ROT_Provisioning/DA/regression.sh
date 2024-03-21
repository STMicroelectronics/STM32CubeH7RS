#!/bin/bash

source ../env.sh

if [ $# -ge 1 ]; then script_mode=$1; else script_mode=MANUAL; fi

connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst"
Debug_authentication_method=""

error()
{
  echo "regression script failed"
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}

read -p "Which debug authentication is used ? [ CERTIFICATE | PASSWORD ]: " Debug_authentication_method
Debug_authentication_method=${Debug_authentication_method}
if [ "$script_mode" == "AUTO" ]; then
  Debug_authentication_method=$(echo "$Debug_authentication_method" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')
fi
if [[ "${Debug_authentication_method^^}" == "CERTIFICATE" ]]; then
  # Configure OBKeys for Debug Authentication (in case of Provisioning Product state)
  "$stm32programmercli" $connect_reset > /dev/null
  "$stm32programmercli" $connect_no_reset -sdp ./Binary/DA_Config.obk > /dev/null
  "$stm32programmercli" -c port=SWD debugauth=2
  "$stm32programmercli" -c port=SWD speed=fast per=a key=./Keys/key_3_leaf.pem cert=./Certificates/cert_leaf_chain.b64 debugauth=1
  if [ $? -ne 0 ]; then error; exit 1; fi

else
  if [[ "${Debug_authentication_method^^}" == "PASSWORD" ]]; then
    # Configure OBKeys for Debug Authentication (in case of Provisioning Product state)
    "$stm32programmercli" $connect_reset > /dev/null
    "$stm32programmercli" $connect_no_reset -sdp ./DA/Binary/DA_ConfigWithPassword.obk > /dev/null

    "$stm32programmercli" -c port=SWD debugauth=2
    "$stm32programmercli" -c port=SWD pwd=./Binary/password.bin debugauth=1
    if [ $? -ne 0 ]; then error; exit 1; fi

  else
    echo "Board not correctly configured"
    error
    exit 1
  fi
fi

echo "regression script success"
$SHELL
exit 0
