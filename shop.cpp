#include "shop.h"
#include "globals.h"
#include "utils.h"
#include <iostream>
using namespace std;

void shop_buy_weapon(int id) {

	clear_screen();

	cout << endl;
	cout << "\tWybrałeś: ";
	print_colored(shop[id].name, COLOR_CYAN);

	cout << endl;

	cout << "\tCena: ";
	r_ui_colored(shop[id].price, "$", COLOR_DARK_GREEN);

	cout << endl;

	if (player_money >= shop[id].price) {

		player_money -= shop[id].price;

		if (player_weapon_name != "nic") {

			inventory[inventory_count].name = player_weapon_name;
			inventory[inventory_count].damage = player_weapon_damage;
			inventory[inventory_count].price = player_weapon_price;

			inventory_count++;
		}

		player_weapon_name = shop[id].name;
		player_weapon_damage = shop[id].damage;
		player_weapon_price = shop[id].price;

		cout << endl << "\t";
		print_colored("Kupiono!", COLOR_GREEN);
		cout << endl << endl;

		pause_game();
	}
	else {

		cout << endl << "\t";
		print_colored("Za mało hajsu!", COLOR_RED);
		cout << endl << endl;

		pause_game();
	}
}

void game_shop() {
	int choice = 0;
	int keyboard_button;

	while (true) {
		clear_screen();
		cout << "ESC - Powrót\n\n";
		cout << "\tSklep:\n\n";
		cout << "\t";
		ui_colored("Hajs: $", player_money, COLOR_DARK_GREEN);
		cout << endl;

		cout << "\tAktualna Broń: ";
		print_colored(player_weapon_name, COLOR_CYAN);

		cout << " - ";
		r_ui_colored(player_weapon_damage, " DMG", COLOR_RED);

		cout << endl << endl;

		for (int i = 0; i < shop_weapon_amount; i++) {

			if (i == choice) {
				cout << "\t►  ";
			}
			else {
				cout << "\t  ";
			}

			number_colored(shop[i].damage, COLOR_RED);
			cout << " DMG | ";

			r_ui_colored(shop[i].price, "$", COLOR_DARK_GREEN);

			cout << " - ";

			print_colored(shop[i].name, COLOR_CYAN);

			cout << endl;
		}

		keyboard_button = _getch();

		if (keyboard_button == 224) {
			keyboard_button = _getch();
			if (keyboard_button == 72) {
				choice--;
				if (choice < 0) choice = shop_weapon_amount - 1;
			}
			else if (keyboard_button == 80) {
				choice++;
				if (choice >= shop_weapon_amount) choice = 0;
			}
		}
		else if (keyboard_button == 13) {
			shop_buy_weapon(choice);
		}
		else if (keyboard_button == 27) {
			return;
		}
	}
}

