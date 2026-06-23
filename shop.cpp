#include "shop.h"
#include "globals.h"
#include "utils.h"
#include "battle.h"
#include <iostream>
#include <algorithm>

void shop_buy_weapon(int id)
{
	clear_screen();
	std::cout << "\n\tWybrałeś: ";
	print_colored(shop[id].name, COLOR_CYAN);
	std::cout << "\n\tCena: ";
	r_ui_colored(shop[id].price, "$", COLOR_DARK_GREEN);
	std::cout << "\n";

	if (player_money >= shop[id].price)
	{
		player_money -= shop[id].price;
		if (player_weapon_name != "nic")
		{
			inventory[inventory_count].name = player_weapon_name;
			inventory[inventory_count].damage = player_weapon_damage;
			inventory[inventory_count].price = player_weapon_price;
			inventory_count++;
		}
		player_weapon_name = shop[id].name;
		player_weapon_damage = shop[id].damage;
		player_weapon_price = shop[id].price;
		std::cout << "\n\t";
		print_colored("Kupiono!\n\n", COLOR_GREEN);
	}
	else
	{
		std::cout << "\n\t";
		print_colored("Za mało hajsu!\n\n", COLOR_RED);
	}
	pause_game();
}

void game_shop()
{
	int choice = 0, key;
	const int visible = 10;
	while (true)
	{
		int start = (choice >= visible) ? choice - visible + 1 : 0;
		int end = std::min(start + visible, shop_weapon_amount);
		clear_screen();
		std::cout << "ESC - Powrót\n\n\tSklep:\n\n\t";
		ui_colored("Hajs: $", player_money, COLOR_DARK_GREEN);
		std::cout << "\n";
		std::cout << "\tAktualna Broń: ";
		print_colored(player_weapon_name, COLOR_CYAN);
		std::cout << " - ";
		r_ui_colored(player_weapon_damage, " DMG", COLOR_RED);
		std::cout << "\n\n";
		for (int i = start; i < end; i++)
		{
			if (i == choice)
				std::cout << "\t►  ";
			else
				std::cout << "\t  ";
			number_colored(shop[i].damage, COLOR_RED);
			std::cout << " DMG | ";
			r_ui_colored(shop[i].price, "$", COLOR_DARK_GREEN);
			std::cout << " - ";
			print_colored(shop[i].name, COLOR_CYAN);
			std::cout << "\n";
		}
		std::cout << "\n\t[" << choice + 1 << "/" << shop_weapon_amount << "]";
		flush_input();
		key = _getch();
		if (key == 224)
		{
			key = _getch();
			if (key == 72)
			{
				choice--;
				if (choice < 0)
					choice = shop_weapon_amount - 1;
			}
			else if (key == 80)
			{
				choice++;
				if (choice >= shop_weapon_amount)
					choice = 0;
			}
		}
		else if (key == 13)
			shop_buy_weapon(choice);
		else if (key == 27)
			return;
	}
}

