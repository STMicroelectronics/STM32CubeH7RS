#!/bin/bash

# Call env.sh script to set up environment variables
source ../env.sh

# Handle the arguments
if [ $# -ge 1 ]; then

  # Get the arguments
  arg1=$1
  if [ $# -ge 2 ]; then arg2=$2; else arg2="NA"; fi

  # Set script_mode
  if [ "$arg1" == "AUTO" ] || [ "$arg2" == "AUTO" ]; then
    script_mode="AUTO"
  else
    script_mode="MANUAL"
  fi

  # Set selected_product_state
  if [ "$arg1" == "OPEN" ] || [ "$arg2" == "OPEN" ]; then
    selected_product_state="OPEN"
  else
    selected_product_state="NOT_OPEN"
  fi

else
  script_mode="MANUAL"
  selected_product_state="NOT_OPEN"
fi

script_error_file="error"

# Data updated with the postbuild of OEMiROT-Boot
wrps0=0
wrps1=0
wrps2=0
wrps3=0
wrps4=0
wrps5=0
wrps6=0
wrps7=0
wrps=$((($wrps0<<0)|($wrps1<<1)|($wrps2<<2)|($wrps3<<3)|($wrps4<<4)|($wrps5<<5)|($wrps6<<6)|($wrps7<<7)))
bootaddress=0x8000000
hdp_start=1
hdp_end=0
hdps=$((($hdp_start<<0)|($hdp_end<<16)))
app_image_number=1

if [ $isGeneratedByCubeMX == "true" ]; then
   appli_dir=$oemirot_boot_path_project
   oemirot_code="$cube_fw_path/Projects/STM32H7S78-DK/Applications/ROT/OEMiROT_Boot/Binary/OEMiROT_Boot.bin"
else
   appli_dir="../../$oemirot_boot_path_project"
   oemirot_code="$project_dir/../../Applications/ROT/OEMiROT_Boot/Binary/OEMiROT_Boot.bin"
fi

oemirot_appli_hex="$appli_dir/Binary/$oemirot_appli"

# CubeProgrammer connection
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=UR"
if [ "$selected_product_state" == "OPEN" ]; then
    connect_normal="-c port=SWD speed=fast ap=1 mode=normal"
else
    connect_normal="-c port=$com_port br=921600"
fi

# =============================================== Remove protections and erase the user flash ===============================================
remove_protect_open="-ob WRPS0=1 WRPS1=1 WRPS2=1 WRPS3=1 WRPS4=1 WRPS5=1 WRPS6=1 WRPS7=1 HDP_AREA_START=1 HDP_AREA_END=0"
erase_all=-"e all"
wrps_disable=0xff
hdp_area_start_disabled=0x0ff
hdp_area_end_disabled=0x000
hdps_disabled=$((($hdp_area_start_disabled<<0)|($hdp_area_end_disabled<<16)))

# ============================================================= Error handling ==============================================================
error()
{
  echo "        Error when trying to \"$action\"" > $script_error_file
  echo "        Programming aborted" >> $script_error_file
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}

# =============================================== Configure Option Bytes ====================================================================

action="Remove Protection and erase All"
echo $action
if [ "$selected_product_state" == "OPEN" ]; then
    "$stm32programmercli" $connect_reset $remove_protect_open $erase_all
    if [ $? -ne 0 ]; then error; return 1; fi
else
    # Disable WRP protections
    "$stm32programmercli" $connect_reset >> /dev/null
    ob_add=0x5200221c
    ob_mask=0x000000ff
    ob_pos=0x00
    "$stm32programmercli" $connect_no_reset -obrss $ob_add $ob_mask $wrps_disable $ob_pos
    if [ $? -ne 0 ]; then error; return 1; fi

    # Disable HDP protection on user flash
    "$stm32programmercli" $connect_reset >> /dev/null
    ob_add=0x52002234
    ob_mask=0x03ff03ff
    ob_pos=0x00
    "$stm32programmercli" $connect_no_reset -obrss $ob_add $ob_mask $hdps_disabled $ob_pos
    if [ $? -ne 0 ]; then error; return 1; fi

    "$stm32programmercli" $connect_reset >> /dev/null
    "$stm32programmercli" $connect_no_reset $erase_all
    if [ $? -ne 0 ]; then error; return 1; fi
fi

action="Set SRAM ECC configuration"
echo $action
# Recommended configuration for secure boot is :
#   - SRAM ECC activated. Hack tentative detection enabled ==> ECC_ON_SRAM=1
"$stm32programmercli" $connect_no_reset -ob ECC_ON_SRAM=1
if [ $? -ne 0 ]; then error; return 1; fi

action="Set XSPI1_HSLV configuration (needed to use external memories)"
echo $action
"$stm32programmercli" $connect_no_reset -ob XSPI1_HSLV=1
if [ $? -ne 0 ]; then error; return 1; fi

action="Set XSPI2_HSLV configuration (needed to use external memories)"
echo $action
"$stm32programmercli" $connect_no_reset -ob XSPI2_HSLV=1
if [ $? -ne 0 ]; then error; return 1; fi

# ================================================== Download OEMiROT_Boot ==================================================================

action="Write OEMiROT_Boot"
echo $action
"$stm32programmercli" $connect_no_reset -d "$oemirot_code" $bootaddress -v
if [ $? -ne 0 ]; then error; return 1; fi

action="OEMiROT_Boot Written"
echo $action

# ======================================================= Extra board protections =========================================================

action="Define write protected area and hide protected area"
echo $action
if [ "$selected_product_state" == "OPEN" ]; then
    "$stm32programmercli" $connect_no_reset -ob WRPS0=$wrps0 WRPS1=$wrps1 WRPS2=$wrps2 WRPS3=$wrps3 WRPS4=$wrps4 WRPS5=$wrps5 WRPS6=$wrps6 WRPS7=$wrps7 HDP_AREA_START=$hdp_start HDP_AREA_END=$hdp_end
    if [ $? -ne 0 ]; then error; return 1; fi
else
    # Configure WRP protection on user flash
    "$stm32programmercli" $connect_reset >> /dev/null
    ob_add=0x5200221c
    ob_mask=0x000000ff
    ob_pos=0x00
    "$stm32programmercli" $connect_no_reset -obrss $ob_add $ob_mask $wrps $ob_pos
    if [ $? -ne 0 ]; then error; return 1; fi

    # Configure HDP protection on user flash
    "$stm32programmercli" $connect_reset >> /dev/null
    ob_add=0x52002234
    ob_mask=0x03ff03ff
    ob_pos=0x00
    "$stm32programmercli" $connect_no_reset -obrss $ob_add $ob_mask $hdps $ob_pos
    if [ $? -ne 0 ]; then error; return 1; fi
fi

# ==================================================== Download images ====================================================================
action="Application images programming in download slots"
echo $action

if [ "$app_image_number" == "1" ]; then
    action="Write OEMiROT_Appli Code"
    echo $action
    if [ "$selected_product_state" == "OPEN" ]; then
        "$stm32programmercli" $connect_normal $stm32ExtLoaderFlashOpen -d "$oemirot_appli_hex" --verify
        if [ $? -ne 0 ]; then error; return 1; fi
    else
        "$stm32programmercli" $connect_normal $stm32ExtLoaderFlash -d "$oemirot_appli_hex" --verify
        if [ $? -ne 0 ]; then error; return 1; fi
    fi

    echo "Appli Written"
fi

# ==================================================== DTCM/ITCM_AXI_SHARE ====================================================================

# Place to change DTCM/ITCM_AXI_SHARE configurations if there is a need
#"$stm32programmercli" $connect_no_reset -ob DTCM_AXI_SHARE=0 ITCM_AXI_SHARE=0

echo "Programming success"
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
return 0