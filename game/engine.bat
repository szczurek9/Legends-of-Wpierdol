@echo off
title LoW-Engine
echo Legends of Wpierdol Engine v1.0 @ 2025 szczurek9
goto nick

:nick
cls
echo.
echo Wpisz swoj nick:
set /p player_nick=nick:
echo Czy twoj nick ci odpowiada (%player_nick%)?
echo 1. Tak
echo 2. Nie
echo.
set /p select=select:
if %select%==1 goto menu
if %select%==2 goto nick

:menu
cls
echo.
echo Nick: %player_nick%
echo Main Bron: %player_main_weapon% (%player_main_weapon_dmg% DMG)
echo Sec Bron: %player_sec_weapon% (%player_sec_weapon_dmg% DMG)
echo Hajs: %hajs%$
echo Lvl: %player_level%
echo.
echo 1. Graj
echo 2. Sklep
echo 3. Opcje
echo.
set /p select=select:
if %select%==1 goto battle
if %select%==2 goto shop
if %select%==3 goto options

:battle
cls
call battle.bat %hajs% %player_hp% %player_main_weapon% %player_main_weapon_dmg% %player_sec_weapon% %player_sec_weapon_dmg% %player_level%

if exist stats_save.bat (
    call stats_save.bat
    del stats_save.bat
)

cls
title LoW-Engine
goto menu

:shop
cls
set /a shop_level=(player_level-1)/5 + 1
call shop.bat %shop_level% %hajs%

if exist temp_shop.bat (
    call temp_shop.bat
    del temp_shop.bat
)

cls
title LoW-Engine
goto menu


:options
cls
call options.bat
cls
title LoW-Engine
goto menu