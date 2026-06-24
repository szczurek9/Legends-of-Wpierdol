#include <clocale>
#include "globals.h"

std::string nickname;
int player_level = 1;
int player_money = 5;
int player_health = 100;
int player_maxhealth = 100;
std::string player_weapon_name = "nic";
int player_weapon_damage = 0;
bool player_used_escape = false;

int player_lifesteal = 0;
int player_bonus_health = 0;
int player_bonus_accuracy = 0;

int player_nickname_color = COLOR_WHITE;

int player_armor = 0;
int player_magic_resist = 0;
int player_armor_pen = 0;

const int MAX_LIFESTEAL = 20;
int MAX_ACCURACY = 30;
const int MAX_CRIT = 100;

bool player_second_breath = false;
int player_escape_count = 0;

int current_wave;
int total_waves;
int current_enemy_health;
int current_enemy_damage;
int current_enemy_max_health;

int player_weapon_price = 0;
int player_health_potion = 0;
int player_accuracy_potion = 0;
int player_vampirism_potion = 0;

int accuracy_potion_bonus = 0;
int accuracy_potion_turns = 0;
int vampirism_potion_bonus = 0;
int vampirism_potion_turns = 0;

int player_crit_chance = 0;

// Mana i czary
int player_mana = 100;
int player_maxmana = 100;
int player_spell_power = 0;
int player_magic_resist_pen = 0;

// Stun
int enemy_stun_turns = 0;

// Klasa postaci
PlayerClass player_class = CLASS_NONE;
int player_class_bonus_ad = 0;

