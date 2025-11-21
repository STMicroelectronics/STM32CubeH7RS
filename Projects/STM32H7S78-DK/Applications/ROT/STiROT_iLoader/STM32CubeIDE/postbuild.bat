@ECHO OFF
:: arg1 is the config type (Debug, Release)
set config=%1
:: Getting the Trusted Package Creator and STM32CubeProgammer CLI path
set "project_dir=%~dp0"

:start
copy %project_dir%\%config%\STM32H7S78-DK_STiROT_iLoader.bin %project_dir%\..\Binary\STiROT_iLoader.bin


echo STM32CubeIDE> "%project_dir%\..\Binary\_ide"
exit 0

:error
exit 1
