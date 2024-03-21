#!/bin/bash

source ../env.sh

# Select Key/certificate and password
key="./Keys/key_3_leaf.pem"
cert="./Certificates/cert_leaf_chain.b64"
pass="./Binary/password.bin"

# Command with both key/certificate and password
if [ $# -ge 1 ]; then
    permission="per=$1"
    if [ $# -ge 2 ]; then script_mode="$2"; else script_mode="MANUAL"; fi
else
    script_mode="MANUAL"
    permission=" "
fi

error()
{
  echo "dbg_auth script failed"
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}

"$stm32programmercli" -c port=SWD debugauth=2
"$stm32programmercli" -c port=SWD $permission key=$key cert=$cert pwd=$pass debugauth=1
if [ $? -ne 0 ]; then error; exit 1; fi

echo "dbg_auth script success"
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
exit 0
