@ECHO OFF

:: ==============================================================================
::                               General
:: ==============================================================================
:: Configure tools installation path
set stm32programmercli_path=C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin
set stm32programmercli_path=%stm32programmercli_path:\=/%
set stm32programmercli="%stm32programmercli_path%/STM32_Programmer_CLI.exe"
