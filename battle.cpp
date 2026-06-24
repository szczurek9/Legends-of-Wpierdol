#include "battle.h"
#include "globals.h"
#include "utils.h"
#include "save.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>

// ===== POMOCNICZE: aktualny limit slotów magicznych =====
int get_max_magic_items()
{
	return (player_class == CLASS_MAGE) ? MAX_MAGIC_ITEMS_MAGE : MAX_MAGIC_ITEMS_OTHER;
}

// ===== POMOCNICZE: AP gracza (suma bazowa * Kostur Zagłady) =====
int get_total_ap()
{
	// Zsumuj base AP z itemów i stacków Księgi Adeptów
	int base_ap = player_spell_power; // już zawiera static AP z itemów
	// Dodaj AP ze stacków Ksiąg (adept_book_stacks przechowuje stacked AP per slot)
	int stacked_ap = 0;
	int max_slots = get_max_magic_items();
	for (int i = 0; i < max_slots; i++)
	{
		if (magic_item_slots[i] >= 0)
		{
			int idx = magic_item_slots[i];
			if (magic_items[idx].adept_ap_per_stack > 0)
			{
				stacked_ap += adept_book_stacks[i];
			}
		}
	}
	int total = base_ap + stacked_ap;
	// Kostur Zagłady: sprawdź czy założony
	float multiplier = 1.0f;
	for (int i = 0; i < max_slots; i++)
	{
		if (magic_item_slots[i] >= 0)
		{
			multiplier *= magic_items[magic_item_slots[i]].ap_multiplier;
		}
	}
	return (int)(total * multiplier);
}

// ===== POMOCNICZE: regeneracja many =====
void regen_mana()
{
	int base_regen_bonus_pct = 0;
	int max_slots = get_max_magic_items();
	for (int i = 0; i < max_slots; i++)
	{
		if (magic_item_slots[i] >= 0)
		{
			base_regen_bonus_pct += magic_items[magic_item_slots[i]].base_mana_regen_bonus;
		}
	}
	if (player_class == CLASS_MAGE)
		base_regen_bonus_pct += 125;
	int base_regen = (int)(player_maxmana * 0.03f);
	int bonus_regen = (base_regen * base_regen_bonus_pct) / 100;
	player_mana += base_regen + bonus_regen;
	if (player_mana > player_maxmana)
		player_mana = player_maxmana;
}

// ===== POMOCNICZE: Księga Adeptów — dodaj stack po użyciu spella =====
// ap_override != 0 -> użyj tej wartości zamiast adept_ap_per_stack (dla klas innych niż Mag)
void adept_book_on_spell(int ap_override)
{
	int max_slots = get_max_magic_items();
	for (int i = 0; i < max_slots; i++)
	{
		if (magic_item_slots[i] < 0)
			continue;
		int idx = magic_item_slots[i];
		if (magic_items[idx].adept_ap_per_stack <= 0)
			continue;
		// Limit stacków
		int ap_limit = adept_book_upgraded[i]
			? magic_items[idx].adept_ap_upgraded_max
			: magic_items[idx].adept_ap_max;
		if (adept_book_stacks[i] < ap_limit)
		{
			int gain = (ap_override > 0) ? ap_override : magic_items[idx].adept_ap_per_stack;
			adept_book_stacks[i] += gain;
			if (adept_book_stacks[i] > ap_limit)
				adept_book_stacks[i] = ap_limit;
		}
	}
}

void clamp_player_armor_pen()
{
	if (player_armor_pen > 70)
		player_armor_pen = 70;
}

// ===== POMOCNICZE: tick cooldownów =====
void tick_cooldowns()
{
	if (cd_primal_strike > 0)
		cd_primal_strike--;
	if (cd_undodgeable_speed > 0)
		cd_undodgeable_speed--;
	if (cd_slayer_of_slowest > 0)
		cd_slayer_of_slowest--;
	if (cd_star_strike > 0)
		cd_star_strike--;
	if (cd_deadly_vines > 0)
		cd_deadly_vines--;
	if (cd_mirror_of_death > 0)
		cd_mirror_of_death--;
	if (cd_stone_bastion > 0)
		cd_stone_bastion--;
	if (cd_accumulated_wrath > 0)
		cd_accumulated_wrath--;
	if (cd_iron_taunt > 0)
		cd_iron_taunt--;
	if (cd_kogeki_strike > 0)
		cd_kogeki_strike--;
	if (cd_mushin_ability > 0)
		cd_mushin_ability--;
	if (undodgeable_turns > 0)
		undodgeable_turns--;
	// Bastion armor effect tick: akumuluj bonus przez czas trwania
	// Bonus NIE zeruje się tu — znika dopiero po kolejnym ataku wroga (do_enemy_attack)
	if (tank_bastion_armor_turns > 0)
	{
		tank_bastion_armor_bonus += (int)(player_armor * 0.02f);
		tank_bastion_armor_turns--;
		if (tank_bastion_armor_turns == 0)
			cd_stone_bastion = 1; // CD 1 tura po zakończeniu efektu
	}
	// Prowokacja: CD 2 tury startuje po wygaśnięciu efektu
	if (tank_taunt_turns > 0)
	{
		tank_taunt_turns--;
		if (tank_taunt_turns == 0)
			cd_iron_taunt = 2;
	}
}

// ===== POMOCNICZE: Spell Vamp =====
void apply_spell_vamp(int raw_magic_damage)
{
	if (player_spell_vamp <= 0)
		return;
	int heal = (raw_magic_damage * player_spell_vamp) / 100;
	if (heal < 1)
		return;
	player_health += heal;
	if (player_health > player_maxhealth)
		player_health = player_maxhealth;
	std::cout << "\t";
	print_colored("Dłonie Wampira uzdrowiły: ", COLOR_PURPLE);
	number_colored(heal, COLOR_GREEN);
	print_colored(" HP!\n", COLOR_PURPLE);
}

// ===== POMOCNICZE: zadaj obrażenia magiczne =====
int deal_magic_damage(int raw_damage, int enemyIndex)
{
	int effective_mr = enemy[enemyIndex].magic_resist - player_magic_resist_pen;
	if (effective_mr < 0)
		effective_mr = 0;
	float mr_reduction = effective_mr / (effective_mr + 100.0f);
	int dealt = (int)(raw_damage * (1.0f - mr_reduction));
	if (dealt < 1)
		dealt = 1;
	if (dealt > current_enemy_health)
		dealt = current_enemy_health;
	return dealt;
}

// ===== POMOCNICZE: nagroda za falę =====
void handle_wave_kill(int enemyIndex, float battle_seconds)
{
	float multiplier = 1.0f;
	if (battle_seconds < 8)
		multiplier = 1.15f;
	else if (battle_seconds < 15)
		multiplier = 1.05f;

	if (player_level > 15)
		multiplier = 1.0f;

	int base_reward = enemy[enemyIndex].kill_reward;
	int final_reward = (int)(base_reward * multiplier);
	int bonus_money = final_reward - base_reward;
	player_money += final_reward;

	if (player_class == CLASS_ASSASSIN)
	{
		int mana_restore = (int)(player_maxmana * 0.10f);
		player_mana += mana_restore;
		if (player_mana > player_maxmana)
			player_mana = player_maxmana;
	}

	if (player_class == CLASS_MAGE)
	{
		int mana_restore = (int)(player_maxmana * 0.30f);
		player_mana += mana_restore;
		if (player_mana > player_maxmana)
			player_mana = player_maxmana;
	}

	clear_screen();
	std::cout << "\n\t";
	print_colored("Przeciwnik pokonany!", COLOR_GREEN);
	std::cout << "\n\n\t";
	print_colored("Otrzymujesz: $", COLOR_DARK_GREEN);
	number_colored(base_reward, COLOR_DARK_GREEN);
	print_colored(" + $", COLOR_YELLOW);
	number_colored(bonus_money, COLOR_YELLOW);
	print_colored(" za czas walki!\n\t", COLOR_BRIGHT_WHITE);
	print_colored("Suma: $", COLOR_GREEN);
	number_colored(final_reward, COLOR_GREEN);
	std::cout << std::endl;

	enemy_stun_turns = 0;
	// Reset Tanka po fali
	tank_bastion_armor_turns = 0;
	tank_bastion_armor_bonus = 0;
	tank_taunt_turns = 0;
	tank_wrath_stored = 0;
	// Reset Samuraja
	samurai_attack_counter = 0;
	mushin_shield_active = false;

	if (player_second_breath)
	{
		int healAmount = (int)(player_maxhealth * 0.25f);
		int manaAmount = (int)(player_maxmana * 0.25f);
		player_health += healAmount;
		player_mana += manaAmount;
		if (player_health > player_maxhealth)
			player_health = player_maxhealth;
		if (player_mana > player_maxmana)
			player_mana = player_maxmana;
		std::cout << "\n\t";
		ui_colored("Drugie Tchnienie przywróciło: ", healAmount, COLOR_GREEN);
		print_colored(" HP!\n\t", COLOR_GREEN);
		ui_colored("Drugie Tchnienie przywróciło: ", manaAmount, COLOR_BLUE);
		print_colored(" many!\n", COLOR_BLUE);
	}
}

// =================================================================
//  CZARY MAGA
// =================================================================

void cast_storm_breeze(int enemyIndex)
{
	const int cost = 25;
	if (player_mana < cost)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Za mało many! (25)\n\n", COLOR_RED);
		pause_game();
		return;
	}
	player_mana -= cost;
	int ap = get_total_ap();
	int raw = (int)(15 + ap * 0.25f);
	int dealt = deal_magic_damage(raw, enemyIndex);
	current_enemy_health -= dealt;
	adept_book_on_spell();
	clear_screen();
	std::cout << "\n\t";
	print_colored("Rzucono: Powiew Burzy!\n\t", COLOR_CYAN);
	print_colored("Zadano ", COLOR_GREEN);
	number_colored(dealt, COLOR_RED);
	std::cout << " obrażeń magicznych!\n";
	apply_spell_vamp(dealt);
	std::cout << "\t";
	print_colored("HP wroga: ", COLOR_BRIGHT_WHITE);
	number_colored(current_enemy_health, COLOR_RED);
	std::cout << "\n\n";
	pause_game();
}

