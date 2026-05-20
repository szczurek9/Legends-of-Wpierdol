#pragma once
#include <string>
using namespace std;

// Struktury
struct shop_weapons {
    string name;
    int price;
    int damage;
};

struct skill_upgrade {
    string name;
    int price;
    string description;
};

struct battle_enemy {
    int lvl;
    string name;
    int health;
    int damage;
    int atk_chance;
    int player_atk_chance;
    int dodge_chance;
    int wave;
    int kill_reward;
};

struct InventoryWeapon {
    string name;
    int damage;
    int price;
};

// Zmienne gracza
extern string nickname;
extern int player_level;
extern int player_money;
extern int player_health;
extern int player_maxhealth;
extern string player_weapon_name;
extern int player_weapon_damage;
extern bool player_used_escape;

extern int player_weapon_price;

// Wzmocnienia
extern int player_lifesteal;
extern int player_bonus_health;
extern int player_bonus_accuracy;

extern const int MAX_LIFESTEAL;
extern const int MAX_ACCURACY;

extern int player_armor;

extern bool player_escape_master;
extern int player_escape_count;

extern int player_health_potion;

// Fale
extern int current_wave;
extern int total_waves;
extern int current_enemy_health;

// Tablice
extern battle_enemy enemy[];
extern shop_weapons shop[];
extern skill_upgrade skills[];

extern const int enemy_amount;
extern const int shop_weapon_amount;
extern const int skills_amount;

const int max_inventory = 50;

extern InventoryWeapon inventory[max_inventory];

extern int inventory_count;