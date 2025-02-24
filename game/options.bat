@echo off
title Options
echo Opcje
goto settings

:settings
cls
echo.
echo Opcje
echo.
echo 1. Wroc do gry
echo 2. Zmien motyw
echo 3. Tworcy
echo 4. Wyjdz z gry
echo.
set /p select=select:
if %select%==1 goto back
if %select%==2 goto color_change
if %select%==3 goto credits
if %select%==4 goto game_quit

:back
exit /b

:color_change
cls
echo.
echo Wybierz motyw:
echo.
echo 1. Ciemny
echo 2. Jasny
echo.
set /p color_select=select:
echo.

if %color_select%==1 (
    color 07
    echo Motyw zostal zmieniony na ciemny
) else if %color_select%==2 (
    color F0
    echo Motyw zostal zmieniony na jasny
) else (
    echo Niewlasciwy wybor!
)
echo.
ping -n 2 127.0.0.1 >nul
pause
goto settings

:credits
cls
echo.
echo           #             #     #         ##      ###
echo           #             #     #        # #     #   #
echo           #      #####  #  #  #          #        #
echo           #      #   #  # # # #          #       #
echo           #####  #####  ##   ##          #  #   ####
echo.
echo  Legends of Wpierdol 1.2 Patch 2 - Copyright @ 2025 szczurek9
echo  Legends of Wpierdol Engine v1.0 - Copyright @ 2025 szczurek9
echo   LoW Level Creator version 1.0 - Copyright @ 2025 szczurek9
echo.
echo          Zglaszanie bledow - DM na Tiktok @szczur_09
echo.
pause
goto settings

:game_quit
cls
echo Nacisnij dowolny przycisk aby wyjsc z gry
pause
exit