void cast_star_strike(int enemyIndex)
{
	const int cost = 100;
	if (player_mana < cost)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Za mało many! (100)\n\n", COLOR_RED);
		pause_game();
		return;
	}
	if (cd_star_strike > 0)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Gwiezdne Uderzenie na CD! (", COLOR_RED);
		number_colored(cd_star_strike, COLOR_YELLOW);
		print_colored(" tur)\n\n", COLOR_RED);
		pause_game();
		return;
	}
	player_mana -= cost;
	cd_star_strike = 1;
	int ap = get_total_ap();
	int raw = (int)(60 + ap * 0.90f);
	int dealt = deal_magic_damage(raw, enemyIndex);
	current_enemy_health -= dealt;
	adept_book_on_spell();
	clear_screen();
	std::cout << "\n\t";
	print_colored("Rzucono: Gwiezdne Uderzenie!\n\t", COLOR_YELLOW);
	print_colored("POTĘŻNY cios! Zadano ", COLOR_YELLOW);
	number_colored(dealt, COLOR_RED);
	std::cout << " obrażeń!\n";
	apply_spell_vamp(dealt);
	std::cout << "\t";
	print_colored("HP wroga: ", COLOR_BRIGHT_WHITE);
	number_colored(current_enemy_health, COLOR_RED);
	std::cout << "\n\n";
	pause_game();
}

void cast_deadly_vines(int enemyIndex)
{
	const int cost = 70;
	if (player_mana < cost)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Za mało many! (70)\n\n", COLOR_RED);
		pause_game();
		return;
	}
	if (cd_deadly_vines > 0)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Zabójcze Pnącza na CD! (", COLOR_RED);
		number_colored(cd_deadly_vines, COLOR_YELLOW);
		print_colored(" tur)\n\n", COLOR_RED);
		pause_game();
		return;
	}
	if (enemy_stun_turns > 0)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Przeciwnik już ogłuszony!\n\n", COLOR_YELLOW);
		pause_game();
		return;
	}
	player_mana -= cost;
	cd_deadly_vines = 4;
	int ap = get_total_ap();
	int raw = (int)(10 + ap * 0.10f);
	int dealt = deal_magic_damage(raw, enemyIndex);
	current_enemy_health -= dealt;
	enemy_stun_turns = 3;
	adept_book_on_spell();
	clear_screen();
	std::cout << "\n\t";
	print_colored("Rzucono: Zabójcze Pnącza!\n\t", COLOR_DARK_GREEN);
	print_colored("Zadano ", COLOR_GREEN);
	number_colored(dealt, COLOR_RED);
	std::cout << " obrażeń magicznych!\n";
	apply_spell_vamp(dealt);
	std::cout << "\t";
	print_colored("Przeciwnik ogłuszony na 3 tury!\n\t", COLOR_YELLOW);
	print_colored("HP wroga: ", COLOR_BRIGHT_WHITE);
	number_colored(current_enemy_health, COLOR_RED);
	std::cout << "\n\n";
	pause_game();
}

void cast_mirror_of_death(int enemyIndex)
{
	const int cost = 50;
	if (player_mana < cost)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Za mało many! (50)\n\n", COLOR_RED);
		pause_game();
		return;
	}
	if (cd_mirror_of_death > 0)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Śmiertelne Lustro na CD! (", COLOR_RED);
		number_colored(cd_mirror_of_death, COLOR_YELLOW);
		print_colored(" tur)\n\n", COLOR_RED);
		pause_game();
		return;
	}
	if (mirror_active)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Śmiertelne Lustro już aktywne!\n\n", COLOR_YELLOW);
		pause_game();
		return;
	}
	player_mana -= cost;
	cd_mirror_of_death = 5;
	mirror_active = true;
	adept_book_on_spell();
	clear_screen();
	std::cout << "\n\t";
	print_colored("Śmiertelne Lustro aktywowane!\n\t", COLOR_CYAN);
	print_colored("Następny śmiertelny atak wroga zostanie odparty!\n\n", COLOR_CYAN);
	pause_game();
	return;
}

// =================================================================
//  CZARY ZABÓJCY
// =================================================================

void cast_primal_strike()
{
	const int cost = 200;
	if (player_mana < cost)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Za mało many! (200)\n\n", COLOR_RED);
		pause_game();
		return;
	}
	if (primal_strike_active)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Pierwotne Uderzenie już aktywne!\n\n", COLOR_YELLOW);
		pause_game();
		return;
	}
	player_mana -= cost;
	primal_strike_active = true;
	adept_book_on_spell(3);
	clear_screen();
	std::cout << "\n\t";
	print_colored("Rzucono: Pierwotne Uderzenie!\n\t", COLOR_RED);
	print_colored("Następny atak zawsze krytuje!\n\n", COLOR_PURPLE);
	pause_game();
	return;
}

void cast_undodgeable_speed()
{
	const int cost = 80;
	if (player_mana < cost)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Za mało many! (80)\n\n", COLOR_RED);
		pause_game();
		return;
	}
	if (cd_undodgeable_speed > 0)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Nieomijalna Prędkość na CD! (", COLOR_RED);
		number_colored(cd_undodgeable_speed, COLOR_YELLOW);
		print_colored(" tur)\n\n", COLOR_RED);
		pause_game();
		return;
	}
	player_mana -= cost;
	adept_book_on_spell(3);
	cd_undodgeable_speed = 4;
	undodgeable_turns = 3;
	clear_screen();
	std::cout << "\n\t";
	print_colored("Rzucono: Nieomijalna Prędkość!\n\t", COLOR_CYAN);
	print_colored("+25% accuracy gracza, -30% accuracy wroga na 3 tury!\n\n", COLOR_CYAN);
	pause_game();
	return;
}

// FIX: porównuje accuracy gracza z accuracy (dodge) przeciwnika
void cast_slayer_of_slowest(int enemyIndex)
{
	const int cost = 20;
	if (player_mana < cost)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Za mało many! (20)\n\n", COLOR_RED);
		pause_game();
		return;
	}
	if (cd_slayer_of_slowest > 0)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Pogromca Powolnych na CD! (", COLOR_RED);
		number_colored(cd_slayer_of_slowest, COLOR_YELLOW);
		print_colored(" tur)\n\n", COLOR_RED);
		pause_game();
		return;
	}

	// accuracy gracza = bonus_accuracy + efekty
	int player_accuracy = player_bonus_accuracy;
	if (undodgeable_turns > 0)
		player_accuracy += 25;

	// accuracy (dodge) przeciwnika po efektach
	int enemy_accuracy = enemy[enemyIndex].atk_chance;
	if (undodgeable_turns > 0)
		enemy_accuracy = std::max(0, enemy_accuracy - 30);

	if (enemy_accuracy >= player_accuracy)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Przeciwnik jest za szybki!\n\t", COLOR_YELLOW);
		print_colored("Twoje accuracy (", COLOR_YELLOW);
		number_colored(player_accuracy, COLOR_CYAN);
		print_colored(") musi być większe niż accuracy wroga (", COLOR_YELLOW);
		number_colored(enemy_accuracy, COLOR_RED);
		print_colored(")\n\n", COLOR_YELLOW);
		pause_game();
		return;
	}

	player_mana -= cost;
	adept_book_on_spell(3);
	cd_slayer_of_slowest = 1;

	// Scaling: 1% (+0.8% za każde 750 weapon damage) max HP wroga — magic damage
	int weapon_dmg = player_weapon_damage + player_class_bonus_ad;
	float pct = 1.0f + (weapon_dmg / 750.0f) * 0.8f;
	int raw_damage = (int)(enemy[enemyIndex].health * pct / 100.0f);
	if (raw_damage < 1)
		raw_damage = 1;
	int dealt = deal_magic_damage(raw_damage, enemyIndex);
	current_enemy_health -= dealt;

	clear_screen();
	std::cout << "\n\t";
	print_colored("Rzucono: Pogromca Powolnych!\n\t", COLOR_PURPLE);
	print_colored("Zadano ", COLOR_GREEN);
	number_colored(dealt, COLOR_RED);
	std::cout << " obrażeń magicznych (% HP)!\n";
	apply_spell_vamp(dealt);
	std::cout << "\t";
	print_colored("HP wroga: ", COLOR_BRIGHT_WHITE);
	number_colored(current_enemy_health, COLOR_RED);
	std::cout << "\n\n";
	pause_game();
}

// =================================================================
//  CZARY TANKA
// =================================================================

void cast_stone_bastion(int enemyIndex)
{
	const int cost = 50;
	if (player_mana < cost)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Za mało many! (50)\n\n", COLOR_RED);
		pause_game();
		return;
	}
	if (cd_stone_bastion > 0)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Kamienny Bastion na CD! (", COLOR_RED);
		number_colored(cd_stone_bastion, COLOR_YELLOW);
		print_colored(" tur)\n\n", COLOR_RED);
		pause_game();
		return;
	}
	if (tank_bastion_armor_turns > 0)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Kamienny Bastion jest jeszcze aktywny! (", COLOR_RED);
		number_colored(tank_bastion_armor_turns, COLOR_YELLOW);
		print_colored(" tur)\n\n", COLOR_RED);
		pause_game();
		return;
	}

	player_mana -= cost;
	adept_book_on_spell(3);
	// Stun 2 tury
	if (enemy_stun_turns < 2)
		enemy_stun_turns = 2;
	// Efekt pancerza: 5 tur, +2% armoru per tura
	tank_bastion_armor_turns = 5;
	tank_bastion_armor_bonus = 0;

	clear_screen();
	std::cout << "\n\t";
	print_colored("Rzucono: Kamienny Bastion!\n\t", COLOR_YELLOW);
	print_colored("Wróg ogłuszony na 2 tury!\n\t", COLOR_YELLOW);
	print_colored("Bonus armor +2% za każdą turę efektu przez 5 tur do następnego otrzymanego damage!\n\n", COLOR_YELLOW);
	pause_game();
}

