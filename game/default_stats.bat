@echo off
title default_stats
echo default_stats set!
set /a hajs=100
set /a player_hp=100
set player_main_weapon=Patyk
set /a player_main_weapon_dmg=1
set player_sec_weapon=nic
set /a player_sec_weapon_dmg=0
set /a player_level=1
call engine.bat %hajs% %player_hp% %player_main_weapon% %player_main_weapon_dmg% %player_sec_weapon% %player_sec_weapon_dmg% %player_level%
ping -n 1 127.0.0.1 >nul
exit