void game_abilities() {

	int choice = 0;
	int keyboard_button;

	while (true) {

		clear_screen();

		cout << "ESC - Powrót\n\n";
		cout << "\tUmiejętności\n\n";

		cout << "\t";
		ui_colored("Hajs: $", player_money, COLOR_DARK_GREEN);
		cout << endl << endl;

		for (int i = 0; i < abilities_amount; i++) {

			if (i == choice)
				cout << "\t►  ";
			else
				cout << "\t  ";

			print_colored(abilities[i].name, COLOR_PURPLE);
			cout << " | ";
			r_ui_colored(abilities[i].price, "$", COLOR_DARK_GREEN);
			cout << endl;
			cout << "\t   " << abilities[i].description << endl;
		}

		keyboard_button = _getch();

		if (keyboard_button == 224) {

			keyboard_button = _getch();

			if (keyboard_button == 72) {

				choice--;

				if (choice < 0)
					choice = abilities_amount - 1;
			}
			else if (keyboard_button == 80) {

				choice++;

				if (choice >= abilities_amount)
					choice = 0;
			}
		}
		else if (keyboard_button == 13) {

			if (player_money < abilities[choice].price) {

				clear_screen();

				cout << "\n\tZa mało hajsu!\n\n";

				pause_game();
				continue;
			}

			switch (choice) {

			case 0:

				if (player_lifesteal >= MAX_LIFESTEAL) {

					clear_screen();

					cout << "\n\tOsiągnięto maksymalny poziom Wampirycznego Ostrza!\n\n";

					pause_game();
					continue;
				}

				player_lifesteal += 10;

				if (player_lifesteal > MAX_LIFESTEAL)
					player_lifesteal = MAX_LIFESTEAL;

				break;

			case 1:

				if (player_bonus_accuracy >= MAX_ACCURACY) {

					clear_screen();

					cout << "\n\tOsiągnięto maksymalną ilość Kryształów Skupienia!\n\n";

					pause_game();
					continue;
				}

				player_bonus_accuracy += 15;

				if (player_bonus_accuracy > MAX_ACCURACY)
					player_bonus_accuracy = MAX_ACCURACY;

				break;
			case 2:
				if (player_crit_chance >= (MAX_CRIT * 0.5)) {

					clear_screen();

					cout << "\n\tOsiągnięto limit zakupu tej umiejętności\n\n";

					pause_game();
					continue;
				}

				player_crit_chance += 10;

				if (player_crit_chance > MAX_CRIT)
					player_crit_chance = MAX_CRIT;
				break;
			case 3:
				if (player_crit_chance >= MAX_CRIT) {

					clear_screen();

					cout << "\n\tOsiągnięto maksymalną ilość szansy na trafienie krytyczne!\n\n";

					pause_game();
					continue;
				}
				if (player_crit_chance < (MAX_CRIT * 0.5)) {

					clear_screen();

					cout << "\n\tMusisz kupić wykupić maksymalną ilość poprzedniej umiejętności!\n\n";

					pause_game();
					continue;
				}

				player_crit_chance += 10;

				if (player_crit_chance > MAX_CRIT)
					player_crit_chance = MAX_CRIT;
				break;
			case 4:

				if (player_second_breath) {

					clear_screen();

					cout << "\n\tMasz już tę umiejętność!\n\n";

					pause_game();
					continue;
				}

				player_second_breath = true;

				break;
			}

			player_money -= abilities[choice].price;

			clear_screen();

			cout << "\n\tKupiono: " << abilities[choice].name << "!\n\n";

			pause_game();
		}
		else if (keyboard_button == 27) {

			return;
		}
	}
}

void game_upgrades() {

	int choice = 0;
	int keyboard_button;

	while (true) {

		clear_screen();

		cout << "ESC - Powrót\n\n";
		cout << "\tWzmocnienia\n\n";

		cout << "\t";
		ui_colored("Hajs: $", player_money, COLOR_DARK_GREEN);

		cout << "\n\t";
		ui_colored("HP: ", player_health, COLOR_GREEN);
		ui_colored("/", player_maxhealth, COLOR_GREEN);

		cout << "\n\t";
		ui_colored("Pancerz: ", player_armor, COLOR_YELLOW);

		cout << "\n\t";
		ui_colored("Przebicie pancerza: ", player_armor_pen, COLOR_RED);

		cout << "\n\n";

		for (int i = 0; i < upgrades_amount; i++) {

			if (i == choice)
				cout << "\t►  ";
			else
				cout << "\t  ";

			print_colored(upgrades[i].name, COLOR_YELLOW);
			cout << " | ";
			r_ui_colored(upgrades[i].price, "$", COLOR_DARK_GREEN);
			cout << endl;
			cout << "\t   " << upgrades[i].description << endl;
		}

		keyboard_button = _getch();

		if (keyboard_button == 224) {

			keyboard_button = _getch();

			if (keyboard_button == 72) {

				choice--;

				if (choice < 0)
					choice = upgrades_amount - 1;
			}
			else if (keyboard_button == 80) {

				choice++;

				if (choice >= upgrades_amount)
					choice = 0;
			}
		}
		else if (keyboard_button == 13) {

			if (player_money < upgrades[choice].price) {

				clear_screen();

				cout << "\n\tZa mało hajsu!\n\n";

				pause_game();
				continue;
			}

			switch (choice) {

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
				// Cap pancerza: Tank = 120, Zabójca = 60, reszta = 90
				int armor_cap = 90;
				if (player_class == CLASS_TANK)     armor_cap = 120;
				if (player_class == CLASS_ASSASSIN) armor_cap = 60;

				if (player_armor >= armor_cap) {

					clear_screen();

					cout << "\n\tOsiągnięto maksymalny poziom pancerza!\n\n";

					pause_game();
					continue;
				}

				player_armor += 15;

				if (player_armor > armor_cap)
					player_armor = armor_cap;
			}
			break;

			case 3:
				// Kolce Odwetu: +10 przebicia pancerza przeciwnika
				player_armor_pen += 10;
				break;
			}

			player_money -= upgrades[choice].price;

			clear_screen();

			cout << "\n\tKupiono: " << upgrades[choice].name << "!\n\n";

			pause_game();
		}
		else if (keyboard_button == 27) {

			return;
		}
	}
}