// Skumulowany Gniew — manualne użycie
void cast_accumulated_wrath(int enemyIndex)
{
	const int cost = 50;
	if (player_mana < cost)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Za mało many! (50)\n\n", COLOR_RED);
		pause_game();
		return;
	}
	if (cd_accumulated_wrath > 0)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Skumulowany Gniew na CD! (", COLOR_RED);
		number_colored(cd_accumulated_wrath, COLOR_YELLOW);
		print_colored(" tur)\n\n", COLOR_RED);
		pause_game();
		return;
	}
	if (tank_wrath_stored <= 0)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Brak skumulowanych obrażeń!\n\n", COLOR_YELLOW);
		pause_game();
		return;
	}

	player_mana -= cost;
	adept_book_on_spell(3);
	cd_accumulated_wrath = 2;

	int raw = tank_wrath_stored * 25 / 100; // 25% jako magic
	int dealt = deal_magic_damage(raw, enemyIndex);
	current_enemy_health -= dealt;

	int heal = tank_wrath_stored * 30 / 100;
	player_health += heal;
	if (player_health > player_maxhealth)
		player_health = player_maxhealth;

	clear_screen();
	std::cout << "\n\t";
	print_colored("Skumulowany Gniew — wyzwolenie!\n\t", COLOR_RED);
	print_colored("Zadano ", COLOR_GREEN);
	number_colored(dealt, COLOR_RED);
	print_colored(" obrażeń magicznych!\n\t", COLOR_GREEN);
	print_colored("Uleczono ", COLOR_GREEN);
	number_colored(heal, COLOR_GREEN);
	print_colored(" HP!\n\n", COLOR_GREEN);

	tank_wrath_stored = 0;
	pause_game();
}

void cast_iron_taunt()
{
	const int cost = 45;
	if (player_mana < cost)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Za mało many! (45)\n\n", COLOR_RED);
		pause_game();
		return;
	}
	if (cd_iron_taunt > 0)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Żelazna Prowokacja na CD! (", COLOR_RED);
		number_colored(cd_iron_taunt, COLOR_YELLOW);
		print_colored(" tur)\n\n", COLOR_RED);
		pause_game();
		return;
	}

	player_mana -= cost;
	adept_book_on_spell(3);
	tank_taunt_turns = 2;
	// CD startuje automatycznie po wygaśnięciu efektu (obsługiwane w tick_cooldowns)

	clear_screen();
	std::cout << "\n\t";
	print_colored("Rzucono: Żelazna Prowokacja!\n\t", COLOR_YELLOW);
	print_colored("Przez 2 tury:\n\t", COLOR_YELLOW);
	print_colored("-5% redukcji obrażeń broni (zamiast -25% masz -20%)\n\t", COLOR_CYAN);
	print_colored("-15% dodge szansy wroga\n\t", COLOR_CYAN);
	print_colored("+10% accuracy gracza\n\n", COLOR_CYAN);
	pause_game();
	return;
}

// =================================================================
//  CZARY SAMURAJA
// =================================================================

void cast_mushin()
{
	const int cost = 50;
	if (player_mana < cost)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Za mało many! (50)\n\n", COLOR_RED);
		pause_game();
		return;
	}
	if (cd_mushin_ability > 0)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Mushin na CD! (", COLOR_RED);
		number_colored(cd_mushin_ability, COLOR_YELLOW);
		print_colored(" tur)\n\n", COLOR_RED);
		pause_game();
		return;
	}

	player_mana -= cost;
	adept_book_on_spell(5);
	cd_mushin_ability = 1;
	int ap = get_total_ap();
	float heal_pct = 0.02f + (ap * 0.005f); // 2% + 0.05% per AP
	int heal = (int)(player_maxhealth * heal_pct);
	if (heal < 1)
		heal = 1;
	player_health += heal;
	if (player_health > player_maxhealth)
		player_health = player_maxhealth;
	mushin_shield_active = true; // 30% redukcja do następnego ataku wroga

	clear_screen();
	std::cout << "\n\t";
	print_colored("Rzucono: Mushin!\n\t", COLOR_CYAN);
	print_colored("Uleczono ", COLOR_GREEN);
	number_colored(heal, COLOR_GREEN);
	print_colored(" HP!\n\t", COLOR_GREEN);
	print_colored("Następny atak wroga zredukowany o 30%!\n\n", COLOR_CYAN);
	pause_game();
	return;
}

void cast_kogeki_strike(int enemyIndex)
{
	const int cost = 30;
	if (player_mana < cost)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Za mało many! (30)\n\n", COLOR_RED);
		pause_game();
		return;
	}
	if (cd_kogeki_strike > 0)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Kōgeki na CD! (", COLOR_RED);
		number_colored(cd_kogeki_strike, COLOR_YELLOW);
		print_colored(" tur)\n\n", COLOR_RED);
		pause_game();
		return;
	}

	player_mana -= cost;
	adept_book_on_spell(3);
	cd_kogeki_strike = 2;

	// Fizyczny dmg (normalny atak)
	int base_dmg = player_weapon_damage + player_class_bonus_ad;
	// Samuraj +15% dmg
	base_dmg = (int)(base_dmg * 1.15f);

	clamp_player_armor_pen();
	int effective_armor = enemy[enemyIndex].armor - player_armor_pen;
	if (effective_armor < 0)
		effective_armor = 0;
	float armor_red = effective_armor / (effective_armor + 100.0f);
	int phys_dealt = (int)(base_dmg * (1.0f - armor_red));
	if (phys_dealt < 1)
		phys_dealt = 1;

	// Magiczny dmg: 50% base pre-reduction weapon dmg
	int raw_magic = (player_weapon_damage + player_class_bonus_ad) / 2;
	int magic_dealt = deal_magic_damage(raw_magic, enemyIndex);

	int total = phys_dealt + magic_dealt;
	if (total > current_enemy_health)
		total = current_enemy_health;
	current_enemy_health -= total;

	clear_screen();
	std::cout << "\n\t";
	print_colored("Rzucono: Kōgeki!\n\t", COLOR_RED);
	print_colored("Fizyczne: ", COLOR_GREEN);
	number_colored(phys_dealt, COLOR_RED);
	print_colored("  Magiczne: ", COLOR_PURPLE);
	number_colored(magic_dealt, COLOR_RED);
	print_colored("  Łącznie: ", COLOR_BRIGHT_WHITE);
	number_colored(total, COLOR_RED);
	std::cout << " obrażeń!\n";
	apply_spell_vamp(magic_dealt);
	std::cout << "\t";
	print_colored("HP wroga: ", COLOR_BRIGHT_WHITE);
	number_colored(current_enemy_health, COLOR_RED);
	std::cout << "\n\n";
	pause_game();
}

void toggle_sen_no_kata()
{
	sen_no_kata_active = !sen_no_kata_active;
	clear_screen();
	std::cout << "\n\t";
	if (sen_no_kata_active)
	{
		print_colored("Sen no Kata - Chikara\n\t", COLOR_YELLOW);
		print_colored("+7.5% DMG fizyczny na każdy atak, +4.5% otrzymywanego DMG.\n\n", COLOR_YELLOW);
	}
	else
	{
		print_colored("Sen no Kata - Bōei\n\t", COLOR_GRAY);
		print_colored("Co 3. atak ogłusza wroga na 1 turę oraz zadaje 10% DMG max weapon damage.\n\n", COLOR_GRAY);
	}
	pause_game();
	return;
}

// =================================================================
//  MENU CZARÓW
// =================================================================

