cd %~dp0%
rmdir Editor\Data Game\Data
mklink /d .\Editor\Data ..\Data
mklink /d .\Game\Data ..\Data
pause