void game_consumables() {

	int choice = 0;
	int keyboard_button;

	while (true) {

		clear_screen();

		cout << "ESC - Powrót\n\n";
		cout << "\tAlchemik\n\n";

		cout << "\t";
		ui_colored("Hajs: $", player_money, COLOR_DARK_GREEN);
		cout << endl;
		cout << "\tPosiadane mikstury:\n";
		if (player_health_potion > 0) {
			cout << "\t\t";
			print_colored("Mikstura Zdrowia", COLOR_GREEN);
			cout << " x" << player_health_potion << "\n";
		}

		if (player_precision_potion > 0) {
			cout << "\t\t";
			print_colored("Eliksir Precyzji", COLOR_CYAN);
			cout << " x" << player_precision_potion << "\n";
		}

		if (player_vampire_potion > 0) {
			cout << "\t\t";
			print_colored("Koktajl Wampira", COLOR_PURPLE);
			cout << " x" << player_vampire_potion << "\n";
		}
		if (player_health_potion <= 0 && player_precision_potion <= 0 && player_vampire_potion <= 0) cout << "\t\tBrak\n";
		cout << "\n";
		for (int i = 0; i < consumables_amount; i++) {

			if (i == choice)
				cout << "\t►  ";
			else
				cout << "\t  ";

			print_colored(consumables[i].name, COLOR_CYAN);
			cout << " | ";
			r_ui_colored(consumables[i].price, "$", COLOR_DARK_GREEN);
			cout << endl;
			cout << "\t   " << consumables[i].description << endl;
		}

		keyboard_button = _getch();

		if (keyboard_button == 224) {

			keyboard_button = _getch();

			if (keyboard_button == 72) {

				choice--;

				if (choice < 0)
					choice = consumables_amount - 1;
			}
			else if (keyboard_button == 80) {

				choice++;

				if (choice >= consumables_amount)
					choice = 0;
			}
		}
		else if (keyboard_button == 13) {

			if (player_money < consumables[choice].price) {

				clear_screen();

				cout << "\n\tZa mało hajsu!\n\n";

				pause_game();
				continue;
			}

			switch (choice) {

			case 0:
				player_health_potion++;
				break;

			case 1:
				player_precision_potion++;
				break;

			case 2:
				player_vampire_potion++;
				break;
			}

			player_money -= consumables[choice].price;

			clear_screen();

			cout << "\n\tKupiono: " << consumables[choice].name << "!\n";

			pause_game();
		}
		else if (keyboard_button == 27) {

			return;
		}
	}
}

// ===== SKLEP PRZEDMIOTÓW MAGICZNYCH =====
// Oblicza sumaryczne statystyki magiczne z założonych itemów
void recalculate_magic_stats() {
	// Resetuj statystyki magiczne (tylko te z itemów)
	player_spell_power = 0;
	player_magic_resist_pen = 0;
	player_magic_resist = 0;
	// maxmana: bazowa 100 + bonusy z klasy + bonusy z itemów
	int mana_bonus = 0;
	if (player_class == CLASS_MAGE) mana_bonus = 150;  // bonus Maga

	int item_mana = 0;
	int item_sp = 0;
	int item_mrpen = 0;
	int item_mr = 0;

	for (int i = 0; i < MAX_MAGIC_ITEMS; i++) {
		if (magic_item_slots[i] >= 0) {
			int idx = magic_item_slots[i];
			item_mana += magic_items[idx].bonus_mana;
			item_sp += magic_items[idx].bonus_spell_power;
			item_mrpen += magic_items[idx].magic_resist_pen;
			item_mr += magic_items[idx].bonus_magic_resist;
		}
	}

	int old_maxmana = player_maxmana;
	player_maxmana = 100 + mana_bonus + item_mana;
	player_spell_power = item_sp;
	player_magic_resist_pen = item_mrpen;
	player_magic_resist = item_mr;

	// Jeśli maxmana wzrosła (nowy item), dodaj różnicę do aktualnej many
	int mana_delta = player_maxmana - old_maxmana;
	if (mana_delta > 0) player_mana += mana_delta;
	// Zawsze upewnij się że mana nie przekracza maksimum
	if (player_mana > player_maxmana) player_mana = player_maxmana;
}

