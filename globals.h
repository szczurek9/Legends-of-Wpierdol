#pragma once
#include <string>

// Struktury
struct shop_weapons {
    std::string name;
    int price;
    int damage;
};

struct battle_enemy {
    int lvl;
    std::string name;
    int health;
    int damage;
    int atk_chance;
    int crit_chance;
    int player_atk_chance;
    int dodge_chance;
    bool boss;
    int wave;
    int kill_reward;
    int armor;          // pancerz przeciwnika (redukcja fizycznych obrażeń gracza)
    int magic_resist;   // odporność magiczna przeciwnika
    int armor_pen;      // przebicie pancerza gracza przez przeciwnika
};

struct InventoryWeapon {
    std::string name;
    int damage;
    int price;
};

struct upgrade_item {
    std::string name;
    int price;
    std::string description;
};

struct skill_item {
    std::string name;
    int price;
    std::string description;
};

struct consumable_item {
    std::string name;
    int price;
    std::string description;
};

// Przedmiot magiczny (std::max 6 slotów)
struct magic_item_def {
    std::string name;
    int price;
    int bonus_mana;
    int bonus_spell_power;
    int magic_resist_pen;  // przebicie odporności magicznej przeciwnika
    int bonus_magic_resist; // odporność magiczna gracza
    int base_mana_regen_bonus; // bonus do bazowej regeneracji many (%)
    std::string description;
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

// Klasy postaci
enum PlayerClass {
    CLASS_NONE = 0,
    CLASS_ASSASSIN = 1,  // Zabójca
    CLASS_MAGE = 2,      // Mag
    CLASS_TANK = 3,      // Tank
    CLASS_WARRIOR = 4    // Wojownik
};

// Zmienne gracza
extern std::string nickname;
extern int player_level;
extern int player_money;
extern int player_health;
extern int player_maxhealth;
extern std::string player_weapon_name;
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
extern int player_magic_resist;         // odporność magiczna gracza
extern int player_armor_pen;            // przebicie pancerza przeciwnika przez gracza
// enemy_armor_pen jest teraz polem w strukturze battle_enemy (per wróg)

extern bool player_second_breath;
extern int player_escape_count;

extern int player_health_potion;

extern int player_crit_chance;

// Mana i czary
extern int player_mana;
extern int player_maxmana;
extern int player_spell_power;          // siła zaklęć
extern int player_magic_resist_pen;     // przebicie odporności magicznej przeciwnika

// Stun
extern int enemy_stun_turns;           // liczba tur ogłuszenia przeciwnika

// Klasa postaci
extern PlayerClass player_class;

// Bonus AD z klasy (osobny od broni, nie nadpisywany przez zakup)
extern int player_class_bonus_ad;

// Sloty na przedmioty magiczne (std::max 6)
extern const int MAX_MAGIC_ITEMS;
extern int magic_item_slots[10];        // indeksy założonych itemów (-1 = pusty slot)
extern int magic_item_count;           // ile przedmiotów założonych

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
extern magic_item_def magic_items[];

extern const int upgrades_amount;
extern const int abilities_amount;
extern const int consumables_amount;
extern const int magic_items_amount;

const int max_inventory = 50;

extern InventoryWeapon inventory[max_inventory];

extern int inventory_count;

extern int player_precision_potion;
extern int player_vampire_potion;

extern int precision_bonus;
extern int precision_turns;

extern int vampire_bonus;
extern int vampire_turns;

// Overkill (nadmiarowe obrażenia przechowane)
extern int overkill_stored;          // przechowane obrażenia Overkill
extern bool overkill_armor_ignore;   // czy następny atak ignoruje 80% pancerza

// Wampiryzm magiczny (Spell Vamp)
extern int player_spell_vamp;        // % leczenia z obrażeń magicznych (std::max 30)
extern const int MAX_SPELL_VAMP;

// Cooldowny spelli Zabójcy
extern int cd_primal_strike;         // 0 = gotowy (brak CD)
extern int cd_undodgable_speed;      // tury do końca cooldownu
extern int cd_slayer_of_slowest;     // tury do końca cooldownu

// Cooldowny spelli Maga
extern int cd_star_strike;
extern int cd_deadly_vines;
extern int cd_mirror_of_death;
// Powiew Burzy i Primal Strike: brak CD

// Aktywne efekty spelli Zabójcy
extern bool primal_strike_active;    // następny atak zawsze krytuje
extern int undodgable_turns;         // ile tur pozostało efektu
extern bool slayer_next_active;      // następny atak Pogromcy gotowy

// Mirror of Death: flaga gotowości (aktywowana przez spell, wypalana przy ataku wroga)
extern bool mirror_active;