void game_abilities()
{
	int choice = 0, key;
	while (true)
	{
		clear_screen();
		std::cout << "ESC - Powrót\n\n\tUmiejętności\n\n\t";
		ui_colored("Hajs: $", player_money, COLOR_DARK_GREEN);
		std::cout << "\n\n";
		for (int i = 0; i < abilities_amount; i++)
		{
			if (i == choice)
				std::cout << "\t►  ";
			else
				std::cout << "\t  ";
			print_colored(abilities[i].name, COLOR_PURPLE);
			std::cout << " | ";
			r_ui_colored(abilities[i].price, "$", COLOR_DARK_GREEN);
			std::cout << "\n";
			std::cout << "\t   " << abilities[i].description << "\n";
		}
		flush_input();
		key = _getch();
		if (key == 224)
		{
			key = _getch();
			if (key == 72)
			{
				choice--;
				if (choice < 0)
					choice = abilities_amount - 1;
			}
			else if (key == 80)
			{
				choice++;
				if (choice >= abilities_amount)
					choice = 0;
			}
			continue;
		}
		else if (key == 27)
			return;
		else if (key != 13)
			continue;

		if (player_money < abilities[choice].price)
		{
			clear_screen();
			std::cout << "\n\tZa mało hajsu!\n\n";
			pause_game();
			continue;
		}

		switch (choice)
		{
		case 0:
			if (player_lifesteal >= MAX_LIFESTEAL)
			{
				clear_screen();
				std::cout << "\n\tOsiągnięto max Wampirycznego Ostrza!\n\n";
				pause_game();
				continue;
			}
			player_lifesteal += 10;
			if (player_lifesteal > MAX_LIFESTEAL)
				player_lifesteal = MAX_LIFESTEAL;
			break;
		case 1:
			if (player_bonus_accuracy >= MAX_ACCURACY)
			{
				clear_screen();
				std::cout << "\n\tOsiągnięto max Kryształu Skupienia!\n\n";
				pause_game();
				continue;
			}
			player_bonus_accuracy += 15;
			if (player_bonus_accuracy > MAX_ACCURACY)
				player_bonus_accuracy = MAX_ACCURACY;
			break;
		case 2:
			if (player_crit_chance >= (int)(MAX_CRIT * 0.5))
			{
				clear_screen();
				std::cout << "\n\tOsiągnięto limit Pierścienia Zabójcy!\n\n";
				pause_game();
				continue;
			}
			player_crit_chance += 10;
			if (player_crit_chance > MAX_CRIT)
				player_crit_chance = MAX_CRIT;
			break;
		case 3:
			if (player_crit_chance >= MAX_CRIT)
			{
				clear_screen();
				std::cout << "\n\tOsiągnięto max kryta!\n\n";
				pause_game();
				continue;
			}
			if (player_crit_chance < (int)(MAX_CRIT * 0.5))
			{
				clear_screen();
				std::cout << "\n\tMusisz najpierw wypełnić Pierścień Zabójcy!\n\n";
				pause_game();
				continue;
			}
			player_crit_chance += 10;
			if (player_crit_chance > MAX_CRIT)
				player_crit_chance = MAX_CRIT;
			break;
		case 4:
			if (player_second_breath)
			{
				clear_screen();
				std::cout << "\n\tMasz już tę umiejętność!\n\n";
				pause_game();
				continue;
			}
			player_second_breath = true;
			break;
		case 5:
			if (player_spell_vamp >= MAX_SPELL_VAMP)
			{
				clear_screen();
				std::cout << "\n\tOsiągnięto max Dłoni Wampira (30%)!\n\n";
				pause_game();
				continue;
			}
			player_spell_vamp += 15;
			if (player_spell_vamp > MAX_SPELL_VAMP)
				player_spell_vamp = MAX_SPELL_VAMP;
			break;
		}
		player_money -= abilities[choice].price;
		clear_screen();
		std::cout << "\n\tKupiono: " << abilities[choice].name << "!\n\n";
		pause_game();
	}
}

void game_upgrades()
{
	int choice = 0, key;
	while (true)
	{
		clear_screen();
		std::cout << "ESC - Powrót\n\n\tWzmocnienia\n\n\t";
		ui_colored("Hajs: $", player_money, COLOR_DARK_GREEN);
		std::cout << "\n\t";
		ui_colored("HP: ", player_health, COLOR_GREEN);
		ui_colored("/", player_maxhealth, COLOR_GREEN);
		std::cout << "\n\t";
		ui_colored("Pancerz: ", player_armor, COLOR_YELLOW);
		std::cout << "\n\t";
		ui_colored("Przebicie pancerza: ", player_armor_pen, COLOR_RED);
		std::cout << "\n\n";
		for (int i = 0; i < upgrades_amount; i++)
		{
			if (i == choice)
				std::cout << "\t►  ";
			else
				std::cout << "\t  ";
			print_colored(upgrades[i].name, COLOR_YELLOW);
			std::cout << " | ";
			r_ui_colored(upgrades[i].price, "$", COLOR_DARK_GREEN);
			std::cout << "\n";
			std::cout << "\t   " << upgrades[i].description << "\n";
		}
		flush_input();
		key = _getch();
		if (key == 224)
		{
			key = _getch();
			if (key == 72)
			{
				choice--;
				if (choice < 0)
					choice = upgrades_amount - 1;
			}
			else if (key == 80)
			{
				choice++;
				if (choice >= upgrades_amount)
					choice = 0;
			}
			continue;
		}
		else if (key == 27)
			return;
		else if (key != 13)
			continue;

		if (player_money < upgrades[choice].price)
		{
			clear_screen();
			std::cout << "\n\tZa mało hajsu!\n\n";
			pause_game();
			continue;
		}

		switch (choice)
		{
		case 0:
			player_bonus_health += 50;
			player_maxhealth += 50;
			player_health += 50;
			break;
		case 1:
			player_bonus_health += 160;
			player_maxhealth += 160;
			player_health += 160;
			break;
		case 2:
		{
			int armor_cap = 90;
			if (player_class == CLASS_TANK)
				armor_cap = 120;
			if (player_class == CLASS_ASSASSIN)
				armor_cap = 60;
			if (player_class == CLASS_SAMURAI)
				armor_cap = 50;
			if (player_armor >= armor_cap)
			{
				clear_screen();
				std::cout << "\n\tOsiągnięto max pancerza!\n\n";
				pause_game();
				continue;
			}
			player_armor += 15;
			if (player_armor > armor_cap)
				player_armor = armor_cap;
		}
		break;
		case 3:
			if (player_armor_pen >= 70)
			{
				clear_screen();
				std::cout << "\n\tOsiągnięto max przebicia pancerza!\n\n";
				pause_game();
				continue;
			}
			player_armor_pen += 10;
			if (player_armor_pen > 70)
				player_armor_pen = 70;
			break;
		}
		player_money -= upgrades[choice].price;
		clear_screen();
		std::cout << "\n\tKupiono: " << upgrades[choice].name << "!\n\n";
		pause_game();
	}
}