void game_magic_shop() {

	int choice = 0;
	int keyboard_button;

	while (true) {

		clear_screen();

		cout << "ESC - Powrót\n\n";
		print_colored("\tMagiczny Rynek\n\n", COLOR_BLUE);

		cout << "\t";
		ui_colored("Hajs: $", player_money, COLOR_DARK_GREEN);

		cout << "\n\t";
		print_colored("Sloty: ", COLOR_BRIGHT_WHITE);
		number_colored(magic_item_count, COLOR_CYAN);
		cout << " / ";
		number_colored(MAX_MAGIC_ITEMS, COLOR_CYAN);

		cout << "\n\t";
		print_colored("Mana: ", COLOR_BLUE);
		number_colored(player_mana, COLOR_BLUE);
		cout << " / ";
		number_colored(player_maxmana, COLOR_BLUE);

		cout << "\n\t";
		print_colored("Siła zaklęć: ", COLOR_PURPLE);
		number_colored(player_spell_power, COLOR_PURPLE);

		cout << "\n\t";
		print_colored("Przebicie MR: ", COLOR_DARK_CYAN);
		number_colored(player_magic_resist_pen, COLOR_DARK_CYAN);

		cout << "\n\t";
		print_colored("Odporność magiczna: ", COLOR_CYAN);
		number_colored(player_magic_resist, COLOR_CYAN);

		cout << "\n\n";

		// Lista założonych itemów
		bool any_equipped = false;
		for (int i = 0; i < MAX_MAGIC_ITEMS; i++) {
			if (magic_item_slots[i] >= 0) {
				if (!any_equipped) {
					print_colored("\tZałożone itemy:\n", COLOR_YELLOW);
					any_equipped = true;
				}
				int idx = magic_item_slots[i];
				cout << "\t  [" << i + 1 << "] ";
				print_colored(magic_items[idx].name, COLOR_YELLOW);
				cout << "\n";
			}
		}
		if (any_equipped) cout << "\n";

		cout << "\tDostępne itemy:\n\n";

		for (int i = 0; i < magic_items_amount; i++) {

			if (i == choice)
				cout << "\t►  ";
			else
				cout << "\t  ";

			print_colored(magic_items[i].name, COLOR_BLUE);
			cout << " | ";
			r_ui_colored(magic_items[i].price, "$", COLOR_DARK_GREEN);
			cout << "\n\t   " << magic_items[i].description << "\n";
		}

		keyboard_button = _getch();

		if (keyboard_button == 224) {

			keyboard_button = _getch();

			if (keyboard_button == 72) {
				choice--;
				if (choice < 0) choice = magic_items_amount - 1;
			}
			else if (keyboard_button == 80) {
				choice++;
				if (choice >= magic_items_amount) choice = 0;
			}
		}
		else if (keyboard_button == 13) {

			// Sprawdź czy mamy wolny slot
			if (magic_item_count >= MAX_MAGIC_ITEMS) {
				clear_screen();
				cout << "\n\tNie masz wolnych slotów na przedmioty magiczne! (max 6)\n\n";
				pause_game();
				continue;
			}

			// Sprawdź czy stać
			int buy_price = magic_items[choice].price;

			// Mag ma 50% rabatu na itemy magiczne
			if (player_class == CLASS_MAGE) {
				buy_price = buy_price / 2;
			}

			if (player_money < buy_price) {
				clear_screen();
				cout << "\n\tZa mało hajsu!\n\n";
				pause_game();
				continue;
			}

			// Znajdź wolny slot i załóż item
			for (int i = 0; i < MAX_MAGIC_ITEMS; i++) {
				if (magic_item_slots[i] < 0) {
					magic_item_slots[i] = choice;
					magic_item_count++;
					break;
				}
			}

			player_money -= buy_price;
			recalculate_magic_stats();

			clear_screen();
			cout << "\n\tKupiono i założono: ";
			print_colored(magic_items[choice].name, COLOR_BLUE);
			if (player_class == CLASS_MAGE) {
				cout << "\n\t";
				print_colored("(Rabat Maga: 50% ceny!)", COLOR_YELLOW);
			}
			cout << "\n\n";
			pause_game();
		}
		else if (keyboard_button == 27) {
			return;
		}
	}
}

