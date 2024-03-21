@ECHO OFF
:: Getting the Trusted Package Creator and STM32CubeProgammer CLI path
set "project_dir=%~dp0"

echo KEIL> "%project_dir%\..\Binary\_ide"
exit 0

:error
exit 1