void battle_spells_mage(int enemyIndex, bool& spell_used)
{
	int choice = 0, key;
	const int N = 5;
	while (true)
	{
		clear_screen();
		print_colored("\n\tCzary Maga\n\n", COLOR_BRIGHT_WHITE);
		std::cout << "\t";
		print_colored("Mana: ", COLOR_BLUE);
		number_colored(player_mana, COLOR_BLUE);
		std::cout << " / ";
		number_colored(player_maxmana, COLOR_BLUE);
		std::cout << "  |  ";
		print_colored("AP: ", COLOR_PURPLE);
		number_colored(get_total_ap(), COLOR_PURPLE);
		std::cout << "\n\n";

		std::string s[N];

		// Powiew Burzy
		s[0] = "Powiew Burzy [Magiczne]\n";
		s[0] += "\t     Zadaje 15 (+25% AP) obrażeń.\n";
		s[0] += "\t		Koszt: 25 Mana";
		if (player_mana < 30)
			s[0] += "  [ZA MAŁO MANY]";

		// Gwiezdne Uderzenie
		s[1] = "Gwiezdne Uderzenie [Magiczne]\n";
		s[1] += "\t     Zadaje 60 (+90% AP) obrażeń.\n";
		s[1] += "\t		Koszt: 100 Mana ";
		if (cd_star_strike > 0) {
			s[1] += "  [CD: " + std::to_string(cd_star_strike) + "]";
		} else {
			s[1] += "  CD: 1 tura";
		}
		if (player_mana < 120)
			s[1] += "  [ZA MAŁO MANY]";

		// Zabójcze Pnącza
		s[2] = "Zabójcze Pnącza [Magiczne]\n";
		s[2] += "\t     Ogłusza na 3 tury. Podczas ogłuszenia zadaje 10 (+10% AP) obrażeń co turę.\n";
		s[2] += "\t		Koszt: 70 Mana ";
		if (cd_deadly_vines > 0) {
			s[2] += "  [CD: " + std::to_string(cd_deadly_vines) + "]";
		}
		else {
			s[2] += "  CD: 4 tury";
		}
		if (enemy_stun_turns > 0)
			s[2] += "  [AKTYWNE]";
		if (player_mana < 80)
			s[2] += "  [ZA MAŁO MANY]";

		// Śmiertelne Lustro
		s[3] = "Śmiertelne Lustro [Fizyczne]\n";
		s[3] += "\t     Jeśli nast. atak wroga byłby śmiertelny:\n";
		s[3] += "\t		blokuje 99 % DMG, odbija 5 %, leczy 15 % zablokowanego.\n";
		s[3] += "\t     Jeśli nie śmiertelny: blokuje 10%, odbija 20% zablokowanego.\n";
		s[3] += "\t		Koszt: 50 Mana ";
		if (mirror_active)
			s[3] += "  [AKTYWNE]";
		if (cd_mirror_of_death > 0) {
			s[3] += "  [CD: " + std::to_string(cd_mirror_of_death) + "]";
		}
		else {
			s[3] += "  CD: 5 tur";
		}
		if (player_mana < 50)
			s[3] += "  [ZA MAŁO MANY]";

		for (int i = 0; i < N; i++)
		{
			if (i == choice)
			{
				std::cout << "\t►  ";
				print_colored(s[i], COLOR_BRIGHT_WHITE);
			}
			else
			{
				std::cout << "\t   " << s[i];
			}
			std::cout << "\n";
		}
		std::cout << "\n";
		key = _getch();
		if (key == 224)
		{
			key = _getch();
			if (key == 72)
			{
				choice--;
				if (choice < 0)
					choice = N - 1;
			}
			else if (key == 80)
			{
				choice++;
				if (choice >= N)
					choice = 0;
			}
		}
		else if (key == 13)
		{
			if (choice == 0)
			{
				cast_storm_breeze(enemyIndex);
				spell_used = true;
				return;
			}
			if (choice == 1)
			{
				cast_star_strike(enemyIndex);
				spell_used = true;
				return;
			}
			if (choice == 2)
			{
				cast_deadly_vines(enemyIndex);
				spell_used = true;
				return;
			}
			if (choice == 3)
			{
				cast_mirror_of_death(enemyIndex);
				spell_used = true;
				return;
			}
			if (choice == 4)
				return;
		}
		else if (key == 27)
			return;
	}
}

void battle_spells_assassin(int enemyIndex, bool& spell_used)
{
	int choice = 0, key;
	const int N = 4;
	while (true)
	{
		clear_screen();
		print_colored("\n\tCzary Zabójcy\n\n", COLOR_BRIGHT_WHITE);
		std::cout << "\t";
		print_colored("Mana: ", COLOR_BLUE);
		number_colored(player_mana, COLOR_BLUE);
		std::cout << " / ";
		number_colored(player_maxmana, COLOR_BLUE);
		std::cout << "\n\n";

		int enemy_acc = enemy[enemyIndex].atk_chance;
		int player_acc = player_bonus_accuracy + (undodgeable_turns > 0 ? 25 : 0);

		std::string s[N];

		// Primal Strike
		s[0] = "Primal Strike [Fizyczne]\n";
		s[0] += "\t		Pasywka: Nadmiar śmiertelnych obrazeń trafia do Overkill.\n";
		s[0] += "\t		Gdy zebrane obrażenia przekroczą 50 % max health enemy, następny atak ignoruje 80 % armora.\n";
		s[0] += "\t		Odzyskujesz HP z obrażeń Overkill (5% przechowanego damage +1%/0.7 armor pen, +1%/0.8 armor).\n";
		s[0] += "\t     Manualne użycie: Nastepny atak krytuje. Jesli miał byc krytem, zadaje 2.25x dmg. \n";
		s[0] += "\t		Koszt: 200 Mana";
		if (primal_strike_active)
			s[0] += "  [AKTYWNE]";
		else if (player_mana < 200)
			s[0] += "  [ZA MAŁO MANY]";

		// Un-Dodgable Speed
		s[1] = "Un-Dodgable Speed [Utility]\n";
		s[1] += "\t     +25% accuracy gracza, -30% accuracy wroga przez 3 tury.\n";
		s[1] += "\t		Koszt: 80 Mana ";
		if (cd_undodgeable_speed > 0) {
			s[1] += "  [AKTYWNE! CD: " + std::to_string(cd_undodgeable_speed) + "]";
		} else { 
			s[1] += "  CD: 4 tury"; 
		}
		if (player_mana < 80)
			s[1] += "  [ZA MAŁO MANY]";

		// Slayer of the Slowest
		s[2] = "Slayer of the Slowest [Magiczne]\n";
		s[2] += "\t     Działa gdy Twoje acc > acc wroga. Zadaje 1% (+0.8% per 750 DMG broni) max HP wroga\n";
		s[2] += "\t     jako magic damage. Koszt: 20 Mana ";
		s[2] += "  [Twoje acc: " + std::to_string(player_acc) + " | Acc wroga: " + std::to_string(enemy_acc) + "]";

		if (cd_slayer_of_slowest > 0) {
			s[2] += "  [CD: " + std::to_string(cd_slayer_of_slowest) + "]";
		}
		else {
			s[2] += "  CD: 1 tura";
		}
		if (player_mana < 20)
			s[2] += "  [ZA MAŁO MANY]";

		for (int i = 0; i < N; i++)
		{
			if (i == choice)
			{
				std::cout << "\t►  ";
				print_colored(s[i], COLOR_BRIGHT_WHITE);
			}
			else
			{
				std::cout << "\t   " << s[i];
			}
			std::cout << "\n";
		}
		std::cout << "\n";
		key = _getch();
		if (key == 224)
		{
			key = _getch();
			if (key == 72)
			{
				choice--;
				if (choice < 0)
					choice = N - 1;
			}
			else if (key == 80)
			{
				choice++;
				if (choice >= N)
					choice = 0;
			}
		}
		else if (key == 13)
		{
			if (choice == 0)
			{
				cast_primal_strike();
				spell_used = true;
				return;
			}
			if (choice == 1)
			{
				cast_undodgeable_speed();
				spell_used = true;
				return;
			}
			if (choice == 2)
			{
				cast_slayer_of_slowest(enemyIndex);
				spell_used = true;
				return;
			}
			if (choice == 3)
				return;
		}
		else if (key == 27)
			return;
	}
}

void battle_spells_tank(int enemyIndex, bool& spell_used)
{
	int choice = 0, key;
	const int N = 4;
	while (true)
	{
		clear_screen();
		print_colored("\n\tCzary Tanka\n\n", COLOR_BRIGHT_WHITE);
		std::cout << "\t";
		print_colored("Mana: ", COLOR_BLUE);
		number_colored(player_mana, COLOR_BLUE);
		std::cout << " / ";
		number_colored(player_maxmana, COLOR_BLUE);
		std::cout << "  |  ";
		print_colored("Skumulowany Gniew: ", COLOR_RED);
		number_colored(tank_wrath_stored, COLOR_RED);
		std::cout << "\n\n";

		std::string s[N];

		// Kamienny Bastion
		s[0] = "Kamienny Bastion [Utility]\n";
		s[0] += "\t     Ogłusza wroga na 2 tury. Przez następne 5 tur kumuluje +2% armoru na turę.\n";
		s[0] += "\t		Efekt zostaje przerwany przy otrzymaniu obrażeń od przeciwnika. Koszt : 50 Mana  CD : 1 tura";
		if (cd_stone_bastion > 0) {
			s[0] += "  [CD: " + std::to_string(cd_stone_bastion) + " tur]";
		} else s[0] += "  CD : 1 tura";
		if (player_mana < 50)
			s[0] += "  [ZA MAŁO MANY]";
		if (tank_bastion_armor_turns > 0)
			s[0] += "  [KUMULUJE: " + std::to_string(tank_bastion_armor_turns) + " tur]";

		// Skumulowany Gniew
		s[1] = "Skumulowany Gniew [Mag/Fiz/Utility]\n";
		s[1] += "\t		Pasywnie: Kumuluje otrzymywany damage w gniew\n";
		s[1] += "\t     Auto-trigger: gdy gniew >= 1.5x DMG broni — zadaje 70% gniewu jako fizyczny DMG, heal 35%, +bonus armor\n";
		s[1] += "\t     Manualna aktywacja: 25% zebranego gniewu jest zadawane jako magic DMG + leczy 30% zebranego. Koszt: 50 Mana ";
		if (cd_accumulated_wrath > 0) {
			s[1] += "  [CD: " + std::to_string(cd_accumulated_wrath) + " tur]";
		} else s[1] += "  CD : 2 tura";
		if (player_mana < 50)
			s[1] += "  [ZA MAŁO MANY]";
		else if (tank_wrath_stored <= 0)
			s[1] += "  [BRAK GNIEWU]";

		// Żelazna Prowokacja
		s[2] = "Żelazna Prowokacja [Utility]\n";
		s[2] += "\t     Przez 2 tury: redukcja broni wynosi -20% zamiast -25%, -15% dodge wroga, dostajesz +10% accuracy. Koszt: 45 Mana ";
		if (cd_iron_taunt > 0) {
			s[2] += "  [CD: " + std::to_string(cd_iron_taunt) + " tur]";
		}
		else s[2] += "  CD: 2 tury (po wygaśnięciu)";
		if (player_mana < 45)
			s[2] += "  [ZA MAŁO MANY]";
		if (tank_taunt_turns > 0)
			s[2] += "  [AKTYWNE: " + std::to_string(tank_taunt_turns) + " tur]";

		for (int i = 0; i < N; i++)
		{
			if (i == choice)
			{
				std::cout << "\t►  ";
				print_colored(s[i], COLOR_BRIGHT_WHITE);
			}
			else
			{
				std::cout << "\t   " << s[i];
			}
			std::cout << "\n";
		}
		std::cout << "\n";
		key = _getch();
		if (key == 224)
		{
			key = _getch();
			if (key == 72)
			{
				choice--;
				if (choice < 0)
					choice = N - 1;
			}
			else if (key == 80)
			{
				choice++;
				if (choice >= N)
					choice = 0;
			}
		}
		else if (key == 13)
		{
			if (choice == 0)
			{
				cast_stone_bastion(enemyIndex);
				spell_used = true;
				return;
			}
			if (choice == 1)
			{
				cast_accumulated_wrath(enemyIndex);
				spell_used = true;
				return;
			}
			if (choice == 2)
			{
				cast_iron_taunt();
				spell_used = true;
				return;
			}
			if (choice == 3)
				return;
		}
		else if (key == 27)
			return;
	}
}

