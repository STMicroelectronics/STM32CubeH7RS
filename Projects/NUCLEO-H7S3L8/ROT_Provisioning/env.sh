#!/bin/bash -

#==============================================================================
#                              General
#==============================================================================
#Configure tools installation path
if [ "$OS" == "Windows_NT" ]; then
    stm32programmercli_path="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin"
    stm32programmercli="$stm32programmercli_path\STM32_Programmer_CLI.exe"
else
    stm32programmercli_path=~/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/
    PATH=$stm32programmercli_path:$PATH
    stm32programmercli="STM32_Programmer_CLI"
fi
