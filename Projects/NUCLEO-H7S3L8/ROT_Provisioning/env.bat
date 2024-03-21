@ECHO OFF

:: ==============================================================================
::                               General
:: ==============================================================================
:: Configure tools installation path
set stm32tool_path=C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin
set stm32tool_path=%stm32tool_path:\=/%
set stm32programmercli="%stm32tool_path%/STM32_Programmer_CLI.exe"
