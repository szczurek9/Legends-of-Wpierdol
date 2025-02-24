@echo off
setlocal enabledelayedexpansion
title Shop

goto pre_shop

:pre_shop
set "shop_file=shop_data/shop%shop_level%.txt"

if exist %shop_file% (
    for /f "tokens=1,2 delims==" %%A in (%shop_file%) do set %%A=%%B
    goto shop
) else (
    echo Blad: Plik poziomu %shop_file% nie istnieje!
    pause
    exit /b
)

:shop
cls
echo.
echo Sklep poziomu %shop_id%
echo.
echo Pieniadze: %hajs%$
echo.
echo Bron glowna:
echo 1. %shop_mainweapon1% - %shop_mainweapon1_price%$ - %shop_mainweapon1_dmg% DMG
echo 2. %shop_mainweapon2% - %shop_mainweapon2_price%$ - %shop_mainweapon2_dmg% DMG
echo.
echo Bron drugorzedna:
echo 3. %shop_secweapon1% - %shop_secweapon1_price%$ - %shop_secweapon1_dmg% DMG
echo 4. %shop_secweapon2% - %shop_secweapon2_price%$ - %shop_secweapon2_dmg% DMG
echo.
echo 5. Wroc
echo.
set /p select=wybierz:

if %select%==1 goto mainweapon1_buy
if %select%==2 goto mainweapon2_buy
if %select%==3 goto secweapon1_buy
if %select%==4 goto secweapon2_buy
if %select%==5 exit /b
goto shop

:mainweapon1_buy
if %hajs% GEQ %shop_mainweapon1_price% (
    set player_main_weapon=!shop_mainweapon1!
    set /a player_main_weapon_dmg=!shop_mainweapon1_dmg!
    set /a hajs-=!shop_mainweapon1_price!
    echo set player_main_weapon=!shop_mainweapon1! > temp_shop.bat
    echo set /a player_main_weapon_dmg=!shop_mainweapon1_dmg! >> temp_shop.bat
    echo set /a hajs-=!shop_mainweapon1_price! >> temp_shop.bat
    cls
    echo.
    echo Kupiles !player_main_weapon!
    echo.
) else (
    cls
    echo.
    echo Nie masz wystarczajaco pieniedzy, aby kupic te bron!
    echo.
)
pause
goto shop

:mainweapon2_buy
if %hajs% GEQ %shop_mainweapon2_price% (
    set player_main_weapon=!shop_mainweapon2!
    set /a player_main_weapon_dmg=!shop_mainweapon2_dmg!
    set /a hajs-=!shop_mainweapon2_price!
    echo set player_main_weapon=!shop_mainweapon2! > temp_shop.bat
    echo set /a player_main_weapon_dmg=!shop_mainweapon2_dmg! >> temp_shop.bat
    echo set /a hajs-=!shop_mainweapon2_price! >> temp_shop.bat
    cls
    echo.
    echo Kupiles !player_main_weapon!
    echo.
) else (
    cls
    echo.
    echo Nie masz wystarczajaco pieniedzy, aby kupic te bron!
    echo.
)
pause
goto shop

:secweapon1_buy
if %hajs% GEQ %shop_secweapon1_price% (
    set player_sec_weapon=!shop_secweapon1!
    set /a player_sec_weapon_dmg=!shop_secweapon1_dmg!
    set /a hajs-=!shop_secweapon1_price!
    echo set player_sec_weapon=!shop_secweapon1! > temp_shop.bat
    echo set /a player_sec_weapon_dmg=!shop_secweapon1_dmg! >> temp_shop.bat
    echo set /a hajs-=!shop_secweapon1_price! >> temp_shop.bat
    cls
    echo.
    echo Kupiles !player_sec_weapon!
    echo.
) else (
    cls
    echo.
    echo Nie masz wystarczajaco pieniedzy, aby kupic te bron!
    echo.
)
pause
goto shop

:secweapon2_buy
if %hajs% GEQ %shop_secweapon2_price% (
    set player_sec_weapon=!shop_secweapon2!
    set /a player_sec_weapon_dmg=!shop_secweapon2_dmg!
    set /a hajs-=!shop_secweapon2_price!
    echo set player_sec_weapon=!shop_secweapon2! > temp_shop.bat
    echo set /a player_sec_weapon_dmg=!shop_secweapon2_dmg! >> temp_shop.bat
    echo set /a hajs-=!shop_secweapon2_price! >> temp_shop.bat
    cls
    echo.
    echo Kupiles !player_sec_weapon!
    echo.
) else (
    cls
    echo.
    echo Nie masz wystarczajaco pieniedzy, aby kupic te bron!
    echo.
)
pause
goto shop
