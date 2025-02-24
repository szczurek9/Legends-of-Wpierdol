@echo off
setlocal enabledelayedexpansion
title Battle
echo Poziom %player_level%
pause
goto pre_battle

:pre_battle
set "level_file=levels/level%player_level%.txt"

if exist %level_file% (
    for /f "tokens=1,2 delims==" %%A in (%level_file%) do set %%A=%%B
    goto battle
) else (
    echo Blad: Plik poziomu %level_file% nie istnieje!
    pause
    goto wyjdz
)

:battle
cls
echo.
echo Twoje HP: %player_hp%
echo.
echo Przeciwnik: %enemy_name%
echo HP przeciwnika: %enemy_hp%
echo DMG przeciwnika: %enemy_dmg%
echo Bonus HP za przeczekanie: %wait_hp_bonus%
echo.
echo 1. Atakuj - Main Bron - %player_main_weapon% (%player_main_weapon_dmg% DMG)
echo 2. Atakuj - Sec Bron - %player_sec_weapon% (%player_sec_weapon_dmg% DMG)
echo 3. Czekaj na atak przeciwnika
echo.
set /p select=select:
if %select%==1 goto fight_main_weapon
if %select%==2 goto fight_sec_weapon
if %select%==3 goto wait

:fight_main_weapon
set /a miss=%random% %% 100
if %miss% LSS %player_miss_chance% (
    cls
    echo.
    echo Pudlo, Przeciwnik uniknal ataku!
    echo.
) else (
    set /a enemy_hp-=player_main_weapon_dmg
    cls
    echo.
    echo Trafiles, Przeciwnik ma teraz !enemy_hp! HP!
    echo.
)

if !enemy_hp! LEQ 0 goto victory

set /a attack_chance=%random% %% 100
if %attack_chance% LSS %enemy_attack_chance% (
    set /a player_hp-=enemy_dmg
    cls
    echo.
    echo Przeciwnik zaatakowal, Otrzymales %enemy_dmg% obrazen!
    echo.
)

if !player_hp! LEQ 0 goto game_over

pause
goto battle

:fight_sec_weapon
set /a miss=%random% %% 100
if %miss% LSS %player_miss_chance% (
    cls
    echo.
    echo Pudlo, Przeciwnik uniknal ataku!
    echo.
) else (
    set /a enemy_hp-=player_sec_weapon_dmg
    cls
    echo.
    echo Trafiles, Przeciwnik ma teraz !enemy_hp! HP!
    echo.
)

if !enemy_hp! LEQ 0 goto victory

set /a attack_chance=%random% %% 100
if %attack_chance% LSS %enemy_attack_chance% (
    set /a player_hp-=enemy_dmg
    cls
    echo.
    echo Przeciwnik zaatakowal, Otrzymales %enemy_dmg% obrazen!
    echo.
)

if !player_hp! LEQ 0 goto game_over

pause
goto battle

:wait
cls
echo.

set /a attack_chance=%random% %% 100
if %attack_chance% LSS %enemy_attack_chance% (
    set /a player_hp-=enemy_dmg
    cls
    echo.
    echo Przeciwnik zaatakowal, Otrzymales %enemy_dmg% obrazen!
    echo.
    pause
    if !player_hp! LEQ 0 goto game_over
) else (
    set /a player_hp+=wait_hp_bonus
    cls
    echo.
    echo Odczekales ruch, regenerujesz %wait_hp_bonus% HP!
)

echo.
pause
goto battle


:victory
cls
echo Pokonales %enemy_name%!
echo Nagroda: %reward_money%$
if "%lvl_up%"=="" (
    echo.
) else (
    echo %lvl_up%
)
echo.
set /a hajs+=%reward_money%
set /a player_level+=1

(
    echo set hajs=%hajs%
    echo set player_level=%player_level%
) > stats_save.bat

pause
exit /b


pause
exit /b

:game_over
cls
echo GAME OVER - Zdechles cwelu
pause
exit
