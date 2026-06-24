#pragma once
#include <string>

// Struktury
struct shop_weapons
{
	std::string name;
	int price;
	int damage;
};

struct battle_enemy
{
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
	int armor;        // pancerz przeciwnika (redukcja fizycznych obrażeń gracza)
	int magic_resist; // odporność magiczna przeciwnika
	int armor_pen;    // przebicie pancerza gracza przez przeciwnika
};

struct InventoryWeapon
{
	std::string name;
	int damage;
	int price;
};

struct upgrade_item
{
	std::string name;
	int price;
	std::string description;
};

struct skill_item
{
	std::string name;
	int price;
	std::string description;
};

struct consumable_item
{
	std::string name;
	int price;
	std::string description;
};

// Przedmiot magiczny
struct magic_item_def
{
	std::string name;
	int price;
	int bonus_mana;
	int bonus_spell_power;
	int magic_resist_pen;
	int bonus_magic_resist;
	int base_mana_regen_bonus;
	std::string description;
	bool discount_eligible; // czy Mag dostaje zniżkę
	// Księga Adeptów: stackujące AP
	int adept_ap_per_stack;    // AP za stack (0 = nie jest Księgą)
	int adept_ap_max;          // bazowy limit stacków AP (0 = nie dotyczy)
	int adept_ap_upgraded_max; // limit po upgrade (0 = nie dotyczy)
	int adept_upgrade_price;   // cena upgrade (0 = brak)
	// Kostur Zagłady: mnożnik AP
	float ap_multiplier; // np. 1.20f = +20% AP (1.0 = brak efektu)
};

enum ConsoleColor
{
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
enum PlayerClass
{
	CLASS_NONE = 0,
	CLASS_ASSASSIN = 1,
	CLASS_MAGE = 2,
	CLASS_TANK = 3,
	CLASS_SAMURAI = 4
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
extern int MAX_ACCURACY;
extern const int MAX_CRIT;

extern int player_armor;
extern int player_magic_resist;
extern int player_armor_pen;

extern bool player_second_breath;
extern int player_escape_count;

extern int player_health_potion;
extern int player_crit_chance;

// Mana i czary
extern int player_mana;
extern int player_maxmana;
extern int player_spell_power;
extern int player_magic_resist_pen;

// Stun
extern int enemy_stun_turns;

// Klasa postaci
extern PlayerClass player_class;
extern int player_class_bonus_ad;

// Sloty na przedmioty magiczne
// Mag: 8 slotów, reszta: 4
extern const int MAX_MAGIC_ITEMS_MAGE;
extern const int MAX_MAGIC_ITEMS_OTHER;
extern int magic_item_slots[8];
extern int magic_item_count;

// Księga Adeptów: stacked AP (per slot, maks 8 slotów)
extern int adept_book_stacks[8];    // ile stacków zebrał item w danym slocie
extern bool adept_book_upgraded[8]; // czy item w danym slocie jest ulepszony

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

extern int player_accuracy_potion;
extern int player_vampirism_potion;

extern int accuracy_potion_bonus;
extern int accuracy_potion_turns;

extern int vampirism_potion_bonus;
extern int vampirism_potion_turns;

// Overkill
extern int overkill_stored;
extern bool overkill_armor_ignore;

// Wampiryzm magiczny (Spell Vamp)
extern int player_spell_vamp;
extern const int MAX_SPELL_VAMP;

// Cooldowny spelli Zabójcy
extern int cd_primal_strike;
extern int cd_undodgeable_speed;
extern int cd_slayer_of_slowest;

// Cooldowny spelli Maga
extern int cd_star_strike;
extern int cd_deadly_vines;
extern int cd_mirror_of_death;

// Aktywne efekty Zabójcy
extern bool primal_strike_active;
extern int undodgeable_turns;

// Mirror of Death
extern bool mirror_active;

// ===== TANK spell state =====
extern int cd_stone_bastion;
extern int cd_accumulated_wrath;
extern int cd_iron_taunt;
extern int tank_bastion_armor_turns; // ile tur pozostało efektu Bastionu
extern int tank_bastion_armor_bonus; // aktualny bonus armoru z Bastionu
extern int tank_wrath_stored;        // skumulowane obrażenia
extern int tank_taunt_turns;         // ile tur Prowokacja aktywna

// ===== SAMURAJ spell/passive state =====
extern int cd_kogeki_strike;
extern int samurai_attack_counter; // licznik ataków do pasywki (co 3. = stun)
extern bool sen_no_kata_active;    // toggle Sen no Kata
extern int cd_mushin_ability;
extern bool mushin_shield_active; // aktywna redukcja Mushin (do następnego ataku wroga)

