@ECHO OFF
:: Getting the Trusted Package Creator and STM32CubeProgammer CLI path
set "project_dir=%~dp0"

:start
copy "%project_dir%\Debug\Exe\Project.bin" "%project_dir%\..\Binary\STiROT_iLoader.bin"

echo IAR> "%project_dir%\..\Binary\_ide"
exit 0

:error
exit 1