void battle_spells_samurai(int enemyIndex, bool& spell_used)
{
	int choice = 0, key;
	const int N = 4;
	while (true)
	{
		clear_screen();
		print_colored("\n\tCzary Samuraja\n\n", COLOR_BRIGHT_WHITE);
		std::cout << "\t";
		print_colored("Mana: ", COLOR_BLUE);
		number_colored(player_mana, COLOR_BLUE);
		std::cout << " / ";
		number_colored(player_maxmana, COLOR_BLUE);
		std::cout << "  |  ";
		print_colored("AP: ", COLOR_PURPLE);
		number_colored(get_total_ap(), COLOR_PURPLE);
		std::cout << "  |  ";
		print_colored("Licznik atakow: ", COLOR_YELLOW);
		number_colored(samurai_attack_counter, COLOR_YELLOW);
		print_colored("/3", COLOR_YELLOW);
		std::cout << "\n\n";

		int ap = get_total_ap();
		std::string s[N];

		// Mushin
		s[0] = "Mushin [Utility]\n";
		s[0] += "\t     Leczy " + std::to_string((int)((0.02f + ap * 0.005f) * 100)) + "% (+0.05% per AP) max HP. Następny atak wroga zredukowany o 30% DMG. Koszt: 50 Mana  CD: 1 tura";
		if (cd_mushin_ability > 0)
			s[0] += "  [CD: " + std::to_string(cd_mushin_ability) + " tur]";
		else if (player_mana < 50)
			s[0] += "  [ZA MAŁO MANY]";

		// Sen no Kata
		s[1] = "Sen no Kata [Fizyczne] — Toggle (bez many i CD)\n";
		if (sen_no_kata_active)
			s[1] += "\t     Chikara: +7.5% DMG fizyczny na każdy atak, +4.5% otrzymywanego DMG. Pasywka stun wyłączona.";
		else
			s[1] += "\t     Bōei: co 3. atak ogłusza wroga na 1 turę oraz zadaje 10% DMG max weapon damage.";

		// Kogeki
		s[2] = "Kōgeki [Magiczne/Fizyczne]\n";
		s[2] += "\t     Normalny atak fizyczny + 50% base weapon DMG jako magic damage. Koszt: 30 Mana ";
		if (cd_kogeki_strike > 0) {
			s[2] += "  [CD: " + std::to_string(cd_kogeki_strike) + " tur]";
		} else s[2] += "  CD: 2 tury";
		if (player_mana < 30)
			s[2] += "  [ZA MAŁO MANY]";

		for (int i = 0; i < N; i++)
		{
			if (i == choice)
			{
				std::cout << "\t►  ";
				print_colored(s[i], COLOR_BRIGHT_WHITE);
			}
			else
			{
				std::cout << "\t   " << s[i];
			}
			std::cout << "\n";
		}
		std::cout << "\n";
		key = _getch();
		if (key == 224)
		{
			key = _getch();
			if (key == 72)
			{
				choice--;
				if (choice < 0)
					choice = N - 1;
			}
			else if (key == 80)
			{
				choice++;
				if (choice >= N)
					choice = 0;
			}
		}
		else if (key == 13)
		{
			if (choice == 0)
			{
				cast_mushin();
				spell_used = true;
				return;
			}
			if (choice == 1)
			{
				toggle_sen_no_kata(); /* toggle nie liczy się jako tura */
				return;
			}
			if (choice == 2)
			{
				cast_kogeki_strike(enemyIndex);
				spell_used = true;
				return;
			}
			if (choice == 3)
				return;
		}
		else if (key == 27)
			return;
	}
}

void battle_spells_menu(int enemyIndex, bool& spell_used)
{
	if (player_class == CLASS_ASSASSIN)
		battle_spells_assassin(enemyIndex, spell_used);
	else if (player_class == CLASS_TANK)
		battle_spells_tank(enemyIndex, spell_used);
	else if (player_class == CLASS_SAMURAI)
		battle_spells_samurai(enemyIndex, spell_used);
	else
		battle_spells_mage(enemyIndex, spell_used);
}

// =================================================================
//  ATAK GRACZA
// =================================================================

bool do_player_attack(int enemyIndex)
{
	int bonusAccuracy = player_bonus_accuracy;
	if (accuracy_potion_turns > 0)
		bonusAccuracy += accuracy_potion_bonus;
	if (undodgeable_turns > 0)
		bonusAccuracy += 25;
	if (tank_taunt_turns > 0)
		bonusAccuracy += 10; // Żelazna Prowokacja

	int enemy_dodge = enemy[enemyIndex].dodge_chance;
	if (undodgeable_turns > 0)
		enemy_dodge = std::max(0, enemy_dodge - 30);
	if (tank_taunt_turns > 0)
		enemy_dodge = std::max(0, enemy_dodge - 15); // Prowokacja

	int final_dodge = enemy_dodge - bonusAccuracy;
	if (final_dodge < 0)
		final_dodge = 0;

	int miss = rand() % 100;
	if (miss < final_dodge)
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Twoja tura!\n\n\t", COLOR_BRIGHT_WHITE);
		print_colored("Przeciwnik uniknął twojego ataku!\n\n", COLOR_RED);
		pause_game();
		return false;
	}

	// ===== SAMURAJ: pasywka co 3. atak (tylko gdy Sen no Kata WYŁĄCZONE) =====
	samurai_attack_counter = (player_class == CLASS_SAMURAI) ? samurai_attack_counter + 1 : 0;
	bool samurai_passive_stun = false;
	if (player_class == CLASS_SAMURAI && !sen_no_kata_active && samurai_attack_counter >= 3)
	{
		samurai_passive_stun = true;
		samurai_attack_counter = 0;
	}
	// Gdy Sen no Kata aktywne — resetuj licznik żeby nie nagromadzić stuna przy wyłączeniu
	if (player_class == CLASS_SAMURAI && sen_no_kata_active)
		samurai_attack_counter = 0;

	bool critical = false, super_crit = false;
	int critRoll = rand() % 100;
	bool would_crit = (critRoll < player_crit_chance);
	if (primal_strike_active)
	{
		if (would_crit)
			super_crit = true;
		critical = true;
		primal_strike_active = false;
	}
	else
	{
		critical = would_crit;
	}

	// Base DMG
	int total_player_dmg = player_weapon_damage + player_class_bonus_ad;

	if (samurai_passive_stun)
	{
		// 3. atak Samuraja (tryb stun): tylko 10% + 0.05% per AP max dmg
		int ap = get_total_ap();
		float pct = 0.10f + (ap * 0.0005f);
		total_player_dmg = (int)(total_player_dmg * pct);
		if (total_player_dmg < 1)
			total_player_dmg = 1;
		enemy_stun_turns = 1; // stun na 1 turę
	}
	else
	{
		// Normalne bonusy klas
		if (player_class == CLASS_ASSASSIN)
			total_player_dmg = (int)(total_player_dmg * 1.10f);
		if (player_class == CLASS_SAMURAI)
		{
			total_player_dmg = (int)(total_player_dmg * 1.15f);
			// Sen no Kata WŁĄCZONE: +7.5% do każdego ataku zamiast pasywki stunującej
			if (sen_no_kata_active)
				total_player_dmg = (int)(total_player_dmg * 1.075f);
		}
		if (player_class == CLASS_TANK && player_weapon_damage > 500)
		{
			float tank_pen = 0.75f; // -25% domyślnie
			if (tank_taunt_turns > 0)
				tank_pen = 0.80f; // -20% z Prowokacją
			total_player_dmg = (int)(total_player_dmg * tank_pen);
		}
	}

	int dealt_damage = total_player_dmg;
	if (super_crit)
		dealt_damage = (int)(dealt_damage * 2.25f);
	else if (critical)
		dealt_damage = (int)(dealt_damage * 1.5f);

	// Pancerz
	clamp_player_armor_pen();
	int eff_armor = enemy[enemyIndex].armor - player_armor_pen;
	bool overkill_ignore = overkill_armor_ignore;
	overkill_armor_ignore = false;
	if (overkill_ignore)
		eff_armor = (int)(eff_armor * 0.20f);
	if (eff_armor < 0)
		eff_armor = 0;
	float armor_red = eff_armor / (eff_armor + 100.0f);
	dealt_damage = (int)(dealt_damage * (1.0f - armor_red));
	if (dealt_damage < 1)
		dealt_damage = 1;

	// Overkill
	int overkill_excess = 0;
	if (dealt_damage > current_enemy_health)
	{
		overkill_excess = dealt_damage - current_enemy_health;
		dealt_damage = current_enemy_health;
	}
	current_enemy_health -= dealt_damage;

	// Lifesteal
	int ls = player_lifesteal;
	if (player_class == CLASS_ASSASSIN)
		ls += 5;
	if (vampirism_potion_turns > 0)
		ls += vampirism_potion_bonus;
	int heal = (dealt_damage * ls) / 100;
	player_health += heal;
	if (player_health > player_maxhealth)
		player_health = player_maxhealth;

	// Tank: Skumulowany Gniew auto-trigger
	bool wrath_triggered = false;
	if (player_class == CLASS_TANK && !samurai_passive_stun)
	{
		// accumulate — tu nic, gniew akumuluje się przy OTRZYMYWANIU dmg
		// auto-trigger sprawdzamy po zadaniu dmg (nie tu — to na etapie enemy attack)
	}

	// Wyświetl
	clear_screen();
	std::cout << "\n\t";
	print_colored("Twoja tura!\n\n\t", COLOR_BRIGHT_WHITE);
	if (samurai_passive_stun)
	{
		print_colored("Sen no Kata - Bōei: Ogłuszający cios!\n\t", COLOR_YELLOW);
		print_colored("(Atak zredukowany, wróg ogłuszony na 1 turę)\n\t", COLOR_YELLOW);
	}
	if (super_crit)
	{
		print_colored("PRIMAL STRIKE SUPER KRYTYK (2.25x)!\n\t", COLOR_PURPLE);
	}
	else if (critical)
	{
		print_colored("TRAFIENIE KRYTYCZNE!\n\t", COLOR_PURPLE);
	}
	if (overkill_ignore)
	{
		print_colored("[Overkill: ignorujesz 80% pancerza!]\n\t", COLOR_YELLOW);
	}
	print_colored("Zadałeś ", COLOR_GREEN);
	number_colored(dealt_damage, COLOR_RED);
	print_colored(" obrażeń!", COLOR_GREEN);
	if (eff_armor > 0 && !overkill_ignore)
	{
		std::cout << "\n\t";
		print_colored("(Pancerz wroga zredukował część obrażeń)", COLOR_YELLOW);
	}
	std::cout << "\n\n\t";
	print_colored("Przeciwnik ma teraz ", COLOR_BRIGHT_WHITE);
	number_colored(current_enemy_health, COLOR_RED);
	print_colored(" HP!", COLOR_BRIGHT_WHITE);
	if (heal > 0)
	{
		std::cout << "\n\n\t";
		print_colored("Odnowiono ", COLOR_GREEN);
		number_colored(heal, COLOR_GREEN);
		print_colored(" HP (lifesteal)!", COLOR_GREEN);
	}

	// Overkill
	if (overkill_excess > 0)
	{
		overkill_stored += overkill_excess;
		float ok_heal_pct = 0.05f + (player_armor_pen * 0.70f / 100.0f) + (player_armor * 0.80f / 100.0f);
		int ok_heal = (int)(overkill_stored * ok_heal_pct);
		if (ok_heal > 0)
		{
			player_health += ok_heal;
			if (player_health > player_maxhealth)
				player_health = player_maxhealth;
			std::cout << "\n\t";
			print_colored("OVERKILL! Uleczono ", COLOR_CYAN);
			number_colored(ok_heal, COLOR_GREEN);
			print_colored(" HP!", COLOR_CYAN);
		}
		if (overkill_stored > (enemy[enemyIndex].health / 2))
		{
			overkill_armor_ignore = true;
			std::cout << "\n\t";
			print_colored("OVERKILL PRÓG! Następny atak ignoruje 80% pancerza!", COLOR_YELLOW);
		}
	}

	std::cout << "\n\n";
	pause_game();
	return (current_enemy_health <= 0);
}

