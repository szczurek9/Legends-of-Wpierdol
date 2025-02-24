@echo off
setlocal enabledelayedexpansion
title LoW Level Creator v1.0
cls

:: Pobranie numeru poziomu
echo.
echo LoW Level Creator version 1.0 - Copyright @ 2025 szczurek9
echo.
set /p level_number=Podaj numer poziomu ktory chcesz utworzyc (numer musi byc inny niz istniejacy):
echo.

:: Sprawdzenie, czy plik już istnieje
set "level_file=level%level_number%.txt"
if exist "%level_file%" (
    echo Blad: Plik %level_file% juz istnieje!
    pause
    exit
)

:: Pobieranie danych od uzytkownika
set /p enemy_name=Podaj nazwe przeciwnika:
set /p enemy_hp=Podaj HP przeciwnika:
set /p enemy_dmg=Podaj DMG przeciwnika:
set /p enemy_attack_chance=Podaj szanse na atak przeciwnika (od 0 do 100):
set /p player_miss_chance=Podaj szanse na missniecie ataku przez gracza (od 0 do 100):
set /p reward_money=Podaj ilosc hajsu za pokonanie przeciwnika:
set /p wait_hp_bonus=Podaj ilosc HP odzyskiwana za przeczekanie tury:
set /p lvl_up=Podaj wiadomosc jaka gracz ma otrzymac na koncu poziomu (aby byla pusta nacisnij enter):

:: Zapis do pliku
(
    echo enemy_name=%enemy_name%
    echo enemy_hp=%enemy_hp%
    echo enemy_dmg=%enemy_dmg%
    echo enemy_attack_chance=%enemy_attack_chance%
    echo player_miss_chance=%player_miss_chance%
    echo reward_money=%reward_money%
    echo wait_hp_bonus=%wait_hp_bonus%
    echo lvl_up=%lvl_up%
) > "%level_file%"

:: Sprawdzenie, czy plik faktycznie został utworzony
if exist "%level_file%" (
    cls
    echo.
    echo Plik z poziomem %level_file% zostal utworzony!
) else (
    cls
    echo.
    echo Blad: Plik NIE zostal utworzony!
)

echo.
pause
exit
