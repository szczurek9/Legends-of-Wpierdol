#pragma once
#include <string>
using namespace std;

// Struktury
struct shop_weapons {
    string name;
    int price;
    int damage;
};

struct battle_enemy {
    int lvl;
    string name;
    int health;
    int damage;
    int atk_chance;
    int crit_chance;
    int player_atk_chance;
    int dodge_chance;
    bool boss;
    int wave;
    int kill_reward;
};

struct InventoryWeapon {
    string name;
    int damage;
    int price;
};

struct upgrade_item {
    string name;
    int price;
    string description;
};

struct skill_item {
    string name;
    int price;
    string description;
};

struct consumable_item {
    string name;
    int price;
    string description;
};


enum ConsoleColor {

    COLOR_BLACK = 0,

    COLOR_DARK_BLUE = 1,
    COLOR_DARK_GREEN = 2,
    COLOR_DARK_CYAN = 3,
    COLOR_DARK_RED = 4,
    COLOR_DARK_PURPLE = 5,
    COLOR_DARK_YELLOW = 6,
    COLOR_WHITE = 7,
    COLOR_GRAY = 8,

    COLOR_BLUE = 9,
    COLOR_GREEN = 10,
    COLOR_CYAN = 11,
    COLOR_RED = 12,
    COLOR_PURPLE = 13,
    COLOR_YELLOW = 14,
    COLOR_BRIGHT_WHITE = 15
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

extern int player_nickname_color;

// Wzmocnienia
extern int player_lifesteal;
extern int player_bonus_health;
extern int player_bonus_accuracy;

extern const int MAX_LIFESTEAL;
extern const int MAX_ACCURACY;
extern const int MAX_CRIT;

extern int player_armor;

extern bool player_second_breath;
extern int player_escape_count;

extern int player_health_potion;

extern int player_crit_chance;

// Fale
extern int current_wave;
extern int total_waves;
extern int current_enemy_health;
extern int current_enemy_damage;
extern int current_enemy_max_health;
// Tablice
extern battle_enemy enemy[];
extern shop_weapons shop[];

extern const int enemy_amount;
extern const int shop_weapon_amount;

extern upgrade_item upgrades[];
extern skill_item abilities[];
extern consumable_item consumables[];

extern const int upgrades_amount;
extern const int abilities_amount;
extern const int consumables_amount;

const int max_inventory = 50;

extern InventoryWeapon inventory[max_inventory];

extern int inventory_count;

extern int player_precision_potion;
extern int player_vampire_potion;

extern int precision_bonus;
extern int precision_turns;

extern int vampire_bonus;
extern int vampire_turns;