// Sloty magiczne: Mag=8, reszta=4
const int MAX_MAGIC_ITEMS_MAGE = 8;
const int MAX_MAGIC_ITEMS_OTHER = 4;
int magic_item_slots[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
int magic_item_count = 0;

// Księga Adeptów stacks
int adept_book_stacks[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
bool adept_book_upgraded[8] = { false, false, false, false, false, false, false, false };

battle_enemy enemy[] = {
// ID, Name, HP, DMG, Atk, Crit, PAtk, Dge, Boss, Wve, Krd, Arm, MR, APen
    {1, "Menel spod Żabki", 30, 3, 25, 2, 95, 5, false, 1, 5, 0, 0, 0},
    {2, "Sebix z osiedla", 40, 4, 28, 2, 93, 6, false, 1, 10, 0, 0, 0},
    {3, "Dresiarz", 55, 5, 30, 3, 92, 8, false, 1, 15, 0, 0, 0},
    {4, "Sąsiad", 75, 7, 32, 3, 90, 10, false, 1, 25, 5, 0, 0},
    {5, "Kierownik Żabki", 110, 10, 38, 5, 88, 12, true, 1, 80, 10, 5, 0},
    {6, "Hazardzista", 80, 6, 34, 4, 88, 10, false, 1, 30, 5, 0, 0},
    {7, "Speedrunner bez snu", 95, 8, 36, 5, 86, 12, false, 1, 35, 8, 0, 0},
    {8, "Discord Mod", 120, 10, 38, 5, 84, 15, false, 1, 40, 10, 5, 0},
    {9, "Eggman po 3 energetykach", 140, 12, 40, 6, 82, 18, false, 1, 45, 12, 5, 0},
    {10, "Peter Griffin", 180, 15, 45, 7, 80, 20, true, 1, 100, 15, 10, 0},
    {11, "Patrick Star", 220, 18, 42, 7, 78, 22, false, 2, 45, 15, 10, 1},
    {12, "Gracz League po tiltcie", 260, 20, 44, 8, 76, 24, false, 2, 50, 18, 10, 2},
    {13, "Yone OTP", 300, 22, 46, 10, 74, 26, false, 2, 100, 20, 15, 5},
    {14, "Edgar z Brawl Stars", 340, 25, 48, 10, 72, 28, false, 2, 100, 22, 15, 6},
    {15, "Kasjerka w Żabce", 420, 30, 52, 12, 70, 30, true, 2, 150, 25, 20, 7},
    {16, "Ash Ketchum", 480, 35, 50, 12, 68, 32, false, 2, 100, 25, 20, 10},
    {17, "Gracz League po 12 rankedach", 550, 40, 52, 13, 66, 34, false, 2, 120, 28, 20, 11},
    {18, "Akali main (tilt mode)", 650, 45, 54, 15, 64, 36, false, 2, 125, 30, 25, 11},
    {19, "Dis gdy JD", 750, 50, 56, 15, 62, 38, false, 2, 150, 32, 25, 12},
    {20, "Adam Smasher z Allegro", 1000, 60, 60, 18, 60, 40, true, 2, 175, 35, 30, 15},
    {21, "Speedrunner DMC", 1200, 65, 58, 18, 58, 42, false, 2, 200, 35, 30, 16},
    {22, "Moderator Reddita", 1400, 70, 60, 20, 56, 44, false, 2, 210, 38, 30, 17},
    {23, "Silver Yasuo 0/10", 1600, 80, 62, 22, 54, 46, false, 2, 220, 40, 35, 18},
    {24, "Speedrunner Elden Ring", 2000, 90, 64, 22, 52, 48, false, 2, 220, 42, 35, 20},
    {25, "Dante z DMC", 2500, 110, 68, 25, 50, 50, true, 2, 240, 45, 40, 25},
    {26, "Irak Bomber", 3000, 120, 66, 25, 48, 52, false, 3, 230, 45, 40, 27},
    {27, "Mariusz Pudzianowski", 3800, 130, 67, 28, 46, 54, false, 3, 240, 50, 40, 27},
    {28, "Vergil z DMC", 4000, 150, 68, 30, 44, 56, false, 3, 240, 52, 45, 27},
    {29, "Chester Bernington", 4500, 170, 69, 30, 42, 58, false, 3, 240, 55, 45, 30},
    {30, "Song So-Mi (Songbird)", 5000, 200, 70, 32, 40, 60, true, 3, 275, 58, 50, 30},
    {31, "Samsung Galaxy S22", 5000, 230, 70, 32, 38, 62, false, 3, 300, 58, 50, 30},
    {32, "6pek", 6000, 260, 71, 34, 36, 64, false, 3, 300, 60, 55, 30},
    {33, "Michael Jackson", 7000, 300, 72, 35, 34, 66, false, 3, 320, 62, 55, 35},
    {34, "Gordon Freeman", 8000, 350, 73, 35, 32, 68, false, 3, 330, 65, 60, 35},
    {35, "Gabe Newell", 10000, 400, 75, 38, 30, 70, true, 3, 350, 68, 65, 35},
    {36, "Kostecka", 13000, 450, 74, 38, 28, 72, false, 3, 330, 68, 65, 40},
    {37, "Walter White", 14000, 500, 75, 40, 26, 74, false, 3, 340, 70, 70, 40},
    {38, "Owcacejk", 15000, 600, 76, 40, 24, 76, false, 3, 367, 72, 70, 40},
    {39, "Pan Ryszard", 16000, 700, 80, 40, 22, 78, false, 3, 400, 75, 75, 40},
    {40, "Legenda Wpierdolu", 15000, 650, 90, 40, 20, 80, true, 5, 500, 80, 80, 50},
    {41, "Poman Rytka (prime)", 17000, 750, 80, 45, 20, 80, false, 4, 525, 85, 75, 45},
    {42, "Torta", 18000, 800, 80, 45, 20, 75, false, 4, 550, 90, 90, 45},
    {43, "Bigos", 18500, 850, 81, 40, 19, 80, false, 4, 575, 90, 100, 50},
    {44, "Kassadin (L9)", 19000, 875, 83, 50, 18, 81, false, 4, 600, 95, 99, 50},
    {45, "Nero z DMC", 18250, 800, 85, 50, 18, 82, true, 5, 650, 100, 100, 55},
    {46, "Malzahar (outplay mode)", 21370, 925, 85, 50, 18, 82, false, 4, 700, 99, 100, 55},
    {47, "Byczkowska", 22000, 950, 86, 52, 17, 83, false, 4, 725, 110, 110, 56},
    {48, "Benek", 23000, 975, 87, 53, 17, 83, false, 4, 750, 115, 105, 57},
    {49, "Błażej", 24000, 999, 89, 54, 16, 84, false, 4, 775, 120, 115, 59},
    {50, "Pobłocki po szlugu", 22500, 900, 90, 55, 15, 85, true, 5, 800, 125, 125, 60} };

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
    {"Shurikeny Akali", 1200, 527},
    {"Yamato", 1650, 700},
    {"Ebony & Ivory", 2000, 820},
    {"Mantis Blades", 2700, 930},
    {"Malorian 3516", 3000, 1000},
    {"GTX 1080 TI", 3600, 1800},
    {"Nokia 3310", 4800, 3310},
    {"Desert Eagle", 5000, 4000},
    {"RAM DDR4", 6700, 5444},
    {"AWP Dragonlore", 7777, 6767},
    {"Red Bull GreatSword", 10000, 8120},
    {"Lightsaber", 12000, 9800},
    {"Devil Sword Dante", 15000, 12255} };

upgrade_item upgrades[] = {
    {"Skała Zdrowia", 180, "   +50 maksymalnego ♥"},
    {"Woda Życia", 560, "   +160 maksymalnego ♥"},
    {"Kolczuga", 220, "   +15 pancerza"},
    {"Kolce Odwetu", 350, "   +10 przebicia pancerza przeciwnika (maksymalnie 70)"} };

skill_item abilities[] = {
    {"Wampiryczne Ostrze", 580, "   Leczy za 10% zadanych obrażeń [Fizyczne] (maksymalnie 20%)"},
    {"Kryształ Skupienia", 420, "   15% mniejsza szansa na nietrafienie ataku (maksymalnie 30%)"},
    {"Pierścień Zabójcy", 500, "   10% szansy na trafienie krytyczne (do 50%)"},
    {"Płaszcz Assasyna", 1000, "   10% szansy na trafienie krytyczne (powyżej 50%)"},
    {"Drugie Tchnienie", 275, "   Pokonanie przeciwnika leczy cię o 25% maksymalnego ♥ i many"},
    {"Dłonie Wampira", 520, "   Leczysz się 15% zadawanych obrażeń [Magiczne], limit 30%"} };

consumable_item consumables[] = {
    {"Mikstura Zdrowia", 60, "   Leczy 25 + 7% maksymalnego ♥ podczas walki (maksymalnie leczy ♥ 250)"},
    {"Eliksir Precyzji", 120, "   Zwiększa twoją szanse na atak o 50% na 7 tur (nie stackuje się)"},
    {"Koktajl Wampira", 150, "   Leczysz się za 10% zadanych obrażeń na 10 tur (nie stackuje się)"} };

// Przedmioty magiczne
// Pola: name, price, mana, ap, mrpen, mr, regen_bonus, desc, discount_eligible,
//        adept_ap_per_stack, adept_ap_max, adept_ap_upgraded_max, adept_upgrade_price, ap_multiplier
magic_item_def magic_items[] = {
    {"Zaklęta Księga", 300, 100, 50, 5, 0, 50,
     "   +100 many, +50 siły zaklęć, +5 przebicia MR, +50% bazowej regeneracji many",
     true, 0, 0, 0, 0, 1.0f},

    {"Różdżka Zniszczenia", 500, 300, 120, 15, 0, 0,
     "   +180 siły zaklęć, +300 many, +15 przebicia MR",
     true, 0, 0, 0, 0, 1.0f},

    {"Naszyjnik Uzdrowicielki", 220, 100, 5, 0, 0, 280,
     "   +100 many, +5 siły zaklęć, +280% bazowej regeneracji many",
     true, 0, 0, 0, 0, 1.0f},

    {"Tarcza Anioła", 450, 0, 0, 0, 30, 150,
     "   +30 odporności magicznej, +150% bazowej regeneracji many",
     true, 0, 0, 0, 0, 1.0f},

     // Księga Adeptów: +5 AP/spell, limit 30 AP; upgrade za $55 -> limit 150 AP
     {"Księga Adeptów", 30, 0, 0, 0, 0, 0,
      "   +5 AP za każdy użyty spell (do 30 AP). Ulepszenie: limit 150 AP ($55)",
      true, 5, 30, 150, 55, 1.0f},

      // Kostur Zagłady: +20% całego AP (mnożnik), brak zniżki
      {"Kostur Zagłady", 600, 0, 160, 30, 0, 0,
       "   +160 siły zaklęć, +30 przebicia MR, +20% całego AP (mnożnik końcowy)",
       false, 0, 0, 0, 0, 1.20f},
};

const int enemy_amount = sizeof(enemy) / sizeof(enemy[0]);
const int shop_weapon_amount = sizeof(shop) / sizeof(shop[0]);
const int upgrades_amount = sizeof(upgrades) / sizeof(upgrades[0]);
const int abilities_amount = sizeof(abilities) / sizeof(abilities[0]);
const int consumables_amount = sizeof(consumables) / sizeof(consumables[0]);
const int magic_items_amount = sizeof(magic_items) / sizeof(magic_items[0]);

InventoryWeapon inventory[max_inventory];
int inventory_count = 0;

// Overkill
int overkill_stored = 0;
bool overkill_armor_ignore = false;

// Wampiryzm magiczny
int player_spell_vamp = 0;
const int MAX_SPELL_VAMP = 30;

// Cooldowny Zabójcy
int cd_primal_strike = 0;
int cd_undodgeable_speed = 0;
int cd_slayer_of_slowest = 0;

// Cooldowny Maga
int cd_star_strike = 0;
int cd_deadly_vines = 0;
int cd_mirror_of_death = 0;

// Aktywne efekty Zabójcy
bool primal_strike_active = false;
int undodgeable_turns = 0;

// Mirror of Death
bool mirror_active = false;

// ===== TANK =====
int cd_stone_bastion = 0;
int cd_accumulated_wrath = 0;
int cd_iron_taunt = 0;
int tank_bastion_armor_turns = 0;
int tank_bastion_armor_bonus = 0;
int tank_wrath_stored = 0;
int tank_taunt_turns = 0;

// ===== SAMURAJ =====
int cd_kogeki_strike = 0;
int samurai_attack_counter = 0;
bool sen_no_kata_active = false;
int cd_mushin_ability = 0;
bool mushin_shield_active = false;
