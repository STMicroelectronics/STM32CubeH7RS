#!/bin/bash

source ../env.sh

if [ $# -ge 1 ]; then script_mode=$1; else script_mode=MANUAL; fi

# CubeProgammer connection
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=UR"

error()
{
  echo "       Error when trying to "$action" > /dev/tty"
  echo "       Programming aborted > /dev/tty"
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}

# =============================================== Remove protections and initialize Option Bytes ===========================================

# =============================================== Erase the user flash =====================================================================
erase_all="-e all"
wrps_disable="0xff"
hdp_area_start="0x0ff"
hdp_area_end="0x000"

# =============================================== Configure Option Bytes ====================================================================
action="Configure Initial OB"
echo $action

action="Remove Protection and erase All"
echo $action
"$stm32programmercli" $connect_no_reset $erase_all
if [ $? -ne 0 ]; then error; return 1; fi

# Disable WRP protections
# WRPS
"$stm32programmercli" $connect_reset >> /dev/null
ob_add="0x5200221c"
ob_mask="0x000000ff"
ob_pos="0x00"
echo "\"$stm32programmercli\" $connect_no_reset -obrss $ob_add $ob_mask $wrps_disable $ob_pos"
"$stm32programmercli" $connect_no_reset -obrss $ob_add $ob_mask $wrps_disable $ob_pos
if [ $? -ne 0 ]; then error; return 1; fi

# Disable HDP protection on user flash
# HDP_AREA_START
"$stm32programmercli" $connect_reset >> /dev/null
ob_add="0x52002230"
ob_mask="0x000003ff"
ob_pos="0x00"
echo "\"$stm32programmercli\" $connect_no_reset -obrss $ob_add $ob_mask $hdp_area_start $ob_pos"
"$stm32programmercli" $connect_no_reset -obrss $ob_add $ob_mask $hdp_area_start $ob_pos
if [ $? -ne 0 ]; then error; return 1; fi

# HDP_AREA_END
"$stm32programmercli" $connect_reset >> /dev/null
ob_add="0x52002230"
ob_mask="0x000003ff"
ob_pos="0x10"
echo "\"$stm32programmercli\" $connect_no_reset -obrss $ob_add $ob_mask $hdp_area_end $ob_pos"
"$stm32programmercli" $connect_no_reset -obrss $ob_add $ob_mask $hdp_area_end $ob_pos
if [ $? -ne 0 ]; then error; return 1; fi

# ==================================================== DTCM/ITCM_AXI_SHARE ====================================================================

# Place to change DTCM/ITCM_AXI_SHARE configurations if there is a need
#"$stm32programmercli" $connect_no_reset -ob DTCM_AXI_SHARE=0 ITCM_AXI_SHARE=0
#echo "\"$stm32programmercli\" $connect_no_reset -ob DTCM_AXI_SHARE=0 ITCM_AXI_SHARE=0"

echo "Programming success"
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
return 0