// =================================================================
//  ATAK PRZECIWNIKA
// =================================================================

bool do_enemy_attack(int enemyIndex)
{
	int eff_player_armor = player_armor - enemy[enemyIndex].armor_pen + tank_bastion_armor_bonus;
	if (eff_player_armor < 0)
		eff_player_armor = 0;
	float reduction = eff_player_armor / (eff_player_armor + 100.0f);
	int raw_dmg = current_enemy_damage;
	bool enemy_crit = (rand() % 100 < enemy[enemyIndex].crit_chance);
	if (enemy_crit)
		raw_dmg = (int)(raw_dmg * 1.5f);
	int final_dmg = (int)(raw_dmg * (1.0f - reduction));
	if (final_dmg < 1)
		final_dmg = 1;
	int dmg_reduced = raw_dmg - final_dmg;

	// Mushin: 30% redukcja
	if (mushin_shield_active)
	{
		mushin_shield_active = false;
		final_dmg = (int)(final_dmg * 0.70f);
		if (final_dmg < 1)
			final_dmg = 1;
	}

	// Sen no Kata: +4.5% otrzymywanego dmg (całość)
	int sen_extra = 0;
	if (player_class == CLASS_SAMURAI && sen_no_kata_active)
	{
		sen_extra = (int)(final_dmg * 0.045f);
		if (sen_extra < 1)
			sen_extra = 1;
	}

	// Mirror of Death
	if (mirror_active)
	{
		mirror_active = false;
		bool is_lethal = ((final_dmg + sen_extra) >= player_health);
		clear_screen();
		std::cout << "\n\t";
		print_colored("Tura Przeciwnika!\n\n\t", COLOR_BRIGHT_WHITE);
		print_colored("Śmiertelne Lustro reaguje!\n\t", COLOR_CYAN);
		if (is_lethal)
		{
			int blocked = (int)(final_dmg * 0.99f);
			int taken = final_dmg - blocked;
			if (taken < 1)
				taken = 1;
			player_health -= taken;
			int counter = (int)(blocked * 0.05f);
			if (counter < 1)
				counter = 1;
			if (counter > current_enemy_health)
				counter = current_enemy_health;
			current_enemy_health -= counter;
			bool killed = (current_enemy_health <= 0);
			int mheal = (int)(blocked * (killed ? 0.50f : 0.15f));
			player_health += mheal;
			if (player_health > player_maxhealth)
				player_health = player_maxhealth;
			print_colored("ATAK ŚMIERTELNY ODPARTY!\n\t", COLOR_CYAN);
			print_colored("Otrzymano ", COLOR_YELLOW);
			number_colored(taken, COLOR_RED);
			print_colored(" obrażeń!\n\t", COLOR_YELLOW);
			print_colored("Kontratak: ", COLOR_GREEN);
			number_colored(counter, COLOR_RED);
			print_colored(" wrogowi!\n\t", COLOR_GREEN);
			print_colored("Uleczono ", COLOR_GREEN);
			number_colored(mheal, COLOR_GREEN);
			print_colored(killed ? " HP (zabójstwo Lustrem)!\n\n" : " HP!\n\n", COLOR_GREEN);
			pause_game();
			if (player_health <= 0)
			{
				clear_screen();
				std::cout << "\n\tGAME OVER!\n\n";
				pause_game();
				exit(0);
			}
			return (current_enemy_health <= 0);
		}
		else
		{
			int blocked = (int)(final_dmg * 0.10f);
			int taken = final_dmg - blocked;
			if (taken < 1)
				taken = 1;
			player_health -= taken;
			int counter = (int)(blocked * 0.20f);
			if (counter < 1)
				counter = 1;
			if (counter > current_enemy_health)
				counter = current_enemy_health;
			current_enemy_health -= counter;
			print_colored("Atak nie był śmiertelny — Lustro blokuje 10%.\n\t", COLOR_YELLOW);
			print_colored("Otrzymano ", COLOR_YELLOW);
			number_colored(taken, COLOR_RED);
			print_colored(" obrażeń, kontratak: ", COLOR_YELLOW);
			number_colored(counter, COLOR_RED);
			print_colored(" wrogowi!\n\n", COLOR_YELLOW);
			pause_game();
			if (player_health <= 0)
			{
				clear_screen();
				std::cout << "\n\tGAME OVER!\n\n";
				pause_game();
				exit(0);
			}
			return (current_enemy_health <= 0);
		}
	}

	// Tank: Skumulowany Gniew — akumuluj
	if (player_class == CLASS_TANK)
	{
		tank_wrath_stored += final_dmg;
	}

	// Normalny atak
	int attack_roll = rand() % 100;
	if (attack_roll < enemy[enemyIndex].atk_chance)
	{
		player_health -= final_dmg + sen_extra;

		// Tank: bonus armor z Gniewu znika po trafieniu
		tank_bastion_armor_bonus = 0;

		clear_screen();
		std::cout << "\n\t";
		print_colored("Tura Przeciwnika!\n\n\t", COLOR_BRIGHT_WHITE);
		if (enemy_crit)
		{
			print_colored("PRZECIWNIK TRAFIŁ KRYTYCZNIE!\n\t", COLOR_PURPLE);
		}
		if (dmg_reduced > 0)
		{
			ui_colored("Obrażenia zredukowane o: ", dmg_reduced, COLOR_YELLOW);
			std::cout << "\n\t";
		}
		print_colored("Przeciwnik zadał Ci ", COLOR_RED);
		number_colored(final_dmg, COLOR_RED);
		print_colored(" obrażeń!", COLOR_RED);
		if (sen_extra > 0)
		{
			std::cout << "\n\t";
			print_colored("Sen no Kata: +", COLOR_YELLOW);
			number_colored(sen_extra, COLOR_YELLOW);
			print_colored(" magicznych obrażeń!", COLOR_YELLOW);
		}
		std::cout << "\n\n";

		// Tank: Skumulowany Gniew auto-trigger (po zadaniu obrażeń)
		if (player_class == CLASS_TANK)
		{
			int trigger_threshold = (int)((player_weapon_damage + player_class_bonus_ad) * 1.5f);
			if (tank_wrath_stored > trigger_threshold && trigger_threshold > 0)
			{
				int auto_phys = tank_wrath_stored * 70 / 100;
				clamp_player_armor_pen();
				int eff_a = enemy[enemyIndex].armor - player_armor_pen;
				if (eff_a < 0)
					eff_a = 0;
				float ar = eff_a / (eff_a + 100.0f);
				int auto_dealt = (int)(auto_phys * (1.0f - ar));
				if (auto_dealt < 1)
					auto_dealt = 1;
				if (auto_dealt > current_enemy_health)
					auto_dealt = current_enemy_health;
				current_enemy_health -= auto_dealt;

				int auto_heal = tank_wrath_stored * 35 / 100;
				player_health += auto_heal;
				if (player_health > player_maxhealth)
					player_health = player_maxhealth;

				int armor_bonus_from_wrath = (int)(tank_wrath_stored * 0.0005f);
				tank_bastion_armor_bonus += armor_bonus_from_wrath;

				std::cout << "\t";
				print_colored("SKUMULOWANY GNIEW AUTO-TRIGGER!\n\t", COLOR_RED);
				print_colored("Zadano ", COLOR_GREEN);
				number_colored(auto_dealt, COLOR_RED);
				print_colored(" fizycznych!\n\t", COLOR_GREEN);
				print_colored("Uleczono ", COLOR_GREEN);
				number_colored(auto_heal, COLOR_GREEN);
				print_colored(" HP!\n\t", COLOR_GREEN);
				if (armor_bonus_from_wrath > 0)
				{
					print_colored("+", COLOR_YELLOW);
					number_colored(armor_bonus_from_wrath, COLOR_YELLOW);
					print_colored(" bonus armoru (do nast. ataku wroga)!\n\t", COLOR_YELLOW);
				}

				tank_wrath_stored = 0;
			}
		}

		pause_game();
	}
	else
	{
		clear_screen();
		std::cout << "\n\t";
		print_colored("Tura Przeciwnika!\n\n\t", COLOR_BRIGHT_WHITE);
		print_colored("Przeciwnik nie trafił!\n\t", COLOR_GREEN);
		print_colored("Nie otrzymujesz obrażeń!\n\n", COLOR_GREEN);
		pause_game();
	}

	if (player_health <= 0)
	{
		clear_screen();
		std::cout << "\n\tGAME OVER!\n\n";
		pause_game();
		exit(0);
	}
	return false;
}