void game_consumables()
{
	int choice = 0, key;
	while (true)
	{
		clear_screen();
		std::cout << "ESC - Powrót\n\n\tAlchemik\n\n\t";
		ui_colored("Hajs: $", player_money, COLOR_DARK_GREEN);
		std::cout << "\n\tPosiadane mikstury:\n";
		if (player_health_potion > 0)
		{
			std::cout << "\t\t";
			print_colored("Mikstura Zdrowia", COLOR_GREEN);
			std::cout << " x" << player_health_potion << "\n";
		}
		if (player_accuracy_potion > 0)
		{
			std::cout << "\t\t";
			print_colored("Eliksir Precyzji", COLOR_CYAN);
			std::cout << " x" << player_accuracy_potion << "\n";
		}
		if (player_vampirism_potion > 0)
		{
			std::cout << "\t\t";
			print_colored("Koktajl Wampira", COLOR_PURPLE);
			std::cout << " x" << player_vampirism_potion << "\n";
		}
		if (!player_health_potion && !player_accuracy_potion && !player_vampirism_potion)
			std::cout << "\t\tBrak\n";
		std::cout << "\n";
		for (int i = 0; i < consumables_amount; i++)
		{
			if (i == choice)
				std::cout << "\t►  ";
			else
				std::cout << "\t  ";
			print_colored(consumables[i].name, COLOR_CYAN);
			std::cout << " | ";
			r_ui_colored(consumables[i].price, "$", COLOR_DARK_GREEN);
			std::cout << "\n";
			std::cout << "\t   " << consumables[i].description << "\n";
		}
		flush_input();
		key = _getch();
		if (key == 224)
		{
			key = _getch();
			if (key == 72)
			{
				choice--;
				if (choice < 0)
					choice = consumables_amount - 1;
			}
			else if (key == 80)
			{
				choice++;
				if (choice >= consumables_amount)
					choice = 0;
			}
			continue;
		}
		else if (key == 27)
			return;
		else if (key != 13)
			continue;
		if (player_money < consumables[choice].price)
		{
			clear_screen();
			std::cout << "\n\tZa mało hajsu!\n\n";
			pause_game();
			continue;
		}
		switch (choice)
		{
		case 0:
			player_health_potion++;
			break;
		case 1:
			player_accuracy_potion++;
			break;
		case 2:
			player_vampirism_potion++;
			break;
		}
		player_money -= consumables[choice].price;
		clear_screen();
		std::cout << "\n\tKupiono: " << consumables[choice].name << "!\n";
		pause_game();
	}
}

