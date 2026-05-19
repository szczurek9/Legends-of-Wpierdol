#include <clocale>
#include "globals.h"

string nickname;
int player_level = 1;
int player_money = 5;
int player_health = 100;
int player_maxhealth = 100;
string player_weapon_name = "nic";
int player_weapon_damage = 0;
bool player_used_escape = false;

int player_lifesteal = 0;
int player_bonus_health = 0;
int player_bonus_accuracy = 0;

const int MAX_LIFESTEAL = 20;
const int MAX_ACCURACY = 30;

int current_wave;
int total_waves;
int current_enemy_health;

battle_enemy enemy[] = {
    {1, "Menel spod Żabki", 30, 3, 25, 95, 5, 1, 5},
    {2, "Sebix z osiedla", 40, 4, 28, 93, 6, 1, 10},
    {3, "Dresiarz", 55, 5, 30, 92, 8, 1, 12},
    {4, "Sąsiad", 75, 7, 32, 90, 10, 1, 21},
    {5, "Kierownik Żabki", 110, 10, 38, 88, 12, 1, 100},
    {6, "Hazardzista", 80, 6, 34, 88, 10, 1, 30},
    {7, "Speedrunner bez snu", 95, 8, 36, 86, 12, 1, 30},
    {8, "Discord Mod", 120, 10, 38, 84, 15, 1, 10},
    {9, "Eggman po 3 energetykach", 140, 12, 40, 82, 18, 1, 5},
    {10, "Peter Griffin", 180, 15, 45, 80, 20, 1, 100},
    {11, "Patrick Star", 220, 18, 42, 78, 22, 2, 30},
    {12, "Gracz League po tiltcie", 260, 20, 44, 76, 24, 2, 50},
    {13, "Yone OTP", 300, 22, 46, 74, 26, 2, 160},
    {14, "Edgar z Brawl Stars", 340, 25, 48, 72, 28, 2, 80},
    {15, "Kasjerka w Żabce", 420, 30, 52, 70, 30, 2, 100},
    {16, "Ash Ketchum", 480, 35, 50, 68, 32, 2, 110},
    {17, "Gracz League po 12 rankedach", 550, 40, 52, 66, 34, 2, 200},
    {18, "Akali main (tilt mode)", 650, 45, 54, 64, 36, 2, 210},
    {19, "Dis gdy JD", 750, 50, 56, 62, 38, 2, 150},
    {20, "Adam Smasher z Allegro", 900, 60, 60, 60, 40, 2, 500},
    {21, "Speedrunner DMC", 1000, 65, 58, 58, 42, 2, 500},
    {22, "Moderator Reddita", 1150, 70, 60, 56, 44, 2, 650},
    {23, "Silver Yasuo 0/10", 1300, 80, 62, 54, 46, 2, 800},
    {24, "Speedrunner Dark Souls", 1500, 90, 64, 52, 48, 2, 1000},
    {25, "Dante z DMC", 1800, 110, 68, 50, 50, 2, 1300},
    {26, "Irak Bomber", 2100, 120, 66, 48, 52, 3, 1500},
    {27, "Mariusz Pudzianowski", 2400, 130, 67, 46, 54, 3, 2000},
    {28, "Vergil z DMC", 2800, 150, 68, 44, 56, 3, 2200},
    {29, "Chester Bernington", 3200, 170, 69, 42, 58, 3, 2500},
    {30, "Song So-Mi (Songbird)", 3800, 200, 70, 40, 60, 3, 3000},
    {31, "Samsung Galaxy S22", 4500, 230, 70, 38, 62, 3, 3500},
    {32, "6pek", 5200, 260, 71, 36, 64, 3, 4000},
    {33, "Michael Jackson", 6000, 300, 72, 34, 66, 3, 5000},
    {34, "Gordon Freeman", 7000, 350, 73, 32, 68, 3, 5100},
    {35, "Gabe Newell", 8200, 400, 75, 30, 70, 3, 5500},
    {36, "Kostecka", 9500, 450, 74, 28, 72, 3, 5000},
    {37, "Walter White", 11000, 500, 75, 26, 74, 3, 6000},
    {38, "Owcacejk", 12500, 600, 76, 24, 76, 3, 6767},
    {39, "Pan Ryszard", 15000, 700, 77, 22, 78, 3, 7000},
    {40, "Legenda Wpierdolu", 20000, 900, 80, 20, 80, 5, 10000}
};

shop_weapons shop[] = {
    {"Patyk", 3, 3},
    {"Trzon od siekiery", 10, 6},
    {"Zardzewiała Rura", 15, 8},
    {"Gazrurka", 20, 10},
    {"Nóż kuchenny", 50, 15},
    {"Maczeta", 70, 26},
    {"Zardzewiała Katana", 120, 50},
    {"Żelazny Miecz", 200, 71},
    {"Baseball z gwoździami", 260, 86},
    {"Katana", 320, 120},
    {"Pistolet Jhin'a", 444, 224},
    {"Latarnia Thresha", 667, 306},
    {"Miecze Yone", 850, 390},
    {"Młot Mordekaisera", 1200, 475},
    {"Shurikeny Akali", 1600, 527},
    {"Red Queen", 2300, 700},
    {"Ebony & Ivory", 5000, 820},
    {"Mantis Blades", 6500, 930},
    {"Malorian 3516", 10000, 1000},
    {"GTX 1080 TI", 18000, 1881},
    {"Nokia 3310", 33100, 3331}
};

skill_upgrade skills[] = {
    {"Wampiryczne Ostrze", 600, "	Leczy za 10% zadanych obrazen (maksymalnie 20%)"},
    {"Skała Zdrowia", 200, "	+50 maksymalnego HP"},
    {"Woda Życia", 800, "	+210 maksymalnego HP"},
    {"Kryształ Skupienia", 450, "	15% mniejsza szansa na nietrafienie ataku (maksymalnie 30%)"}
};

const int enemy_amount = sizeof(enemy) / sizeof(enemy[0]);
const int shop_weapon_amount = sizeof(shop) / sizeof(shop[0]);
const int skills_amount = sizeof(skills) / sizeof(skills[0]);