@echo off
title LoW Launcher
echo.
echo Legends of Wpierdol - Launcher
echo.
echo 1. Graj
echo 2. Wyjdz
echo.
set /p select=select:
if %select%==1 goto graj
if %select%==2 goto wyjdz

:graj
echo start
cd /d "%~dp0"
call default_stats.bat
pause
exit

:wyjdz
exit