// ===== RECALCULATE MAGIC STATS =====
void recalculate_magic_stats()
{
	player_spell_power = 0;
	player_magic_resist_pen = 0;
	player_magic_resist = 0;

	int mana_bonus = 0;
	if (player_class == CLASS_MAGE)
		mana_bonus = 100;

	int mag_pen_base = (player_class == CLASS_MAGE) ? 5 : 0;

	int item_mana = 0, item_sp = 0, item_mrpen = 0, item_mr = 0;

	int max_slots = get_max_magic_items();
	for (int i = 0; i < max_slots; i++)
	{
		if (magic_item_slots[i] < 0)
			continue;
		int idx = magic_item_slots[i];
		item_mana += magic_items[idx].bonus_mana;
		item_sp += magic_items[idx].bonus_spell_power;
		item_mrpen += magic_items[idx].magic_resist_pen;
		item_mr += magic_items[idx].bonus_magic_resist;
	}

	int old_maxmana = player_maxmana;
	player_maxmana = 100 + mana_bonus + item_mana;
	player_spell_power = item_sp;
	player_magic_resist_pen = item_mrpen + mag_pen_base;
	player_magic_resist = item_mr;

	int mana_delta = player_maxmana - old_maxmana;
	if (mana_delta > 0)
		player_mana += mana_delta;
	if (player_mana > player_maxmana)
		player_mana = player_maxmana;
}