// =================================================================
//  STUN TICK
// =================================================================

void do_enemy_stun_tick(int enemyIndex)
{
	int dot_dealt = 0;
	if (cd_deadly_vines > 0 && enemy_stun_turns > 0)
	{
		int ap = get_total_ap();
		int raw_dot = (int)(5 + ap * 0.03f);
		dot_dealt = deal_magic_damage(raw_dot, enemyIndex);
		if (dot_dealt > current_enemy_health)
			dot_dealt = current_enemy_health;
		current_enemy_health -= dot_dealt;
	}
	enemy_stun_turns--;
	clear_screen();
	std::cout << "\n\t";
	print_colored("Tura Przeciwnika!\n\n\t", COLOR_BRIGHT_WHITE);
	print_colored("Przeciwnik jest ogłuszony i nie może zaatakować!\n", COLOR_YELLOW);
	if (dot_dealt > 0)
	{
		std::cout << "\t";
		print_colored("Zabójcze Pnącza zadają ", COLOR_DARK_GREEN);
		number_colored(dot_dealt, COLOR_RED);
		print_colored(" obrażeń!\n", COLOR_DARK_GREEN);
		apply_spell_vamp(dot_dealt);
	}
	if (enemy_stun_turns > 0)
	{
		std::cout << "\t";
		print_colored("Pozostałe tury ogłuszenia: ", COLOR_YELLOW);
		number_colored(enemy_stun_turns, COLOR_YELLOW);
	}
	std::cout << "\n\n";
	pause_game();
}

// =================================================================
//  RESET fali
// =================================================================

void reset_wave_enemy(int enemyIndex)
{
	if (enemy[enemyIndex].boss)
	{
		float hpM = 1.0f + ((current_wave - 1) * 0.25f);
		float dmM = 1.0f + ((current_wave - 1) * 0.15f);
		current_enemy_max_health = (int)(enemy[enemyIndex].health * hpM);
		current_enemy_health = current_enemy_max_health;
		current_enemy_damage = (int)(enemy[enemyIndex].damage * dmM);
	}
	else
	{
		current_enemy_max_health = enemy[enemyIndex].health;
		current_enemy_health = current_enemy_max_health;
		current_enemy_damage = enemy[enemyIndex].damage;
	}
}

// =================================================================
//  GŁÓWNA PĘTLA WALKI
// =================================================================

void game_battle()
{
	clock_t battle_start = clock();

	const int choice_amount = 4;
	std::string choices[] = { "Walcz", "Czary", "Przedmioty", "Uciekaj" };

	int enemyIndex = player_level - 1;
	if (enemyIndex >= enemy_amount)
	{
		clear_screen();
		std::cout << "\n\tUkończyłeś wszystkie dostępne poziomy gry!\n\tDzięki za grę :)\n\n";
		pause_game();
		return;
	}

	total_waves = enemy[enemyIndex].wave;
	current_wave = 1;
	reset_wave_enemy(enemyIndex);

	int choice = 0, key;

	while (true)
	{
		clear_screen();
		std::cout << "\n";
		print_colored("\tLvl", COLOR_BRIGHT_WHITE);
		std::cout << ": " << player_level << " | ";
		print_colored("Runda", COLOR_BRIGHT_WHITE);
		std::cout << ": " << current_wave << "/" << total_waves << "\n\n";

		std::cout << "\t                    ";
		print_colored(enemy[enemyIndex].name, COLOR_RED);
		if (enemy_stun_turns > 0)
		{
			print_colored(" [OGŁUSZONY:", COLOR_YELLOW);
			number_colored(enemy_stun_turns, COLOR_YELLOW);
			print_colored("t]", COLOR_YELLOW);
		}
		std::cout << "\n\t                    ";
		draw_enemy_hp_bar(current_enemy_health, current_enemy_max_health);
		std::cout << "\n\n\t                    ";
		print_colored("DMG:", COLOR_RED);
		std::cout << current_enemy_damage;
		std::cout << "  ";
		print_colored("Armor:", COLOR_YELLOW);
		std::cout << enemy[enemyIndex].armor;
		std::cout << "  ";
		print_colored("MR:", COLOR_CYAN);
		std::cout << enemy[enemyIndex].magic_resist;
		std::cout << "\n\n\t";
		print_colored(nickname, player_nickname_color);
		std::cout << ":\n       ";
		draw_player_hp_bar(player_health, player_maxhealth);
		std::cout << "\n\t";
		draw_player_mana_bar(player_mana, player_maxmana);
		std::cout << "\n\n\t";
		print_colored("Broń", COLOR_BRIGHT_WHITE);
		std::cout << ": ";
		print_colored(player_weapon_name, COLOR_CYAN);
		std::cout << " - ";
		int show_dmg = player_weapon_damage + player_class_bonus_ad;
		if (player_class == CLASS_ASSASSIN)
			show_dmg = (int)(show_dmg * 1.10f);
		if (player_class == CLASS_SAMURAI)
			show_dmg = (int)(show_dmg * 1.15f);
		number_colored(show_dmg, COLOR_RED);
		print_colored(" DMG", COLOR_RED);
		std::cout << "\n\t";
		print_colored("Aktywne Mikstury: ", COLOR_GREEN);
		if (accuracy_potion_turns > 0) {
			std::cout << "\n\t";
			print_colored("Eliksir Precyzji: ", COLOR_CYAN);
			std::cout << "pozostało "; std::cout << accuracy_potion_turns;
			if (accuracy_potion_turns == 1) std::cout << " tura"; else std::cout << " tur";
		}
		if (vampirism_potion_turns > 0) {
			std::cout << "\n\t";
			print_colored("Koktajl Wampira: ", COLOR_PURPLE);
			std::cout << "pozostało "; std::cout << vampirism_potion_turns;
			if (vampirism_potion_turns == 1) std::cout << " tura"; else std::cout << " tur";	
		}
		if (vampirism_potion_turns == 0 || accuracy_potion_turns == 0) std::cout << "\n\tbrak";

		// Aktywne efekty
		if (primal_strike_active)
		{
			std::cout << "\n\t";
			print_colored("[Pierwotne Uderzenie!]", COLOR_PURPLE);
		}
		if (mirror_active)
		{
			std::cout << "\n\t";
			print_colored("[Śmiertelne Lustro!]", COLOR_CYAN);
		}
		if (undodgeable_turns > 0)
		{
			std::cout << "\n\t";
			print_colored("[Nieomijalna Prędkość:", COLOR_CYAN);
			number_colored(undodgeable_turns, COLOR_CYAN);
			print_colored("]", COLOR_CYAN);
		}
		if (overkill_stored > 0)
		{
			std::cout << "\n\t";
			print_colored("[Overkill:", COLOR_YELLOW);
			number_colored(overkill_stored, COLOR_YELLOW);
			print_colored("]", COLOR_YELLOW);
			if (overkill_armor_ignore)
				print_colored("[Ignoruj 80% pancerza!]", COLOR_RED);
		}
		if (tank_bastion_armor_turns > 0)
		{
			std::cout << "\n\t";
			print_colored("[Bastion:", COLOR_YELLOW);
			number_colored(tank_bastion_armor_turns, COLOR_YELLOW);
			print_colored("t +", COLOR_YELLOW);
			number_colored(tank_bastion_armor_bonus, COLOR_YELLOW);
			print_colored("arm]", COLOR_YELLOW);
		}
		if (tank_taunt_turns > 0)
		{
			std::cout << "\n\t";
			print_colored("[Prowokacja:", COLOR_CYAN);
			number_colored(tank_taunt_turns, COLOR_CYAN);
			print_colored("t]", COLOR_CYAN);
		}
		if (tank_wrath_stored > 0)
		{
			std::cout << "\n\t";
			print_colored("[Gniew skumulowany:", COLOR_RED);
			number_colored(tank_wrath_stored, COLOR_RED);
			print_colored("]", COLOR_RED);
		}
		if (sen_no_kata_active)
		{
			std::cout << "\n\t";
			print_colored("[Sen no Kata - Chikara]", COLOR_RED);
		}
		if (mushin_shield_active)
		{
			std::cout << "\n\t";
			print_colored("[Mushin: redukcja 30%!]", COLOR_CYAN);
		}
		if (player_class == CLASS_SAMURAI)
		{
			if (sen_no_kata_active == false) {
				std::cout << "\n\t";
				print_colored("[Sen no Kata - Bōei]", COLOR_GREEN);
				print_colored("[Atak: ", COLOR_GRAY);
				number_colored(samurai_attack_counter, COLOR_GRAY);
				print_colored("/3]", COLOR_GRAY);
			}
		}

		std::cout << "\n\n";

		for (int i = 0; i < choice_amount; i++)
		{
			if (i == choice)
			{
				std::cout << "\t►  ";
				print_colored(choices[i], COLOR_BRIGHT_WHITE);
			}
			else
			{
				std::cout << "\t  " << choices[i];
			}
			std::cout << "\n";
		}

		key = _getch();
		if (key == 224)
		{
			key = _getch();
			if (key == 72)
			{
				choice--;
				if (choice < 0)
					choice = choice_amount - 1;
			}
			else if (key == 80)
			{
				choice++;
				if (choice >= choice_amount)
					choice = 0;
			}
			continue;
		}
		if (key != 13)
			continue;

		// ── WALCZ ──
		if (choice == 0)
		{
			bool killed = do_player_attack(enemyIndex);
			if (killed)
			{
				clock_t now = clock();
				float sec = (float)(now - battle_start) / CLOCKS_PER_SEC;
				handle_wave_kill(enemyIndex, sec);
				current_wave++;
				battle_start = clock();
				if (current_wave > total_waves)
				{
					std::cout << "\tGratulacje! Przeszłeś poziom " << player_level << "!\n\n";
					player_level++;
					player_escape_count = 0;
					overkill_stored = 0;
					save_game();
					pause_game();
					return;
				}
				reset_wave_enemy(enemyIndex);
				pause_game();
				continue;
			}
			if (enemy_stun_turns > 0)
				do_enemy_stun_tick(enemyIndex);
			else
				do_enemy_attack(enemyIndex);
			if (current_enemy_health <= 0)
			{
				clock_t now = clock();
				float sec = (float)(now - battle_start) / CLOCKS_PER_SEC;
				handle_wave_kill(enemyIndex, sec);
				current_wave++;
				battle_start = clock();
				if (current_wave > total_waves)
				{
					std::cout << "\tGratulacje! Przeszłeś poziom " << player_level << "!\n\n";
					player_level++;
					player_escape_count = 0;
					overkill_stored = 0;
					save_game();
					pause_game();
					return;
				}
				reset_wave_enemy(enemyIndex);
				pause_game();
				continue;
			}
			if (accuracy_potion_turns > 0)
				accuracy_potion_turns--;
			if (vampirism_potion_turns > 0)
				vampirism_potion_turns--;
			// Prowokacja: CD startuje po minięciu efektu
			if (tank_taunt_turns == 0 && cd_iron_taunt == 0)
			{ /* idle */
			}
			tick_cooldowns();
			regen_mana();
		}

		// ── CZARY ──
		else if (choice == 1)
		{
			// Pauza timera — zapisujemy czas przed menu
			clock_t before_menu = clock();

			bool spell_used = false;
			battle_spells_menu(enemyIndex, spell_used);

			// Dolicz czas spędzony w menu do battle_start żeby nie liczyć go do bonusu
			clock_t after_menu = clock();
			battle_start += (after_menu - before_menu);

			if (!spell_used)
				continue;

			if (current_enemy_health <= 0)
			{
				clock_t now = clock();
				float sec = (float)(now - battle_start) / CLOCKS_PER_SEC;
				handle_wave_kill(enemyIndex, sec);
				current_wave++;
				battle_start = clock();
				if (current_wave > total_waves)
				{
					std::cout << "\tGratulacje! Przeszłeś poziom " << player_level << "!\n\n";
					player_level++;
					player_escape_count = 0;
					overkill_stored = 0;
					save_game();
					pause_game();
					return;
				}
				reset_wave_enemy(enemyIndex);
				pause_game();
				continue;
			}

			if (enemy_stun_turns > 0)
				do_enemy_stun_tick(enemyIndex);
			else
				do_enemy_attack(enemyIndex);

			if (current_enemy_health <= 0)
			{
				clock_t now = clock();
				float sec = (float)(now - battle_start) / CLOCKS_PER_SEC;
				handle_wave_kill(enemyIndex, sec);
				current_wave++;
				battle_start = clock();
				if (current_wave > total_waves)
				{
					std::cout << "\tGratulacje! Przeszłeś poziom " << player_level << "!\n\n";
					player_level++;
					player_escape_count = 0;
					overkill_stored = 0;
					save_game();
					pause_game();
					return;
				}
				reset_wave_enemy(enemyIndex);
				pause_game();
				continue;
			}

			if (accuracy_potion_turns > 0)
				accuracy_potion_turns--;
			if (vampirism_potion_turns > 0)
				vampirism_potion_turns--;
			tick_cooldowns();
			regen_mana();
		}

		// ── PRZEDMIOTY ──
		else if (choice == 2)
		{
			battle_items_menu();
		}

		// ── UCIEKAJ ──
		else if (choice == 3)
		{
			if (player_escape_count >= 1)
			{
				clear_screen();
				std::cout << "\n\tMożesz uciec tylko raz na poziom!\n\n";
				pause_game();
				continue;
			}
			clear_screen();
			int heal = player_maxhealth / 2;
			int mana_restore = player_maxmana / 2;
			player_health += heal;
			player_mana += mana_restore;
			if (player_health > player_maxhealth)
				player_health = player_maxhealth;
			if (player_mana > player_maxmana)
				player_mana = player_maxmana;
			player_escape_count++;
			std::cout << "\n\tUciekasz z pola walki!\n\tOdzyskano " << heal << " HP i " << mana_restore << " many!\n\n";
			pause_game();
			return;
		}
	}
}