// ===== WYBÓR KLASY POSTACI =====
void choose_player_class() {

	const int class_amount = 4;

	string class_names[class_amount] = {
		"Zabójca - duży bonus AD, mniejszy cap pancerza",
		"Mag - 150 bonus many, 50% zniżki na itemy magiczne",
		"Tank - większy cap pancerza, +150 HP na start, -25% dmg broń >500 dmg",
		"Wojownik - trochę bonus AD i trochę pancerza na start"
	};

	int choice = 0;
	int keyboard_button;

	while (true) {

		clear_screen();
		cout << endl;
		print_colored("\tWybierz klasę postaci:\n\n", COLOR_BRIGHT_WHITE);

		for (int i = 0; i < class_amount; i++) {
			if (i == choice) {
				cout << "\t►  "; print_colored(class_names[i], COLOR_BRIGHT_WHITE); cout << endl;
			}
			else
				cout << "\t  " << class_names[i] << endl;
		}

		cout << endl;

		keyboard_button = _getch();

		if (keyboard_button == 224) {
			keyboard_button = _getch();
			if (keyboard_button == 72) {
				choice--;
				if (choice < 0) choice = class_amount - 1;
			}
			else if (keyboard_button == 80) {
				choice++;
				if (choice >= class_amount) choice = 0;
			}
		}
		else if (keyboard_button == 13) {
			break;
		}
	}

	// Zastosuj bonusy klasy
	switch (choice) {

	case 0: // Zabójca
		player_class = CLASS_ASSASSIN;
		player_class_bonus_ad = 30;  // bonus AD jako osobna zmienna
		// Mniejszy cap pancerza - obsługiwane w logice zakupu (cap = 60 zamiast 90)
		clear_screen();
		cout << "\n\t";
		print_colored("Klasa: Zabójca\n", COLOR_RED);
		cout << "\t+30 bonus AD\n";
		cout << "\tMaksymalny pancerz: 60\n\n";
		pause_game();
		break;

	case 1: // Mag
		player_class = CLASS_MAGE;
		// 150 bonus many - recalculate_magic_stats() ustawi maxmanę po starcie
		player_maxmana = 250;  // 100 base + 150 bonus
		player_mana = 250;
		clear_screen();
		cout << "\n\t";
		print_colored("Klasa: Mag\n", COLOR_BLUE);
		cout << "\t+150 many (250 łącznie)\n";
		cout << "\t50% zniżka na itemy magiczne\n\n";
		pause_game();
		break;

	case 2: // Tank
		player_class = CLASS_TANK;
		// +150 HP na start
		player_maxhealth += 150;
		player_health += 150;
		// Wiekszy cap pancerza (120 zamiast 90) - obsługiwane w logice zakupu
		// -25% dmg dla broni powyżej 500 DMG - obsługiwane w walce
		clear_screen();
		cout << "\n\t";
		print_colored("Klasa: Tank\n", COLOR_YELLOW);
		cout << "\t+150 HP (250 łącznie)\n";
		cout << "\tMaksymalny pancerz: 120\n";
		cout << "\t-25% obrażeń broń powyżej 500 DMG\n\n";
		pause_game();
		break;

	case 3: // Wojownik
		player_class = CLASS_WARRIOR;
		player_class_bonus_ad = 10;   // trochę bonus AD
		player_armor += 10;           // trochę pancerza na start
		clear_screen();
		cout << "\n\t";
		print_colored("Klasa: Wojownik\n", COLOR_GREEN);
		cout << "\t+10 bonus AD\n";
		cout << "\t+10 pancerza na start\n\n";
		pause_game();
		break;
	}
}