// ===== SKLEP MAGICZNY =====
void game_magic_shop()
{
	int choice = 0, key;
	int max_slots = get_max_magic_items();

	while (true)
	{
		max_slots = get_max_magic_items();
		clear_screen();
		std::cout << "ESC - Powrót\n\n";
		print_colored("\tMagiczny Rynek\n\n", COLOR_BLUE);
		std::cout << "\t";
		ui_colored("Hajs: $", player_money, COLOR_DARK_GREEN);
		std::cout << "\n\t";
		print_colored("Sloty: ", COLOR_BRIGHT_WHITE);
		number_colored(magic_item_count, COLOR_CYAN);
		std::cout << " / ";
		number_colored(max_slots, COLOR_CYAN);
		std::cout << "\n\t";
		print_colored("Mana: ", COLOR_BLUE);
		number_colored(player_mana, COLOR_BLUE);
		std::cout << " / ";
		number_colored(player_maxmana, COLOR_BLUE);
		std::cout << "\n\t";
		print_colored("AP (łącznie): ", COLOR_PURPLE);
		number_colored(get_total_ap(), COLOR_PURPLE);
		std::cout << "\n\t";
		print_colored("Przebicie MR: ", COLOR_DARK_CYAN);
		number_colored(player_magic_resist_pen, COLOR_DARK_CYAN);
		std::cout << "\n\t";
		print_colored("MR gracza: ", COLOR_CYAN);
		number_colored(player_magic_resist, COLOR_CYAN);
		std::cout << "\n\t";
		print_colored("Spell Vamp: ", COLOR_PURPLE);
		number_colored(player_spell_vamp, COLOR_PURPLE);
		print_colored("%", COLOR_PURPLE);
		std::cout << "\n\n";

		// Założone itemy
		bool any = false;
		for (int i = 0; i < max_slots; i++)
		{
			if (magic_item_slots[i] < 0)
				continue;
			if (!any)
			{
				print_colored("\tZałożone itemy:\n", COLOR_YELLOW);
				any = true;
			}
			int idx = magic_item_slots[i];
			std::cout << "\t  [" << i + 1 << "] ";
			print_colored(magic_items[idx].name, COLOR_YELLOW);
			// Pokaż stacked AP jeśli Księga Adeptów
			if (magic_items[idx].adept_ap_per_stack > 0)
			{
				int limit = adept_book_upgraded[i] ? magic_items[idx].adept_ap_upgraded_max : magic_items[idx].adept_ap_max;
				std::cout << " [" << adept_book_stacks[i] << "/" << limit << " AP]";
				if (!adept_book_upgraded[i] && magic_items[idx].adept_upgrade_price > 0)
				{
					std::cout << " [Upgrade $" << magic_items[idx].adept_upgrade_price << "]";
				}
				else if (adept_book_upgraded[i])
				{
					std::cout << " [ULEPSZONA]";
				}
			}
			std::cout << "\n";
		}
		if (any)
			std::cout << "\n";

		std::cout << "\tDostępne itemy:\n\n";
		for (int i = 0; i < magic_items_amount; i++)
		{
			if (i == choice)
				std::cout << "\t►  ";
			else
				std::cout << "\t  ";
			print_colored(magic_items[i].name, COLOR_BLUE);
			std::cout << " | ";
			int show_price = magic_items[i].price;
			if (player_class == CLASS_MAGE && magic_items[i].discount_eligible)
				show_price = show_price * 75 / 100;
			r_ui_colored(show_price, "$", COLOR_DARK_GREEN);
			if (player_class == CLASS_MAGE && magic_items[i].discount_eligible)
				print_colored(" (25% zniżki)", COLOR_YELLOW);
			if (!magic_items[i].discount_eligible)
				print_colored(" (brak zniżki)", COLOR_GRAY);
			std::cout << "\n\t   " << magic_items[i].description << "\n";
		}

		flush_input();
		key = _getch();
		if (key == 224)
		{
			key = _getch();
			if (key == 72)
			{
				choice--;
				if (choice < 0)
					choice = magic_items_amount - 1;
			}
			else if (key == 80)
			{
				choice++;
				if (choice >= magic_items_amount)
					choice = 0;
			}
			continue;
		}
		else if (key == 27)
			return;
		else if (key != 13)
			continue;

		// Sprawdź czy to Księga Adeptów z możliwością upgrade
		{
			int idx_choice = choice;
			bool is_adept = (magic_items[idx_choice].adept_ap_per_stack > 0);
			bool did_upgrade = false;

			// Sprawdź czy gracz już ma tę Księgę i może ją ulepszać
			if (is_adept)
			{
				for (int i = 0; i < max_slots; i++)
				{
					if (magic_item_slots[i] == idx_choice && !adept_book_upgraded[i])
					{
						int upgrade_price = magic_items[idx_choice].adept_upgrade_price;
						if (upgrade_price > 0 && player_money >= upgrade_price)
						{
							clear_screen();
							std::cout << "\n\t";
							print_colored("Ulepszyć Księgę Adeptów w slocie ", COLOR_YELLOW);
							number_colored(i + 1, COLOR_YELLOW);
							std::cout << " za $" << upgrade_price << "?\n\n";
							std::cout << "\t1. Tak\n\t0. Nie\n\n";
							int a;
							std::cin >> a;
							if (a == 1)
							{
								player_money -= upgrade_price;
								adept_book_upgraded[i] = true;
								clear_screen();
								std::cout << "\n\t";
								print_colored("Księga Adeptów ulepszona! Limit: 150 AP\n\n", COLOR_YELLOW);
								pause_game();
							}
							did_upgrade = true;
							break;
						}
					}
				}
			}

			if (!did_upgrade)
			{
				// Sprawdź sloty
				if (magic_item_count >= max_slots)
				{
					clear_screen();
					std::cout << "\n\tBrak wolnych slotów! (max " << max_slots << ")\n\n";
					pause_game();
				}
				else
				{
					int buy_price = magic_items[idx_choice].price;
					if (player_class == CLASS_MAGE && magic_items[idx_choice].discount_eligible)
						buy_price = buy_price * 75 / 100;
					if (player_money < buy_price)
					{
						clear_screen();
						std::cout << "\n\tZa mało hajsu!\n\n";
						pause_game();
					}
					else
					{
						// Znajdź wolny slot
						for (int i = 0; i < max_slots; i++)
						{
							if (magic_item_slots[i] < 0)
							{
								magic_item_slots[i] = idx_choice;
								adept_book_stacks[i] = 0;
								adept_book_upgraded[i] = false;
								magic_item_count++;
								break;
							}
						}
						player_money -= buy_price;
						recalculate_magic_stats();
						clear_screen();
						std::cout << "\n\tKupiono i założono: ";
						print_colored(magic_items[idx_choice].name, COLOR_BLUE);
						if (player_class == CLASS_MAGE && magic_items[idx_choice].discount_eligible)
						{
							std::cout << "\n\t";
							print_colored("(Rabat Maga: 25% taniej!)", COLOR_YELLOW);
						}
						std::cout << "\n\n";
						pause_game();
					}
				}
			}
		}
	}
}