void battle_items_menu()
{
	int choice = 0, key;
	const int N = 4;
	std::string items[N] = { "Mikstura Zdrowia", "Eliksir Precyzji", "Koktajl Wampira", "Powrót" };

	while (true)
	{
		clear_screen();
		print_colored("\n\tPrzedmioty\n\n", COLOR_BRIGHT_WHITE);
		std::cout << "\t";
		print_colored("Mikstura Zdrowia: ", COLOR_GREEN);
		player_health_potion > 0 ? number_colored(player_health_potion, COLOR_GREEN) : number_colored(player_health_potion, COLOR_RED);
		std::cout << "\n\t";
		print_colored("Eliksir Precyzji: ", COLOR_CYAN);
		player_accuracy_potion > 0 ? number_colored(player_accuracy_potion, COLOR_CYAN) : number_colored(player_accuracy_potion, COLOR_RED);
		std::cout << "\n\t";
		print_colored("Koktajl Wampira: ", COLOR_PURPLE);
		player_vampirism_potion > 0 ? number_colored(player_vampirism_potion, COLOR_PURPLE) : number_colored(player_vampirism_potion, COLOR_RED);
		std::cout << "\n\n";

		for (int i = 0; i < N; i++)
		{
			if (i == choice)
			{
				std::cout << "\t►  ";
				print_colored(items[i], COLOR_BRIGHT_WHITE);
			}
			else
			{
				std::cout << "\t  " << items[i];
			}
			std::cout << "\n";
		}

		key = _getch();
		if (key == 224)
		{
			key = _getch();
			if (key == 72)
			{
				choice--;
				if (choice < 0)
					choice = N - 1;
			}
			else if (key == 80)
			{
				choice++;
				if (choice >= N)
					choice = 0;
			}
			continue;
		}
		if (key == 27)
			return;
		if (key != 13)
			continue;

		if (choice == 0)
		{
			if (player_health_potion <= 0)
			{
				clear_screen();
				std::cout << "\n\tNie masz Mikstur Zdrowia!\n\n";
				pause_game();
				continue;
			}
			int h = std::min(250, (int)(25 + player_maxhealth * 0.07f));
			player_health += h;
			if (player_health > player_maxhealth)
				player_health = player_maxhealth;
			player_health_potion--;
			clear_screen();
			std::cout << "\n\t";
			print_colored("Użyto Mikstury Zdrowia!\n\t", COLOR_GREEN);
			print_colored("Odzyskano ", COLOR_GREEN);
			number_colored(h, COLOR_GREEN);
			print_colored(" HP!\n\n", COLOR_GREEN);
			pause_game();
			return;
		}
		if (choice == 1)
		{
			if (player_accuracy_potion <= 0)
			{
				clear_screen();
				std::cout << "\n\tNie masz Eliksiru Precyzji!\n\n";
				pause_game();
				continue;
			}
			if (accuracy_potion_turns > 0)
			{
				clear_screen();
				std::cout << "\n\tEfekt Eliksiru Precyzji już aktywny!\n\n";
				pause_game();
				continue;
			}
			accuracy_potion_bonus = 50;
			accuracy_potion_turns = 7;
			player_accuracy_potion--;
			clear_screen();
			std::cout << "\n\t";
			print_colored("Użyto Eliksiru Precyzji!\n\t+50% szansy trafienia na 7 tur!\n\n", COLOR_CYAN);
			pause_game();
			return;
		}
		if (choice == 2)
		{
			if (player_vampirism_potion <= 0)
			{
				clear_screen();
				std::cout << "\n\tNie masz Koktajlu Wampira!\n\n";
				pause_game();
				continue;
			}
			if (vampirism_potion_turns > 0)
			{
				clear_screen();
				std::cout << "\n\tEfekt Koktajlu Wampira już aktywny!\n\n";
				pause_game();
				continue;
			}
			vampirism_potion_bonus = 10;
			vampirism_potion_turns = 10;
			player_vampirism_potion--;
			clear_screen();
			std::cout << "\n\t";
			print_colored("Użyto Koktajlu Wampira!\n\t+10% lifestealu na 10 tur!\n\n", COLOR_PURPLE);
			pause_game();
			return;
		}
		if (choice == 3)
			return;
	}
}