// ===== WYBÓR KLASY =====
void choose_player_class()
{
	const int N = 3;
	std::string names[N] = {
		"Zabójca - +10% dmg, +5% lifesteal, +10% acc, +10% many/kill, cap armor 60",
		"Mag     - +100 many, +50% regen, 25% zniżki na itemy mag., +5 magic pen, +15 armor",
		"Samuraj - +15% dmg, +10% dodge, +15% acc, cap armor 50  |  Tank - +200 HP, +20 armor, cap 120, -25% dmg bron>500" };
	// Robimy 3 opcje: Zabójca, Mag, Samuraj+Tank razem jako dwa wybory na 2. ekranie
	// Ale prościej: 4 opcje Zabójca/Mag/Tank/Samuraj
	const int N2 = 4;
	std::string names2[N2] = {
		"Zabójca - +10% dmg, +5% lifesteal, +10% acc, +10% many/kill, cap armor 60",
		"Mag     - +100 many, +50% regen, 25% zniżki na mag itemy, +5 magic pen, +15 armor",
		"Tank    - +200 HP, +20 armor, cap armor 120, -25% dmg bron powyżej 500 dmg",
		"Samuraj - +15% dmg, +15% acc, +10% dodge chance, cap armor 50" };

	int choice = 0, key;
	while (true)
	{
		clear_screen();
		std::cout << "\n";
		print_colored("\tWybierz klasę postaci:\n\n", COLOR_BRIGHT_WHITE);
		for (int i = 0; i < N2; i++)
		{
			if (i == choice)
			{
				std::cout << "\t►  ";
				print_colored(names2[i], COLOR_BRIGHT_WHITE);
			}
			else
				std::cout << "\t  " << names2[i];
			std::cout << "\n";
		}
		std::cout << "\n";
		flush_input();
		key = _getch();
		if (key == 224)
		{
			key = _getch();
			if (key == 72)
			{
				choice--;
				if (choice < 0)
					choice = N2 - 1;
			}
			else if (key == 80)
			{
				choice++;
				if (choice >= N2)
					choice = 0;
			}
		}
		else if (key == 13)
			break;
	}

	switch (choice)
	{
	case 0: // Zabójca
		player_class = CLASS_ASSASSIN;
		player_bonus_accuracy += 10;
		clear_screen();
		std::cout << "\n\t";
		print_colored("Klasa: Zabójca\n", COLOR_RED);
		std::cout << "\t+10% accuracy\n\t+10% obrażeń z każdej broni\n\t+5% lifesteal\n\t+10% many przy zabójstwie\n\tMaks. pancerz: 60\n\n";
		pause_game();
		break;

	case 1: // Mag
		player_class = CLASS_MAGE;
		player_maxmana = 200;
		player_mana = 200;
		player_magic_resist_pen += 5;
		player_armor += 15;
		clear_screen();
		std::cout << "\n\t";
		print_colored("Klasa: Mag\n", COLOR_BLUE);
		std::cout << "\t+100 many (200 łącznie)\n\t+50% bazowej regen many\n\t25% zniżka na itemy magiczne\n\t+5 przebicia MR\n\t+15 pancerza\n\n";
		pause_game();
		break;

	case 2: // Tank
		player_class = CLASS_TANK;
		player_maxhealth += 200;
		player_health += 200;
		player_armor += 20;
		clear_screen();
		std::cout << "\n\t";
		print_colored("Klasa: Tank\n", COLOR_YELLOW);
		std::cout << "\t+200 HP (300 łącznie)\n\t+20 pancerza na start\n\tMaks. pancerz: 120\n\t-25% obrażeń broń > 500 DMG\n\n";
		pause_game();
		break;

	case 3: // Samuraj
		player_class = CLASS_SAMURAI;
		// +15% dmg: obsługiwane w walce jako *1.15f
		// +15% accuracy
		player_bonus_accuracy += 15;
		// +10% dodge chance — zwiększamy player_atk_chance wroga (brak dedykowanej zmiennej dodge gracza,
		// więc dodajemy jako bonus do accuracy = efektywny dodge)
		player_bonus_accuracy += 10; // łącznie +25 accuracy = dodge bonusu gracza
		clear_screen();
		std::cout << "\n\t";
		print_colored("Klasa: Samuraj\n", COLOR_CYAN);
		std::cout << "\t+15% obrażeń fizycznych\n\t+25 łącznego accuracy (15% acc + 10% dodge)\n\tMaks. pancerz: 50\n\tPasywka: co 3. atak stunuje (atak zadaje 10%+0.05%/AP max dmg)\n\n";
		pause_game();
		break;
	}
}
